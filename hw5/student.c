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

	printf("I am student %d\n", number);

	srandom((unsigned)time(NULL));
	sleep_time = (int)((random() % MAX_SLEEP_TIME) + 1);
	programming(sleep_time);
	
	return NULL;
}
