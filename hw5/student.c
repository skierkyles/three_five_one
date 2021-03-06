/**
 * General structure of a student.
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "ta.h"

void *student_loop(void *param)
{
	int number = *((int *)param);
	int sleep_time;

	printf("I am student %d programming on my own\n", number);

	srandom((unsigned)time(NULL));
	sleep_time = (int)((random() % MAX_SLEEP_TIME) + 1);
	programming(sleep_time);
	
	bool taken = take_chair();
	if (taken == true) {
		printf("Student %d has a chair waiting for help\n", number);
		
		// Wait is effectively notifying the TA that a 
		// student needs help.
		sem_wait(&notify_student);

		// Give the chair up.
		leave_chair();
		printf("Student %d is getting helped\n", number);

		// This thread sleeps for the same time as the TA thread.
		sleep(1);		
	}

	student_loop((void *)&number);

	return NULL;
}
