#ifndef CPP_THREADS_THREADPOOL_H
#define CPP_THREADS_THREADPOOL_H

#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>

#include "States.h"

class ThreadPool {

public:
    ThreadPool();

    ~ThreadPool();

    explicit ThreadPool(unsigned nThreads);

    void addJob(const std::function<void()> &job, States::ServerState beginState);

    [[nodiscard]] const std::vector<States::ServerState> &getWorkerStates() const;

private:

    const unsigned int nThreads;
    std::vector<std::thread> pool;
    std::vector<States::ServerState> workerStates;

    bool terminateThreadPool;

    std::mutex jobMutex;
    std::condition_variable jobCondVar;
    std::queue<std::pair<std::function<void()>, States::ServerState>> jobs;

    void init();

    void workerLoop(int workerID);

};


#endif //CPP_THREADS_THREADPOOL_H
