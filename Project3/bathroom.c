//Bathroom.c by Daniel McDonough 2/7/2018
#include <stdio.h>
#include "bathroom.h"

//update stats functions
void updateOverallmin(Thebathroom *YOSHISCOOKIE, long qtime) {
    if (YOSHISCOOKIE->Overall_min_q_time > qtime) {
        YOSHISCOOKIE->Overall_min_q_time = qtime;

    }

}
//sumofq
void sumQtime(Thebathroom *YOSHISCOOKIE, int qtime) {

    YOSHISCOOKIE->Overall_total_q_time += qtime;
}
//updatemaxtime
void updateOverallmax(Thebathroom *YOSHISCOOKIE, int qtime) {
    if (YOSHISCOOKIE->Overall_max_q_time < qtime) {
        YOSHISCOOKIE->Overall_max_q_time = qtime;

    }


}


//initialize bathroom
void Initialize(Thebathroom *YOSHISCOOKIE) {
    // initialize all variables in the bathroom

    pthread_mutex_init(&(YOSHISCOOKIE->mutex), NULL);
    pthread_cond_init(&(YOSHISCOOKIE->condition), NULL);
    YOSHISCOOKIE->state = 0; //state unoccupied = 0; occupied = 1
    YOSHISCOOKIE->current_INTHEBATHROOM = 0; //Ya know, "IN THE BATHROOM" ~ NCS/Weird AL. https://www.youtube.com/watch?v=JsCLnMrWbsw
    YOSHISCOOKIE->current_waiting = 0; //current num people in q
    YOSHISCOOKIE->Overall_total_inQ = 0; //number of people who were in the q through the sim
    YOSHISCOOKIE->Overall_total_q_time = 0; //overall total q time (sum of all people's q time)
    YOSHISCOOKIE->Overall_min_q_time = 0; //overall min q time of all people
    YOSHISCOOKIE->Overall_max_q_time = 0; //overall max q time of all people
    YOSHISCOOKIE->Overall_avg_q_time = 0; //Overall_total_q_time/Overall_total_inQ = overall avg q time
    YOSHISCOOKIE->total_uses = 0; //total uses the bathroom has gone through
    YOSHISCOOKIE->Overall_avg_users = 0;

    //get time for the start time
    gettimeofday(&(YOSHISCOOKIE->start), NULL);

}


void Enter(Thebathroom *YOSHISCOOKIE, gender g) {

//printf("IN THE BATHROOM ~ NCS\n\n");
    pthread_mutex_lock(&(YOSHISCOOKIE->mutex));
    if (YOSHISCOOKIE->state == 0) {    //if unoccupied

//change state of bathroom
        if (g == male) {
            YOSHISCOOKIE->state = 1;
        } else {
            YOSHISCOOKIE->state = 2;
        }
        YOSHISCOOKIE->current_INTHEBATHROOM++; //increment num users in bathroom
    } else if ((YOSHISCOOKIE->state == 1 && g == male) ||
               (YOSHISCOOKIE->state == 2 && g == female)) { //bathroom gender is same as user gender

        YOSHISCOOKIE->current_INTHEBATHROOM++; //increment num users in bathroom
    } else {
        YOSHISCOOKIE->current_waiting++; //increment wait queue;

        YOSHISCOOKIE->Overall_total_inQ++; //increment total waitq



        pthread_cond_wait(&(YOSHISCOOKIE->condition), &(YOSHISCOOKIE->mutex)); //wait for condition
        //swap gender
        if (YOSHISCOOKIE->state == 1) {
            YOSHISCOOKIE->state = 2;
        } else {
            YOSHISCOOKIE->state = 1;
        }

        YOSHISCOOKIE->current_INTHEBATHROOM++; //increment num users in bathroom

    }
//printf("oh k!\n\n");
    pthread_mutex_unlock(&(YOSHISCOOKIE->mutex));
//printf("oh k!\n\n");
}

//on leave
void Leave(Thebathroom *YOSHISCOOKIE) {
    //lock
    pthread_mutex_lock(&(YOSHISCOOKIE->mutex));

    //remove person from bathroom
    YOSHISCOOKIE->current_INTHEBATHROOM--;

    if (YOSHISCOOKIE->current_INTHEBATHROOM <= 0) {

        YOSHISCOOKIE->state = 0; //unoccupied

        pthread_cond_broadcast(&(YOSHISCOOKIE->condition)); //bathroom is open

        YOSHISCOOKIE->current_waiting = 0; //reset wait current queue
    }
    YOSHISCOOKIE->total_uses++;
    //unlock
    pthread_mutex_unlock(&(YOSHISCOOKIE->mutex));
}

//FINALIZE
void Finalize(Thebathroom *YOSHISCOOKIE) {
    //find time at end of program
    gettimeofday(&(YOSHISCOOKIE->end), NULL); // get the Finalize time

    //find differece of start and end time of program
    YOSHISCOOKIE->timediff = getDiffTime(&(YOSHISCOOKIE->start), &(YOSHISCOOKIE->end));

    //calc avg q time
    YOSHISCOOKIE->Overall_avg_q_time =
            (float) YOSHISCOOKIE->Overall_total_q_time / (float) YOSHISCOOKIE->Overall_total_inQ;
    //calc avg users per milisecond
    YOSHISCOOKIE->Overall_avg_users = (float) YOSHISCOOKIE->total_uses / (float) YOSHISCOOKIE->timediff;

    //remove cancer
    pthread_mutex_destroy(&(YOSHISCOOKIE->mutex));
    pthread_cond_destroy(&(YOSHISCOOKIE->condition));

    //print stats
    printf("Bathroom Statistics:\n");
    printf("Total Time of Simulation: %d ms\n", YOSHISCOOKIE->timediff);
    printf("Total uses: %d\n", YOSHISCOOKIE->total_uses);
    printf("Total times people were in the Queue:: %d ms\n", YOSHISCOOKIE->Overall_total_inQ);
    printf("Total Time spent in Queue:: %d ms\n", YOSHISCOOKIE->Overall_total_q_time);
    printf("Minimum Time spent in Queue: %d ms\n", YOSHISCOOKIE->Overall_min_q_time);
    printf("Maximum Time spent in Queue: %d ms\n", YOSHISCOOKIE->Overall_max_q_time);
    printf("Avg Time spent in Queue: %f ms\n", YOSHISCOOKIE->Overall_avg_q_time);
    printf("Avg Users in the Bathroom at any time: %f \n\n", YOSHISCOOKIE->Overall_avg_users);

}

//get difference of two time structs
long getDiffTime(struct timeval *start, struct timeval *end) {
    unsigned long long diff = 0;
    unsigned long long sec = 0;
    unsigned long long ms = 0;

    sec = end->tv_sec - start->tv_sec; //get number of seconds
    diff += sec * 1000; //convert to milliseconds

    if (start->tv_usec > end->tv_usec) { 
        ms = start->tv_usec - end->tv_usec; //microseconds
        diff += ms / 1000; //convert to milliseconds
    } else {
        ms = end->tv_usec - start->tv_usec;
        diff += ms / 1000; //conver to milliseconds
    }

    return diff;
}

/*...get the toilet scrub brush,
Use some Woolite® to get it out,
Pour some bleach when you're finished,
To eliminate odors.

You don't want your bathroom smelling,
Like piss and shit mixed together,
Because when it happens like that,
No one will want to come-

Over to your house,
And visit you,
Because the stench of poop,
Is so very-

Oh, I fell through the floor it stunk so bad; I fell through the floor.

Take a dump on the toilet,
Flush it when you are finished up,
I know that I just said that,
But it is still funny.
 */
