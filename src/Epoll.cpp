#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include "Channel.h"
#include <cstring>

#define MAX_EVENTS 1024

Epoll::Epoll() : epoll_fd(-1), events(nullptr) {
    epoll_fd = epoll_create1(0);
    check_error(epoll_fd == -1, "epoll create failed");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epoll_fd != -1) {
        close(epoll_fd);
        epoll_fd = -1;
    }
    delete[] events;
}

void Epoll::add_fd(int fd, uint32_t op) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    check_error(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add fd failed");
}

void Epoll::updateChannel(Channel* channel) {
    int fd = channel->getfd();
    epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    if (!channel->inEpoll()) {
        check_error(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add fd failed");
        channel->setInEpoll();
    }
    else {
        check_error(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify fd failed");
    }
}

std::vector<Channel*> Epoll::poll(int timeout) {
    std::vector<Channel*> activeEvents;
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
    activeEvents.reserve(nfds);
    check_error(nfds == -1, "epoll wait failed");
    for (int i = 0; i < nfds; i++) {
        Channel* ch = (Channel*)events[i].data.ptr;
        ch->setRevents(events[i].events);
        activeEvents.push_back(ch);
    }
    return activeEvents;
}