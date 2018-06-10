//
// Created by zhuanxu on 2018/5/24.
//

#include <muduo/net/EventLoopThreadPool.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>

using namespace muduo;
using namespace muduo::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop)
        : baseLoop_(baseLoop),
          started_(false),
          numThreads_(0),
          next_(0) {
}


void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;

    for (int i = 0; i < numThreads_; ++i)
    {
        auto * t = new EventLoopThread(cb);

        threads_.push_back(t);
        loops_.push_back(t->startLoop());	// 启动EventLoopThread线程，在进入事件循环之前，会调用cb
    }
    if (numThreads_ == 0 && cb)
    {
        // 只有一个EventLoop，在这个EventLoop进入事件循环之前，调用cb
        cb(baseLoop_);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    EventLoop* loop = baseLoop_;

    // 如果loops_为空，则loop指向baseLoop_
    // 如果不为空，按照round-robin（RR，轮叫）的调度方式选择一个EventLoop
    if (!loops_.empty())
    {
        // round-robin
        loop = loops_[next_];
        ++next_;
        if (implicit_cast<size_t>(next_) >= loops_.size())
        {
            next_ = 0;
        }
    }
    return loop;
}

EventLoopThreadPool::~EventLoopThreadPool() {

}
