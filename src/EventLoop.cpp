#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include <vector>

EventLoop::EventLoop() : ep(nullptr), quit(false) {
    ep = new Epoll();
}

EventLoop::~EventLoop() {
    delete ep;
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*> activeChannels = ep->poll();
        for(auto& channel : activeChannels) {
            channel->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* channel) {
    ep->updateChannel(channel);
}