INCLUDES = -I../../../include
LDFLAGS = -lboost_unit_test_framework-mt -lboost_system-mt
CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++ -Wall -pedantic $(INCLUDES)
# CXX = g++-4.9
# CXXFLAGS = -std=c++11 -Wall -pedantic $(INCLUDES)

SRCS = output_test.cpp \
	   copy_ttl_out_test.cpp \
	   copy_ttl_in_test.cpp \
	   set_mpls_ttl_test.cpp \
	   decrement_mpls_ttl_test.cpp \
	   push_vlan_test.cpp \
	   pop_vlan_test.cpp \
	   push_mpls_test.cpp \
	   pop_mpls_test.cpp \
	   set_queue_test.cpp \
	   group_test.cpp \
	   set_nw_ttl_test.cpp \
	   decrement_nw_ttl_test.cpp \
	   set_field_test.cpp \
	   push_pbb_test.cpp \
	   pop_pbb_test.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET_LIB = libaction_test.dylib
TARGET = all_test

ALL_TESTS = $(OBJS:.o=)

DEPENDS = $(addprefix .,$(OBJS:.o=.depends))

define build_test
$(CXX) -DBOOST_TEST_MODULE=$@ $(CXXFLAGS) -c ../../driver.cpp -o ../../driver.o
$(CXX) $(CXXFLAGS) -o $@ ../../driver.o $^ $(LDFLAGS)
endef

.PHONY: all clean run run_each

all: $(DEPENDS) $(TARGET)

.%.depends: %.cpp
	$(CXX) -MM $(CXXFLAGS) $< > $@

%: %.o
	$(build_test)

$(TARGET_LIB): $(OBJS)
	$(CXX) -dynamiclib $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(TARGET): $(TARGET_LIB)
	$(build_test)

run: all
	./$(TARGET) -l message

run_each: $(ALL_TESTS)
	for test in $(ALL_TESTS); do \
		echo ========= $$test ========= ; \
		./$$test ; \
		echo ; \
	done

clean:
	-rm *.o $(DEPENDS) $(ALL_TESTS) $(TARGET_LIB) $(TARGET) 2> /dev/null

-include $(DEPENDS)

