#ifndef CPP_THREADS_GAMESESSION_H
#define CPP_THREADS_GAMESESSION_H

#include <vector>
#include <mutex>
#include <condition_variable>

class Player;

class Server;

class GameSession {

public:

    GameSession(unsigned int id, Server *server);

    enum class GameSessionState {
        Idle,
        WaitingForPlayers,
        CancelingSession,
        Running
    };

    [[noreturn]] // Game session loop
    void operator()();

    void onPlayersCollected(const std::vector<Player *> &collectedPlayers);

    void onCancelRequestPlayers(bool wasRequestSuccessful);

    [[nodiscard]] unsigned int getId() const;

    [[nodiscard]] GameSessionState getState() const;

    [[nodiscard]] std::vector<Player *> getCurrentPlayers() const;

private:

    unsigned int id;
    GameSessionState currentGameSessionState;
    Server *server;

    std::mutex collectedPlayersMutex;
    std::condition_variable collectedPlayersCondVar;
    bool playersCollected;
    std::vector<Player *> currentSessionPlayers;

    std::mutex collectPlayersRequestCancelMutex;
    std::condition_variable collectPlayersRequestCancelCondVar;
    bool cancelRequestProcessed;
    bool collectPlayersRequestCanceled;
};

#include "Player.h"
#include "Server.h"


#endif //CPP_THREADS_GAMESESSION_H
