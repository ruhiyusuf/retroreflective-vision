CXX=g++ 
RM=rm

# flags
CXXFLAGS	:=-Wall
LIBS		:=

# opencv flags for linux
# CXXFLAGS	+=`pkg-config --cflags opencv4`
# LIBS		+=`pkg-config --libs opencv4`

# opencv flags for mingw64
# CXXFLAGS 	+=-I"C:\opencv\build\include" 
# CXXFLAGS 	+=-I"C:\opencv41\include"
CXXFLAGS	+=-I"C:\opencv-4.1.0\opencv-4.1.0\build\install\include"
CXXFLAGS 	+=-L"C:\opencv-4.1.0\opencv-4.1.0\build\install\x64\vc16\lib"
CXXFLAGS 	+=-L"C:\opencv-4.1.0\opencv-4.1.0\build\install\x64\vc16\bin"
# CXXFLAGS 	+=-L"C:\opencv41\build\x86\mingw\lib"
# LIBS 		+= -lopencv_core410 -lopencv_highgui410 -lopencv_videoio410 -lopencv_video410
LIBS 		+= -lopencv_core410 -lopencv_highgui410 -lopencv_videoio410 -lopencv_video410 -lopencv_imgproc410

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECS=$(SOURCES:%.cpp=%)

.PHONY: all
all: $(OBJECTS) $(EXECS)

.cpp.o:
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LIBS)

.o:
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean
clean: 
	-@ $(RM) *.o
	-@ $(RM) $(EXECS)
