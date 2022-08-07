CXX=g++ 
RM=rm

# flags
CXXFLAGS	:=-std=c++11
LIBS		:=-lstdc++

# opencv flags
CXXFLAGS	+=`pkg-config --cflags opencv4`
LIBS		+=`pkg-config --libs opencv4`

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECS=$(SOURCES:%.cpp=%)

.PHONY: all
all: $(OBJECTS) $(EXECS)

.cpp.o:
	$(CXX) -c -o $@ $< $(CXXFLAGS)

.o:
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean
clean: 
	-@ $(RM) *.o
	-@ $(RM) $(EXECS)
