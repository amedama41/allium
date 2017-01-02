INCLUDES=-I../../../include -I../../../bulb/include
CXX=clang++
CXXFLAGS=-std=c++11 -stdlib=libc++ -Wall -pedantic $(INCLUDES)
LDFLAGS=-lboost_system-mt
TARGET=repeater_hub

all: $(TARGET)

clean:
	-rm $(TARGET)

run: all
	./$(TARGET)

