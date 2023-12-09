////////////////////////////////////////////////////////////////////////////////
// Main File:        send_signal.c
// This File:        send_signal.c
// Other Files:      my_c_signal_handler.c
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
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/* Checks for correct args and formats
 * Sends signals SIGINT or SIGUSR1 to a valid pid
 * 
 * argc - number of args
 * *argv[] - cstring of args
 */
int main (int argc, char *argv[]) {
	//correct number of argc
	if (argc != 3) {
		printf("Usage: <signal type> <pid>\n");
		exit(0);
	}
	
	//checking the signal and format
	char * signal_cmd = argv[1];
	char signal = signal_cmd[1];

	//THIS CHECK IS NOT IN WRITEUP
	//CHECK FOR '-' IN FRONT OF SIGNAL
	if (signal_cmd[0] != '-') {
		printf("<signal type> is -<signal>\n");
		exit(0);
	}	

	if (signal != 'u' && signal != 'i') {
		printf("Usage: <signal type> <pid>\n");
		exit(0);
	} 
	
	//checking for valid pid	
	char* char_pid = argv[2];
	
	if (atoi(char_pid) == 0) {
		printf("Usage: <signal type> <pid>\n");
		exit(0);
	}

	int pid = atoi(char_pid);
	
	//checking which signal to send
	if (signal == 'u') {
		if (kill(pid, SIGUSR1) <0) {
			printf("SIGUSR1 failed to send\n");
			exit(0);
		}
	}
	
	if (signal == 'i') {
		if (kill(pid, SIGINT) < 0) {
			printf("SIGINT failed to send\n");
			exit(0);
		}
	}

	return 0;
}
