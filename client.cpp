#include "Socket.h"
#include "InetAddress.h"
#include <cstring>
#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <memory>
#include <string>
#include "util.h"

#define BUFFER_SIZE 1024

int main() {

    std::unique_ptr<Socket> sock(new Socket());
    std::unique_ptr<InetAddress> addr(new InetAddress("127.0.0.1", 8888));
    sock->connect(addr.get());
    
    std::string send_msg, recv_msg;

    while (true) {
        std::cin >> send_msg;          // 从键盘读入输入

        // 发送数据
        ssize_t write_bytes = write(sock->getSockfd(), send_msg.c_str(), send_msg.size());
        if (write_bytes == -1) {
            printf("write failed, socket may be closed already.\n");
            break;
        }
        
        char buf[BUFFER_SIZE];
        // 接收数据
        while (true) {
            bzero(buf, sizeof(buf));    
            ssize_t read_bytes = read(sock->getSockfd(), buf, sizeof(buf));
            if (read_bytes > 0) {
                recv_msg.append(buf, read_bytes);
            }
            else if (!read_bytes) {
                printf("Server closed the connection.\n");
                break;
            }
            if (recv_msg.size() >= send_msg.size()) {
                printf("Message from server: %s\n", recv_msg.c_str());
                break;
            }
        }
        recv_msg.clear();
    }

    return 0;
}