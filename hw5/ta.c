/**
 * General structure of the teaching assistant.
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "ta.h"

void *ta_loop(void *param)
{
	printf("I am the TA\n");


	for (;;) 
	{
		// The TA is sleeping
		sem_wait(&notify_ta);

		sem_post(&notify_student);
		help_student();

	}
}


bool take_chair() 
{

	pthread_mutex_lock(&chair_mutex);
	if (available_chairs > 0) {
		sem_post(&notify_ta);
		available_chairs--;
		pthread_mutex_unlock(&chair_mutex);

		return true;
	}
	pthread_mutex_unlock(&chair_mutex);
	return false;

}

void leave_chair() 
{
	pthread_mutex_lock(&chair_mutex);
	available_chairs++;
	pthread_mutex_unlock(&chair_mutex);
}