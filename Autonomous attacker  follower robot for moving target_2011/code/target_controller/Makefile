
GLLIBS = -lGL -lGLU -lglut

PROGRAMS=control

.SUFFIXES: .c .cpp

all: $(PROGRAMS)

clean:
	rm -f $(PROGRAMS)
	rm -f *.o
	rm -f *~

control: control.cpp
	g++ -Wall -g control.cpp -o $@ $(GLLIBS)
