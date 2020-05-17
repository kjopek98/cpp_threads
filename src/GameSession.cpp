#include "GameSession.h"

#include <chrono>
#include <random>

GameSession::GameSession(unsigned int id, Server *server) : id(id), currentGameSessionState(GameSessionState::Idle),
                                                            server(server), playersCollected(false),
                                                            cancelRequestProcessed(false),
                                                            collectPlayersRequestCanceled(false) {
}

[[noreturn]] void GameSession::operator()() {
    std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> idleTime(1, 10);
    std::uniform_int_distribution<> nPlayers(1, 1);
    std::uniform_int_distribution<> gameSessionDuration(15, 30);
    std::uniform_real_distribution<> cancelSessionRoll(0, 1);
    RequestData requestData;

    while (true) {
        currentGameSessionState = GameSessionState::Idle;
        std::this_thread::sleep_for(std::chrono::seconds(idleTime(gen)));

        requestData.type = RequestData::Type::RequestPlayers;
        requestData.gameSession = this;
        requestData.nPlayers = nPlayers(gen);
        server->requestServerAction(requestData);
        currentGameSessionState = GameSessionState::WaitingForPlayers;
        {
            std::unique_lock<std::mutex> lock(collectedPlayersMutex);
            collectedPlayersCondVar.wait(lock, [this] { return playersCollected; });
            playersCollected = false;
        }
        // Further modification of playersCollected or currentSessionPlayers is an error - code does not expect this

        if (cancelSessionRoll(gen) < 0.1) {
            requestData.type = RequestData::Type::CancelRequestPlayers;
            requestData.gameSession = this;
            requestData.acquiredPlayers = &currentSessionPlayers;
            server->requestServerAction(requestData);
            currentGameSessionState = GameSessionState::CancelingSession;
            {
                std::unique_lock<std::mutex> lock(collectPlayersRequestCancelMutex);
                collectPlayersRequestCancelCondVar.wait(lock, [this] { return cancelRequestProcessed; });
                cancelRequestProcessed = false;
            }
            if (collectPlayersRequestCanceled) {
                currentSessionPlayers.clear();
                continue;
            }
        }

        for (Player *player : currentSessionPlayers) {
            player->onGameFound(this);
        }

        currentGameSessionState = GameSessionState::Running;
        std::this_thread::sleep_for(std::chrono::seconds(gameSessionDuration(gen)));
        for (Player *player : currentSessionPlayers) {
            player->onGameEnd();
        }
        currentSessionPlayers.clear();
    }
}

void GameSession::onPlayersCollected(const std::vector<Player *> &collectedPlayers) {
    {
        std::unique_lock<std::mutex> lock(collectedPlayersMutex);
        currentSessionPlayers = collectedPlayers;
        playersCollected = true;
    }
    collectedPlayersCondVar.notify_one();
}

void GameSession::onCancelRequestPlayers(bool wasRequestSuccessful) {
    {
        std::unique_lock<std::mutex> lock(collectPlayersRequestCancelMutex);
        collectPlayersRequestCanceled = wasRequestSuccessful;
        cancelRequestProcessed = true;
    }
    collectPlayersRequestCancelCondVar.notify_one();
}

unsigned int GameSession::getId() const {
    return id;
}

GameSession::GameSessionState GameSession::getState() const {
    return currentGameSessionState;
}

std::vector<Player *> GameSession::getCurrentPlayers() const {
    return currentSessionPlayers;
}
