//
// Created by zhuanxu on 2018/5/23.
//
#include <muduo/net/TcpServer.h>
#include <muduo/net/Acceptor.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/SocketsOps.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;


TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg)
        : loop_(loop),
          hostport_(listenAddr.toIpPort()),
          name_(nameArg),
          acceptor_(new Acceptor(loop, listenAddr)),
          started_(false),
          nextConnId_(1) {
    using namespace std::placeholders;
    acceptor_->setNewConnectionCallback(
            std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
    loop_->assertInLoopThread();
    LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";
}

void TcpServer::start() {
    if (!started_){
        started_ = true;
    }
    if (!acceptor_->listenning()){
        loop_->runInLoop(std::bind(&Acceptor::listen,acceptor_.get()));
    }
}

// io 线程中调用
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf),":%s#%d",hostport_.c_str(),nextConnId_++);
    string connName = name_ + buf;

    LOG_INFO << "TcpServer::newConnection [" << name_
             << "] - new connection [" << connName
             << "] from " << peerAddr.toIpPort();
    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    TcpConnectionPtr conn(new TcpConnection(loop_,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);

    conn->connectEstablished();
}

