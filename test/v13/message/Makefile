INCLUDES = -I../../../include
LDFLAGS = -lboost_unit_test_framework-mt
CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++ -Wall -pedantic $(INCLUDES)
# CXX = g++-4.9
# CXXFLAGS = -std=c++11 -Wall -pedantic $(INCLUDES)

SRCS = switch_features_test.cpp \
	   switch_config_test.cpp \
	   table_mod_test.cpp \
	   packet_in_test.cpp \
	   packet_out_test.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = message_test

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

$(TARGET): $(OBJS)
	$(build_test)

run: all
	./$(TARGET)

run_each: $(ALL_TESTS)
	for test in $(ALL_TESTS); do \
		echo ========= $$test ========= ; \
		./$$test ; \
		echo ; \
	done

clean:
	-rm *.o $(DEPENDS) $(ALL_TESTS) $(TARGET) 2> /dev/null

-include $(DEPENDS)

