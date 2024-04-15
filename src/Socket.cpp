#include "Socket.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>

#include "InetAddress.h"
#include "util.h"

Socket::Socket() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    check_error(sockfd == -1, "create socket failed");
}

Socket::Socket(int fd) : sockfd(fd) {
    check_error(sockfd == -1, "create socket failed");
}

Socket::~Socket() {
    if (sockfd != -1) {
        close(sockfd);
        sockfd = -1;
    }
}

void Socket::bind(InetAddress* addr) {
    check_error(::bind(sockfd, reinterpret_cast<sockaddr*>(&addr->addr),
                       addr->addr_len) == -1,
                "bind failed");
}

void Socket::listen() {
    check_error(::listen(sockfd, SOMAXCONN) == -1, "listen failed");
}

void Socket::setNonblocking() {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
}

bool Socket::isNonblocking() {
    return fcntl(sockfd, F_GETFL) & O_NONBLOCK;
}

// 供服务器接受连接
int Socket::accept(InetAddress* addr) {
    int cln_fd = -1;
    sockaddr_in tmp_addr;
    socklen_t tmp_len = sizeof(tmp_addr);
    if (isNonblocking()) {
        while (true) {
            cln_fd = ::accept(sockfd, reinterpret_cast<sockaddr*>(&tmp_addr), &tmp_len);
            if (cln_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                continue;
            }
            if (cln_fd == -1) {
                check_error(true, "accept failed");
            }
            else break;
        }
    }
    else {
        cln_fd = ::accept(sockfd, reinterpret_cast<sockaddr*>(&tmp_addr), &tmp_len);
        check_error(cln_fd == -1, "accept failed");
    }
    addr->addr = tmp_addr;
    return cln_fd;
}

// 供客户端建立连接
void Socket::connect(InetAddress* addr) {
    sockaddr_in tmp_addr = addr->addr;
    if (isNonblocking()) {
        while (true) {
            int ret = ::connect(sockfd, reinterpret_cast<sockaddr*>(&tmp_addr), sizeof(tmp_addr));
            if (ret == 0) break;
            if (ret == -1 && errno == EINPROGRESS) {
                continue;
            }
            if (ret == -1) {
                check_error(true, "connect failed");
            }
        }
    }
    else {
        check_error(::connect(sockfd, reinterpret_cast<sockaddr*>(&tmp_addr), sizeof(tmp_addr)), "connect failed");
    }
}

void Socket::connect(const char* ip, uint16_t port) {
    InetAddress* addr = new InetAddress(ip, port);
    connect(addr);
    delete addr;
}

int Socket::getSockfd() { return sockfd; }