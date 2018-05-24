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
    // 优雅的关闭连接
    for (auto &it : connections_) {
        TcpConnectionPtr conn = it.second;
        // 释放链接
        it.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
        conn.reset();// 不主动释放也行，会析构的时候自动减少引用
    }
}

void TcpServer::start() {
    if (!started_) {
        started_ = true;
    }
    if (!acceptor_->listenning()) {
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

// io 线程中调用
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), ":%s#%d", hostport_.c_str(), nextConnId_++);
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
    LOG_TRACE << "[1] usecount=" << conn.use_count();
    connections_[connName] = conn;
    LOG_TRACE << "[2] usecount=" << conn.use_count();
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);

    conn->setCloseCallback(
            std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    conn->connectEstablished();
    LOG_TRACE << "[5] usecount=" << conn.use_count();
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnectionInLoop [" << name_
             << "] - connection " << conn->name();


    LOG_TRACE << "[8] usecount=" << conn.use_count();
    size_t n = connections_.erase(conn->name());
    LOG_TRACE << "[9] usecount=" << conn.use_count();

    (void)n;
    assert(n == 1);

    loop_->queueInLoop(
            std::bind(&TcpConnection::connectDestroyed, conn));
    LOG_TRACE << "[10] usecount=" << conn.use_count();
}

