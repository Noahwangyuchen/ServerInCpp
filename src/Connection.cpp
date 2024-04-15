#include "Connection.h"

#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <functional>
#include <cassert>
#include <iostream>
#include <utility>

#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Buffer.h"
#include "util.h"

#define BUFFER_SIZE 1024

Connection::Connection(EventLoop* _loop, Socket* _sock)
    : m_loop(_loop), m_sock(_sock) {
    if (m_loop != nullptr) {
        m_channel = new Channel(m_loop, m_sock->getSockfd());
        m_channel->enableRead();
        m_channel->useET();
    }
    m_readBuffer = new Buffer();
    m_sendBuffer = new Buffer();
    m_state = State::Connected;
}

Connection::~Connection() {
    if (m_loop != nullptr) {
        delete m_channel;
    }
    delete m_sock;
    delete m_readBuffer;
    delete m_sendBuffer;
}

void Connection::read() {
    ASSERT(m_state == State::Connected, "connection is not connected!");
    m_readBuffer->clear();
    if (m_sock->isNonblocking()) {
        ReadNonBlocking();
    }
    else {
        ReadBlocking();
    }
}

void Connection::write() {
    ASSERT(m_state == State::Connected, "connection is not connected!");
    if (m_sock->isNonblocking()) {
        WriteNonBlocking();
    }
    else {
        WriteBlocking();
    }
    m_sendBuffer->clear();
}

void Connection::send(std::string& str) {
    setSendBuffer(str.c_str());
    write();
}

void Connection::business() {
    read();
    m_onMessageCallback(this);
}

void Connection::ReadNonBlocking() {
    int sockfd = m_sock->getSockfd();
    char buf[BUFFER_SIZE];
    while (true) {
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0) {
            m_readBuffer->append(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR) {
            continue;
        }
        else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        }
        else if (bytes_read == 0) {
            printf("read EOF, client fd %d disconnected\n", sockfd);
            m_state = State::Closed;
            break;
        }
        else {
            printf("Other error on client fd %d\n", sockfd);
            m_state = State::Closed;
            break;
        }
    }
}

void Connection::WriteNonBlocking() {
    int sockfd = m_sock->getSockfd(), data_size = m_sendBuffer->size();
    char buf[data_size];
    memcpy(buf, m_sendBuffer->c_str(), data_size);
    int data_left = data_size;
    while (data_left > 0) {
        ssize_t bytes_write = ::write(sockfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 & errno == EINTR) {
            continue;
        }
        if (bytes_write == -1 && errno ==EAGAIN) {
            break;
        }
        if (bytes_write == -1) {
            printf("Other error on client fd %d\n", sockfd);
            m_state = State::Closed;
            break;
        }
        data_left -= bytes_write;
    }
}

void Connection::ReadBlocking() {
    int sockfd = m_sock->getSockfd();
    unsigned int rcv_size = 0;
    socklen_t len = sizeof(rcv_size);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);
    char buf[rcv_size];
    ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
        m_readBuffer->append(buf, bytes_read);
    }
    else if (bytes_read == 0) {
        printf("EOF, blocking client fd %d disconnected\n", sockfd);
        m_state = State::Closed;
    }
    else if (bytes_read == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = State::Closed;
    }
}

void Connection::WriteBlocking() {
    int sockfd = m_sock->getSockfd();
    ssize_t bytes_write = ::write(sockfd, m_sendBuffer->c_str(), m_sendBuffer->size());
    if (bytes_write == -1) {
        printf("Other error on blocking client fd %d\n", sockfd);
        m_state = State::Closed;
    }
}

void Connection::close() {
    m_deleteConnectionCallback(m_sock->getSockfd());
}

Connection::State Connection::getState() { return m_state; }

void Connection::setSendBuffer(const char* str) { m_sendBuffer->setBuf(str); }

Buffer* Connection::getReadBuffer() { return m_readBuffer; }

const char* Connection::readBuffer() { return m_readBuffer->c_str(); }

Buffer* Connection::getSendBuffer() { return m_sendBuffer; }

const char* Connection::sendBuffer() { return m_sendBuffer->c_str(); }

void Connection::getlineSendBuffer() { m_sendBuffer->getline(); }

Socket* Connection::getSocket() { return m_sock; }

void Connection::setOnConnectCallback(std::function<void(Connection*)> const &callback) { 
    m_onConnectCallback = callback; 
    // m_channel->setReadCallback([this]() { m_onConnectCallback(this); });    
}

void Connection::setDeleteConnectionCallback(
    std::function<void(int)> const& _callback) {
    m_deleteConnectionCallback = _callback;
}

void Connection::setOnMessageCallback(std::function<void(Connection*)> const &callback) {
    m_onMessageCallback = callback;
    m_channel->setReadCallback(std::bind(&Connection::business, this));
}
