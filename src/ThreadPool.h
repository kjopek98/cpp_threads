#ifndef CPP_THREADS_THREADPOOL_H
#define CPP_THREADS_THREADPOOL_H

#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {

public:
    ThreadPool();

    ~ThreadPool();

    explicit ThreadPool(unsigned nThreads);

    void addJob(const std::function<void()> &job);

private:

    const unsigned int nThreads;
    std::vector<std::thread> pool;

    std::atomic<bool> terminateThreadPool;

    std::mutex jobMutex;
    std::condition_variable jobCondVar;
    std::queue<std::function<void()>> jobs;

    void init();

    void workerLoop();

};


#endif //CPP_THREADS_THREADPOOL_H
