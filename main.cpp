#include <iostream>
using namespace std;

char board[6][7];
int playerTurn = 1;
int blocksDropped = 0;
// This function prints the entire 6x7 Connect Four table.
void printTable() {
    //Printing out the table
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            cout << board[row][col] << ' ';
        }
        cout << '\n';
    }
    
}

// This function drops an 'X' or 'O' based on the player's choice.
bool droppingShape(int choice) {
    int playerChoice = choice - 1;
    for (int row = 5; row >= 0; row--) {
        if (board[row][playerChoice] == '.') {
            if (playerTurn == 1) {
                board[row][playerChoice] = 'X';
                return true;
            }
            else {
                board[row][playerChoice] = 'O';
                return true;
            }
        }
    }
    return false;
}

// This function checks if a player has won.
bool checkWin(int player) {
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

int main() {
    //Initalize the table by filling all empty spots with '.'.
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            board[row][col] = '.';
        }
    }

    int playerChoice;
    while (true) {
        printTable();
        cout << "Player " << playerTurn << ": Your turn!\n";
        cout << "Choose the column (1-7) to drop your shape!\n";
        while (true) {
            cin >> playerChoice;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Please enter a number from 1-7!\n";
                continue;
            }
            if (playerChoice < 1 || playerChoice > 7) {
                cout << "Invalid Column, try again!\n";
                continue;
            }
            if (!droppingShape(playerChoice)) {
                cout << "The Column is full! Try again!\n";
                continue;
            }
            break;
        }
        blocksDropped++;
        //Checking if a player has won.
        if (checkWin(playerTurn)) {
            cout << "Player " << playerTurn << " wins!\n";
            printTable();
            break;
        }
        if (blocksDropped == 42) {
            cout << "It's a draw!!";
            printTable();
            break;
        }

        //Switching Player turn.
        if (playerTurn == 1) {
            playerTurn = 2;
        } else {
            playerTurn = 1;
        }

    }
    cout << "Game ends.";

}
