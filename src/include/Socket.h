#pragma once

#include "Macros.h"
#include <string>

class Socket {

private:
    int m_sockfd;

public:
    Socket();
    Socket(int sockfd);
    ~Socket();

    DISALLOW_COPY_AND_MOVE(Socket);

    RC create();
    RC bind(const char* ip, uint16_t port) const;
    RC listen() const;
    RC setNonblocking() const;
    bool isNonblocking() const;
    RC accept(int &cln_fd) const;
    RC connect(const char *ip, uint16_t port) const;
    
    int getSockfd() const;
    std::string getPeerAddr() const;
    int getBytesToRead() const;

};