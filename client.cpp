#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include "util.h"

#define BUFFER_SIZE 1024

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    check_error(sockfd == -1, "socket create faild");

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8888);
    check_error(connect(sockfd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1, "connect faild");

    while (true) {
        char buf[BUFFER_SIZE];
        bzero(buf, sizeof(buf));
        scanf("%s", buf);           // 从键盘读入输入

        // 发送数据
        ssize_t write_bytes = write(sockfd, buf, strlen(buf));
        if (write_bytes == -1) {
            printf("write failed, socket may be closed already.\n");
            break;
        }

        bzero(buf, sizeof(buf));
        
        // 接收数据
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("Message from server: %s\n", buf);
        }
        else if (!read_bytes) {
            printf("Server closed the connection.\n");
            break;
        }
        else if (read_bytes == -1) {
            close(sockfd);
            check_error(true, "socket read error");
        }
    }

}