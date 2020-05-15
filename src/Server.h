#ifndef CPP_THREADS_SERVER_H
#define CPP_THREADS_SERVER_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Player.h"
#include "GameSession.h"
#include "RequestData.h"
#include "ThreadPool.h"


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

    // Entry point to the server
    void requestServerAction(const RequestData &requestData);

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

    ServerState currentServerState;

    const unsigned int PLAYER_QUEUE_SIZE = 10;
    std::mutex playerQueueMutex;
    std::condition_variable playerQueueCondVar;
    std::vector<Player *> waitingPlayerQueue;

    const unsigned int N_SERVER_WORKERS = 10;
    ThreadPool serverWorkers;
};


#endif //CPP_THREADS_SERVER_H
