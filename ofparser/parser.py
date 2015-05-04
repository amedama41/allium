from future_builtins import map, filter
from collections import OrderedDict
import itertools

import clang.cindex
from clang.cindex import Index, TranslationUnit, CursorKind, TypeKind

def traverse_node(node, filename, func, level=0):
    if func(node, level):
        return
    # indent_str = ''.join(itertools.repeat(' ', level))
    # print "{}{} : {}".format(indent_str, node.kind.name, node.displayname)
    for child in filter(
            lambda node: str(node.location.file) == filename,
            node.get_children()):
        traverse_node(child, filename, func, level + 1)

def print_struct(node, indent):
    indent_str = ''.join(itertools.repeat('  ', indent))
    print "{}{} : {}".format(indent_str, node.raw.kind.name, node.name)
    for child in node.members:
        print '{}  {} : {}'.format(indent_str, child.name, child.type)

def print_enum(node, indent):
    indent_str = ''.join(itertools.repeat('  ', indent))
    print "{}{} : {}".format(indent_str, node.kind.name, node.displayname)
    for child in node.get_children():
        print '{}  {} = {}'.format(indent_str, child.displayname, child.enum_value)

def print_macro(node, indent):
    indent_str = ''.join(itertools.repeat('  ', indent))
    tokens = list(node.get_tokens())[:-1]
    if len(tokens) == 2:
        print "{}{} : {} = {}".format(indent_str, node.kind.name, node.displayname, tokens[1].spelling)
    else:
        print "{}{} : {}".format(indent_str, node.kind.name, ' '.join(map(lambda token: token.spelling, tokens)))

class StructDecl(object):
    def __init__(self, node):
        self._node = node

    @property
    def name(self):
        return self._node.displayname

    @property
    def size(self):
        return self._node.type.get_size()

    @property
    def members(self):
        return map(StructMember, self._node.get_children())

    @property
    def raw(self):
        return self._node

class StructMember(object):
    def __init__(self, node):
        self._node = node

    @property
    def name(self):
        return self._node.displayname

    @property
    def type(self):
        if self.is_array():
            return '{type}[{count}]'.format(
                    type=self.element_type, count=self.element_count)
        return self._node.type.get_declaration().displayname

    @property
    def element_type(self):
        return self._node.type.element_type.spelling

    @property
    def element_count(self):
        return self._node.type.element_count

    def is_array(self):
        return self._node.type.kind == TypeKind.CONSTANTARRAY

    def is_zero_elements_array(self):
        return self.is_array() and self.element_count == 0

    @property
    def size(self):
        return self._node.type.get_size()

class DeclarationCollector(object):
    def __init__(self):
        self._struct_decls = OrderedDict()
        self._enum_decls = OrderedDict()
        self._macro_defs = OrderedDict()

    def __call__(self, node, level):
        if node.kind is CursorKind.STRUCT_DECL:
            if node.displayname == '':
                return True
            self._struct_decls[node.displayname] = StructDecl(node)
            # print_struct(StructDecl(node), level)
            return True
        if node.kind is CursorKind.ENUM_DECL:
            if node.displayname == '':
                return True
            self._enum_decls[node.displayname] = node
            # print_enum(node, level)
            return True
        if node.kind is CursorKind.MACRO_DEFINITION:
            self._macro_defs[node.displayname] = node
            # print_macro(node, level)
            return True
        return False

    def collect(self, filename):
        index = Index.create()
        tu = index.parse(filename, options=TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
        traverse_node(tu.cursor, filename, self)

    @property
    def version(self):
        return int(list(self.macro_defs['OFP_VERSION'].get_tokens())[1].spelling, 16) + 9

    @property
    def struct_decls(self):
        return self._struct_decls

    @property
    def enum_decls(self):
        return self._enum_decls

    @property
    def macro_defs(self):
        return self._macro_defs

