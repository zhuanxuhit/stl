//
// Created by Wang,Chao(EBDPRD02) on 2018/5/21.
//

#include <muduo/net/TimerQueue.h>
#include <sys/timerfd.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Timer.h>
#include <muduo/net/Channel.h>

#include <iterator>
#include <vector>
#include <functional>


namespace muduo {
    namespace net {
        namespace detail {
            // 创建定时器
            int createTimerfd() {
                int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                               TFD_NONBLOCK | TFD_CLOEXEC);
                if (timerfd < 0) {
                    LOG_SYSFATAL << "Failed in timerfd_create";
                }
                return timerfd;
            }
            // 清除定时器，避免一直触发
            void readTimerfd(int timerfd, Timestamp now)
            {
                uint64_t howmany;
                ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
                LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
                if (n != sizeof howmany)
                {
                    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
                }
            }
            // 计算超时时刻与当前时间的时间差
            struct timespec howMuchTimeFromNow(Timestamp when)
            {
                int64_t microseconds = when.microSecondsSinceEpoch()
                                       - Timestamp::now().microSecondsSinceEpoch();
                if (microseconds < 100)
                {
                    microseconds = 100;
                }
                struct timespec ts;
                ts.tv_sec = static_cast<time_t>(
                        microseconds / Timestamp::kMicroSecondsPerSecond);
                ts.tv_nsec = static_cast<long>(
                        (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
                return ts;
            }
            // 重置定时器的超时时间
            void resetTimerfd(int timerfd, Timestamp expiration)
            {
                // wake up loop by timerfd_settime()
                struct itimerspec newValue;
                struct itimerspec oldValue;
                bzero(&newValue, sizeof newValue);
                bzero(&oldValue, sizeof oldValue);
                newValue.it_value = howMuchTimeFromNow(expiration);
                int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
                if (ret)
                {
                    LOG_SYSERR << "timerfd_settime()";
                }
            }
        }
    }

}


using namespace muduo;
using namespace muduo::net;
using namespace muduo::net::detail;


TimerQueue::TimerQueue(EventLoop *loop) :
        loop_(loop),
        timerfd_(createTimerfd()),
        timerFdChannel_(loop, timerfd_),
        callingExpiredTimers_(false),
        timers_() {
    timerFdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead,this,std::placeholders::_1));
    timerFdChannel_.enableReading();
}

TimerQueue::~TimerQueue() {
    ::close(timerfd_);
    for (auto entry : timers_) {
        delete entry.second;
    }
}

void TimerQueue::handleRead(Timestamp receiveTime) {
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());

    readTimerfd(timerfd_,now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for(auto &entry : expired){
        (entry.second)->run();
    }
    callingExpiredTimers_ = false;
    // 不是一次性定时器，需要重启
    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry sentry(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
    // lower_bound 返回第一个大于 sentry 的迭代器
    // end->first > now
    auto end =  timers_.lower_bound(sentry);
//    std::copy(std::make_move_iterator(timers_.begin()),
//              std::make_move_iterator(end),
//              std::back_inserter(expired));
    std::copy(timers_.begin(),end,std::back_inserter(expired));

    timers_.erase(timers_.begin(),end);

    return expired;
}

void TimerQueue::reset(const std::vector<TimerQueue::Entry> &expired, Timestamp now) {
    // 因为定时器超时了，我们现在需要重新设定定时器参数
    Timestamp nextExpire;
    for (auto &it : expired){
        ActiveTimer timer(it.second,it.second->sequence());
        if (it.second->repeat() && cancelingTimers_.find(timer) == cancelingTimers_.end()){
            it.second->restart(now);
            insert(it.second);
        }else {
            delete it.second;
        }
    }
    if (!timers_.empty()){
        nextExpire = timers_.begin()->second->expiration();
    }
    if (nextExpire.valid()){
        resetTimerfd(timerfd_,nextExpire);
    }
}

TimerId TimerQueue::addTimer(const TimerCallback &cb, Timestamp when, double interval) {
    auto timer = new Timer(cb,when,interval);
    addTimerInLoop(timer);
    return {timer,timer->sequence()};
}

void TimerQueue::addTimerInLoop(Timer *timer) {
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged){
        resetTimerfd(timerfd_,timer->expiration());
    }
}

bool TimerQueue::insert(Timer *timer) {
    // 类似于加锁
    loop_->assertInLoopThread();
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    if (timers_.empty() || when < timers_.begin()->first) {
        earliestChanged = true;
    }
    timers_.insert(Entry(when,timer));
    activeTimers_.insert(ActiveTimer(timer,timer->sequence()));
    return earliestChanged;
}

void TimerQueue::cancel(const TimerId &timerId) {
    cancelInLoop(timerId);
}

void TimerQueue::cancelInLoop(const TimerId &timerId) {
    loop_->assertInLoopThread();
    ActiveTimer activeTimer(timerId.timer_,timerId.sequence_);
    auto it =  activeTimers_.find(activeTimer);
    if (it != activeTimers_.end()){
        timers_.erase(Entry(it->first->expiration(),it->first));
        activeTimers_.erase(it);
    }
    else if (callingExpiredTimers_){
        // 没找到有可能是正在调用超时定时器
        cancelingTimers_.insert(activeTimer);
    }
}
