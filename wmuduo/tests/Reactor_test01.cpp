//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#include <cstdio>
#include <muduo/base/CurrentThread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Thread.h>
//#include <zconf.h>
using namespace muduo;
using namespace muduo::net;


void threadFunc()
{
    printf("threadFunc(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());

    EventLoop loop;
    loop.loop();
}

int main() {
    printf("main(): pid = %d, tid = %d\n",getpid(),CurrentThread::tid());

    EventLoop loop;
    Thread t(threadFunc);
    t.start();
    loop.loop();
    t.join();
    return 0;
}