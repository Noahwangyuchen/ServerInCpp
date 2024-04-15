#include "ThreadPool.h"

ThreadPool::ThreadPool(unsigned int size) : stop(false) {
    while (size--) {
        threads.emplace_back(std::thread([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]() { return stop || !tasks.empty(); });
                    if (stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    // stop是一个critical section，需要加锁
    {
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}