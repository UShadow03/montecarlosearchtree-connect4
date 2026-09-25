#include "GameState.hpp"
#include "MCTS.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
using namespace std;

struct CliOptions {
    bool haveSimCount = false;
    int simCount = 0;
    int firstPlayer = 1;
    bool showHelp = false;
    bool aiEnabled = false;
};

void printUsage(const char* progName) {
    cout << "Usage: " << progName << " [options]\n\n"
         << "Options:\n"
         << "  --simulations N, --sim N   Use a fixed simulation count for MCTS assist\n"
         << "                              (skips the interactive prompt).\n"
         << "  --difficulty LEVEL         Set simulation count by difficulty preset:\n"
         << "                              easy (100), medium (2000), hard (10000).\n"
         << "                              Overrides --simulations if both are given.\n"
         << "  --first-player N           Which player moves first: 1 or 2 (default 1).\n"
         << "  -h, --help                 Show this help message and exit.\n";
}

// Returns -1 on unrecognized/invalid argument (after printing an error).
int parseArgs(int argc, char* argv[], CliOptions& opts) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            opts.showHelp = true;
            return 0;
        }
        else if (arg == "--simulations" || arg == "--sim") {
            if (i + 1 >= argc) {
                cerr << "Error: " << arg << " requires a value.\n";
                return -1;
            }
            opts.simCount = atoi(argv[++i]);
            if (opts.simCount <= 0) {
                cerr << "Error: simulation count must be a positive integer.\n";
                return -1;
            }
            opts.haveSimCount = true;
        }
        else if (arg == "--difficulty") {
            if (i + 1 >= argc) {
                cerr << "Error: --difficulty requires a value (easy, medium, hard).\n";
                return -1;
            }
            string level = argv[++i];
            if (level == "easy") {
                opts.simCount = 100;
            }
            else if (level == "medium") {
                opts.simCount = 2000;
            }
            else if (level == "hard") {
                opts.simCount = 10000;
            }
            else {
                cerr << "Error: unknown difficulty '" << level << "'. Use easy, medium, or hard.\n";
                return -1;
            }
            opts.haveSimCount = true;
            opts.aiEnabled = true;
        }
        else if (arg == "--first-player") {
            if (i + 1 >= argc) {
                cerr << "Error: --first-player requires a value (1 or 2).\n";
                return -1;
            }
            opts.firstPlayer = atoi(argv[++i]);
            if (opts.firstPlayer != 1 && opts.firstPlayer != 2) {
                cerr << "Error: --first-player must be 1 or 2.\n";
                return -1;
            }
        }
        else {
            cerr << "Error: unrecognized argument '" << arg << "'.\n";
            cerr << "Run with --help for usage.\n";
            return -1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    CliOptions opts;
    if (parseArgs(argc, argv, opts) != 0) {
        return 1;
    }
    if (opts.showHelp) {
        printUsage(argv[0]);
        return 0;
    }

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
    realGame.playerTurn = opts.firstPlayer;
    realGame.blocksDropped = 0;
    while (true) {
        realGame.printTable();
        if (opts.aiEnabled && realGame.playerTurn == 2) {
            cout << "AI (Player 2): Your turn!\n";
            cout << "AI is thinking...\n";

            MCTSResult result =
                simulation.findBestMove(realGame, opts.simCount, 2);

            cout << "AI chooses column " << result.bestMove + 1 << "\n";

            realGame.droppingShape(result.bestMove + 1);
            realGame.blocksDropped++;

            if (realGame.checkWin(2)) {
                cout << "AI wins!\n";
                realGame.printTable();
                break;
            }

            if (realGame.checkDraw()) {
                cout << "It's a draw!!\n";
                realGame.printTable();
                break;
            }

            realGame.playerTurn = 1;
            continue;
        }
        cout << "Player " << realGame.playerTurn << ": Your turn!\n";  
        cout << "Would you like to use an MC Tree Search algorithm to help making your decisions? (Y/N)\n";
        cin >> choice;
        while (true) {
            if (choice == 'Y') {
                if (opts.haveSimCount) {
                    // A fixed count was supplied on the command line — skip the prompt.
                    numSim = opts.simCount;
                    cout << "Using " << numSim << " simulations (set via command line).\n";
                }
                else {
                    cout << "How many simulations do you want to run?\n";
                    cin >> numSim;
                    while (cin.fail() || numSim <= 0) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Please enter a positive number of simulations.\n";
                        cin >> numSim;
                    }
                }
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
