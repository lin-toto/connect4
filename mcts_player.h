#ifndef CONNECT4_MCTS_PLAYER_H
#define CONNECT4_MCTS_PLAYER_H

#include "board.h"
#include "player.h"
#include <unordered_map>
#include <memory>
#include <chrono>
#include <optional>
#include <cmath>

struct Node {
    bool isTerminal = false;
    std::optional<Pos> currentMove = std::nullopt;
    double reward = 0;
    int accessCount = 1;

    Node *parent = nullptr;
    std::unordered_map<Pos, std::unique_ptr<Node>> children;

    Node() = default;
    explicit Node(Pos move): currentMove(move) {}
    void addChild(Node *child);
    [[nodiscard]] Node * getChild(const Pos &move) const noexcept;
};

class MCTSPlayer: virtual public BasePlayer {
public:
    MCTSPlayer(const Game &currentGame, int timeBudget);
    virtual Pos requestNextMove(std::optional<Pos> lastOpponentMovePosition);
private:
    Game simulationGame;
    std::unique_ptr<Node> treeRoot;

    std::chrono::microseconds timeBudgetPerStep;
    std::chrono::time_point<std::chrono::system_clock> startTime;

    const double CP = 0.707;

    void makeNewRoot(Node *node);
    Node * treePolicy();
    Node * expand(Node *node);
    Node * bestChild(Node *node) const;
    double defaultPolicy();
    void backup(Node *node, double reward) noexcept;

    void startTimer() noexcept;
    [[nodiscard]] bool checkWithinTimeLimit() const noexcept;
};

#endif //CONNECT4_MCTS_PLAYER_H
