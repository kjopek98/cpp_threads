#ifndef CPP_THREADS_SERVER_H
#define CPP_THREADS_SERVER_H

#include <vector>
#include <mutex>
#include <condition_variable>

#include "Player.h"
#include "GameSession.h"
#include "RequestData.h"
#include "ThreadPool.h"
#include "States.h"

class Server {

public:

    Server();

    // Entry point to the server
    void requestServerAction(const RequestData &requestData);

    void requestGameSession(Player &player);

    void cancelRequestGameSession(Player &player);

    void requestPlayers(GameSession &gameSession, unsigned int nPlayers);

    void cancelRequestPlayers(GameSession &gameSession, const std::vector<Player *> &acquiredPlayers);

    // Thread safe
    [[nodiscard]] const std::vector<States::ServerState> &getWorkerStates() const;

    [[nodiscard]] const std::vector<Player *> &getWaitingPlayerQueue() const;

private:

    const unsigned int PLAYER_QUEUE_SIZE = 10;
    std::mutex playerQueueMutex;
    std::condition_variable playerQueueCondVar;
    std::vector<Player *> waitingPlayerQueue;

    const unsigned int N_SERVER_WORKERS = 10;
    ThreadPool serverWorkers;

};


#endif //CPP_THREADS_SERVER_H
