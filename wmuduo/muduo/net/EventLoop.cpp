//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//


#include "EventLoop.h"
#include <muduo/base/Logging.h>
#include <muduo/net/Poller.h>
#include <muduo/net/Channel.h>
#include <poll.h>



using namespace muduo;
using namespace muduo::net;

namespace {
    __thread EventLoop* t_loopInThisThread = nullptr;
    const int kPollTimeMs = 10 * 1000;
}


EventLoop *EventLoop::getEventLoopOfCurrentThread() const {
    return t_loopInThisThread;
}

EventLoop::EventLoop():
    looping_(false),
    quit_(false),
    threadId_(CurrentThread::tid()){
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
// 该函数只能被创建的线程调用
void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    LOG_TRACE << "EventLoop " << this << " start looping";

    while (!quit_){
        activeChannels_.clear();
        // 调用Poller得到有效Channel
        pollerReturnTime_ =  poller_->poll(kPollTimeMs,&activeChannels_);
        // 打印调试
        if (Logger::logLevel() <= Logger::TRACE){
            printActiveChannels();
        }
        // 挨个调用 channel 的处理方法
//        ::poll(nullptr, 0, 5*1000);
        for(auto channel : activeChannels_){
            channel->handleEvent(pollerReturnTime_);
        }
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

void EventLoop::printActiveChannels() const {
    for(auto channel :activeChannels_){
        LOG_TRACE << "{" << channel->reventsToString() << "}";
    }
}

void EventLoop::quit() {
    quit_ = true;
}



void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    // no lock needed
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    // no lock needed
    poller_->removeChannel(channel);
}



