#include "Server.h"

#include <thread>
#include <chrono>

Server::Server() : serverWorkers(N_SERVER_WORKERS) {}

void Server::requestServerAction(const RequestData &requestData) {
    switch (requestData.type) {
        case RequestData::Type::RequestGame:
            serverWorkers.addJob([this, requestData] { requestGameSession(*(requestData.player)); },
                                 States::ServerState::RequestGameSession);
            break;
        case RequestData::Type::CancelRequestGame:
            serverWorkers.addJob([this, requestData] { cancelRequestGameSession(*(requestData.player)); },
                                 States::ServerState::CancelRequestGameSession);
            break;
        case RequestData::Type::RequestPlayers:
            serverWorkers.addJob(
                    [this, requestData] { requestPlayers(*(requestData.gameSession), requestData.nPlayers); },
                    States::ServerState::RequestPlayers);
            break;
        case RequestData::Type::CancelRequestPlayers:
            serverWorkers.addJob([this, requestData] {
                cancelRequestPlayers(*(requestData.gameSession), *(requestData.acquiredPlayers));
            }, States::ServerState::CancelRequestPlayers);
            break;
    }
}

void Server::requestGameSession(Player &player) {
    {
        std::unique_lock<std::mutex> locker(playerQueueMutex);
        playerQueueCondVar.wait(locker, [this] { return waitingPlayerQueue.size() < PLAYER_QUEUE_SIZE; });

        std::this_thread::sleep_for(std::chrono::seconds(3));
        waitingPlayerQueue.emplace_back(&player);
    }
    playerQueueCondVar.notify_all();
}

void Server::cancelRequestGameSession(Player &player) {
    {
        std::unique_lock<std::mutex> lock(playerQueueMutex);

        std::this_thread::sleep_for(std::chrono::seconds(2));
        for (unsigned int i = 0; i < waitingPlayerQueue.size(); ++i) {
            if (waitingPlayerQueue[i] == &player) {
                waitingPlayerQueue.erase(waitingPlayerQueue.begin() + i);
                lock.unlock();
                playerQueueCondVar.notify_all();
                player.onGameSessionRequestCancel(true);
                return;
            }
        }
    }
    player.onGameSessionRequestCancel(false);
}

void Server::requestPlayers(GameSession &gameSession, unsigned int nPlayers) {
    std::vector<Player *> assignedPayers;

    for (unsigned int i = 0; i < nPlayers; ++i) {
        {
            std::unique_lock<std::mutex> locker(playerQueueMutex);
            playerQueueCondVar.wait(locker, [this] { return !waitingPlayerQueue.empty(); });

            std::this_thread::sleep_for(std::chrono::milliseconds(800));
            assignedPayers.emplace_back(waitingPlayerQueue.front());
            waitingPlayerQueue.erase(waitingPlayerQueue.begin());
        }
        playerQueueCondVar.notify_all();
    }
    gameSession.onPlayersCollected(assignedPayers);
}

void Server::cancelRequestPlayers(GameSession &gameSession, const std::vector<Player *> &acquiredPlayers) {
    {
        std::unique_lock<std::mutex> lock(playerQueueMutex);

        std::this_thread::sleep_for(std::chrono::seconds(2));
        if (waitingPlayerQueue.size() + acquiredPlayers.size() > PLAYER_QUEUE_SIZE) {
            gameSession.onCancelRequestPlayers(false);
            return;
        }
        waitingPlayerQueue.insert(waitingPlayerQueue.end(), acquiredPlayers.begin(), acquiredPlayers.end());
    }
    playerQueueCondVar.notify_all();
    gameSession.onCancelRequestPlayers(true);
}

const std::vector<States::ServerState> &Server::getWorkerStates() const {
    return serverWorkers.getWorkerStates();
}

const std::vector<Player *> &Server::getWaitingPlayerQueue() const {
    return waitingPlayerQueue;
}


