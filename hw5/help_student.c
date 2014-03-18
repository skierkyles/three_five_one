/**
 * Simulate helping a student
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ta.h"

void help_student() 
{
	// srandom((unsigned)time(NULL));
	// int sleep_time = (int)((random() % MAX_SLEEP_TIME) + 1);

	int sleep_time = 1;

	printf("Helping a student for %d seconds\n", sleep_time);
	sleep(sleep_time);
}
