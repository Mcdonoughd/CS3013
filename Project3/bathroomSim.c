//Bathroom Simulator by Daniel McDonough 2/7/18

#include <math.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "bathroom.h"

typedef struct {
    int pid; //person id
    gender mygender; //person gender

/* 
So apparently you cant put all of this in here and it belongs in individual function~ Lauer

Why idk.
int loopcount; //number of bathroom trips a person takes
long inQueue; //times person was in the queue
long minQueuetime; //min wait time //long because int overflow is possible
long maxQueuetime; //max wait time
long totalQueuetime; //total wait time
long avgQueuetime; //totalQueuetime/inQueue = avg wait time
int staytime; //time it takes to poo
int arrivaltime; //time the person gets arrives in the queue
*/
} person_info;


Thebathroom YOSHISCOOKIE; //BURP: YOSHI'S COOKIE
pthread_mutex_t printing; //mutex for several lines of printing

int meanLoopCount;
int meanArrival;
int meanStay;

//generate random number from a given mean.
int genNormDistro(int mean) {
    float a = drand48();
    float b = drand48();
    float z = sqrt(-2 * log(a)) * cos(2 * M_PI * b); //box-muller equation
    float num = ((mean / 2) * z) + mean; //random result within our mean
    if (num < 1) { //check if rand result makes sense
        num = 1;
    }
    if (num > 1000) { //hold on there space cowboy...
        num = 1000;
    }
    return (int) floor(num); //turn float to int and round down
}

void *Individual(void *info) {
    person_info *Stranger = (person_info *) info;
    unsigned long minQueuetime = 999999999; //minimum time spent in queue
    long avgQueuetime = 0; //average time spent in queue
    long maxQueuetime = 0; //maximum time spent in queue
    long totalQueuetime = 0; //total time spent in queue
    int inQueue = 0; //total number of times waited in queue

    struct timeval tryEntercall; //finds the time person trys to enter
    struct timeval Entercall; //finds the time person actually entered


    int loopcount = genNormDistro(meanLoopCount);


    for (int i = 0; i < loopcount; i++) {
        //printf("Stranger %d is on trip number %d\n",Stranger->pid,i++);

        int arrivaltime = genNormDistro(meanArrival);
        int staytime = genNormDistro(meanStay);

        /* wait for a random length of time
           based on arrival_time */

        usleep(arrivaltime * 1000);

        gettimeofday(&tryEntercall, NULL); //get time before
        Enter(&YOSHISCOOKIE, Stranger->mygender);
        gettimeofday(&Entercall, NULL); //get time after

        /* wait for a random length of time
           based on stay time */
        usleep(staytime * 1000);

        Leave(&YOSHISCOOKIE);

        //do some time calculations
        long diff = getDiffTime(&tryEntercall, &Entercall);

        if (diff) {
            if (diff > maxQueuetime) {
                maxQueuetime = diff; //update person max
                updateOverallmax(&YOSHISCOOKIE, diff); //update bathroom max
            }

            if (diff < minQueuetime) {
                minQueuetime = diff; //update person min
                updateOverallmin(&YOSHISCOOKIE, diff); //update bathroom min
            }

            inQueue++;
            totalQueuetime += diff; //update person sum
            sumQtime(&YOSHISCOOKIE, diff); //update bathroom sum
        }
    }

    // get average from the total / the loop count if they were in a q
    if (inQueue) {
        avgQueuetime = totalQueuetime / inQueue;
    }

    //change value for printing
    if (minQueuetime == 999999999) {
        minQueuetime = 0;
    }

   

    pthread_mutex_lock(&printing); //lock so only this thread prints

    printf("\nPerson: %d has Finished.\n", Stranger->pid);

    if (Stranger->mygender == 0) {
        printf("Gender: Male\n");
    } else {
        printf("Gender: Female\n");
    }

    printf("Bathroom Trips: %d\n", loopcount);
    printf("Min queue time: %ld ms\n", minQueuetime);
    printf("Max queue time: %ld ms\n", maxQueuetime);
    printf("Avg queue time: %ld ms\n", avgQueuetime);
    printf("Times in queue: %d\n", inQueue);
    printf("Total queue time: %ld ms\n\n\n", totalQueuetime);


    //Upload stats to bathroom
    //YOSHISCOOKIE->total_uses += loopcount; need to use a getter



    pthread_mutex_unlock(&printing); 
    //unlock

    pthread_exit(NULL); //kill thread
}


//THE ALMIGHTY MAIN FUNCTION
int main(int argc, char *argv[]) {

    if (argc == 5) { //check valid # of input
//check for valid input 
//check greater that 1000
        if (atoi(argv[1]) > 1000 || atoi(argv[2]) > 1000 || atoi(argv[3]) > 1000 || atoi(argv[4]) > 1000) {
            printf("\nHold on there space cowboy...\n");
            printf("You're gonna clog the plumming!\n");
            printf("Please enter values less than 1000.\n\n");
            exit(1);
        }
//check less than 1
        if (atoi(argv[1]) < 1 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1 || atoi(argv[4]) < 1) {
            printf("\nHold on there space cowboy...\n");
            printf("You're gonna break space-time!\n");
            printf("Please enter values 1 or greater.\n\n");
            exit(1);
        }


        int nUsers = atoi(argv[1]); //# of users
        meanLoopCount = atoi(argv[2]); //mean loop count
        meanArrival = atoi(argv[3]); //mean arrival time
        meanStay = atoi(argv[4]); //mean stay


        printf("\nValid Input recognized!\n");
        printf("Number of Users:%d\n", nUsers);
        printf("Mean Loop Count:%d\n", meanLoopCount);
        printf("Mean Arrival:%d\n", meanArrival);
        printf("Mean Stay:%d\n\n", meanStay);
//VALID INPUT!


        pthread_mutex_init(&printing, NULL); //init print mutex

        pthread_t People[nUsers]; //produce thread of size users
        Initialize(&YOSHISCOOKIE); //init the bathroom

//generate nuser threads
        for (int i = 0; i < nUsers; i++) {
            person_info *person = malloc(sizeof(person_info *));
            person->pid = i; //thread number of the thread

            double determine_gender = drand48(); //determine gender
            if (determine_gender < .5) { //get 50/50 split
                person->mygender = male;
            } else {
                person->mygender = female;
            }


            pthread_create(&People[i], NULL, Individual, (void *) person);
            //individual cant pass avgs so they must be globals
        }
//printf("HELLO!\n\n");
        for (int i = 0; i < nUsers; ++i) {

            pthread_join(People[i], NULL);

        }
//printf("HELLO!\n\n");
        Finalize(&YOSHISCOOKIE);
//return 0;
    } else { //BAD # of args
        printf("\nStupoid, please use the program like so:");
        printf("\n\t./bathroomSim nUsers meanLoopCount meanArrival meanStay\n\n");
        exit(1);
    }


    return 0;
}
