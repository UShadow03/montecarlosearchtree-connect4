#include "MCTS.hpp"
#include "GameState.hpp"
#include <vector>
#include <random>
#include <algorithm>
using namespace std;

random_device rd;
mt19937 gen(rd());
std::uniform_int_distribution<int> distrib(1, 7);

Node::Node(GameState state, Node* parent)
    : state(state), parent(parent), visits(0), wins(0) {
}

Node* MCTS::expand(Node* node) {
    vector<int> moves = node->state.getLegalMoves();

    // Remove moves that have already been expanded.
    for (Node* child : node->children) {
        for (int col = 0; col < 7; ++col) {
            for (int row = 0; row < 6; ++row) {
                if (node->state.board[row][col] != child->state.board[row][col]) {
                    moves.erase(
                        remove(moves.begin(), moves.end(), col),
                        moves.end()
                    );
                    break;
                }
            }
        }
    }

    // No unexpanded moves remain.
    if (moves.empty()) {
        return nullptr;
    }
    
    int move = moves[0];

    GameState newState = node->state;
    newState.droppingShape(move + 1);

    if (newState.playerTurn == 1) {
        newState.playerTurn = 2;
    }
    else {
        newState.playerTurn = 1;
    }

    newState.blocksDropped++;

    Node* child = new Node(newState, node);
    node->children.push_back(child);

    return child;
}

int MCTS::simulate(GameState state) {
    while (true) {

        if (state.checkWin(1)) {
            return 1;
        }
        if (state.checkWin(2)) {
            return 2;
        }
        if (state.checkDraw()) {
            return 0;
        }

        vector<int> moves = state.getLegalMoves();
        int randomNum = distrib(gen) - 1;
        while (find(moves.begin(), moves.end(), randomNum) == moves.end()) {
            randomNum = distrib(gen) - 1;
        }

        state.droppingShape(randomNum + 1);

        // Switch player.
        if (state.playerTurn == 1) {
            state.playerTurn = 2;
        }
        else {
            state.playerTurn = 1;
        }

        state.blocksDropped++;


    }
}

