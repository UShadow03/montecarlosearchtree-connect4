#include "MCTS.hpp"
#include "GameState.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <limits>
using namespace std;

random_device rd;
mt19937 gen(rd());
std::uniform_int_distribution<int> distrib(1, 7);
double C = sqrt(2);
Node::Node(GameState state, Node* parent, int move)
    : state(state), parent(parent), move(move), visits(0), wins(0) {
}

Node* MCTS::expand(Node* node) {
    vector<int> moves = node->state.getLegalMoves();

    for (Node* child : node->children) {
        moves.erase(
            remove(moves.begin(), moves.end(), child->move),
            moves.end()
        );
    }

    // No unexpanded moves remain.
    if (moves.empty()) {
        return nullptr;
    }
    // Choose one unexpanded move.
    int move = moves[0];

    // Create the new game state.
    GameState newState = node->state;
    newState.droppingShape(move + 1);

    // Switch player.
    if (newState.playerTurn == 1) {
        newState.playerTurn = 2;
    }
    else {
        newState.playerTurn = 1;
    }

    newState.blocksDropped++;

    // Create exactly ONE child.
    Node* child = new Node(newState, node, move);
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

        // Does the player have an immediete winning move?
        int winningMove = -1;
        for (int move : moves) {
            GameState testState = state;
            testState.droppingShape(move + 1);
            if (testState.checkWin(state.playerTurn)) {
                winningMove = move;
                break;
            }
        }

        int chosenMove;

        // If they have a winning move, play it.
        if (winningMove != -1) {
            chosenMove = winningMove;
        }
        else {
            //If no immediete winning move, generate a random one.
            chosenMove = distrib(gen) - 1;
            while (find(moves.begin(), moves.end(), chosenMove) == moves.end()) {
                chosenMove = distrib(gen) - 1;
            }

        }
        state.droppingShape(chosenMove + 1);

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

Node* MCTS::select(Node* node) {
    while (isFullyExpanded(node)&& !node->state.checkWin(1) && !node->state.checkWin(2) && !node->state.checkDraw()) {
        Node* bestChild = node->children[0];
        double bestUCT = calculateUCT(bestChild);

        for (Node* child : node->children) {
            double uct = calculateUCT(child);

            if (uct > bestUCT) {
                bestUCT = uct;
                bestChild = child;
            }
        }

        node = bestChild;
    }

    return node;
}

double MCTS::calculateUCT(Node* node) {
    if (node->visits == 0) {
        return numeric_limits<double>::max();
    }

    double exploitation = static_cast<double> (node->wins)/node->visits;
    double exploration = C*sqrt(log(node->parent->visits)/node->visits);

    return exploitation + exploration;
}

bool MCTS::isFullyExpanded(Node* node) {
    return node->children.size() == node->state.getLegalMoves().size();
}

void MCTS::backpropagate(Node* node, int result, int player) {
    while (node != nullptr) {
        node->visits++;

        if (result == player) {
            node->wins++;
        }

        node = node->parent;
    }
}

MCTSResult MCTS::findBestMove(GameState state, int simulations, int player) {
    Node* root = new Node(state);
    MCTS mcts;
    for (int i = 0; i < simulations; ++i) {
        Node* selection = mcts.select(root);
        Node* expansion = mcts.expand(selection);
        if (expansion == nullptr) {
            continue;
        }
        int result = mcts.simulate(expansion->state);
        mcts.backpropagate(expansion, result, player);
    }

    Node* bestChild = root->children[0];

    for (Node* child : root->children) {
        if (child->visits > bestChild->visits) {
            bestChild = child;
        }
    }
    vector<MoveStats> stats;
    for (Node* child : root->children) {
        MoveStats moveStats;

        moveStats.move = child->move;
        moveStats.visits = child->visits;
        moveStats.wins = child->wins;
        moveStats.winRate =
            static_cast<double>(child->wins) / child->visits;

        stats.push_back(moveStats);
    }

    MCTSResult result;

    result.bestMove = bestChild->move;
    result.stats = stats;

    return result;

}

