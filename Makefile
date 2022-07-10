CXX=g++ 
INCLUDE_DIR="/usr/local/include/opencv4"
CXXFLAGS=-I$(INCLUDE_DIR)
DEPS=rpistream.h
OBJ=rpistream.o

%.o: %.c $(DEPS)
	$CXX -c -o $@ $< $(CFLAGS)

rpistream: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS)

clean: 
	rm *.o rpistream
