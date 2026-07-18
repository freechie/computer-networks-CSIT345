#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define LIMIT 3

int main(int argc, char *argv[]) 
{
	printf("Starting Racers...\n\n");
	sleep(2);
	printf("%s,%s,%s\n\n",argv[1],argv[2],argv[3]);
	int waiting=atoi(argv[3]);
	int i;

	srand((long) time(NULL));
	for(i = 1; i <= 3; i++){
		printf("There GOES %s \n", argv[i]);
		sleep(1);
	}
	printf("The racer from %s FINISHES!!!\n ", argv[3]);

	printf("\nRacer file  process ID is %i\n", getpid());
	sleep(waiting);
	exit(0);
}
