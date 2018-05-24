//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//


#include <muduo/net/EventLoop.h>
#include <muduo/net/TimerQueue.h>
#include <muduo/base/Logging.h>
#include <muduo/net/Poller.h>
#include <muduo/net/Channel.h>

#include <algorithm>
#include <memory>
#include <sys/eventfd.h>

using namespace muduo;
using namespace muduo::net;

namespace {
    __thread EventLoop *t_loopInThisThread = nullptr;
    const int kPollTimeMs = 10 * 1000;

    int createEventfd()
    {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0)
        {
            LOG_SYSERR << "Failed in eventfd";
            abort();
        }
        return evtfd;
    }
}


EventLoop *EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

EventLoop::EventLoop() :
        looping_(false),
        quit_(false),
        eventHandling_(false),
        currentActiveChannel_(nullptr),
        threadId_(CurrentThread::tid()),
        wakeupFd_(createEventfd()),
        wakeupChannel_(new Channel(this,wakeupFd_)),
        callingPendingFuncors_(false),
        timerQueue_(new TimerQueue(this)),
        poller_( Poller::newDefaultPoller(this) ){
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    // 如果当前线程已经创建过 eventloop 了，则终止
    if (t_loopInThisThread != nullptr) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    } else {
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this, std::placeholders::_1));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    t_loopInThisThread = nullptr;
    ::close(wakeupFd_);
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
        doPendingFunctors();
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
    if (!isInLoopThread()){
        wakeup();
    }
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

TimerId EventLoop::runAt(const Timestamp &time, const TimerCallback &cb) {
    return timerQueue_->addTimer(cb, time, 0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback &cb) {
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback &cb) {
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::cancel(TimerId timerId) {
    timerQueue_->cancel(timerId);
}

void EventLoop::runInLoop(const EventLoop::Functor &cb) {
    if (isInLoopThread()) {
        cb();
    }
    else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const EventLoop::Functor &cb) {
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }
    // 为何只有在IO线程调用的时候才不需要唤醒呢？
    // 因为之后马上就会调用 doPendingFunctors 调用
    // 如果有多个回调函数在非io线程中调用  queueInLoop 那不是会多次唤醒吗？
    // 这个没有关系？
    if (!isInLoopThread() || callingPendingFuncors_){
        wakeup();
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    //ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead(Timestamp receiveTime) {
    uint64_t one = 1;
    //ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    ssize_t n = ::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFuncors_ = true;

    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(auto cb : functors){
        cb();
    }
    callingPendingFuncors_ = false;
}



