//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//


#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include <muduo/net/Poller.h>
#include <muduo/net/Channel.h>
#include <algorithm>


using namespace muduo;
using namespace muduo::net;

namespace {
    __thread EventLoop *t_loopInThisThread = nullptr;
    const int kPollTimeMs = 10 * 1000;
}


EventLoop *EventLoop::getEventLoopOfCurrentThread() const {
    return t_loopInThisThread;
}

EventLoop::EventLoop() :
        looping_(false),
        quit_(false),
        eventHandling_(false),
        currentActiveChannel_(nullptr),
        threadId_(CurrentThread::tid()),
        poller_(Poller::newDefaultPoller(this)) {
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    // 如果当前线程已经创建过 eventloop 了，则终止
    if (t_loopInThisThread != nullptr) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    t_loopInThisThread = nullptr;
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}

// one-thread-per-loop
// 该函数只能被创建的线程调用
void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    LOG_TRACE << "EventLoop " << this << " start looping";

    while (!quit_) {
        activeChannels_.clear();
        // 调用Poller得到有效Channel
        pollerReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        // 打印调试
        if (Logger::logLevel() <= Logger::TRACE) {
            printActiveChannels();
        }
        // 挨个调用 channel 的处理方法
//        ::poll(nullptr, 0, 5*1000);
        eventHandling_ = true;
        for (auto channel : activeChannels_) {
            currentActiveChannel_ = channel;
            channel->handleEvent(pollerReturnTime_);
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

void EventLoop::printActiveChannels() const {
    for (auto channel :activeChannels_) {
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

// 移除channel，限制是：如果正在处理channel的响应事件，
// 需要满足移除的Channel是当前Channel，
// 或者移除的Channel不再当前处理的Channel中
void EventLoop::removeChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_) {
        assert(currentActiveChannel_ == channel ||
               std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    // no lock needed
    poller_->removeChannel(channel);
}



