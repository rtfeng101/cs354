////////////////////////////////////////////////////////////////////////////////
// Main File:        my_c_signal_handler.c
// This File:        my_c_signal_handler.c
// Other Files:      send_signal.c
// Semester:         CS 354 Lecture 001 Fall 2023
// Grade Group:      gg15  (See canvas.wisc.edu/groups for your gg#)
// Instructor:       deppeler
// 
// Author:           Richard Feng
// Email:            rtfeng@wisc.edu
// CS Login:         Richard
//
///////////////////////////  WORK LOG  //////////////////////////////
//  Document your work sessions on your copy http://tiny.cc/work-log
//  Download and submit a pdf of your work log for each project.
/////////////////////////// OTHER SOURCES OF HELP ////////////////////////////// 
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
// 
// AI chats:         save a transcript and submit with project.
//////////////////////////// 80 columns wide ///////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int seconds = 5; //alrm buffer
int usr_count = 0; //usr input count

/*SIGALRM handler displays the pid and time
 */
void handler_SIGALRM() {
	//use time
	time_t curr_time;
	//check for valid time
	if (time(&curr_time) == -1) {
		printf("Error time\n");
		exit(1);
	}
	
	printf("PID: %i CURRENT TIME: %s", getpid(), ctime(&curr_time));
	alarm(seconds); //rearm
}

/*SIGUSR! handler counts user input
 */
void handler_SIGUSR1() {
	usr_count++;
	printf("SIGUSR1 handled and counted!\n");
}

/*SIGINT handler displays count of user input and exits
 */
void handler_SIGINT() {
	printf("\nSINGINT handled.\n");
	//extra \n for correct spacing in writeup
	printf("SIGUSR1 was handled %i times. Exiting now.\n", usr_count);
	exit(0);
}


/* Runs the alarm, user input counter, and sigint and
 * creates handlers.
 * Enters infinite loop
 */
int main() {
	printf("PID and time print every 5 seconds.\n");
	printf("Enter ^C to end the program.\n");
	
	//sets up SIGLARM
	alarm(seconds);

	//alarm handler
	struct sigaction sa;
	memset (&sa, 0, sizeof(sa));
	sa.sa_handler = handler_SIGALRM;
	if (sigaction(SIGALRM, &sa, NULL) != 0) {
		printf("Error building SIGALRM handler.\n");
		exit(1);
	}

	//sigusr1 handler
	struct sigaction su;
	memset (&su, 0, sizeof(su));
	su.sa_handler = handler_SIGUSR1;
	if (sigaction(SIGUSR1, &su, NULL) != 0) {
		printf("Error building SIGUSR1 handler.\n");
		exit(1);
	}

	//sigint handler
	struct sigaction si;
	memset (&si, 0, sizeof(si));
	si.sa_handler = handler_SIGINT;
	if (sigaction(SIGINT, &si, NULL) != 0) {
		printf("Error building SIGINT handler.\n");
		exit(1);
	}
	
	//enter infinite loop
	while (1) {
	}
	return 0;
}
