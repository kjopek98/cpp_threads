#include "Server.h"

#include <thread>
#include <chrono>

Server::Server() : currentServerState(ServerState::Idle) {
}

void Server::requestGameSession(Player &player) {
    std::unique_lock<std::mutex> locker(queueMutex);
    queueCondVar.wait(locker, [this] { return waitingPlayerQueue.size() < QUEUE_SIZE; });

    currentServerState = ServerState::RequestGameSession;
    waitingPlayerQueue.push(&player);
    currentServerState = ServerState::Idle;

    locker.unlock();
    queueCondVar.notify_all();
}

bool Server::cancelRequestGameSession(Player &player) {
    return false;
}

void Server::requestPlayers(GameSession &gameSession, unsigned int nPlayers) {
    std::vector<Player *> assignedPayers;
    std::unique_lock<std::mutex> locker(queueMutex);
    for (unsigned i = 0; i < nPlayers; ++i) {
        queueCondVar.wait(locker, [this] { return !waitingPlayerQueue.empty(); });
        currentServerState = ServerState::RequestPlayers;
        // maybe slow down with
//        std::this_thread::sleep_for(std::chrono::seconds(2));
        assignedPayers.emplace_back(waitingPlayerQueue.front());
        waitingPlayerQueue.pop();
        currentServerState = ServerState::Idle;
    }
    locker.unlock();
    queueCondVar.notify_all();
    gameSession.onPlayersCollected(assignedPayers);
}

bool Server::cancelRequestPlayers(const std::vector<Player *> &acquiredPlayers) {
    return false;
}

Server::ServerState Server::getState() const {
    return currentServerState;
}
