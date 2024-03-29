#ifndef CONNECT4_MCTS_PLAYER_H
#define CONNECT4_MCTS_PLAYER_H

#include "board.h"
#include "base_player.h"
#include <unordered_map>
#include <memory>
#include <optional>
#include <cmath>
#include <limits>
#include <random>

struct Node {
    std::optional<Chess> winner = std::nullopt;
    bool isCurrentPlayerMove = false;
    std::optional<Pos> currentMove = std::nullopt;
    double reward = 0;
    int accessCount = 1;

    Node *parent = nullptr;
    std::unordered_map<Pos, std::unique_ptr<Node>> children;

    Node() = default;
    explicit Node(Pos move): currentMove(move) {}
    void addChild(Node *child);
    [[nodiscard]] Node * getChild(const Pos &move) const noexcept;
    [[nodiscard]] Node * getBestChild(double CP, bool isCurrentPlayer) const noexcept;
};

class MCTSPlayer: public ComputerPlayer {
public:
    MCTSPlayer(const Game &currentGame, Chess myChess, int timeBudget);
    Pos requestNextMove(std::optional<Pos> lastOpponentMovePosition) override;
private:
    std::unique_ptr<Node> treeRoot;

    const double CP = 0.5, Gamma = 0.8, randomExploreFactor = 0.2;

    void makeNewRoot(Node *node);
    Node * treePolicy(Game &game);
    double defaultPolicy(Node *node, Game &simulationGame);
    void backup(Node *node, double reward) noexcept;
};

#endif //CONNECT4_MCTS_PLAYER_H
