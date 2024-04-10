#include "Connection.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"
#include <cstring>
#include <unistd.h>
#include <functional>

#define BUFFER_SIZE 1024

Connection::Connection(EventLoop* _loop, Socket* _sock) : loop(_loop), sock(_sock) {
    channel = new Channel(loop, sock->getSockfd());
    channel->enableReading();
    channel->setCallback(std::bind(&Connection::echo, this, sock->getSockfd()));
}

Connection::~Connection() {
    delete channel;
    delete sock;
}

void Connection::echo(int sockfd) {
    char buf[BUFFER_SIZE];
    while (true) {
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("Message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, read_bytes); // �ط���Ϣ
        }
        else if (!read_bytes) {
            printf("Client fd %d disconnected!\n", sockfd);
            deleteConnectionCallback(sock);
            break;
        }
        else if (read_bytes == -1 && errno == EINTR) { // �ͻ��������жϡ�������ȡ
            printf("continue reading\n");
            continue;
        }
        else if (read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { // ��������Ϊ��,������
            printf("Finish reading once, errno: %d\n", errno);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> _callback) {
    deleteConnectionCallback = _callback;
}