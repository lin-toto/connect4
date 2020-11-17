#ifndef CONNECT4_QLEARN_PLAYER_H
#define CONNECT4_QLEARN_PLAYER_H

#include "board.h"
#include "player.h"
#include <unordered_set>
#include <unordered_map>
#include <functional>

class QLearnPlayer: public ComputerPlayer {
public:
    explicit QLearnPlayer(const Game &currentGame, Chess myChess, int timeBudget)
        : ComputerPlayer(currentGame, myChess, timeBudget) {}
    Pos requestNextMove(std::optional<Pos> lastOpponentMovePosition) override;
private:
    std::unordered_map<Board, std::unordered_map<Pos, double>> stateActionRewardMap;

    const double Alpha = 0.5, Gamma = 0.8, randomExploreFactor = 0.1;

    std::optional<Pos> chooseAction(const Game &simulationGame, bool isCurrentPlayer) noexcept;
    void learn(const Board &oldBoard, const Pos &move, const Board &newBoard, double reward) noexcept;

    [[nodiscard]] inline double getRewardByStateAction(const Board &board, const Pos &pos) const noexcept {
        auto boardMapElement = stateActionRewardMap.find(board);
        if (boardMapElement == stateActionRewardMap.end())
            return 1.0;

        auto rewardElement = boardMapElement->second.find(pos);
        if (rewardElement == boardMapElement->second.end())
            return 1.0;

        return rewardElement->second;
    }

    inline void setRewardByStateAction(const Board &board, const Pos &pos, double reward) noexcept {
        auto boardMapElement = stateActionRewardMap.find(board);
        if (boardMapElement == stateActionRewardMap.end()) {
            stateActionRewardMap[board] = std::unordered_map<Pos, double>();
        }

        stateActionRewardMap[board][pos] = reward;
    }
};


#endif //CONNECT4_QLEARN_PLAYER_H
