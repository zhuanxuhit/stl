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


EventLoop* g_loop;

void threadFunc()
{
    printf("threadFunc(): pid = %d, tid = %d\n",
           getpid(), CurrentThread::tid());
    g_loop->loop();
}

// 由于创建和启动的thread不同，会报错
int main() {
    printf("main(): pid = %d, tid = %d\n",getpid(),CurrentThread::tid());

    EventLoop loop;
    g_loop = &loop;
    Thread t(threadFunc);
    t.start();
    t.join();
    return 0;
}