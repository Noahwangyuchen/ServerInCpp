#include "Channel.h"
#include "EventLoop.h"
#include <functional>

Channel::Channel(EventLoop* _loop, int _fd) : loop(_loop), fd(_fd), events(0), revents(0), in_epoll(false) {}

Channel::~Channel() {}

void Channel::enableReading() {
    events = EPOLLIN | EPOLLET;
    loop->updateChannel(this);
}

void Channel::handleEvent() {
    callback();
}

int Channel::getfd() {
    return fd;
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

void Channel::setCallback(std::function<void()> _callback) {
    callback = _callback;
}