#include "EventLoop.h"

#include <vector>

#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"

EventLoop::EventLoop() : m_epoll(nullptr) {
    m_epoll = std::make_unique<Epoll>();
}

EventLoop::~EventLoop() {}

void EventLoop::loop() {
    while (true) {
        for (auto& channel : m_epoll->poll()) {
            channel->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* channel) {
    m_epoll->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    m_epoll->removeChannel(channel);
}