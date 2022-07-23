CXX=g++ 
RM=rm
INCLUDE_DIR="/usr/local/include/opencv4"
CXXFLAGS=-I $(INCLUDE_DIR)
LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio

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
