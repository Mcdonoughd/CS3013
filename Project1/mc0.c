//Midday Commander v0 By Daniel McDonough 1/17/17

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <sys/time.h>
#include  <sys/resource.h>






void PrintStats(struct timeval start,struct timeval end,long fault_init,long recl_init, long fault_post, long recl_post){
    long fault_dif;
    long recl_dif;
    //Do Calculations first
    float time;
    time =  ((float) end.tv_usec - (float) start.tv_usec)/1000 + ((float)end.tv_sec - (float) start.tv_sec)*1000;
    fault_dif = (fault_post - fault_init);
    recl_dif = (recl_post - recl_init);




    printf("--- Satistics ---\n");


    //getrusage(RUSAGE_CHILDREN, usage); //to calculate the info of the children

    printf("Elapsed Time: %.3f milliseconds\n", time);//, timeElap.tv_usec);

    printf("Page Faults: %ld\n", fault_dif);

    printf("Page Faults (reclaimed): %ld\n\n", recl_dif);
}
void CommandHandler(int pid, char* command, char* env[]){
    if (pid == 0) {    //this is the child process
        execvp(command, env); //it runs the given command
    } else {   //parent process
        wait(NULL);
        printf("\n");
    }
}


void OptionHandler(char *p) {
    struct timeval start_timer;
    struct timeval end_timer;
    struct rusage info;
    long fault_init, recl_init, fault_post, recl_post;

    //convert input to int
    switch (atoi(p)) {
        //if 0 do WHOAMI
        case 0:

            printf("\n-- Who Am I? --\n");
            //setup command info
            char* command0 = "whoami";
            char* envp0[] = {"whoami", NULL}; //no arguments

            //Get Stats Prior to starting the timer
            getrusage(RUSAGE_CHILDREN, &info); //Stats of self should be transferred to child
            fault_init = info.ru_majflt;
            recl_init = info.ru_minflt;

            gettimeofday(&start_timer, NULL); //start the timer

            int pid0 = fork(); //creates new process to handle the command


            //handle the command
            CommandHandler(pid0,command0,envp0);

            gettimeofday(&end_timer, NULL); //end the timer after command ends
            //get info on the child
            getrusage(RUSAGE_CHILDREN, &info);
            fault_post = info.ru_majflt;
            recl_post = info.ru_minflt;

            PrintStats(start_timer,end_timer,fault_init,recl_init,fault_post,recl_post);

            break;
        //if 1 do last
        case 1:

            printf("\n-- Last Logins --\n");
            char* command1 = "last";
            char* envp1[] = {"last","-n 1",NULL};//-n 1 argument for the most recent login

            //Get Stats Prior to starting the timer
            getrusage(RUSAGE_CHILDREN, &info); //Stats of self should be transferred to child
            fault_init = info.ru_majflt;
            recl_init = info.ru_minflt;

            gettimeofday(&start_timer, NULL); //start the timer

            int pid1 = fork(); //creates new process to handle the command
            //handle the command
            CommandHandler(pid1,command1,envp1);


            gettimeofday(&end_timer, NULL); //end the timer after command ends
            //get info on the child
            getrusage(RUSAGE_CHILDREN, &info);
            fault_post = info.ru_majflt;
            recl_post = info.ru_minflt;

            PrintStats(start_timer,end_timer,fault_init,recl_init,fault_post,recl_post);




            break;

        //if 2 do ls
        case 2:
            printf("-- Directory Listing --\n");

            //GET ARGUMENTS
            printf("Arguments?:\n");
            char*a = (char *) malloc(sizeof(char*));
            scanf("%s", a); //scan input

            //GET DIRECTORY
            printf("Directory?:\n");
            char*d = (char *) malloc(sizeof(char*));
            scanf("%s", d); //scan input

            //Handle Command
            char* command2 = "ls";
            char* envp2[] = {"ls",a,d,NULL};//-n 1 argument for the most recent login
            //Get Stats Prior to starting the timer
            getrusage(RUSAGE_CHILDREN, &info); //Stats of self should be transferred to child
            fault_init = info.ru_majflt;
            recl_init = info.ru_minflt;

            gettimeofday(&start_timer, NULL); //start the timer


            int pid2 = fork(); //creates new process to handle the command
            //handle the command
            CommandHandler(pid2,command2,envp2);

            //Fly like an eagle, until i'm free
            free(a);
            free(d);


            gettimeofday(&end_timer, NULL); //end the timer after command ends
            //get info on the child
            getrusage(RUSAGE_CHILDREN, &info);
            fault_post = info.ru_majflt;
            recl_post = info.ru_minflt;

            PrintStats(start_timer,end_timer,fault_init,recl_init,fault_post,recl_post);


            break;

        default:
            //if input is not 0,1, or 2
            fprintf(stderr, "\n ERROR: Please, type the number of one of the listed options!\n\n");
            break;

    }
}


void main(void) {
    printf("==== Mid-Day Commander, v0 ====\n");
    while (1) {
        //Print "UI"
        printf("G'day, Commander! What command would you like to run?\n");
        printf("   0. whoami : Prints out the result of the whoamicommand\n");
        printf("   1. last   : Prints out the result of the last command\n");
        printf("   2. ls     : Prints out the result of a listing on a user-specified path\n");
        printf("Option?:");
        //allocate space for input
        char* p = (char*) malloc(sizeof(char*));
        scanf("%s", p); //scan input
        OptionHandler(p);//hadle the input
        free(p); //free the memory allocated by the input
    }

}

