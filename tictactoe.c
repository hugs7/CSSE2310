// tictactoe.c
// - written by Hugo Burton
// Plays tic-tac-toe
// X is the first player (user). O is the computer

// includes

#include <stdio.h>
#include <stdlib.h>

// function prototypes
int game_over(char board[3][3]);
int pos_input(void);
int input_valid(int position, char board[3][3]);
int get_computer_move(char board[3][3]);
void print_board(char board[3][3]);

// Main

int main() {
    // declare and initaialise game board - all blank to start with
    char board[3][3] = { { ' ', ' ' , ' '},
			 { ' ', ' ' , ' '},
			 { ' ', ' ' , ' '}};
    int position;
    int turn = 0;	// player = 0, computer = 1
    // initialise game board

    print_board(board);
    	
    while (!game_over(board)) {
	
	if (turn == 0) {
	   
	    // If user's turn, ask for move
	    // Check for valid input
	    // If not valid, ask them again
	    do {
		position = pos_input();
	    } while(!input_valid(position, board));
	    
	    // Play user's position
	    board[(position - 1) / 3][(position - 1) % 3] = 'X';
	    
	    turn = 1;
	} else { // computer's turn
	    position = get_computer_move(board);
	    // put an O in that position
	    board[(position - 1) / 3][(position - 1) % 3] = 'O';
	    turn = 0;
	}

	print_board(board);

    }
}

int game_over(char board[3][3]) {
   return 0; 
}

int pos_input(void) {
    char buffer[10];

    fgets(buffer, 10, stdin);
    return atoi(buffer);
}

int input_valid(int position, char board[3][3]) {
    if (position < 1 || position > 9) {
	return 0;
    }

    int row = (position - 1) / 3;
    int col = (position - 1) % 3;

    return (board[row][col] == ' ');
}

int get_computer_move(char board[3][3]) {
    int position = 1;
    while (!input_valid(position, board)) {
	position++;
    }

    return position;
}

void print_board(char board[3][3]) {
    for (int r = 0; r < 3; r++) {
	for (int c = 0; c < 3; c++) {
	    printf("%c", board[r][c]);
	    if (c < 2) {
		printf("|");
	    }
	}
	printf("\n");
	if (r < 2) {
	    printf("-+-+-\n");
	}
    }
}
