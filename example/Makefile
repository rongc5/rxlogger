CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O2 -pthread
INCLUDES = -I../include
LIBDIR = ../
LIBS = -L$(LIBDIR) -llogger -lpthread

TARGET = simple_example
SOURCE = simple_example.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(LIBS) -o $@

run: $(TARGET)
	mkdir -p logs
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf logs audit.log 