#include "Channel.h"
#include "EventLoop.h"
#include <functional>

Channel::Channel(EventLoop* _loop, int _fd) : loop(_loop), fd(_fd), events(0), 
        revents(0), in_epoll(false), useThreadPool(false) {}

Channel::~Channel() {}

void Channel::enableReading() {
    events = EPOLLIN | EPOLLPRI;
    loop->updateChannel(this);
}

void Channel::handleEvent() {
    if (revents & (EPOLLIN | EPOLLPRI)) {
        if (useThreadPool) {
            loop->addToPool(readCallback);
        }
        else readCallback();
    }
    if (revents & EPOLLOUT) {
        if (useThreadPool) {
            loop->addToPool(writeCallback);
        }
        else writeCallback();
    }
}

int Channel::getfd() {
    return fd;
}

void Channel::useET() {
    events |= EPOLLET;
    loop->updateChannel(this);
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getRevents() {
    return revents;
}

bool Channel::inEpoll() {
    return in_epoll;
}

void Channel::setInEpoll() {
    in_epoll = true;
}

void Channel::setRevents(uint32_t _revents) {
    revents = _revents;
}

void Channel::setReadCallback(std::function<void()> _callback) {
    readCallback = _callback;
}

void Channel::setUseThreadPool(bool use) {
    useThreadPool = use;
}