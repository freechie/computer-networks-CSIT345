#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define PROCESS_NUM 3

struct country
{
	char *countryName;
	char *athlete;
	int bib;
};


int main (int argc, char *argv[])
{	
	int index, i, result = 1;
	char *countriesArr[] = {"usa", "cuba", "colombia", "mexico", "china", "vietnam", "japan", "bhutan", "nepal", "tibet", "pakistan", "italy", "canada"};
	int parentpid, country1pid, country2pid, country3pid;
	char asciiparentpid[100];
	int wstatus;

	int countries[PROCESS_NUM];
	int pipes[PROCESS_NUM + 1][2];

	// Competing Countries Struct instance
	struct country compCountry1;
	struct country compCountry2;
	struct country compCountry3;

	// assigning values from argv to countryName variable in struct
	char *country1 = argv[1];
	char *country2 = argv[2];
	char *country3 = argv[3];

	// assigning value of country1,2,3 to char countryName
	compCountry1.countryName = country1;	
	compCountry2.countryName = country2;
	compCountry3.countryName = country3;
	
	// Assigning int value to bib
	compCountry1.bib = 1;
	compCountry2.bib = 2;
	compCountry3.bib = 3;
	
	char *country1Athlete = "Phelps";
	char *country2Athlete = "Savon";
	char *country3Athlete = "Molina";
	
	// Assigning int value to bib
	compCountry1.athlete = country1Athlete;
	compCountry2.athlete = country2Athlete;
	compCountry3.athlete = country3Athlete;;
	
	// to clear prompt using command call
	char command[128];

	// parsing through command line arguments
	for(index=1;index<argc;index++){
		
		// verify 3 inputs
		if(argc<4){ // less than 3 inputs is too little (while acknowledging argv[0])
			printf("Please enter at least 3 countries. Exiting...\n");
			break;
		} else if(argc > 4) { // greater than 4 inputs is too much
			printf("Please enter only 3 countries. Exiting...\n");
			break;
		} else {
			// Print Competing Countries
			printf("Racer: %s", compCountry1.athlete);
			printf("\nBib Number %d\nCountry: %s\n", compCountry1.bib, compCountry1.countryName);
			printf("\nRacer: %s", compCountry2.athlete);
			printf("\nBib Number %d\nCountry: %s\n", compCountry2.bib, compCountry2.countryName);
			printf("\nRacer: %s", compCountry3.athlete);
			printf("\nBib Number %d\nCountry: %s\n", compCountry3.bib, compCountry3.countryName);
		}	
		// iterate through countries entered
		for(i=1;i<13;i++){
			// compares input with country if match = 0
			result = strcmp(argv[index],countriesArr[i]);
			if(result == 0) {
				break;
			}
		}
		// verifies that result isn't match. if no match, break out of loop
		if(result !=0){
			break;
		}
		
		// Copy names of countries to struct instances
		strcpy(compCountry1.countryName, country1);
		strcpy(compCountry2.countryName, country2);
		strcpy(compCountry3.countryName, country3);
	}


	// clear prompt for race
	sleep(4);	
	strcpy(command,"clear");
	system(command);	

	// Starting program...
	printf("On your Marks...\n\n");
	sleep(1);
	printf("Get Set...\n\n");
	sleep(1);
	printf("Go!!!!!\n\n");	
	sleep(2);
	
	// get main process ID
	parentpid = getpid();
	// assign parent pid to array of chars to grep
	// show ps -ef grep verbose command & run it via sys call
	sprintf(asciiparentpid,"%d", parentpid);
	strcpy(command, "ps -ef|grep ");
	strcat(command,asciiparentpid);
	strcat(command,"|grep -v grep");
	printf("%s\n\n", command);
	system(command);

	// error handling for pipes 
	for(i = 0; i < PROCESS_NUM +1; i++){
		if (pipe(pipes[i]) == -1){
			printf("Error with creating pipe.\n");
			return 1;
		}
	}
	
	// creating forks via PROCESS_NUM
	for (i = 0;i < PROCESS_NUM; i++){
		countries[i] = fork();		
		if (countries[i] == -1){
			printf("Error with creating process\n");
			return 2;
		}
		
		if(countries[i] == 0){
			// child process
			int j;
			for (j = 0; j < PROCESS_NUM; j++){
				if (i != j){
					close(pipes[j][0]);
				}
				if (i + 1 != j) {
					close(pipes[j][1]);
				}
			}
			// error handling for reading in from pipes
			int processNum;
			if (read(pipes[i][0], &processNum, sizeof(int)) == -1){
				printf("Error at reading\n");
				return 3;
			}
			
			printf("(%d) Got ID  %d\n", i, processNum);
			processNum++;
			if(write(pipes[i+1][1], &processNum, sizeof(int)) == -1){
				printf("Error at writing\n");
				return 4;
			}
			printf("(%d) Sent %d\n", i, processNum);
			close(pipes[i][0]);
			close(pipes[i+1][1]);
			return 0;
		}

	}

	// Main process
	int j;
	for(j = 0; j < PROCESS_NUM + 1; j++){
		if(j !=PROCESS_NUM){
			close(pipes[j][0]);
		}
		if(j != 0){
			close(pipes[j][1]);
		}
	}
	
	printf("\nMain process sent ID #%d\n\n", parentpid);
	
	if(write(pipes[0][1], &parentpid, sizeof(int)) == -1){
		printf("Error at writing\n");
		return 4;
	}
	
	if(read(pipes[PROCESS_NUM][0],&parentpid, sizeof(int)) == -1){
		printf("Error at reading\n");
		return 3;
	}
	printf("\nProcesses Running:\n");
	system(command);
	
	execvp("./racer", argv);
	
	close(pipes[0][1]);
	close(pipes[PROCESS_NUM][0]);
	// wait for processes to terminate
	for(i=0; i < PROCESS_NUM; i++){
		wait(NULL);
	}
	// process check with print function	
	printf("\nProcesses now Running:\n");
	system(command);
	printf("\nWhat a great race!\n\n");

	
	return 0;
}
