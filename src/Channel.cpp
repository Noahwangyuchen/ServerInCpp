#include "Channel.h"

#include <functional>

#include "EventLoop.h"

const int Channel::READ_EVENT = 1;
const int Channel::WRITE_EVENT = 2;
const int Channel::ET = 4;

Channel::Channel(EventLoop* _loop, int _fd)
    : loop(_loop), fd(_fd), events(0), revents(0), in_epoll(false) {}

Channel::~Channel() { loop->removeChannel(this); }

void Channel::enableRead() {
    events |= READ_EVENT;
    loop->updateChannel(this);
}

void Channel::enableWrite() {
    events |= WRITE_EVENT;
    loop->updateChannel(this);
}

void Channel::handleEvent() {
    if (revents & READ_EVENT) {
        readCallback();
    }
    if (revents & WRITE_EVENT) {
        writeCallback();
    }
}

int Channel::getfd() { return fd; }

void Channel::useET() {
    events |= ET;
    loop->updateChannel(this);
}

short Channel::getEvents() { return events; }

short Channel::getRevents() { return revents; }

bool Channel::inEpoll() { return in_epoll; }

void Channel::setInEpoll(bool _in_epoll) { in_epoll = _in_epoll; }

void Channel::setRevents(int _revents) {
    if (_revents & READ_EVENT) {
        revents |= READ_EVENT;
    }
    if (_revents & WRITE_EVENT) {
        revents |= WRITE_EVENT;
    }
    if (_revents & ET) {
        revents |= ET;
    }
}

void Channel::setReadCallback(std::function<void()> const& _callback) {
    readCallback = _callback;
}
