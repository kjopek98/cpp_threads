#ifndef CPP_THREADS_PLAYER_H
#define CPP_THREADS_PLAYER_H

#include <string>
#include <mutex>
#include <condition_variable>

#include "GameSession.h"

class Server;

class Player {

public:

    Player(std::string name, Server *server);

    enum class PlayerState {
        Idle,
        WaitingForGame,
        CancelingGameRequest,
        Playing
    };

    // Player loop (start thread with this)
    [[noreturn]]
    void operator()();

    void onGameFound(const GameSession *inGameSession);

    void onGameEnd();

    void onGameSessionRequestCancel(bool wasRequestCanceled);

    [[nodiscard]] PlayerState getState() const;

    [[nodiscard]] std::string getName() const;

private:

    std::string playerName;
    PlayerState currentPlayerState;
    Server *server;

    std::mutex gameSessionMutex;
    std::condition_variable gameSessionCondVar;
    bool gameSessionFound;
    const GameSession *gameSession;

    std::mutex cancelRequestGameSessionMutex;
    std::condition_variable cancelRequestGameSessionCondVar;
    bool cancelRequestProcessed;
    bool gameSessionRequestCanceled;

    std::mutex gameEndMutex;
    std::condition_variable gameEndCondVar;
    bool gameEnded;
};

#include "Server.h"

#endif //CPP_THREADS_PLAYER_H
