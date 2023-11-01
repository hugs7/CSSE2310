#include <stdio.h>
#include <stdlib.h>
#define NUM_ROWS 2
#define NUM_COLS 2

// Define your EMPTY_CELL macro here
#define EMPTY_CELL '_'
/**
 * Initialises a 2D char array of numRows and numCols provided
 * declares each element as EMPTY_CELL character
**/
char** init_board(int numRows, int numCols) {
	// Initialise 2D char array and allocate the num of rows
	char** arr = (char**)malloc(sizeof(char*) * numRows);
	// For each row of the board allocate the num of cols
	for (int i = 0; i < numRows; i++) {
		arr[i] = (char*)malloc(sizeof(char) * numCols);
	}
	// Set value of each cell to empty cell char
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++) {
			arr[i][j] = EMPTY_CELL;
		}
	}
	// return the board
	return arr;
}

int main(void) {
	char** board = init_board(NUM_ROWS, NUM_COLS);
	printf("%c%c\n", board[0][0], board[0][1]);
	printf("%c%c\n", board[1][0], board[1][1]);

	return 0;
}
