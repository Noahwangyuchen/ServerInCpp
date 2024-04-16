#include "Connection.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include <utility>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "util.h"

#define BUFFER_SIZE 1024

Connection::Connection(EventLoop* _loop, int _fd) {
    m_sock = std::make_unique<Socket>(_fd);
    if (_loop != nullptr) {
        m_channel = std::make_unique<Channel>(_loop, _fd);
        m_channel->enableRead();
        m_channel->useET();
    }
    m_readBuffer = std::make_unique<Buffer>();
    m_sendBuffer = std::make_unique<Buffer>();
    m_state = State::Connected;
}

Connection::~Connection() {}

RC Connection::read() {
    if (m_state != State::Connected) {
        perror("connection is not connected!");
        return RC::RC_CONNECTION_ERROR;
    }
    m_readBuffer->clear();
    if (m_sock->isNonblocking()) {
        return ReadNonBlocking();
    } else {
        return ReadBlocking();
    }
}

RC Connection::write() {
    if (m_state != State::Connected) {
        perror("connection is not connected!");
        return RC::RC_CONNECTION_ERROR;
    }
    RC rc = RC_UNDEFINED;
    if (m_sock->isNonblocking()) {
        rc = WriteNonBlocking();
    } else {
        rc = WriteBlocking();
    }
    m_sendBuffer->clear();
    return rc;
}

RC Connection::send(std::string& str) {
    setSendBuffer(str.c_str());
    return write();
}

void Connection::business() {
    if (read() == RC::RC_SUCCESS) {
        m_onMessageCallback(this);
    }
}

RC Connection::ReadNonBlocking() {
    int sockfd = m_sock->getSockfd();
    char buf[BUFFER_SIZE];
    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            m_readBuffer->append(buf, bytes_read);
        } else if (bytes_read == -1 && errno == EINTR) {
            continue;
        } else if (bytes_read == -1 &&
                   (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        } else if (bytes_read == 0) {
            printf("read EOF, client fd %d disconnected\n", sockfd);
            m_state = State::Closed;
            close();
            return RC::RC_CONNECTION_ERROR;
        } else {
            printf("Other error on client fd %d\n", sockfd);
            m_state = State::Closed;
            close();
            return RC::RC_CONNECTION_ERROR;
        }
    }
    return RC::RC_SUCCESS;
}

RC Connection::WriteNonBlocking() {
    int sockfd = m_sock->getSockfd(), data_size = m_sendBuffer->size();
    char buf[data_size];
    memcpy(buf, m_sendBuffer->c_str(), data_size);
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t bytes_write =
            ::write(sockfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 & errno == EINTR) {
            continue;
        }
        if (bytes_write == -1 && errno == EAGAIN) {
            break;
        }
        if (bytes_write == -1) {
            printf("Other error on client fd %d\n", sockfd);
            m_state = State::Closed;
            break;
        }
        data_left -= bytes_write;
    }
    return RC::RC_SUCCESS;
}

RC Connection::ReadBlocking() {
    int sockfd = m_sock->getSockfd();
    char buf[BUFFER_SIZE];
    ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
        m_readBuffer->append(buf, bytes_read);
    } else if (bytes_read == 0) {
        printf("EOF, blocking client fd %d disconnected\n", sockfd);
        m_state = State::Closed;
    } else if (bytes_read == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = State::Closed;
    }
    return RC::RC_SUCCESS;
}

RC Connection::WriteBlocking() {
    int sockfd = m_sock->getSockfd();
    ssize_t bytes_write =
        ::write(sockfd, m_sendBuffer->c_str(), m_sendBuffer->size());
    if (bytes_write == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = State::Closed;
    }
    return RC::RC_SUCCESS;
}

void Connection::close() { m_deleteConnectionCallback(m_sock->getSockfd()); }

Connection::State Connection::getState() { return m_state; }

void Connection::setSendBuffer(const char* str) { m_sendBuffer->setBuf(str); }

Buffer* Connection::getReadBuffer() { return m_readBuffer.get(); }

const char* Connection::readBuffer() { return m_readBuffer->c_str(); }

Buffer* Connection::getSendBuffer() { return m_sendBuffer.get(); }

const char* Connection::sendBuffer() { return m_sendBuffer->c_str(); }

void Connection::getlineSendBuffer() { m_sendBuffer->getline(); }

Socket* Connection::getSocket() { return m_sock.get(); }

void Connection::setOnConnectCallback(
    std::function<void(Connection*)> const& callback) {
    m_onConnectCallback = callback;
    // m_channel->setReadCallback([this]() { m_onConnectCallback(this); });
}

void Connection::setDeleteConnectionCallback(
    std::function<void(int)> const& _callback) {
    m_deleteConnectionCallback = _callback;
}

void Connection::setOnMessageCallback(
    std::function<void(Connection*)> const& callback) {
    m_onMessageCallback = callback;
    m_channel->setReadCallback(std::bind(&Connection::business, this));
}
