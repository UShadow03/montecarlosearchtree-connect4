#ifndef MCTS_HPP
#define MCTS_HPP
#include <vector>
#include "GameState.hpp"
using namespace std;

struct Node {
    GameState state;

    Node* parent;

    vector<Node*> children;

    int visits;
    int wins;

    Node(GameState state, Node* parent = nullptr);
};

class MCTS {
public:
    int simulate(GameState state);
    Node* expand(Node* node);
};

#endif