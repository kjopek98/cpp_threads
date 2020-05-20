#include "ThreadPool.h"
#include <thread>
#include <mutex>

ThreadPool::ThreadPool() : nThreads(std::thread::hardware_concurrency()),
                           workerStates(std::thread::hardware_concurrency(), States::ServerState::Idle),
                           terminateThreadPool(false) {
    init();
}

ThreadPool::ThreadPool(unsigned int nThreads) : nThreads(nThreads),
                                                workerStates(nThreads, States::ServerState::Idle),
                                                terminateThreadPool(false) {
    init();
}

void ThreadPool::init() {
    for (int i = 0; i < nThreads; ++i) {
        pool.emplace_back(&ThreadPool::workerLoop, this, i);
    }
}

void ThreadPool::workerLoop(int workerID) {
    std::pair<std::function<void()>, States::ServerState> job;
    while (true) {
        {
            std::unique_lock<std::mutex> lock(jobMutex);

            jobCondVar.wait(lock, [this] { return !jobs.empty() || terminateThreadPool; });
            if (terminateThreadPool) {
                break;
            } else {
                job = jobs.front();
                jobs.pop();
            }
        }
        workerStates[workerID] = job.second;
        job.first();
        workerStates[workerID] = States::ServerState::Idle;
    }
}

void ThreadPool::addJob(const std::function<void()> &job, States::ServerState beginState) {
    {
        std::unique_lock<std::mutex> lock(jobMutex);
        jobs.emplace(job, beginState);
    }
    jobCondVar.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(jobMutex);
        terminateThreadPool = true;
    }
    jobCondVar.notify_all();

    for (std::thread &worker : pool) {
        worker.join();
    }
}

const std::vector<States::ServerState> &ThreadPool::getWorkerStates() const {
    return workerStates;
}
