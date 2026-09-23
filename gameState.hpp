#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP
#include <vector>

struct GameState {
    char board[6][7];
    int playerTurn;
    int blocksDropped;
    void printTable();
    bool droppingShape(int choice);
    bool checkWin(int player);
    bool checkDraw();
    std::vector<int> getLegalMoves();
};

#endif