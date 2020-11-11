#ifndef CONNECT4_MCTS_PLAYER_H
#define CONNECT4_MCTS_PLAYER_H

#include "board.h"
#include "player.h"
#include <unordered_map>
#include <memory>

class Node {
public:

private:
    Node *parent;
    std::unordered_map<Pos, std::unique_ptr<Node>> children;
};

class MCTSPlayer: virtual public BasePlayer {
    
};

#endif //CONNECT4_MCTS_PLAYER_H
