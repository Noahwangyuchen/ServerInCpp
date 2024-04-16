#pragma once

#include <functional>
#include <string>
#include <memory>
#include "Macros.h"

class EventLoop;
class Socket;
class Channel;
class Buffer;

/*
    ����һ��TCP���ӡ�
    
    �������ڣ�Acceptor��������,callback Server -> Server����Connection ->
        Channel�ص�Connection -> Connection�з��ֶϿ����� -> callback Server,ע��Connection

    �����¼��������̣�EventLoop��ȡ�¼� -> Channel -> Connection
*/
class Connection {

public:
    enum State {
        Invalid = 1,
        Handshaking = 2,
        Connected = 3,
        Closed = 4,
        Failed = 5,
    };

    Connection(EventLoop* _loop, int _fd);
    ~Connection();

    DISALLOW_COPY_AND_MOVE(Connection);

    RC read();
    RC write();
    RC send(std::string &message);
    

    void setDeleteConnectionCallback(std::function<void(int)> const &callback);
    void setOnConnectCallback(std::function<void(Connection*)> const &callback);
    void setOnMessageCallback(std::function<void(Connection*)> const &callback);
    State getState();
    void close();
    void setSendBuffer(const char* str);
    Buffer* getReadBuffer();
    const char* readBuffer();
    Buffer* getSendBuffer();
    const char* sendBuffer();
    void getlineSendBuffer();
    Socket* getSocket();

    void OnConnet(std::function<void()> const &func);
    void OnMessage(std::function<void()> const &func);

private:
    std::unique_ptr<Socket> m_sock;
    std::unique_ptr<Channel> m_channel;
    State m_state{State::Invalid};
    std::unique_ptr<Buffer> m_readBuffer;
    std::unique_ptr<Buffer> m_sendBuffer;
    std::function<void(int)> m_deleteConnectionCallback;
    std::function<void(Connection*)> m_onConnectCallback;
    std::function<void(Connection*)> m_onMessageCallback;

    RC ReadNonBlocking();
    RC WriteNonBlocking();
    RC ReadBlocking();
    RC WriteBlocking();
    void business();
};