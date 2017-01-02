INCLUDES=-I../../../include -I../../../bulb/include
CXX=clang++
CXXFLAGS=-std=c++11 -stdlib=libc++ -Wall -pedantic -O3 -DNDEBUG $(INCLUDES)
LDFLAGS=-lboost_system-mt -lboost_coroutine-mt

all: cbench_switch

clean:
	-rm cbench_switch

