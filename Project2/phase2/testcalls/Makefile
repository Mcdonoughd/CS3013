all: procAncestry

procAncestry: proc_Ancestry.o
	gcc -pthread proc_Ancestry.o -o procAncestry

procAncestry.o: proc_Ancestry.c
	gcc -g -c proc_Ancestry.c

clean:
	rm -f *.o procAncestry
