#include "Server.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <memory>

Server::Server(EventLoop* _loop) : loop(_loop) {
    Socket* srv_sock = new Socket();
    std::unique_ptr<InetAddress> srv_addr(new InetAddress("127.0.0.1", 8888));
    srv_sock->setnonblocking();
    srv_sock->bind(srv_addr.get());
    srv_sock->listen();

    Channel* srv_channel = new Channel(loop, srv_sock->getSockfd());
    std::function<void()> cbk = std::bind(&Server::newConnection, this, srv_sock);
    srv_channel->setCallback(cbk);
    srv_channel->enableReading();
}

Server::~Server() {}

void Server::handleReadEvent(int sockfd) {
    char buf[BUFFER_SIZE];
    while (true) {
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("Message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, read_bytes); // 回发消息
        }
        else if (!read_bytes) {
            printf("Client fd %d disconnected!\n", sockfd);
            close(sockfd);
            break;
        }
        else if (read_bytes == -1 && errno == EINTR) { // 客户端正常中断、继续读取
            printf("continue reading\n");
            continue;
        }
        else if (read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { // 读缓冲区为空,读完了
            printf("Finish reading once, errno: %d\n", errno);
            break;
        }
    }
}

void Server::newConnection(Socket* srv_sock) {
    std::unique_ptr<InetAddress> cln_addr(new InetAddress());
    Socket* cln_sock = new Socket(srv_sock->accept(cln_addr.get()));        // Memory Leak!!
    printf("New client fd %d! IP: %s Port: %d\n", cln_sock->getSockfd(), inet_ntoa(cln_addr->addr.sin_addr), ntohs(cln_addr->addr.sin_port));
    cln_sock->setnonblocking();        
    
    Channel* cln_channel = new Channel(loop, cln_sock->getSockfd());
    std::function<void()> cbk = std::bind(&Server::handleReadEvent, this, cln_sock->getSockfd());
    cln_channel->enableReading();
    cln_channel->setCallback(cbk);
}
