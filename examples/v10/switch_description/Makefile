INCLUDES=-I../../../include -I../../../bulb/include
CXX=clang++
CXXFLAGS=-std=c++11 -stdlib=libc++ -Wall -pedantic $(INCLUDES)
LDFLAGS=-lboost_system-mt
TARGET=switch_description

all: $(TARGET)

run: all
	./$(TARGET)

clean:
	-rm $(TARGET)

