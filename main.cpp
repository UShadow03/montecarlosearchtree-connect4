#include "GameState.hpp"
#include "MCTS.hpp"
#include <iostream>
using namespace std;

int main() {
    GameState realGame;
    //Initalize the table by filling all empty spots with '.'.
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            realGame.board[row][col] = '.';
        }
    }

    int playerChoice;
    MCTS simulation;
    int numSim;
    realGame.playerTurn = 1;
    realGame.blocksDropped = 0;
    while (true) {
        realGame.printTable();
        cout << "Player " << realGame.playerTurn << ": Your turn!\n";
        cout << "Choose the column (1-7) to drop your shape!\n";
        
        cout << "How many simulations do you want to run?\n";
        cin >> numSim;
        int p1Wins = 0;
        int p2Wins = 0;
        int ties = 0;
        for (int i = 0; i < numSim; ++i) {
            int result = simulation.simulate(realGame);
            if (result == 0) ++ties;
            if (result == 1) ++p1Wins;
            if (result == 2) ++p2Wins;
        }
        cout << "Number of ties:" << ties << "\n";
        cout << "Number of Player 1 wins:" << p1Wins << "\n";
        cout << "Number of Player 2 wins:" << p2Wins << "\n";
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
            if (!realGame.droppingShape(playerChoice)) {
                cout << "The Column is full! Try again!\n";
                continue;
            }
            break;
        }
        realGame.blocksDropped++;
        //Checking if a player has won.
        if (realGame.checkWin(realGame.playerTurn)) {
            cout << "Player " << realGame.playerTurn << " wins!\n";
            realGame.printTable();
            break;
        }
        if (realGame.checkDraw()) {
            cout << "It's a draw!!";
            realGame.printTable();
            break;
        }

        //Switching Player turn.
        if (realGame.playerTurn == 1) {
            realGame.playerTurn = 2;
        } else {
            realGame.playerTurn = 1;
        }

    }
    cout << "Game ends.";

}
