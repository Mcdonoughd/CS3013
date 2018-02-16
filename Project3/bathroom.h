
//Bathroom Simulator by Daniel McDonough 2/7/18

//Bathroom.h by Daniel McDonough 2/14/2018 
#ifndef BATHROOM_H //include once
#define BATHROOM_H

#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

//enumeration of gender
typedef enum {
    male, female
} gender;

//the bathroom struct
typedef struct {
    pthread_mutex_t mutex; //the mutex
    pthread_cond_t condition; //the condition

    int state; //state unoccupied = 0; 1=male 2=female
    int current_INTHEBATHROOM; //Ya know, "IN THE BATHROOM" ~ NCS/Weird AL.
    int current_waiting; //current num people in q
    int Overall_total_inQ; //number of people who were in the q through the sim
    int Overall_total_q_time; //overall total q time (sum of all people's q time)
    int Overall_min_q_time; //overall min q time of all people
    int Overall_max_q_time; //overall max q time of all people
    float Overall_avg_q_time; //Overall_total_q_time/Overall_total_inQ = overall avg q time
    int total_uses; //total uses the bathroom has gone through
    struct timeval start; //the time the start of init
    struct timeval end; //the time the finalize
    int timediff; //dif between start and end
    float Overall_avg_users; //total_uses/(end-start) = avg users through simulation
} Thebathroom;
//end bathroom struct

//enter
void Enter(Thebathroom *Thebathroom, gender g);

//leave
void Leave(Thebathroom *Thebathroom);

//init
void Initialize(Thebathroom *Thebathroom);

//fin
void Finalize(Thebathroom *Thebathroom);

//get difference of time between two timeval structs
long getDiffTime(struct timeval *start, struct timeval *end);

//input min qtime stats
void updateOverallmin(Thebathroom *YOSHISCOOKIE, long qtime);

//update max qtime stats
void updateOverallmax(Thebathroom *YOSHISCOOKIE, int qtime);

//input sum qtime stat
void sumQtime(Thebathroom *YOSHISCOOKIE, int qtime);



/*
Take a dump on the toilet,
Take a dump on the toilet,
It smells bad!

Take a dump on the toilet,
It smells so bad!

Take a dump on the toilet,
It smells just like,
Rotten fish and cheese,
Rotten fish and cheese,
PU-EY!" */

#endif
