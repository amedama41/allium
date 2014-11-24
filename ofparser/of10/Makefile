CXX=clang++
CXXFLAGS=-std=c++1y -stdlib=libc++ -Wall -pedantic -I..
CXXLIBS=-lboost_filesystem-mt -lboost_system-mt -lboost_thread-mt -lboost_wave-mt

OBJS=main.o \
	 ../comment_to_space.o \
	 ../parse.o \
	 ../preprocess.o

TARGET=ofparser

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(CXXLIBS)

run: all
	./$(TARGET) ./openflow.h

clean:
	-rm $(OBJS) $(TARGET)
