//
// Created by Wang,Chao(EBDPRD02) on 2018/5/21.
//

#ifndef PROJECT_TIMERQUEUE_H
#define PROJECT_TIMERQUEUE_H


#include <boost/core/noncopyable.hpp>
#include <muduo/base/Timestamp.h>
#include <muduo/net/Channel.h>
#include <muduo/net/Callbacks.h>
#include <set>
#include <vector>
#include <memory>

//#include "TimerId.h"

namespace muduo {
    namespace net {
        class EventLoop;
        class Timer;
        class TimerId;
//        class TimerCallback;
//        class Timestamp;

        class TimerQueue : boost::noncopyable{
        public:
            explicit TimerQueue(EventLoop* loop);
            ~TimerQueue();
            // 需要线程安全
            TimerId addTimer(const TimerCallback &cb, Timestamp when , double interval);
            void cancel(const TimerId &timerId);



        private:
            // 内存怎么存储这些定时器，通过二叉树
            typedef std::pair<Timestamp,Timer*> Entry;
            typedef std::set<Entry> TimerList;
            typedef std::pair<Timer*,int64_t> ActiveTimer;
            typedef std::set<ActiveTimer> ActiveTimerSet;


            const int timerfd_;
            Channel timerFdChannel_;
            TimerList timers_; // timers_是按到期时间排序
            ActiveTimerSet activeTimers_;// 按照timer地址对象排序
            EventLoop* loop_;
            bool callingExpiredTimers_; /* atomic */
            ActiveTimerSet cancelingTimers_;	// 保存的是被取消的定时器



            // 返回超时的定时器列表
            std::vector<Entry> getExpired(Timestamp now);
            void reset(const std::vector<Entry>& expired, Timestamp now);

            // 以下成员函数只可能在其所属的I/O线程中调用，因而不必加锁。
            // 服务器性能杀手之一是锁竞争，所以要尽可能少用锁
            void addTimerInLoop(Timer* timer);
            void cancelInLoop(const TimerId & timerId);

            void handleRead(Timestamp receiveTime);
            bool insert(Timer* timer);
        };
    }
}




#endif //PROJECT_TIMERQUEUE_H
