#pragma once

#include <functional>

class EventLoop;
class Socket;
class Channel;

/*
    ����һ��TCP���ӡ�
    
    �������ڣ�Acceptor��������,callback Server -> Server����Connection ->
        Channel�ص�Connection -> Connection�з��ֶϿ����� -> callback Server,ע��Connection

    �����¼��������̣�EventLoop��ȡ�¼� -> Channel -> Connection
*/
class Connection {

private:
    EventLoop* loop;
    Socket* sock;
    Channel* channel;
    std::function<void(Socket*)> deleteConnectionCallback;

public:
    Connection(EventLoop* _loop, Socket* _sock);
    ~Connection();

    void echo(int sockfd);
    void setDeleteConnectionCallback(std::function<void(Socket*)> callback);
};