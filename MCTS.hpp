#ifndef MCTS_HPP
#define MCTS_HPP
#include <vector>
#include "GameState.hpp"
using namespace std;

struct Node {
    GameState state;

    Node* parent;

    vector<Node*> children;

    int move;
    int visits;
    int wins;

    Node(GameState state, Node* parent = nullptr, int move = -1);
};

struct MoveStats {
    int move;
    int visits;
    int wins;
    double winRate;
};

struct MCTSResult {
    int bestMove;
    std::vector<MoveStats> stats;
};

class MCTS {
public:
    int simulate(GameState state);
    Node* expand(Node* node);
    bool isFullyExpanded(Node* node);
    Node* select(Node* node);
    double calculateUCT(Node* node);
    void backpropagate(Node* node, int result, int player);
    MCTSResult findBestMove(GameState state, int simulations, int player);
};

#endif