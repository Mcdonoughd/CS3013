#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

//given struct
typedef struct ancestry {
  pid_t ancestors[10];
  pid_t siblings[100];
  pid_t children[100];
} ancestry;

//the test call
#define __NR_cs3013_syscall2 334

//given test callw
long testCall2 (unsigned short* num, ancestry* a) {
  int pid = fork();
  if (pid < 0){
    printf("THIS SHOULD NOT HAPPEN!!!!\n\n\n");
    return -1;
  } 
  else if(pid == 0){ // child
    while(1);
  } 
  else{
    *num = 10; //a pid that is known to be a process via "ps axjf"
    printf("%d\n", pid);
    return (long) syscall(__NR_cs3013_syscall2, num, a);
  }
}

int main () {
//forced unsigned short to work with the testcall
  unsigned short* pid = (unsigned short*) malloc(sizeof(unsigned short));
  ancestry* info = (ancestry*)malloc(sizeof(ancestry));
  printf("The return values of the system calls are:\n");

  printf("cs3013_syscall2: %ld\n", testCall2(pid, info));
  printf("The first parent:, %d\n", info->ancestors[0]); 
  printf("The second parent:, %d\n", info->ancestors[1]);
  printf("The third parent:, %d\n", info->ancestors[2]);

  return 0;
}
