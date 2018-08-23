
// File:   Lab6.c
// Author: Ting Ray (Kingston) Wang
// Created on March 10, 2017, 12:00 PM

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "lab7part2lib.h"

// This function prints the board. 
void printBoard(char board[][26], int n) {
    printf("  ");
    for (int h = 0; h < n; h++) {
        printf("%c", h + 'a');
    }
    printf("\n");
    
    for (int i = 0; i < n; i++) {
        printf("%c ", i + 'a');
        for (int j = 0; j < n; j++) {
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}

// This function prompts user for the board dimension. 
int enterBoardDimension(void) {
    int dimension;
    printf("Enter the board dimension: ");
    scanf("%d", &dimension);
    return dimension;
}

// This function populates the board for initial setup and asks for the computer's colour. 
char initializeBoard(char board[][26], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            board[i][j] = 'U';
        }
    }
    
    board[n / 2 - 1][n / 2 - 1] = 'W';
    board[n / 2 - 1][n / 2] = 'B';
    board[n / 2][n / 2 - 1] = 'B';
    board[n / 2][n / 2] = 'W';
    
    char cpu;
    printf("Computer plays (B/W) : ");
    scanf(" %c", &cpu);
    
    printBoard(board, n);
    return cpu;
}

// This function checks if the position indicated is of the opposite colour. 
bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int dRow, int dCol) {
    if (row + dRow < 0 || row + dRow >= n || col + dCol < 0 || col + dCol >= n) return false;
    
    char otherColour = 'W';
    if (colour == 'W') otherColour = 'B';
    
    if (board[row + dRow][col + dCol] == otherColour) {
        for (int i = 2; ; i++) {
            if (row + dRow * i < 0 || row + dRow * i >= n || col + dCol * i < 0 || col + dCol * i >= n) return false;
            
            if (board[row + dRow * i][col + dCol * i] == otherColour);   
            else if (board[row + dRow * i][col + dCol * i] == colour) return true;
            else return false;
        }
    } else return false;
}

// This function flips the tiles in all directions.
void flipTiles (char board[][26], int n, int row, int col, char colour) {
    for (int dRow = -1; dRow <= 1; dRow++)
        for (int dCol = -1; dCol <= 1; dCol++)
            if (checkLegalInDirection(board, n, row, col, colour, dRow, dCol)) {
                board[row][col] = colour;
                
                bool done = false;
                for (int i = 0; !done; i++) {
                    if (checkLegalInDirection(board, n, row + dRow * i, col + dCol * i, colour, dRow, dCol)) {
                        board[row + dRow * (i + 1)][col + dCol * (i + 1)] = colour;
                    } else done = true;
                }
            }
}

// This function looks at whether a position is favourable using a few common strategies for Reversi
// First, the function checks if the position is a corner or along the side of the board. 
// If not, the function checks how many opponent's tiles can the user flip.
int riskCheck(char board[][26], int n, int row, int col, char colour) {
    
    int score = 0, currentScore = 0;
    char otherColour = 'W';
    if (colour == 'W') otherColour = 'B';
    
    if (row == 0 && col == 0) return n;
    else if (row == n-1 && col == 0) return n;
    else if (row == 0 && col == n-1) return n;
    else if (row == n-1 && col == n-1) return n;
    
    if (board[0][0] != colour) {
        if (row == 0 && col == 1 && board[0][n-1] != colour) return -n;
        else if (row == 1 && col == 0 && board[n-1][0] != colour) return -n;
        else if (row == 1 && col == 1) return -n;
    }
    
    if (board[0][n-1] != colour) {
        if (row == 0 && col == n-2 && board[0][0] != colour) return -n;
        else if (row == 1 && col == n-1 && board[n-1][n-1] != colour) return -n;
        else if (row == 1 && col == n-2) return -n;
    }
    
    if (board[n-1][0] != colour) {
        if (row == n-2 && col == 0 && board[0][0] != colour) return -n;
        else if (row == n-1 && col == 1 && board[n-1][n-1] != colour) return -n;
        else if (row == n-2 && col == 1) return -n;
    }
    
    if (board[n-1][n-1] != colour) {
        if (row == n-1 && col == n-2 && board[n-1][0] != colour) return -n;
        else if (row == n-2 && col == n-1 && board[0][n-1] != colour) return -n;
        else if (row == n-2 && col == n-2) return -n;
    }
    
    if ((row == 0 || row == n-1) && board[row][col+1] != otherColour && board[row][col-1] != otherColour) return n * 2/3;
    else if ((col == 0 || col == n-1) && board[row+1][col] != otherColour && board[row-1][col] != otherColour) return n * 2/3;
    
    int countU = 0;
    for (int deltaRow = -1; deltaRow <= 1; deltaRow++)
        for (int deltaCol = -1; deltaCol <= 1; deltaCol++)
            if (board[row + deltaRow][col + deltaCol] == 'U') countU++;
    if (countU >= 7) return -n/3;
    
    return 0;
}

// This function looks at unoccupied positions and searches for possible moves. 
// Then, the function sends this position to be evaluated, and the position with the best score is chosen. 
void initiate(char board[][26], int n, int row, int col, char colour, int *level, int *chosenRow, int *chosenCol, int *bestScore) {
    bool activate = false;

    if (board[row][col] == 'U')
        for (int deltaRow = -1; deltaRow <= 1 && !activate; deltaRow++)
            for (int deltaCol = -1; deltaCol <= 1 && !activate; deltaCol++)
                if (checkLegalInDirection(board, n, row, col, colour, deltaRow, deltaCol)) activate = true;

    if (activate) {
        int score = riskCheck(board, n, row, col, colour);
        if (score > *bestScore) {
            *chosenRow = row;
            *chosenCol = col;
            *bestScore = score;
        }
    }
}

// This function sends the positions to the next function, depending on the current board configuration. 
// When those functions return, this function makes the best move. 
void enterMoveCPU(char board[][26], int n, char colour) {
    int level, chosenRow, chosenCol, bestScore = -1000;
    if (n < 8) level = 7;
    else if (n < 12) level = 5;
    else level = 3;
    
    if (board[0][0] == 'U')
        for (int row = 0; row < n; row++)
            for (int col = 0; col < n; col++)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    else if (board[0][n-1] == 'U')
        for (int row = 0; row < n; row++)
            for (int col = n-1; col >= 0; col--)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    else if (board[n-1][0] == 'U')
        for (int col = 0; col < n; col++)
            for (int row = n-1; row >= 0; row--)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    else if (board[n-1][n-1] == 'U')
        for (int row = n-1; row >= 0; row--)
            for (int col = n-1; col >= 0; col--)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    else if (board[0][0] == colour)
        for (int row = 0; row < n; row++)
            for (int col = 0; col < n; col++)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    else if (board[0][n-1] == colour)
        for (int row = 0; row < n; row++)
            for (int col = n-1; col >= 0; col--)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    else if (board[n-1][0] == colour)
        for (int col = 0; col < n; col++)
            for (int row = n-1; row >= 0; row--)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    else if (board[n-1][n-1] == colour)
        for (int row = n-1; row >= 0; row--)
            for (int col = n-1; col >= 0; col--)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    else
        for (int row = 0; row < n; row++)
            for (int col = 0; col < n; col++)
                initiate(board, n, row, col, colour, &level, &chosenRow, &chosenCol, &bestScore);
    
    printf("Computer places %c at %c%c.\n", colour, chosenRow + 'a', chosenCol + 'a');
    flipTiles(board, n, chosenRow, chosenCol, colour);
    printBoard(board, n);
}

// This function prompts user for the move, and sends it to be checked if it is valid. 
bool enterMove(char board[][26], int n, char user) {
    
    printf("Enter move for colour %c (RowCol): ", user);
    char rowChar, colChar;
    scanf(" %c%c", &rowChar, &colChar);
    int row = (int) (rowChar - 'a');
    int col = (int) (colChar - 'a');
    
    if (board[row][col] != 'U') {
        printf("Invalid move. \n");
        return false;
    }
    
    if (row < 0 || row >= n || col < 0 || col >=n) {
        printf("Invalid move. \n");
        return false;
    }
    
    bool validMove = false;
    for (int deltaRow = -1; deltaRow <= 1; deltaRow++)
        for (int deltaCol = -1; deltaCol <= 1; deltaCol++)
            if (checkLegalInDirection(board, n, row, col, user, deltaRow, deltaCol)) validMove = true;
    
    if (!validMove) printf("Invalid Move. \n");
    else {
        flipTiles(board, n, row, col, user);
        printBoard(board, n);
    } return validMove;
}

// This function checks if each colour has at least one move. 
void proceed(char board[][26], int n, char cpu, char user, bool *outOfMoves1, bool *outOfMoves2) {
    *outOfMoves1 = true;
    *outOfMoves2 = true;
    
    for (int row = 0; row < n; row++)
        for (int col = 0; col < n ; col++)
            if (board[row][col] == 'U')
                for (int deltaRow = -1; deltaRow <= 1; deltaRow++)
                    for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
                        if (checkLegalInDirection(board, n, row, col, cpu, deltaRow, deltaCol)) *outOfMoves1 = false;
                        if (checkLegalInDirection(board, n, row, col, user, deltaRow, deltaCol)) *outOfMoves2 = false;
                        if (!(*outOfMoves1) && !(*outOfMoves2)) return;
                    }
}

// This function counts the number of tiles for each colour and declare the winner. 
void winner(char board[][26], int n){
    int countB = 0, countW = 0;
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n ; col++) {
            if (board[row][col] == 'B') countB++;
            if (board[row][col] == 'W') countW++;
        }
    }
    if (countB > countW) printf("B player wins.");
    else if (countB < countW) printf("W player wins.");
    else printf("Draw!");
}

// This function alternates the turn between the computer and the user. 
int main(void) {
    
    int n = enterBoardDimension();
    char board[n][26];
    char cpu = initializeBoard(board, n);
    
    char user = 'W';
    if (cpu == 'W') user = 'B';
    
    int turn = 1;
    if (cpu == 'W') turn = -1;
    
    bool outOfMoves1, outOfMoves2, previousInvalidCPU;
    
    while (true) {
        
        proceed(board, n, cpu, user, &outOfMoves1, &outOfMoves2);
        
        if (outOfMoves1 && outOfMoves2) {
            winner(board, n);
            return 0;
        }
        
        else if (turn == 1) {
            if (outOfMoves2 && previousInvalidCPU) printf("%c player has no valid move.\n", user);
            if (!outOfMoves1) {
                enterMoveCPU(board, n, cpu);
                previousInvalidCPU = true;
            }
            turn *= -1;
        }
        else if (turn == -1) {
            if (outOfMoves1 && !previousInvalidCPU) printf("%c player has no valid move.\n", cpu);
            if (!outOfMoves2) {
                previousInvalidCPU = false;
                if (!enterMove(board, n, user)) {
                    printf("%c player wins.\n", cpu);
                    return 0;
                }
            }
            turn *= -1;
        }
    }

    return (EXIT_SUCCESS);
}