INCLUDES=-I../../../include
CXX=clang++
CXXFLAGS=-std=c++11 -stdlib=libc++ -Wall -pedantic $(INCLUDES)
LDFLAGS=-lboost_system-mt -lboost_log-mt -lboost_log_setup-mt
TARGET=topology

all: $(TARGET)

clean:
	-rm $(TARGET)

