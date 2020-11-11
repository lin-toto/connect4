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

MCTSPlayer::MCTSPlayer(const Game &currentGame, int timeBudget):
    BasePlayer(currentGame), timeBudgetPerStep(timeBudget), simulationGame(currentGame) {
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

    while (checkWithinTimeLimit()) {
        Node * node = treePolicy();
        double reward = defaultPolicy();
        backup(node, reward);
    }

    auto move = bestChild(treeRoot.get())->currentMove;
    if (!move.has_value())
        throw std::runtime_error("Current move must be specified");

    return move.value();
}

Node * MCTSPlayer::treePolicy() {

}

Node * MCTSPlayer::expand(Node *node) {

}

Node * MCTSPlayer::bestChild(Node *node) const {
    if (node == nullptr)
        throw std::runtime_error("Node is nullptr");

    double maxWeight = 0;
    Node *bestChild = nullptr;
    for (auto &child : node->children) {
        Node *childNode = child.second.get();
        double weight = childNode->reward / childNode->accessCount
                + CP * sqrt(2 * log(node->accessCount) / childNode->accessCount);

        if (weight > maxWeight) {
            bestChild = childNode;
            maxWeight = weight;
        }
    }

    return bestChild;
}

double MCTSPlayer::defaultPolicy() {

}

void MCTSPlayer::backup(Node *node, double reward) noexcept { // NOLINT
    for (Node *ptr = node; ptr != nullptr; ptr = node->parent) {
        ptr->accessCount++;
        ptr->reward += reward;
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

void MCTSPlayer::startTimer() noexcept {
    startTime = std::chrono::system_clock::now();
}

bool MCTSPlayer::checkWithinTimeLimit() const noexcept {
    auto currentTime = std::chrono::system_clock::now();
    return currentTime - startTime >= timeBudgetPerStep;
}