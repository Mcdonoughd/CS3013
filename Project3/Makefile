all: bathroomSim bathroom.o

bathroomSim: bathroomSim.o bathroom.o bathroom.h
	gcc bathroomSim.o bathroom.o -o bathroomSim -Wall -pthread -lm
bathroomSim.o: bathroomSim.c bathroom.h
	gcc -g -c bathroomSim.c -Wall -pthread
bathroom.o: bathroom.c bathroom.h
	gcc -g -c bathroom.c -Wall -pthread
clean:
	rm -f *.o bathroomSim
