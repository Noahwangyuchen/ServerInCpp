#pragma once

#include <functional>
#include <string>
#include "Macros.h"

class EventLoop;
class Socket;
class Channel;
class Buffer;

/*
    代表一个TCP连接。
    
    生命周期：Acceptor建立连接,callback Server -> Server创建Connection ->
        Channel回调Connection -> Connection中发现断开连接 -> callback Server,注销Connection

    连接事件处理流程：EventLoop获取事件 -> Channel -> Connection
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

    Connection(EventLoop* _loop, Socket* _sock);
    ~Connection();

    DISALLOW_COPY_AND_MOVE(Connection);

    void read();
    void write();
    void send(std::string &message);
    void business();

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
    EventLoop* m_loop;
    Socket* m_sock;
    Channel* m_channel;
    State m_state{State::Invalid};
    Buffer* m_readBuffer;
    Buffer* m_sendBuffer;
    std::function<void(int)> m_deleteConnectionCallback;
    std::function<void(Connection*)> m_onConnectCallback;
    std::function<void(Connection*)> m_onMessageCallback;

    void ReadNonBlocking();
    void WriteNonBlocking();
    void ReadBlocking();
    void WriteBlocking();
};