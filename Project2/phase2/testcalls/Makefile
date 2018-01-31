all: procAncestry test

procAncestry: proc_Ancestry.o
	gcc -pthread proc_Ancestry.o -o procAncestry

test: test.o
	gcc -pthread test.o -o test

procAncestry.o: proc_Ancestry.c
	gcc -g -c proc_Ancestry.c

test.o:test.c
	gcc -g -c test.c

clean:
	rm -f *.o procAncestry
	rm -f *.o test
