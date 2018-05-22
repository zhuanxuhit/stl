//
// Created by Wang,Chao(EBDPRD02) on 2018/5/22.
//

#include <muduo/net/EventLoopThread.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

EventLoopThread::EventLoopThread(const EventLoopThread::ThreadInitCallback &cb) :
        loop_(nullptr),
        exiting_(false),
        thread_(std::bind(&EventLoopThread::threadFunc, this)),
        mutex_(),
        cond_(mutex_),
        callback_(cb) {
}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    loop_->quit();
    thread_.join();
}

void EventLoopThread::threadFunc() {
    EventLoop loop;

    if (callback_)
    {
        callback_(&loop);
    }

    {
        MutexLockGuard lock(mutex_);
        // loop_指针指向了一个栈上的对象，threadFunc函数退出之后，这个指针就失效了
        // threadFunc函数退出，就意味着线程退出了，EventLoopThread对象也就没有存在的价值了。
        // 因而不会有什么大的问题
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();
}

EventLoop* EventLoopThread::startLoop()
{
    assert(!thread_.started());
    thread_.start();

    {
        MutexLockGuard lock(mutex_);
        while (loop_ == nullptr)
        {
            cond_.wait();
        }
    }

    return loop_;
}
