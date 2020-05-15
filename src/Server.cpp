#include "Server.h"

#include <thread>
#include <chrono>

Server::Server() : currentServerState(ServerState::Idle), serverWorkers(N_SERVER_WORKERS) {}

void Server::requestServerAction(const RequestData &requestData) {
    switch (requestData.type) {
        case RequestData::Type::RequestGame:
            serverWorkers.addJob([this, requestData] { requestGameSession(*(requestData.player)); });
            break;
        case RequestData::Type::CancelRequestGame:
            serverWorkers.addJob([this, requestData] { cancelRequestGameSession(*(requestData.player)); });
            break;
        case RequestData::Type::RequestPlayers:
            serverWorkers.addJob(
                    [this, requestData] { requestPlayers(*(requestData.gameSession), requestData.nPlayers); });
            break;
        case RequestData::Type::CancelRequestPlayers:
            serverWorkers.addJob([this, requestData] { cancelRequestPlayers(*(requestData.acquiredPlayers)); });
            break;
    }
}

void Server::requestGameSession(Player &player) {
    {
        std::unique_lock<std::mutex> locker(playerQueueMutex);
        playerQueueCondVar.wait(locker, [this] { return waitingPlayerQueue.size() < PLAYER_QUEUE_SIZE; });

        currentServerState = ServerState::RequestGameSession;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        waitingPlayerQueue.emplace_back(&player);
        currentServerState = ServerState::Idle;
    }
    playerQueueCondVar.notify_all();
}

bool Server::cancelRequestGameSession(Player &player) {
    {
        std::unique_lock<std::mutex> lock(playerQueueMutex);

        currentServerState = ServerState::CancelRequestGameSession;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (unsigned int i = 0; i < waitingPlayerQueue.size(); ++i) {
            if (waitingPlayerQueue[i] == &player) {
                waitingPlayerQueue.erase(waitingPlayerQueue.begin() + i);
                playerQueueCondVar.notify_all();
                currentServerState = ServerState::Idle;
                return true;
            }
        }
        currentServerState = ServerState::Idle;
    }
    return false;
}

void Server::requestPlayers(GameSession &gameSession, unsigned int nPlayers) {
    std::vector<Player *> assignedPayers;
    {
        std::unique_lock<std::mutex> locker(playerQueueMutex);
        for (unsigned int i = 0; i < nPlayers; ++i) {
            playerQueueCondVar.wait(locker, [this] { return !waitingPlayerQueue.empty(); });

            currentServerState = ServerState::RequestPlayers;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            assignedPayers.emplace_back(waitingPlayerQueue.front());
            waitingPlayerQueue.erase(waitingPlayerQueue.begin());
            currentServerState = ServerState::Idle;
        }
    }
    playerQueueCondVar.notify_all();
    gameSession.onPlayersCollected(assignedPayers);
}

bool Server::cancelRequestPlayers(const std::vector<Player *> &acquiredPlayers) {
    {
        std::unique_lock<std::mutex> lock(playerQueueMutex);

        currentServerState = ServerState::CancelRequestPlayers;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (waitingPlayerQueue.size() + acquiredPlayers.size() > PLAYER_QUEUE_SIZE) {
            currentServerState = ServerState::Idle;
            return false;
        }
        waitingPlayerQueue.insert(waitingPlayerQueue.end(), acquiredPlayers.begin(), acquiredPlayers.end());
        currentServerState = ServerState::Idle;
    }
    playerQueueCondVar.notify_all();
    return true;
}

Server::ServerState Server::getState() const {
    return currentServerState;
}
