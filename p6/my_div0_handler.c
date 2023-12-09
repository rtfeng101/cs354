////////////////////////////////////////////////////////////////////////////////
// Main File:        my_div0_handler.c
// This File:        my_div0_handler.c
// Other Files:      
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


#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int valid = 0; //counts number of valid divisions

/*SIGINT handler prints error message and displays valid divisions
 */
void handler_SIGFPE() {
	printf("Error: a division by 0 operation was attempted.\n");
	printf("Total number of operations completed successfully: %i\n", valid);
	printf("The program will be terminated.\n");
	exit(0);
}

/*SIGINT handler displays valid divisions and exits
 */
void handler_SIGINT() {
	printf("\nTotal number of operations completed successfully: %i\n", valid);
	//extra \n to match formatting of write up
	printf("The program will be terminated.\n");
	exit(0);
}

/* Creates handlers and prompts user for 2 numbers to divide by
 * returning the answer and remainder.
 */
int main () {
	//SIGFPE handler
	struct sigaction sf;
	memset (&sf, 0, sizeof(sf));
	sf.sa_handler = handler_SIGFPE;
	if (sigaction(SIGFPE, &sf, NULL) != 0) {
		printf("Error building SIGFPE handler\n");
		exit(1);
	}

	//SIGINT handler
	struct sigaction si;
	memset (&si, 0, sizeof(si));
	si.sa_handler = handler_SIGINT;
	if (sigaction(SIGINT, &si, NULL) != 0) {
		printf("Error building SIGINT handler\n");
		exit(1);
	}

	//enter infinite loop	
	while (1) {
		int first; //first usr input
		int second; //second usr input

		//prompt user and get first input
		printf("Enter first integer: ");
		char buffer [100];
		fgets(buffer, 100, stdin);
		first = atoi(buffer);

		//prompt user and get second input
		printf("Enter second integer: ");
		fgets(buffer, 100, stdin);
		second = atoi(buffer);

		//do division
		int division = first/second;
		int remainder = first%second;
		
		//print out and increment valid divisions
		printf("%i / %i is %i with a remainder of %i\n", 
			first, second, division, remainder);
		valid++;
	}

	return 0;
}	
