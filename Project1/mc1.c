//Midday Commander v1 By Daniel McDonough 1/17/17

#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <sys/wait.h>
#include  <sys/time.h>
#include  <sys/resource.h>
#include  <assert.h>

#define MaxCommands 10

char* NewCommands[MaxCommands];
//counter=3 because 0,1,2 are already taken
int Command_counter = 3;

//Calculate and print Stats
void PrintStats(struct timeval start,struct timeval end,long fault_init,long recl_init, long fault_post, long recl_post){
    long fault_dif;
    long recl_dif;
    float time;//keeps on slipin slipin slipin into the future...

    //Do Calculations first
    time =  ((float) end.tv_usec - (float) start.tv_usec)/1000 + ((float)end.tv_sec - (float) start.tv_sec)*1000;
    fault_dif = (fault_post - fault_init);
    recl_dif = (recl_post - recl_init);
    //print stats
    printf("--- Satistics ---\n");
    printf("Elapsed Time: %.3f milliseconds\n", time);
    printf("Page Faults: %ld\n", fault_dif);
    printf("Page Faults (reclaimed): %ld\n\n", recl_dif);
}

//Produces/Executes the Commands themselves
void CommandHandler(int pid, char* command, char* env[]){
    if (pid == 0) {    //this is the child process
        execvp(command, env); //it runs the given command
    } else {   //parent process
        wait(NULL);
        printf("\n");
    }
}

int str_split (char* command, char* fullCommand[]){	

	char delim[3] = " \n";

	char* token = strtok(command, delim);

	int i = 0;//COUNTER



	while(token != NULL){

		fullCommand[i] = (char*) malloc(sizeof(token));

		strcpy(fullCommand[i], token);

		token = strtok(NULL, delim);

		i++;
	}

	fullCommand[i] = NULL;

	//printf("fullCommand[0], fullCommand[1], fullCommand[2]: %s, %s, %s\n", fullCommand[0],fullCommand[1],fullCommand[2]);

	return i;
}



//Handle a numerical input
void NumOptionHandler(char* c){
    int p = atoi(c);
    struct timeval start_timer;
    struct timeval end_timer;
    struct rusage info;
    long fault_init, recl_init, fault_post, recl_post;

    switch (p) {
        //if 0 do WHOAMI
        case 0:

            printf("\n-- Who Am I? --\n");
            //setup command info
            char *command0 = "whoami";
            char *envp0[] = {"whoami", NULL}; //no arguments

            //Get Stats Prior to starting the timer
            getrusage(RUSAGE_CHILDREN, &info); //Stats of self should be transferred to child
            fault_init = info.ru_majflt;
            recl_init = info.ru_minflt;

            gettimeofday(&start_timer, NULL); //start the timer

            int pid0 = fork(); //creates new process to handle the command


            //handle the command
            CommandHandler(pid0, command0, envp0);

            gettimeofday(&end_timer, NULL); //end the timer after command ends
            //get info on the child
            getrusage(RUSAGE_CHILDREN, &info);
            fault_post = info.ru_majflt;
            recl_post = info.ru_minflt;

            PrintStats(start_timer, end_timer, fault_init, recl_init, fault_post, recl_post);

            break;
            //if 1 do last
        case 1:

            printf("\n-- Last Logins --\n");
            char *command1 = "last";
            char *envp1[] = {"last", "-n 1", NULL};//-n 1 argument for the most recent login

            //Get Stats Prior to starting the timer
            getrusage(RUSAGE_CHILDREN, &info); //Stats of self should be transferred to child
            fault_init = info.ru_majflt;
            recl_init = info.ru_minflt;

            gettimeofday(&start_timer, NULL); //start the timer

            int pid1 = fork(); //creates new process to handle the command
            //handle the command
            CommandHandler(pid1, command1, envp1);


            gettimeofday(&end_timer, NULL); //end the timer after command ends
            //get info on the child
            getrusage(RUSAGE_CHILDREN, &info);
            fault_post = info.ru_majflt;
            recl_post = info.ru_minflt;

            PrintStats(start_timer, end_timer, fault_init, recl_init, fault_post, recl_post);


            break;

            //if 2 do ls
        case 2:
            printf("-- Directory Listing --\n");

            //GET ARGUMENTS
            printf("Arguments?:\n");
            char *a = (char *) malloc(sizeof(char *));
            scanf("%s", a); //scan input

            //GET DIRECTORY
            printf("Directory?:\n");
            char *d = (char *) malloc(sizeof(char *));
            scanf("%s", d); //scan input

            //Handle Command
            char *command2 = "ls";
            char *envp2[] = {"ls", a, d, NULL};//-n 1 argument for the most recent login
            //Get Stats Prior to starting the timer
            getrusage(RUSAGE_CHILDREN, &info); //Stats of self should be transferred to child
            fault_init = info.ru_majflt;
            recl_init = info.ru_minflt;

            gettimeofday(&start_timer, NULL); //start the timer


            int pid2 = fork(); //creates new process to handle the command
            //handle the command
            CommandHandler(pid2, command2, envp2);

            //Fly like an eagle, until i'm free
            free(a);
            free(d);


            gettimeofday(&end_timer, NULL); //end the timer after command ends
            //get info on the child
            getrusage(RUSAGE_CHILDREN, &info);
            fault_post = info.ru_majflt;
            recl_post = info.ru_minflt;

            PrintStats(start_timer, end_timer, fault_init, recl_init, fault_post, recl_post);


            break;
//if input is not 0,1, or 2
        default:

            //check if custom command
            if(p<Command_counter){
                //printf("THIS IS A CUSTOM COMMAND!\n");
//copy input command from NewCommands[p] to prevent over writing on UI
		char* inputCommand = (char*) malloc(sizeof(NewCommands[p]));
		strcpy(inputCommand, NewCommands[p]);

                printf("-- Command %s --\n",inputCommand);
//calloc memory as before
		char** envpA = (char**) calloc(strlen(NewCommands[p]), sizeof(char*)); 
//split the string for arguments
		int argNum = str_split(inputCommand, envpA); 

//THEcommand is the finalized command		
		char* THEcommand = (char*) malloc(sizeof(envpA[0]));
//compy the data into envpA
		strcpy(THEcommand, envpA[0]);

                //Get Stats Prior to starting the timer
                getrusage(RUSAGE_CHILDREN, &info); //Stats of self should be transferred to child
                fault_init = info.ru_majflt;
                recl_init = info.ru_minflt;

                gettimeofday(&start_timer, NULL); //start the timer

                int pidA = fork(); //creates new process to handle the command
                //handle the command

                CommandHandler(pidA, THEcommand, envpA);


                gettimeofday(&end_timer, NULL); //end the timer after command ends
                //get info on the child
                getrusage(RUSAGE_CHILDREN, &info);

                fault_post = info.ru_majflt;
                recl_post = info.ru_minflt;

                //print the stats
                PrintStats(start_timer, end_timer, fault_init, recl_init, fault_post, recl_post);


            }
            else {
                fprintf(stderr, "\n ERROR: Please, type the number of one of the listed options!\n\n");
            }
            break;
    }
}

//handles char input
void CharOptionHandler(char* p){
    switch(*p){
        case'a':
            //check if command counter reaches maximum

            if(Command_counter==(MaxCommands+3)){
                printf("\n -- BRZZT CANT FIT ANY MORE COMMANDS -- \n\n ");
                break;
            }

            printf("\n-- Add Command --\n");
            printf("Command to add: ");

            size_t buffer = 100; //a buffer

            char* input = (char*) malloc(buffer*sizeof(char)); //allocate for input

            getchar();

            size_t Line = getline(&input, &buffer,  stdin);

            //end the input as /0 instead  of /n
            if(input[Line - 1] == '\n'){
                input[Line - 1] = '\0';
            }
            //allocate for command list
            NewCommands[Command_counter] = (char*) malloc(buffer*sizeof(char));
            //copy string
            strcpy(NewCommands[Command_counter], input);
            //print ID with counter because 0,1,2 are already taken
            printf("Okay, added with ID %d\n\n", (Command_counter));
            //increment counter
            Command_counter++;

            //FLY LIKE AN EAGLE AND LET MY SPIRIT CARRY ME
            //free(buffer);
            free(input);
            break;
        case'c':
            printf("-- Change Directory --\n");
            printf("Directory?:");
            char *d = (char *) malloc(sizeof(char *));
            scanf("%s", d); //scan input
            chdir(d); //run chdir instead to change directory
            //free(d); Apparently you shouldnt free after chdir
            printf("\n");
            break;
        case'e':
            printf("Logging you out, Commander.\n");
            exit(0);
            break;
        case'p':
            printf("-- Current Directory --\n");
            char *commandp = "pwd";
            char *envpp[] = {"pwd",NULL};
            int pidp = fork(); //creates new process to handle the command
            //handle the command
            CommandHandler(pidp, commandp, envpp);
            break;
        default:
            fprintf(stderr, "\n ERROR: Please, type the number of one of the listed options!\n\n");
            break;
    }

}



//Handles the input
void OptionHandler(char *p) {
    if(isdigit(*p)){
        //If input is a number run numerical input handler
        NumOptionHandler(p);

    }

    else{ //input is a letter
        CharOptionHandler(p);
    }
}

int printNewCommand(char* CommandList[], int Command_index ){
//if no new commands just return
    if(Command_index == 3){
        return 1;
    }else{
        for(int i = 3; i < Command_index; i++){
            //increment i by 3 to get actual number representation
            //print the CommandList at index i (not incremented
            printf("   %d. %s   : User added Command\n", i, CommandList[i]);
        }
    }
    return 0;
}
void main(void) {
    printf("==== Mid-Day Commander, v1 ====\n");
    while (1) {
        //Print "UI"
        printf("G'day, Commander! What command would you like to run?\n");
        printf("   0. whoami : Prints out the result of the whoamicommand\n");
        printf("   1. last   : Prints out the result of the last command\n");
        printf("   2. ls     : Prints out the result of a listing on a user-specified path\n");
        //print new commands added
        printNewCommand(NewCommands,Command_counter);
        printf("   a. add command : Adds a new command to the menu\n");
        printf("   c. change directory : Changes process working directory\n");
        printf("   e. exit : Leave Mid-Day Commander\n");
        printf("   p. pwd : Prints working directory\n");
        printf("Option?:");
        //allocate space for input
        char* p = (char*) malloc(sizeof(char*));
        scanf("%s", p); //scan input
        OptionHandler(p);//hadle the input
        free(p); //free the memory allocated by the input
    }

}

