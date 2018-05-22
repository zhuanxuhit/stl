//
// Created by Wang,Chao(EBDPRD02) on 2018/5/21.
//


#include <muduo/net/EventLoop.h>
#include <muduo/net/Channel.h>
#include <sys/timerfd.h>
#include <strings.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

EventLoop* g_loop;
int timerfd;

void timeout(Timestamp receiveTime){
    printf("timeout!\n");
    uint64_t  howmany;
    // 返回超时的个数
    ::read(timerfd,&howmany, sizeof(howmany));
    using std::cout;
    using std::endl;
    cout << howmany << endl;
    g_loop->quit();
}

int main() {
    EventLoop loop;
    g_loop = &loop;
    timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(g_loop, timerfd);
    using namespace std::placeholders;
    channel.setReadCallback(std::bind(timeout,_1));
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 1;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);

    loop.loop();

    ::close(timerfd);
}