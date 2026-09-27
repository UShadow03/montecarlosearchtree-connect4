#include "GameState.hpp"
#include <iostream>
#include <vector>
using namespace std;

// This function prints the entire 6x7 Connect Four table.
void GameState::printTable() {
    // Column headers so the player doesn't have to count characters
    // to figure out which digit corresponds to which column.
    for (int col = 1; col <= 7; col++) {
        cout << col << ' ';
    }
    cout << '\n';

    //Printing out the table
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            cout << board[row][col] << ' ';
        }
        cout << '\n';
    }
    
}

vector<int> GameState::getLegalMoves() {
    vector<int> moves;
    for (int col = 0; col < 7; ++col) {
        for (int row = 0; row < 6; ++row) {
            if (board[row][col] == '.') {
                moves.push_back(col);
                break;
            }
        }
    }
    return moves;
}


// This function drops an 'X' or 'O' based on the player's choice.
bool GameState::droppingShape(int choice) {
    int playerChoice = choice - 1;
    for (int row = 5; row >= 0; row--) {
        if (board[row][playerChoice] == '.') {
            if (playerTurn == 1) {
                board[row][playerChoice] = 'X';
            }
            else {
                board[row][playerChoice] = 'O';
            }
            return true;
        }
    }
    return false;
}

bool GameState::checkWin(int player) {
    char playerSymbol;
    if (player == 1) playerSymbol = 'X';
    if (player == 2) playerSymbol = 'O';

    int inaRow = 0;
    // Checking for win in a horizontal line.
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            if (board[row][col] == playerSymbol) {
                inaRow++;
                if (inaRow == 4) return true;
            }
            else {
                inaRow = 0;
            }
        }
        inaRow = 0;
    }
    // Checking for win in a vertical line.
    for (int col = 0; col < 7; col++) {
        for (int row = 0; row < 6; row++) {
            if (board[row][col] == playerSymbol) {
                inaRow++;
                if (inaRow == 4) return true;
            }
            else {
                inaRow = 0;
            }
        }
        inaRow = 0;
    }
    // Checking for win in a diagonal top-left to bottom-right.
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 3; row++) {
            if (board[row][col] == playerSymbol && board[row+1][col+1] == playerSymbol && board[row+2][col+2] == playerSymbol && board[row+3][col+3] == playerSymbol) {
                return true;
            }
        }
    }
    // Checking for win in a diagonal top-right to bottom-left.
    for (int col = 3; col < 7; col++) {
        for (int row = 0; row < 3; row++) {
            if (board[row][col] == playerSymbol && board[row+1][col-1] == playerSymbol && board[row+2][col-2] == playerSymbol && board[row+3][col-3] == playerSymbol) {
                return true;
            }
        }
    }
    return false;
}

bool GameState::checkDraw() {
    return blocksDropped == 42;
}
