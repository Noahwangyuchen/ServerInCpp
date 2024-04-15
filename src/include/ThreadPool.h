#pragma once

#include "Macros.h"
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <functional>

class ThreadPool {

private:
    std::mutex mtx;     // 对任务队列进行读写加锁
    std::condition_variable cv;  // 条件变量，用于通知线程池中有任务需要处理
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    bool stop;

public:
    ThreadPool(unsigned int size = std::thread::hardware_concurrency());
    ~ThreadPool();

    DISALLOW_COPY_AND_MOVE(ThreadPool);

    template<typename F, typename... Args>
    auto add(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;
};

template<typename F, typename... Args>
auto ThreadPool::add(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    
    // 将任务包装成一个异步可调用对象
    auto task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (stop) {
            throw std::runtime_error("ThreadPool is stopped");
        }
        tasks.emplace([task](){ (*task)(); });
    }

    cv.notify_one();
    return task->get_future();
}