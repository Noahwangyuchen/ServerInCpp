#include <cstdio>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include "Socket.h"
#include "InetAddress.h"
#include "Epoll.h"
#include "util.h"

#define MAX_EVENTS 1024

void handleRead(int sockfd);

int main() {

    std::unique_ptr<Socket> srv_sock(new Socket());

    std::unique_ptr<InetAddress> srv_addr(new InetAddress("127.0.0.1", 8888));

    srv_sock->setnonblocking();
    srv_sock->bind(srv_addr.get());
    srv_sock->listen();

    std::unique_ptr<Epoll> epoll(new Epoll());
    epoll->add_fd(srv_sock->getSockfd(), EPOLLIN | EPOLLET);

    while (true) {
        std::vector<epoll_event> events = epoll->poll();

        for (auto &event : events) {
            if (event.data.fd == srv_sock->getSockfd()) { // 有新的客户端连接
                std::unique_ptr<InetAddress> cln_addr(new InetAddress());
                Socket* cln_sock = new Socket(srv_sock->accept(cln_addr.get()));        // Memory Leak!!

                printf("New client fd %d! IP: %s Port: %d\n", cln_sock->getSockfd(), inet_ntoa(cln_addr->addr.sin_addr), ntohs(cln_addr->addr.sin_port));
                cln_sock->setnonblocking();
                epoll->add_fd(cln_sock->getSockfd(), EPOLLIN | EPOLLET);
            }
            else if (event.events & EPOLLIN) { // 可读事件
                handleRead(event.data.fd);
            }
            else {
                printf("Unknown event: %d\n", event.events);
            }
        }
    }
    return 0;
}

void handleRead(int sockfd) {
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