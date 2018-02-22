#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

//given ancestor struct
typedef struct ancestry {
  pid_t ancestors[10];
  pid_t siblings[100];
  pid_t children[100];
} ancestry;

//location on the syscall table
#define __NR_cs3013_syscall2 334

//given
long testCall2 (unsigned short* num, ancestry* a) {
  return (long) syscall(__NR_cs3013_syscall2, num, a);
}

int main (int argc, char* argv[]) {
//check if arguments are valid
  if(argc<2){
	printf("Please enter a pid as an argument\n\n");
	exit(-1);
  }
  
//get first argument, make it a number, make it a numerical 
  unsigned short pid = atoi(argv[1]);
  unsigned short* p;
  p = &pid;

  //malloc the pointers to be passed in; set up values
  ancestry* info = (ancestry*)malloc(sizeof(ancestry));

  //run syscall2
  printf("The return values of the system calls are:\n");
  printf("\tcs3013_syscall2: %ld\n", testCall2(p, info));
  return 0;

}
