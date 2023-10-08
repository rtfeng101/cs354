///////////////////////////////////////////////////////////////////////////////
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Used by permission, CS 354 Spring 2022, Deb Deppeler
////////////////////////////////////////////////////////////////////////////////
   
////////////////////////////////////////////////////////////////////////////////
// Main File:        my_magic_square.c
// This File:        my_magic_square.c
// Other Files:      
// Semester:         CS 354 Lecture 001 Fall 2023
// Instructor:       deppeler
// 
// Author:           Richard Feng
// Email:            rtfeng@wisc.edu
// CS Login:         richardf
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

// Structure that represents a magic square
typedef struct {
    int size;           // dimension of the square
    int **magic_square; // pointer to heap allocated magic square
} MagicSquare;

/* TODO:
 * Promps the user for the magic square's size, read it,
 * check if it's an odd number >= 3 (if not, display the required
 * error message and exit)
 *
 * return the valid number
 */
int getSize() {
	int num;

	printf("Enter magic square's size (odd integer >= 3)\n");
	scanf("%d", &num); //write user input into num
	
	//conditions
	if ((num % 2) == 0) {
		printf("Magic square size must be odd.\n");
		exit(1);
	}
	if (num < 3) {
		printf("Magic square size must be >= 3.\n");
		exit(1);
	}

    return num;   
} 
   
/* TODO:
 * Makes a magic square of size n,
 * and stores it in a MagicSquare (on the heap)
.*
 * It may use the Siamese magic square algorithm 
 * or alternate from assignment 
 * or another valid algorithm that produces a magic square.
 *
 * n - the number of rows and columns
 *
 * returns a pointer to the completed MagicSquare struct.
 */
MagicSquare *generateMagicSquare(int n) {
	//create magic square
	MagicSquare* square = malloc(sizeof(MagicSquare));
	if (square == NULL) {
		perror("No memory avaliable to make magic square.");
		exit(1);
	}
	//assign values
	square -> size = n;	
	//matrix
	square -> magic_square = malloc(sizeof(int *) * n);
	if (square -> magic_square == NULL) {
		perror("No memory avaliable to make magic square.");
		exit(1);
	}

	for (int i = 0; i < n; i++) {
		*(square -> magic_square + i) = malloc(sizeof(int) * n);
		if (*(square -> magic_square + i) == NULL) {
			perror("No memory avaliable to make magic square.");
			exit(1);
		}

	}

	//Siamese Algo
	int i = 0; //starting row
	int j = n/2; //starting col

	//there are n*n numbers, fill from 1-n*n in each cell
	for (int num = 1; num <= n * n; num++) {
		*(*(square -> magic_square + i) + j) = num;
		//calculate the next indecies
		i--; //go up 1 row
		j++; //go to right 1

		//if the space is filled (multiple of the size due to diagonal length),
		//move to the next open space, down 2 and left 1 (because we have incremented once already)
		if (num % n == 0) {
			i += 2;
			--j;
			
		}
		//bounds wrapping
		else {
			if (j == n)
				j -= n;
			else if (i < 0)
				i += n;
		}
	}

	return square;    
} 

/* TODO:  
 * Opens a new file (or overwrites the existing file)
 * and writes the magic square values to the file
 * in the specified format.
 *
 * magic_square - the magic square to write to a file
 * filename - the name of the output file
 */
void fileOutputMagicSquare(MagicSquare *magic_square, char *filename) {
	//open the file into write mode
	FILE *fptr = fopen(filename, "w");
	if (fptr == NULL) {
		perror("Can't load file.");
		exit(1);
	}

	//add the size into the file
	fprintf(fptr, "%i\n", magic_square -> size);

	//move through matrix
	for(int i = 0; i < magic_square -> size; i++) {
		//go through all columns except for last
		int j = 0;
		for ( ; j < magic_square -> size - 1; j++) {
			fprintf(fptr, "%i,", *(*(magic_square -> magic_square +i) + j));
		}
		//add last elt in the row without comma and newline
		fprintf(fptr, "%i\n", *(*(magic_square -> magic_square +i) + j++));
	}
	//close the file
	if (fclose(fptr) != 0) {
		perror("File close error.");
		exit(1);
	}
}


/* TODO:
 * Generates a magic square of the user specified size and
 * outputs the square to the output filename.
 * 
 * Add description of required CLAs here
 * Needs 2 CLAs: executable file name and file to write to
 */
int main(int argc, char **argv) {
    // TODO: Check input arguments to get output filename
	if (argc != 2) {
		printf("Usage: ./my_magic_square<output_filename>\n");
		exit(1);
	}
    // TODO: Get magic square's size from user
	int size = getSize();	

    // TODO: Generate the magic square by correctly interpreting 
    //       the algorithm(s) in the write-up or by writing or your own.  
    //       You must confirm that your program produces a 
    //       Magic Sqare as described in the linked Wikipedia page.
	
	//look for the text file
	char * fileName = *(argv + 1); //error if not valid
	MagicSquare *square = generateMagicSquare(size);
    if (square == NULL) {
		perror("Can't allocate memory");
		exit(1);
	}
	// TODO: Output the magic square
	fileOutputMagicSquare(square, fileName);

	//free magic square
	for (int i = 0; i < square -> size; i++) {
		free (*(square -> magic_square + i));
	}

	free (square -> magic_square);
	square -> magic_square = NULL;

	free (square);

    return 0;
} 

//    F23


