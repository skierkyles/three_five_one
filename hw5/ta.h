/**
 * Header file for dining philosophers
 */

#include <pthread.h>
#include <semaphore.h>

typedef enum { false, true } bool;

// the maximum time (in seconds) to sleep
#define MAX_SLEEP_TIME	5

// number of maximum waiting students
#define MAX_WAITING_STUDENTS	3

// number of potential students
#define NUM_OF_STUDENTS		5

// number of available seats
#define NUM_OF_SEATS	3

// the numeric id of each student
int student_id[NUM_OF_STUDENTS];

// student function prototype
void *student_loop(void *param);

// ta function prototype
void *ta_loop(void *param);

// simulate programming
void programming(int sleeptime);

// simulate helping a student
void help_student();

// why don't you have a seat please
bool take_chair();
void leave_chair();

// semaphores
extern sem_t notify_ta;
extern sem_t notify_student;

// mutex
extern pthread_mutex_t chair_mutex;

extern int available_chairs;