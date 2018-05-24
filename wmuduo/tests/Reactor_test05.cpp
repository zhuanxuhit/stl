//
// Created by Wang,Chao(EBDPRD02) on 2018/5/22.
//


#include <muduo/net/EventLoop.h>
//#include <muduo/net/EventLoopThread.h>
//#include <muduo/base/Thread.h>

#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

EventLoop* g_loop;
int g_flag = 0;

void run4()
{
    g_loop->assertInLoopThread();
    assert(g_loop->eventHandling());
    printf("run4(): pid = %d, flag = %d\n", getpid(), g_flag);
    g_loop->quit();
}

void run3()
{
    g_loop->assertInLoopThread();
    assert(g_loop->callingPendingFuncors());
    printf("run3(): pid = %d, flag = %d\n", getpid(), g_flag);
    g_loop->runAfter(3, run4);
    g_flag = 3;
}

void run2()
{
    g_loop->assertInLoopThread();
    printf("run2(): pid = %d, flag = %d\n", getpid(), g_flag);
    // 此处是io线程调用 queueInLoop ,将其放入到 io线程中调用
    g_loop->queueInLoop(run3);
}

void run1()
{
    g_loop->assertInLoopThread();
    assert(g_loop->eventHandling());
    g_flag = 1;
    printf("run1(): pid = %d, flag = %d\n", getpid(), g_flag);
    g_loop->runInLoop(run2);
    g_flag = 2;
}

int main()
{
    printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);

    EventLoop loop;
    g_loop = &loop;

    loop.runAfter(2, run1);
    loop.loop();
    printf("main(): pid = %d, flag = %d\n", getpid(), g_flag);
}
