//
// Created by Wang,Chao(EBDPRD02) on 2018/5/18.
//

#include <muduo/net/Poller.h>
#include <muduo/net/poller/PollPoller.h>
//#include <muduo/net/poller/EPollPoller.h>

using namespace muduo::net;

Poller* Poller::newDefaultPoller(muduo::net::EventLoop *loop) {
    if (::getenv("MUDUO_USE_POLL")){
        return new PollPoller(loop);
    }
    else {

    }
}
