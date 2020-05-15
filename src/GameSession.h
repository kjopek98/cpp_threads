#ifndef CPP_THREADS_GAMESESSION_H
#define CPP_THREADS_GAMESESSION_H

#include <vector>

class Player;

class Server;

class GameSession {

public:

    GameSession(unsigned int id, Server *server);

    enum class GameSessionState {
        Idle,
        WaitingForPlayers,
        Running
    };

    // Game session loop
    void operator()();

    void onPlayersCollected(const std::vector<Player *> &collectedPlayers);

    [[nodiscard]] unsigned int getId() const;

    [[nodiscard]] GameSessionState getState() const;

    [[nodiscard]] int getCurrentSessionDuration() const;

private:

    unsigned int id;
    GameSessionState currentGameSession;
    Server *server;

    int currentSessionDuration;
};

#include "Player.h"
#include "Server.h"


#endif //CPP_THREADS_GAMESESSION_H
