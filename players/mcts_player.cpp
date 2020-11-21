#include "mcts_player.h"

void Node::addChild(Node *child) {
    if (!child->currentMove.has_value())
        throw std::runtime_error("Current move must be specified");

    const Pos move = child->currentMove.value();
    if (children.find(move) != children.end())
        throw std::runtime_error("Child with the same move already exists in this node");

    child->parent = this;
    children[move] = std::unique_ptr<Node>(child);
};

Node * Node::getChild(const Pos &move) const noexcept {
    auto child = children.find(move);
    if (child == children.end())
        return nullptr;

    return child->second.get();
}

Node * Node::getBestChild(double CP, bool isCurrentPlayer) const noexcept {
    double maxWeight = std::numeric_limits<double>::lowest();
    Node *bestChild = nullptr;
    for (auto &child : children) {
        Node *childNode = child.second.get();
        double weight = (childNode->reward / childNode->accessCount) * (isCurrentPlayer ? 1 : -1)
                        + CP * sqrt(2 * log(accessCount) / childNode->accessCount);

        if (weight > maxWeight) {
            bestChild = childNode;
            maxWeight = weight;
        }
    }

    return bestChild;
}

MCTSPlayer::MCTSPlayer(const Game &currentGame, Chess myChess, int timeBudget):
    ComputerPlayer(currentGame, myChess, timeBudget) {
    treeRoot = std::make_unique<Node>();
}

Pos MCTSPlayer::requestNextMove(std::optional<Pos> lastOpponentMovePosition) {
    if (lastOpponentMovePosition.has_value()) {
        const Pos &move = lastOpponentMovePosition.value();
        Node * child = treeRoot->getChild(move);
        if (child == nullptr) {
            child = new Node(move);
            treeRoot->addChild(child);
        }

        makeNewRoot(child);
    }

    startTimer();
    while (checkWithinTimeLimit()) {
        Game simulationGame = game;
        Node * node = treePolicy(simulationGame);
        double reward = defaultPolicy(node, std::move(simulationGame));
        backup(node, reward);
    }

    Node *bestChild = treeRoot->getBestChild(-CP, true);
    auto move = bestChild->currentMove;
    if (!move.has_value())
        throw std::runtime_error("Current move must be specified");

    makeNewRoot(bestChild);

    return move.value();
}

Node * MCTSPlayer::treePolicy(Game &simulationGame) {
    Node *node = treeRoot.get();
    while (!node->winner.has_value()) { // Until node is terminal
        std::unordered_set<Pos> availableMoves = simulationGame.getAvailableMoves();
        if (availableMoves.size() != node->children.size()) {
            // Some available moves are not tried, expand the tree
            for (auto &kv: node->children) {
                availableMoves.erase(availableMoves.find(kv.first));
            }

            auto randomMove = *getRandomElement(availableMoves);

            Node * child = new Node(randomMove);
            child->isCurrentPlayerMove = !node->isCurrentPlayerMove;
            if (!simulationGame.tryPlace(randomMove, getMoveChessType(child->isCurrentPlayerMove)))
                throw std::runtime_error("Cannot place move");

            child->winner = simulationGame.checkWin(randomMove);
            node->addChild(child);

            return child;
        } else {
            if (node->children.empty())
                return node;

            double random = getRandomNumber(0, 1);

            if (random < randomExploreFactor) {
                node = getRandomElement(node->children)->second.get();
            } else {
                 node = node->getBestChild(CP, node->isCurrentPlayerMove);
            }

            if (node->currentMove.has_value()) {
                if (!simulationGame.tryPlace(node->currentMove.value(), getMoveChessType(node->isCurrentPlayerMove)))
                    throw std::runtime_error("Cannot place move");
            }
        }
    }

    return node;
}

double MCTSPlayer::defaultPolicy(Node *node, Game simulationGame) {
    if (node == nullptr)
        throw std::runtime_error("Node is nullptr");

    if (!node->currentMove.has_value())
        throw std::runtime_error("Current move must be specified");

    std::optional<Chess> winner = node->winner;
    bool isCurrentPlayer = !node->isCurrentPlayerMove;
    int moveCount = 0;
    while (!winner.has_value() && !game.checkDraw() && moveCount <= 11) {
        moveCount++;
        auto availableMoves = simulationGame.getAvailableMoves();

        if (availableMoves.empty())
            break;

        auto randomMove = *getRandomElement(availableMoves);

        if (!simulationGame.tryPlace(randomMove, getMoveChessType(isCurrentPlayer)))
            throw std::runtime_error("Cannot place move");

        winner = simulationGame.checkWin(randomMove);
        isCurrentPlayer = !isCurrentPlayer;
    }

    if (!winner.has_value()) {
        return 0.0;
    } else if (winner.value() != chess) {
        if (moveCount <= 2) {
            return -100000.0;
        } else return -1.0 * pow(Gamma, moveCount);
    } else {
        return 1.0 * pow(Gamma, moveCount);
    }
}

void MCTSPlayer::backup(Node *node, double reward) noexcept { // NOLINT
    for (Node *ptr = node; ptr != nullptr; ptr = ptr->parent) {
        ptr->accessCount++;
        ptr->reward += reward;
        reward *= Gamma;
    }
}

void MCTSPlayer::makeNewRoot(Node *node) {
    if (node->parent == nullptr)
        throw std::runtime_error("Node has no parent, probably is already root");

    if (!node->currentMove.has_value())
        throw std::runtime_error("Current move must be specified");

    auto nodeInChildrenMap = node->parent->children.find(node->currentMove.value());
    if (nodeInChildrenMap == node->parent->children.end())
        throw std::runtime_error("Node is not found in parent children map");

    node->parent = nullptr;
    treeRoot = std::move(nodeInChildrenMap->second);

    // Unnecessary parts of the tree will be cleaned up automatically, because we're using unique_ptr
}
