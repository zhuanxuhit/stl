//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//


#include "EventLoop.h"
#include <muduo/base/Logging.h>
#include <poll.h>

using namespace muduo;
using namespace muduo::net;


namespace {
    __thread EventLoop* t_loopInThisThread = nullptr;
}

EventLoop::EventLoop():
    looping_(false), threadId_(CurrentThread::tid()){
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    // 如果当前线程已经创建过 eventloop 了，则终止
    if (t_loopInThisThread != nullptr){
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    }else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    t_loopInThisThread = nullptr;
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " <<  CurrentThread::tid();
}

// one-thread-per-loop
// 该线程只能
void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    LOG_TRACE << "EventLoop " << this << " start looping";

    ::poll(NULL, 0, 5*1000);

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}



