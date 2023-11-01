#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_ROWS 10
#define NUM_COLS 5

#define EMPTY_CELL '_'

/**
 * Initialises a 2D char array of numRows and numCols provided
 * declares each element as EMPTY_CELL character
**/
char** init_board(int numRows, int numCols) {
	char** board = malloc(sizeof(char*) * numRows);
	for (int i = 0; i < numRows; i++) {
		board[i] = malloc(sizeof(char) * numCols);
		memset(board[i], EMPTY_CELL, sizeof(char) * numCols);
	}

	return board;
}

/**
 * Prints each row of the char** on a new line
**/
void print_board(char** board, int numRows, int numCols) {
	// loop through board array and print rows
	for (int i = 0; i < numRows; i++) {
		printf("%s\n", board[i]);
	}
}

int main(int argc, char** argv) {
	 char** board = init_board(NUM_ROWS, NUM_COLS);
	 print_board(board, NUM_ROWS, NUM_COLS);

	 return 0;
}
