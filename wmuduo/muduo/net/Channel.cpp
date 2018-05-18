//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#include <sstream>

#include "Channel.h"

#include <poll.h>


using namespace muduo;
using namespace muduo::net;

Channel::Channel(EventLoop *loop, int fd):loop_(loop),fd_(fd) {

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


