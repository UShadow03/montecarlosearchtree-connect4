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
    char choice;
    realGame.playerTurn = 1;
    realGame.blocksDropped = 0;
    while (true) {
        realGame.printTable();
        cout << "Player " << realGame.playerTurn << ": Your turn!\n";  
        cout << "Would you like to use an MC Tree Search algorithm to help making your decisions? (Y/N)\n";
        cin >> choice;
        while (true) {
            if (choice == 'Y') {
                cout << "How many simulations do you want to run?\n";
                cin >> numSim;
                MCTSResult result = simulation.findBestMove(realGame, numSim, realGame.playerTurn);
                cout << "Statistics:\n";
                for (const MoveStats& stat : result.stats) {
                    cout << "Column " << stat.move + 1 << ": Visits: " << stat.visits << " Wins: " << stat.wins << " Winrate: " << stat.winRate << "\n";
                }
                cout << "Best Move: " << result.bestMove + 1 << "\n";
                break;
            }
            else if (choice == 'N') {
                break;
            }
            else {
                cout << "Choice invalid! Try again.\n";
                cin >> choice;
            }
        }
        cout << "Which column do you want to drop your shape? (1-7)\n";
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
