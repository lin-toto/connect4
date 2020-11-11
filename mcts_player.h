#ifndef CONNECT4_MCTS_PLAYER_H
#define CONNECT4_MCTS_PLAYER_H

#include "board.h"
#include "player.h"
#include <unordered_map>
#include <memory>
#include <chrono>
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
    [[nodiscard]] Node * getBestChild(double CP) const noexcept;
};

class MCTSPlayer: public BasePlayer {
public:
    MCTSPlayer(const Game &currentGame, Chess myChess, int timeBudget);
    Pos requestNextMove(std::optional<Pos> lastOpponentMovePosition) override;
private:
    std::unique_ptr<Node> treeRoot;

    std::chrono::milliseconds timeBudgetPerStep;
    std::chrono::time_point<std::chrono::system_clock> startTime;

    std::default_random_engine randomEngine;

    const double CP = 0.707, Gamma = 0.6;

    [[nodiscard]] constexpr Chess getMoveChessType(bool isCurrentPlayer) const noexcept;

    void makeNewRoot(Node *node);
    Node * treePolicy(Game &game);
    double defaultPolicy(Node *node, Game simulationGame);
    void backup(Node *node, double reward) noexcept;

    void startTimer() noexcept;
    [[nodiscard]] bool checkWithinTimeLimit() const noexcept;
    [[nodiscard]] int getRandomNumber(int lower, int upper) noexcept;
};

#endif //CONNECT4_MCTS_PLAYER_H
