#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

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
    check_error(::bind(sockfd, reinterpret_cast<sockaddr*>(&addr->addr), addr->addr_len) == -1, "bind failed");
}

void Socket::listen() {
    check_error(::listen(sockfd, SOMAXCONN) == -1, "listen failed");
}

void Socket::setnonblocking() {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr) {
    int connfd = ::accept(sockfd, reinterpret_cast<sockaddr*>(&addr->addr), &addr->addr_len);
    check_error(connfd == -1, "accept failed");
    return connfd;
}

void Socket::connect(InetAddress* addr) {
    check_error(::connect(sockfd, reinterpret_cast<sockaddr*>(&addr->addr), addr->addr_len) == -1, "connect failed");
}

int Socket::getSockfd() {
    return sockfd;
}