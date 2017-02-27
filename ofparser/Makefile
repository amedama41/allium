CXX=clang++
CXXFLAGS=-std=c++1y -stdlib=libc++ -Wall -pedantic
CXXLIBS=-lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt -lboost_wave-mt

OBJS=main.o \
	 comment_to_space.o \
	 parse.o \
	 preprocess.o \
	 generate_error.o \
	 generate_enum_to_string.o \
	 generate_fusion_adaptor.o \
	 generate_byteorder.o
TARGET=ofparser

cppparser: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(CXXLIBS)

run_cppparser: cppparser
	./$(TARGET) ./of13/openflow.h

clean:
	-rm $(OBJS) $(TARGET)

