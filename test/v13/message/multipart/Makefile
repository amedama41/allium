INCLUDES = -I../../../../include
LDFLAGS = -lboost_unit_test_framework-mt
CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++ -Wall -pedantic $(INCLUDES)
# CXX = g++-4.9
# CXXFLAGS = -std=c++11 -Wall -pedantic $(INCLUDES)

SRCS = description_test.cpp \
	   flow_stats_test.cpp \
	   aggregate_stats_test.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET_LIB = libmultipart_test.dylib
TARGET = multipart_test

ALL_TESTS = $(OBJS:.o=)

DEPENDS = $(addprefix .,$(OBJS:.o=.depends))

define build_test
$(CXX) -DBOOST_TEST_MODULE=$@ $(CXXFLAGS) -c ../../../driver.cpp -o ../../driver.o
$(CXX) $(CXXFLAGS) -o $@ ../../../driver.o $^ $(LDFLAGS)
endef

.PHONY: all clean run run_each

all: $(DEPENDS) $(TARGET)

.%.depends: %.cpp
	$(CXX) -MM $(CXXFLAGS) $< > $@

%: %.o
	$(build_test)

$(TARGET_LIB): $(OBJS)
	$(CXX) -dynamiclib -std=c++11 -o $@ $^ $(LDFLAGS)

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

