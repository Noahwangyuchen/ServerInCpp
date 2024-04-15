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
    std::mutex mtx;     // ��������н��ж�д����
    std::condition_variable cv;  // ��������������֪ͨ�̳߳�����������Ҫ����
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
    
    // �������װ��һ���첽�ɵ��ö���
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