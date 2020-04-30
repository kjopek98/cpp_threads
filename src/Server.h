#ifndef CPP_THREADS_SERVER_H
#define CPP_THREADS_SERVER_H

#include <queue>
#include <mutex>
#include <condition_variable>

#include "Player.h"
#include "GameSession.h"


class Server {

public:

    enum class ServerState {
        Idle,
        RequestGameSession,
        CancelRequestGameSession,
        RequestPlayers,
        CancelRequestPlayers
    };

    Server();

    void requestGameSession(Player &player);

    // Returns true if request was processed successfully,
    // otherwise returns false
    bool cancelRequestGameSession(Player &player);

    void requestPlayers(GameSession &gameSession, unsigned int nPlayers);

    // Returns true if request was processed successfully,
    // otherwise returns false
    bool cancelRequestPlayers(const std::vector<Player *> &acquiredPlayers);

    // Thread safe
    [[nodiscard]] ServerState getState() const;

private:
    const unsigned int QUEUE_SIZE = 10;

    std::mutex queueMutex;
    std::condition_variable queueCondVar;
    std::queue<Player *> waitingPlayerQueue;
    ServerState currentServerState;

};


#endif //CPP_THREADS_SERVER_H
