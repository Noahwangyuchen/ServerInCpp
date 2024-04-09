#include "Epoll.h"
#include "util.h"
#include <unistd.h>
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

std::vector<epoll_event> Epoll::poll(int timeout) {
    std::vector<epoll_event> activeEvents;
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
    activeEvents.reserve(nfds);
    check_error(nfds == -1, "epoll wait failed");
    for (int i = 0; i < nfds; i++) {
        activeEvents.push_back(events[i]);
    }
    return activeEvents;
}