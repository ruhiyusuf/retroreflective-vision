CXX=g++ 
INCLUDE_DIR="/usr/local/include/opencv4"
CXXFLAGS=-I $(INCLUDE_DIR)
DEPS=rpistream.h
OBJ=rpistream.o

LIBS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio

%.o: %.c $(DEPS)
	$CXX -c -o $@ $< $(CXXFLAGS)

rpistream: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean: 
	rm *.o rpistream
