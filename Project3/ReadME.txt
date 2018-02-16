Bathroom Simulator by Daniel McDonough 2/14/2018

This simulator is ment to simulate a communal bathroom in which only one gender can use it at a time. This is a program that takes in the number of users to be simulated, the mean time they arrive, the average time they take in the bathroom and the average number of times the same person uses the bathroom.
It then gives each user a random staytime, arrivaltime and useagetime to simulate a normal distrobution.
The program then keeps tracks and produces the following stats per person: 
 	person gender
	Number of bathroom trips
	min queue time
	max queue time
	Avg queue time
	times waited in the queue
	Total time spent in the queue

Following stats of the bathroom as a whole:
	Total simulation time
	total times the bathroom wwas used	
	total occurences people has to wait in a queue
	total time spent in the queue
	min time in queue
	max time in queue
	avg time in queue
	avg people in the bathroom

How to run:
	In this directory:
		make
		./bathroomSim nUsers meanLoopCount meanArrival meanStay

		where no argument is negative or over 1000 or not a number

How the program runs (& Invariants):

Upon running the program Two structs are created: Thebathroom and person_info
Thebathroom:
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


person_info:
    int pid; //person id
    gender mygender; //person gender

The program then creates all users as threads and generates a random loopcount, arrival stay time to fin in the given norm distro and then genders to that it is 50/50 random. until 

The threads all for their arrivaltime the bathroom's gender is swapped to the first person to enter while the bathroom is enter and stays that gender until exit. Upon each attempted entry of a person the gender is checked. If the gender is not the current bathrooms gender tey are put in a pool of people who are waiting and the inQ and currently waiting feilds are incrmented. upon entry, the time the number of poeple INTHEBATHROOM and total uses and stay there for the given stay duration. When the bathroom swaps genders, all who where in the queue enter as the bathroom seems to have unlimited toilets. Where the queue is set to 0 and totalq time is sumed between the bathroom for all people and the max and min que times are checked for new more accurate values. Upon completetion, the bathroom calculates the time it look for the simulation to finish, by subtraction the start and end times. It also calculates the avg q time between all of those whom where in the queue and the avg number of users over the given time of the program.

Therefore, the invarents of the program are:

person ID
person gender (once calculated)
person loopcount
person staytime (although it changes per loopcount)
person arrivetime (although it changes per loopcount)
bathroom starttime
bathroom endtime
bathroom timediff (calculated at the end)
bathroom avgusers (calulated at the end)
bathroom argquetime (calculated at the end)
















