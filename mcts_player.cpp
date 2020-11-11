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

Node * Node::getBestChild(double CP) const noexcept {
    double maxWeight = std::numeric_limits<double>::lowest();
    Node *bestChild = nullptr;
    for (auto &child : children) {
        Node *childNode = child.second.get();
        double weight = childNode->reward / childNode->accessCount
                        + CP * sqrt(2 * log(accessCount) / childNode->accessCount);

        if (weight > maxWeight) {
            bestChild = childNode;
            maxWeight = weight;
        }
    }

    return bestChild;
}

MCTSPlayer::MCTSPlayer(const Game &currentGame, Chess myChess, int timeBudget): // NOLINT
    BasePlayer(currentGame, myChess), timeBudgetPerStep(timeBudget) {
    treeRoot = std::make_unique<Node>();
    std::random_device rd;
    randomEngine = std::default_random_engine{rd()};
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

    Node *bestChild = treeRoot->getBestChild(CP);
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

            auto randomMove = *std::next(
                    std::begin(availableMoves),
                    getRandomNumber(0, static_cast<int>(availableMoves.size()) - 1));

            Node * child = new Node(randomMove);
            child->isCurrentPlayerMove = !node->isCurrentPlayerMove;
            if (!simulationGame.tryPlace(randomMove, getMoveChessType(child->isCurrentPlayerMove)))
                throw std::runtime_error("Cannot place move");

            child->winner = simulationGame.checkWin(randomMove);
            node->addChild(child);

            return child;
        } else {
            if (node->isCurrentPlayerMove) {
                if (node->children.empty())
                    return node;

                 Node * randomChild = std::next(
                        std::begin(node->children),
                        getRandomNumber(0, static_cast<int>(node->children.size()) - 1))->second.get();

                 node = randomChild;
            } else {
                Node * bestChild = node->getBestChild(CP);
                if (bestChild == nullptr) // No child, also terminal status
                    return node;

                node = bestChild;
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
    while (!winner.has_value() || moveCount <= 7) {
        moveCount++;
        auto availableMoves = simulationGame.getAvailableMoves();

        if (availableMoves.empty())
            break;

        auto randomMove = *std::next(
                std::begin(availableMoves),
                getRandomNumber(0, static_cast<int>(availableMoves.size()) - 1));

        if (!simulationGame.tryPlace(randomMove, getMoveChessType(isCurrentPlayer)))
            throw std::runtime_error("Cannot place move");

        winner = simulationGame.checkWin(randomMove);
        isCurrentPlayer = !isCurrentPlayer;
    }

    if (!winner.has_value()) {
        return 0.0;
    } else if (winner.value() != chess) {
        return -1.0;
    } else {
        return 0.5 * pow(Gamma, moveCount);
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

constexpr Chess MCTSPlayer::getMoveChessType(bool isCurrentPlayer) const noexcept {
    if (chess == Player1) {
        return isCurrentPlayer ? Player1 : Player2;
    } else {
        return isCurrentPlayer ? Player2 : Player1;
    }
}

void MCTSPlayer::startTimer() noexcept {
    startTime = std::chrono::system_clock::now();
}

bool MCTSPlayer::checkWithinTimeLimit() const noexcept {
    auto currentTime = std::chrono::system_clock::now();
    return currentTime - startTime < timeBudgetPerStep;
}

int MCTSPlayer::getRandomNumber(int lower, int upper) noexcept {
    std::uniform_int_distribution<int> uniformDist(lower, upper);
    return uniformDist(randomEngine);
}