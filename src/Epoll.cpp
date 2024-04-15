#include "Epoll.h"

#include <unistd.h>

#include <cstring>

#include "Channel.h"
#include "util.h"

#define MAX_EVENTS 1024

Epoll::Epoll() : epoll_fd(-1), events(nullptr) {
    epoll_fd = epoll_create1(0);
    check_error(epoll_fd == -1, "epoll create failed");
    events = new epoll_event[MAX_EVENTS];
    memset(events, 0, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epoll_fd != -1) {
        close(epoll_fd);
        epoll_fd = -1;
    }
    delete[] events;
}

void Epoll::updateChannel(Channel* channel) {
    int fd = channel->getfd();
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = channel;
    int ch_ev = channel->getEvents();
    if (ch_ev & Channel::READ_EVENT) {
        ev.events |= EPOLLIN | EPOLLPRI;
    }
    if (ch_ev & Channel::WRITE_EVENT) {
        ev.events |= EPOLLOUT;
    }
    if (ch_ev & Channel::ET) {
        ev.events |= EPOLLET;
    }
    if (!channel->inEpoll()) {
        check_error(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1,
                    "epoll add fd failed");
        channel->setInEpoll();
    } else {
        check_error(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1,
                    "epoll modify fd failed");
    }
}

std::vector<Channel*> Epoll::poll(int timeout) {
    std::vector<Channel*> activeEvents;
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
    if (nfds == -1 && errno == EINTR) {   // Õý³£ÖÐ¶Ï
        return activeEvents;
    }
    check_error(nfds == -1, "epoll wait failed");
    activeEvents.reserve(nfds);
    for (int i = 0; i < nfds; i++) {
        Channel* ch = reinterpret_cast<Channel*>(events[i].data.ptr);
        int ev = events[i].events, rev = 0;
        if (ev & (EPOLLIN | EPOLLPRI)) {
            rev |= Channel::READ_EVENT;
        }
        if (ev & EPOLLOUT) {
            rev |= Channel::WRITE_EVENT;
        }
        if (ev & EPOLLET) {
            rev |= Channel::ET;
        }
        ch->setRevents(rev);
        activeEvents.push_back(ch);
    }
    return activeEvents;
}

void Epoll::removeChannel(Channel* channel) {
    int fd = channel->getfd();
    check_error(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1, "epoll remove failed");
    channel->setInEpoll(false);
}