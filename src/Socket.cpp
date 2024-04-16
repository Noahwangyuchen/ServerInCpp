#include "Socket.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

Socket::Socket() : m_sockfd(-1) {}

Socket::Socket(int fd) : m_sockfd(fd) {}

Socket::~Socket() {
    if (m_sockfd != -1) {
        close(m_sockfd);
        m_sockfd = -1;
    }
}

RC Socket::create() {
    assert(m_sockfd == -1);
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1) {
        perror("create socket failed");
        return RC_SOCKET_ERROR;
    }
    return RC_SUCCESS;
}

RC Socket::bind(const char* ip, uint16_t port) const {
    assert(m_sockfd != -1);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if (::bind(m_sockfd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) ==
        -1) {
        perror("bind failed");
        return RC_SOCKET_ERROR;
    }
    return RC_SUCCESS;
}

RC Socket::listen() const {
    assert(m_sockfd != -1);
    if (::listen(m_sockfd, SOMAXCONN) == -1) {
        perror("listen failed");
        return RC_SOCKET_ERROR;
    }
    return RC_SUCCESS;
}

RC Socket::setNonblocking() const {
    if (fcntl(m_sockfd, F_SETFL, fcntl(m_sockfd, F_GETFL) | O_NONBLOCK) == -1) {
        perror("set nonblocking failed");
        return RC_SOCKET_ERROR;
    }
    return RC_SUCCESS;
}

bool Socket::isNonblocking() const {
    return fcntl(m_sockfd, F_GETFL) & O_NONBLOCK;
}

// 供服务器接受连接
RC Socket::accept(int& cln_fd) const {
    assert(m_sockfd != -1);
    if (isNonblocking()) {
        while (true) {
            cln_fd = ::accept(m_sockfd, nullptr, nullptr);
            if (cln_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                continue;
            }
            if (cln_fd == -1) {
                perror("Failed to accept socket");
                return RC_SOCKET_ERROR;
            } else
                break;
        }
    } else {
        cln_fd = ::accept(m_sockfd, nullptr, nullptr);
        if (cln_fd == -1) {
            perror("Failed to accpet socket");
            return RC_SOCKET_ERROR;
        }
    }
    return RC_SUCCESS;
}

// 供客户端建立连接
RC Socket::connect(const char* ip, uint16_t port) const {
    assert(m_sockfd != -1);
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if (isNonblocking()) {
        while (true) {
            int ret = ::connect(m_sockfd, reinterpret_cast<sockaddr*>(&addr),
                                sizeof(addr));
            if (ret == 0) break;
            if (ret == -1 && errno == EINPROGRESS) {
                continue;
            }
            if (ret == -1) {
                perror("connect failed");
                return RC_SOCKET_ERROR;
            }
        }
    } else {
        if (::connect(m_sockfd, reinterpret_cast<sockaddr*>(&addr),
                      sizeof(addr)) == -1) {
            perror("connect failed");
            return RC_SOCKET_ERROR;
        }
    }
    return RC_SUCCESS;
}

int Socket::getSockfd() const { return m_sockfd; }

std::string Socket::getPeerAddr() const {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);
    if (::getpeername(m_sockfd, reinterpret_cast<sockaddr*>(&addr), &len) ==
        -1) {
        perror("get peer addr failed");
        return "";
    }
    std::string ret(inet_ntoa(addr.sin_addr));
    ret.push_back(':');
    ret += std::to_string(ntohs(addr.sin_port));
    return ret;
}

// 返回socket中等待读取的字节数
int Socket::getBytesToRead() const {
    int size = -1;
    if (ioctl(m_sockfd, FIONREAD, &size) == -1) {
        perror("get recv bufsize failed");
    }
    return size;
}