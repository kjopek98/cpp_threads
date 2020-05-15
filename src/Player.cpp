#include "Player.h"

#include <random>
#include <ctime>
#include <chrono>


Player::Player(std::string name, Server *server) : playerName(std::move(name)), currentPlayerState(PlayerState::Idle),
                                                   server(server), gameSessionFound(false), gameSession(nullptr),
                                                   cancelRequestProcessed(false), gameSessionRequestCanceled(false),
                                                   gameEnded(false) {
}

[[noreturn]] void Player::operator()() {
    std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> idleTime(1, 10);
    std::uniform_int_distribution<> waitForGameTime(15, 25);
    RequestData requestData;

    while (true) {
        currentPlayerState = PlayerState::Idle;
        std::this_thread::sleep_for(std::chrono::seconds(idleTime(gen)));

        requestData.type = RequestData::Type::RequestGame;
        requestData.player = this;
        server->requestServerAction(requestData);
        currentPlayerState = PlayerState::WaitingForGame;
        std::this_thread::sleep_for(std::chrono::seconds(waitForGameTime(gen)));
        {
            std::unique_lock<std::mutex> lock(gameSessionMutex);
            if (!gameSessionFound) {
                gameSessionMutex.unlock();
                currentPlayerState = PlayerState::CancelingGameRequest;
                requestData.type = RequestData::Type::CancelRequestGame;
                requestData.player = this;
                server->requestServerAction(requestData);
                {
                    std::unique_lock<std::mutex> inLock(cancelRequestGameSessionMutex);
                    cancelRequestGameSessionCondVar.wait(inLock, [this] { return cancelRequestProcessed; });
                    cancelRequestProcessed = false;
                    if (gameSessionRequestCanceled) {
                        continue;
                    }
                }
            }
        }
        // Further modification of gameSessionFound or gameSession is an error - code does not expect this
        gameSessionFound = false;
        currentPlayerState = PlayerState::Playing;
        {
            std::unique_lock<std::mutex> lock(gameEndMutex);
            gameEndCondVar.wait(lock, [this] { return gameEnded; });
            gameEnded = false;
        }
    }
}

void Player::onGameFound(const GameSession *inGameSession) {
    {
        std::unique_lock<std::mutex> lock(gameSessionMutex);
        this->gameSession = inGameSession;
        gameSessionFound = true;
    }
}

void Player::onGameEnd() {
    {
        std::unique_lock<std::mutex> lock(gameEndMutex);
        gameEnded = true;
    }
    gameEndCondVar.notify_one();
}

void Player::onGameSessionRequestCancel(bool wasRequestCanceled) {
    {
        std::unique_lock<std::mutex> lock(cancelRequestGameSessionMutex);
        gameSessionRequestCanceled = wasRequestCanceled;
        cancelRequestProcessed = true;
    }
    cancelRequestGameSessionCondVar.notify_one();
}

Player::PlayerState Player::getState() const {
    return currentPlayerState;
}

std::string Player::getName() const {
    return playerName;
}
