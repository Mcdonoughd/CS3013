//testcalls.c by Daniel McDonough 1/27/18

#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>

#define __NR_cs3013_syscall1 333

//check if file exists
int exists(const char *fname) {
    if (fopen(fname, "r") != NULL) {
        //fclose(file);
        return 1;
    }
    return 0;
}

long testCall1(void) {
    //run the syscall
    return (long) syscall(__NR_cs3013_syscall1);
}

int main() {
    //create some char buffers
    char buff1[300];
    char buff2[300];
    //printf("YOU DIED HERE!\n");
    FILE* virus;
    FILE* not_virus;
    //open both a virus file and non virus file
    if (exists("virus.txt")) {
        virus = fopen("virus.txt", "r");
        fscanf(virus, "%s", buff1);
    } else {
        printf("virus.txt does not exist!\n");
    }
    if (exists("not_virus.txt")) {
        not_virus = fopen("not_virus.txt", "r");
        fscanf(not_virus, "%s", buff2);
    } else {
        printf("not_virus.txt does not exist!\n");
    }
    //close the books
    fclose(virus);
    fclose(not_virus);
    printf("cs3013_syscall1: %ld\n", testCall1());
    return 0;
}
