#ifndef CPP_THREADS_PLAYER_H
#define CPP_THREADS_PLAYER_H

#include <string>

#include "GameSession.h"

class Server;

class Player {

public:

    Player(std::string name, Server *server);

    enum class PlayerState {
        Idle,
        WaitingForGame,
        Playing
    };

    // Player loop (start thread with this)
    void operator()();

    void onGameFound(const GameSession *gameSession);

    [[nodiscard]] PlayerState getState() const;

    [[nodiscard]] std::string getName() const;

private:

    std::string playerName;
    PlayerState currentPlayerState;
    Server *server;

};

#include "Server.h"

#endif //CPP_THREADS_PLAYER_H
