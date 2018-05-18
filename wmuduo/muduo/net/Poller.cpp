//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#include "Poller.h"

using namespace muduo;
using namespace muduo::net;


Poller::Poller(muduo::net::EventLoop *loop) : ownerLoop_(loop) {}

Poller::~Poller() {}

