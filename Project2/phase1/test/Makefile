all: testcalls

testcalls: testcalls.o
	gcc -pthread testcalls.o -o testcalls

testcalls.o: testcalls.c
	gcc -g -c testcalls.c

clean:
	rm -f *.o testcalls
