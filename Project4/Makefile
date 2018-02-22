all: vmSim vmSim.o

vmSim: vmSim.o 
	gcc vmSim.o -o vmSim -Wall -pthread -lm

clean:
	rm -f *.o vmSim
