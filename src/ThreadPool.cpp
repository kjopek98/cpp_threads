#include "ThreadPool.h"
#include <thread>
#include <mutex>
#include <condition_variable>

ThreadPool::ThreadPool() : nThreads(std::thread::hardware_concurrency()), terminateThreadPool(false) {
    init();
}

ThreadPool::ThreadPool(unsigned int nThreads) : nThreads(nThreads), terminateThreadPool(false) {
    init();
}

void ThreadPool::init() {
    for (int i = 0; i < nThreads; ++i) {
        pool.emplace_back(&ThreadPool::workerLoop, this);
    }
}

void ThreadPool::workerLoop() {
    std::function<void()> job;
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
        job();
    }
}

void ThreadPool::addJob(const std::function<void()> &job) {
    {
        std::unique_lock<std::mutex> lock(jobMutex);
        jobs.push(job);
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
