INCLUDES=-I../../include
CXX=clang++
CXXFLAGS=-std=c++11 -stdlib=libc++ -Wall -pedantic $(INCLUDES)
LDFLAGS=-lboost_system-mt -lboost_coroutine-mt
TARGET=coroutine

all: $(TARGET)

clean:
	-rm $(TARGET)

