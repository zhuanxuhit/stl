//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#include <muduo/net/Poller.h>

using namespace muduo;
using namespace muduo::net;


Poller::Poller(EventLoop *loop) : ownerLoop_(loop) {}

Poller::~Poller() {}

