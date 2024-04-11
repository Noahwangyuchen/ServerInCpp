#include <unistd.h>
#include <functional>
#include "util.h"
#include "InetAddress.h"
#include "ThreadPool.h"
#include "Socket.h"
#include <cstring>
#include <string>
#include <iostream>

void oneClient() {
    int msgs = 10, wait = 0;
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8888);
    sock->connect(addr);

    int sockfd = sock->getSockfd();

    sleep(wait);
    int count = 0;
    while(count < msgs){
        std::string s("I'm client!");
        ssize_t write_bytes = write(sockfd, s.c_str(), s.size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        char buf[1024];    //这个buf大小无所谓
        std::string reads;
        while(true){
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                reads.append(buf, read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){         //EOF
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= s.size()){
                printf("count: %d, message from server: %s\n", count++, reads.c_str());
                break;
            } 
        }
    }
    delete addr;
    delete sock;
}

int main() {
    int threads = 10000;
    ThreadPool *pool = new ThreadPool();
    std::function<void()> func = std::bind(oneClient);
    while(threads--) {
        pool->add(func);
    }
    delete pool;
    return 0;
}