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
	-rm *.pyc ./generator/*.pyc
	-rm $(OBJS) $(TARGET)

run:
	python ./generator.py ./of10/openflow.h
	mv ./of10/error.hpp ../include/canard/network/protocol/openflow/v10/
	python ./generator.py ./of13/openflow.h
	mv ./of13/error.hpp ../include/canard/network/protocol/openflow/v13/

