PROGRAMS=attacker

.SUFFIXES: .cpp

all: $(PROGRAMS)

clean:
	rm -f $(PROGRAMS)
	rm -f *.o
	rm -f *~

attacker:attacker.cpp
	gcc `pkg-config --cflags opencv` `pkg-config --libs opencv` attacker.cpp -o attacker
