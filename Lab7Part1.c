
// File:   Lab6.c
// Author: Ting Ray (Kingston) Wang
// Created on March 9, 2017, 9:00 PM

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

// This function asks for the computer's colour. 
char enterComputerColour(void) {
    char cpu;
    printf("Computer plays (B/W) : ");
    scanf(" %c", &cpu);
    return cpu;
}

// This function populates the board for initial setup. 
void initializeBoard(char board[][26], int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            board[i][j] = 'U';
        }
    }
    
    board[n / 2 - 1][n / 2 - 1] = 'W';
    board[n / 2 - 1][n / 2] = 'B';
    board[n / 2][n / 2 - 1] = 'B';
    board[n / 2][n / 2] = 'W';
    
    printBoard(board, n);
}

// This function checks if a position in within the bounds of the board. 
bool positionInBounds(char board[][26], int n, int row, int col) {
    if ((row >= 0) && (row < n) && (col >= 0) && (col < n)) return true;
    else return false;
}

// This function checks if the position indicated is of the opposite colour. 
// It also calculates the number of tiles that could be flipped in the given direction. 
bool checkLegalInDirectionCPU(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol, int *score) {
    if (!positionInBounds(board, n, row + deltaRow, col + deltaCol)) return false;
    
    char otherColour = 'W';
    if (colour == 'W') otherColour = 'B';
    if (board[row + deltaRow][col + deltaCol] == otherColour) {
        for (int i = 2; ; i++) {
            if (!positionInBounds(board, n, row + deltaRow * i, col + deltaCol * i)) return false;
            
            if (board[row + deltaRow * i][col + deltaCol * i] == otherColour);
            else if (board[row + deltaRow * i][col + deltaCol * i] == colour) {
                (*score) = (*score) + i - 1;
                return true;
            }
            else return false;
        }
    }
    else return false;
}

// This function checks if the position indicated is of the opposite colour. 
bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol) {
    if (!positionInBounds(board, n, row + deltaRow, col + deltaCol)) return false;
    
    char otherColour = 'W';
    if (colour == 'W') otherColour = 'B';
    
    if (board[row + deltaRow][col + deltaCol] == otherColour) {
        for (int i = 2; ; i++) {
            if (!positionInBounds(board, n, row + deltaRow * i, col + deltaCol * i)) return false;
            
            if (board[row + deltaRow * i][col + deltaCol * i] == otherColour);   
            else if (board[row + deltaRow * i][col + deltaCol * i] == colour) return true;
            else return false;
        }
    }
    else return false;
}

// This function flips the tiles in all directions.
void flippingTiles (char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol) {
    board[row][col] = colour;
    
    for (int i = 0; ; i++) {
        if (checkLegalInDirection(board, n, row + deltaRow * i, col + deltaCol * i, colour, deltaRow, deltaCol)) {
            board[row + deltaRow * (i + 1)][col + deltaCol * (i + 1)] = colour;
        }
        else return;
    }
}

// This function sends the eight directions around a position to the next function. 
// If that function evaluates to true, this function calls another function to check more positions along that direction. 
// If all conditions are satisfied, the function send the indices to a function to be flipped. 
char checkThisLocation(char board[][26], int n, int row, int col, char colour) {
    bool validMove = false;
    for (int deltaRow = -1; deltaRow <= 1; deltaRow++) {
        for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
            if (checkLegalInDirection(board, n, row, col, colour, deltaRow, deltaCol)) {
                flippingTiles(board, n, row, col, colour, deltaRow, deltaCol);
                validMove = true;
            }
        }
    }
    
    if (!validMove) printf("Invalid Move. \n");
    return validMove;
}

// This function looks at unoccupied positions and searches for possible moves. 
// Then, the function sends this position to be evaluated, and the position with the best score is chosen. 
void enterMoveCPU(char board[][26], int n, char colour) {
    int row = 0, col = 0, bestRow = 0, bestCol = 0, bestScore = 0, currentScore;
    int *score = &currentScore;
    
    for (row = 0; row < n; row++) {
        for (col = 0; col < n ; col++) {
            *score = 0;
            if (board[row][col] == 'U') {
                for (int deltaRow = -1; deltaRow <= 1; deltaRow++) {
                    for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
                        if (checkLegalInDirectionCPU(board, n, row, col, colour, deltaRow, deltaCol, score)) {
                            if (*score > bestScore) {
                                bestRow = row;
                                bestCol = col;
                                bestScore = *score;
                            }
                        }
                    }
                }
            }
        }
    }
    
    printf("Computer places %c at %c%c.\n", colour, bestRow + 'a', bestCol + 'a');
    for (int deltaRow = -1; deltaRow <= 1; deltaRow++) {
        for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
            if (checkLegalInDirection(board, n, bestRow, bestCol, colour, deltaRow, deltaCol)) {
                flippingTiles(board, n, bestRow, bestCol, colour, deltaRow, deltaCol);
            }
        }
    }
    printBoard(board, n);
}

// This function prompts user for the move, and sends it to be checked if it is valid. 
bool enterMove(char board[][26], int n, char user) {
    printf("Enter move for colour %c (RowCol): ", user);
    char row, col;
    scanf(" %c%c", &row, &col);
    
    if (board[row - 'a'][col - 'a'] != 'U') {
        printf("Invalid move. \n");
        return false;
    }
    
    if (!positionInBounds(board, n, row - 'a', col - 'a')) {
        printf("Invalid move. \n");
        return false;
    }
    
    if (checkThisLocation(board, n, row - 'a', col - 'a', user)) {
        printBoard(board, n);
        return true;
    } else return false;
}

// This function checks if each colour has at least one move. 
void proceed(char board[][26], int n, char cpu, char user, bool *outOfMoves1, bool *outOfMoves2) {
    
    *outOfMoves1 = true;
    *outOfMoves2 = true;
    
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n ; col++) {
            if (board[row][col] == 'U') {
                for (int deltaRow = -1; deltaRow <= 1; deltaRow++) {
                    for (int deltaCol = -1; deltaCol <= 1; deltaCol++) {
                        if (checkLegalInDirection(board, n, row, col, cpu, deltaRow, deltaCol)) *outOfMoves1 = false;
                        if (checkLegalInDirection(board, n, row, col, user, deltaRow, deltaCol)) *outOfMoves2 = false;
                        if (!(*outOfMoves1) && !(*outOfMoves2)) return;
                    }
                }
            }
        }
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
    char cpu = enterComputerColour();
    char board[n][26];
    initializeBoard(board, n);
    
    char user = 'W';
    if (cpu == 'W') user = 'B';
    
    int turn = 1;
    if (cpu == 'W') turn = -1;
    
    bool outOfMoves1, outOfMoves2, previousInvalidCPU;
    bool *outOfMoves1P = &outOfMoves1;
    bool *outOfMoves2P = &outOfMoves2;
    
    while (true) {
        
        proceed(board, n, cpu, user, outOfMoves1P, outOfMoves2P);
        
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