import sys
from parser import DeclarationCollector
import generator.error_code as error_code

IGNORE_ERROR_TYPE = {
        10: {},
        13: {'OFPET_EXPERIMENTER'},
}

if len(sys.argv) != 2:
    print 'usage {} <openflow.h path>'.format(sys.argv[0])
    sys.exit()

collector = DeclarationCollector()
filename = sys.argv[1]
collector.collect(filename)

dirname = 'of{}'.format(collector.version)
with open(dirname + '/error.hpp', 'w') as f:
    f.write(error_code.generate(collector, IGNORE_ERROR_TYPE[collector.version]))

