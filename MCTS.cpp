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


    int move = -1;

    // Prefer an unexpanded move that wins immediately for this node's mover.
    for (int m : moves) {
        GameState testState = node->state;
        testState.droppingShape(m + 1);
        if (testState.checkWin(node->state.playerTurn)) {
            move = m;
            break;
        }
    }

    // Otherwise, prefer an unexpanded move that blocks the opponent's
    // immediate win.
    if (move == -1) {
        int opponent = (node->state.playerTurn == 1) ? 2 : 1;
        for (int m : moves) {
            GameState testState = node->state;
            testState.playerTurn = opponent;
            testState.droppingShape(m + 1);
            if (testState.checkWin(opponent)) {
                move = m;
                break;
            }
        }
    }

    // If no moves was selected, pick the first move.
    if (move = -1) {
        move = moves[0];
    }


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

        // Does the opponent have an immediete winning move that must be blocked?
        int blockingMove = -1;
        int opponent = (state.playerTurn == 1) ? 2 : 1;
        for (int move : moves) {
            GameState testState = state;
            // Simulate the opponent dropping in this column instead of us.
            testState.playerTurn = opponent;
            testState.droppingShape(move + 1);
            if (testState.checkWin(opponent)) {
                blockingMove = move;
                break;
            }
        }

        int chosenMove;

        // If they have a winning move, play it.
        if (winningMove != -1) {
            chosenMove = winningMove;
        }
        // If the opponent has a winning move next turn, block it!
        else if (blockingMove != -1) {
            chosenMove = blockingMove;
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

void MCTS::backpropagate(Node* node, int result) {
    while (node != nullptr) {
        node->visits++;
        int moverPlayer = (node->state.playerTurn == 1) ? 2 : 1; // playerTurn was already flipped in expand()
        if (result == moverPlayer) {
            node->wins++;
        }
        node = node->parent;
    }
}

static void deleteTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    for (Node* child : node->children) {
        deleteTree(child);
    }
    delete node;
}

MCTSResult MCTS::findBestMove(GameState state, int simulations, int player) {
    Node* root = new Node(state);
    MCTS mcts;
    for (int i = 0; i < simulations; ++i) {
        Node* selection = mcts.select(root);
        if (selection->state.checkWin(1) || selection->state.checkWin(2) || selection->state.checkDraw()) {
            int result;
            if (selection->state.checkWin(1)) {
                result = 1;
            }
            else if (selection->state.checkWin(2)) {
                result = 2;
            }
            else {
                result = 0;
            }
            mcts.backpropagate(selection, result);
            continue;
        }
        Node* expansion = mcts.expand(selection);
        if (expansion == nullptr) {
            continue;
        }
        int result = mcts.simulate(expansion->state);
        mcts.backpropagate(expansion, result);
    }

    MCTSResult result{};

    if (root->children.empty()) {
        deleteTree(root);
        result.bestMove = -1;
        return result;
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
        moveStats.winRate = child->visits == 0
            ? 0.0
            : static_cast<double>(child->wins) / child->visits;

        stats.push_back(moveStats);
    }
    result.bestMove = bestChild->move;
    result.stats = stats;
    
    deleteTree(root);

    return result;

}

