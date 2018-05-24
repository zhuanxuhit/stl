//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#include <sstream>

#include <muduo/net/Channel.h>
#include <muduo/net/EventLoop.h>

#include <poll.h>
#include <cassert>


using namespace muduo;
using namespace muduo::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd):
        loop_(loop),
        fd_(fd),
        events_(kNoneEvent),
        revents_(kNoneEvent),
        index_(-1),
        tied_(false){

}

Channel::~Channel() {

}


string Channel::reventsToString() const {
    std::ostringstream oss;
    oss << fd_ << ": ";
    if (revents_ & POLLIN){
        oss << "IN ";
    }
    if (revents_ & POLLPRI){
        oss << "PRI ";
    }
    if (revents_ & POLLOUT)
        oss << "OUT ";
    if (revents_ & POLLHUP)
        oss << "HUP ";
    if (revents_ & POLLRDHUP)
        oss << "RDHUP ";
    if (revents_ & POLLERR)
        oss << "ERR ";
    if (revents_ & POLLNVAL)
        oss << "NVAL ";

    return oss.str().c_str();
}

void Channel::handleEvent(Timestamp receiveTime) {
    // 根据 revents 调用相应的回调函数
    // 对方已关闭
    if ( (revents_ & POLLHUP) && !(revents_ & POLLIN) ) {
        if (closeCallback_) closeCallback_();
    }

    if (revents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_) readCallback_(receiveTime);
    }
    if (revents_ & POLLOUT)
    {
        if (writeCallback_) writeCallback_();
    }
}

EventLoop *Channel::ownerLoop() const {
    return loop_;
}

void Channel::update() {
    this->ownerLoop()->updateChannel(this);
}

void Channel::remove() {
    assert(isNoneEvent());
    this->ownerLoop()->removeChannel(this);
}

void Channel::tie(const std::shared_ptr<void> &obj) {
    tie_ = obj;
    tied_ = true;
}




