//
// Created by zhuanxu on 2018/5/23.
//

#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Channel.h>
#include <muduo/net/Socket.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;


TcpConnection::TcpConnection(EventLoop *loop, const string &name, int sockfd, const InetAddress &localAddr,
                             const InetAddress &peerAddr)
        : loop_(loop),
          name_(name),
          localAddr_(localAddr),
          peerAddr_(peerAddr),
          state_(kConnecting),
          socket_(new Socket(sockfd)),
          channel_(new Channel(loop, sockfd)) {
// 通道可读事件到来的时候，回调TcpConnection::handleRead，_1是事件发生时间
    channel_->setReadCallback(
            std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
    LOG_DEBUG << "TcpConnection::ctor[" << name_ << "] at " << this
              << " fd=" << sockfd;
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
              << " fd=" << channel_->fd();
}

void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    // 将this通过share_ptr传出去，对方接收使用 std::weak_ptr 防止延长生命周期
    channel_->tie(shared_from_this());
    channel_->enableReading();	// TcpConnection所对应的通道加入到Poller关注

    connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    loop_->assertInLoopThread();
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
    messageCallback_(shared_from_this(), buf, n);
}


