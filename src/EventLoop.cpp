#include "EventLoop.h"

#include <vector>

#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"

EventLoop::EventLoop() : ep(nullptr), quit(false) { ep = new Epoll(); }

EventLoop::~EventLoop() { delete ep; }

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*> activeChannels = ep->poll();
        for (auto& channel : activeChannels) {
            channel->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* channel) { ep->updateChannel(channel); }
void EventLoop::removeChannel(Channel* channel) { ep->removeChannel(channel); }