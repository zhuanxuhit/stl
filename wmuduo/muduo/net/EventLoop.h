//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#pragma once

#include <boost/core/noncopyable.hpp>
#include <muduo/base/Timestamp.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/TimerId.h>

//#include <muduo/net/Poller.h> // 交叉引用问题
#include <memory>
#include <vector>
#include <muduo/base/Mutex.h>

namespace muduo {
    namespace net {
        // 前向声明
        class Poller;
        class Channel;
        class TimerQueue;
//        class TimerId;


        class EventLoop : boost::noncopyable {
        public:
            typedef std::function<void()> Functor;

            EventLoop();

            ~EventLoop();

            void loop();

            void quit();

            Timestamp pollReturnTime() const { return pollerReturnTime_; }

            void updateChannel(Channel *channel);

            void removeChannel(Channel *channel);

            void assertInLoopThread() {
                if (!isInLoopThread()) {
                    abortNotInLoopThread();
                }
            }

            bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

            static EventLoop *getEventLoopOfCurrentThread();


            // 定时器相关
            TimerId runAt(const Timestamp& time, const TimerCallback& cb);
            TimerId runAfter(double delay, const TimerCallback& cb);
            TimerId runEvery(double interval, const TimerCallback& cb);
            void cancel(TimerId timerId);

            void runInLoop(const Functor&cb);
            void queueInLoop(const Functor&cb);
            void wakeup();

            bool eventHandling() const { return eventHandling_; }
            bool callingPendingFuncors() const { return callingPendingFuncors_;}
        private:
            void handleRead(Timestamp receiveTime);
            void doPendingFunctors();

            void printActiveChannels() const;

            void abortNotInLoopThread();

            bool looping_;
            bool quit_;
            bool eventHandling_;
            bool callingPendingFuncors_;
            const pid_t threadId_;
            const int wakeupFd_;
            std::unique_ptr<Channel> wakeupChannel_;

            typedef std::vector<Channel *> ChannelList;
            ChannelList activeChannels_;
            Timestamp pollerReturnTime_;
            Channel* currentActiveChannel_;
            // auto delete
            std::unique_ptr<Poller> poller_;
            std::unique_ptr<TimerQueue> timerQueue_;

            MutexLock mutex_;
            std::vector<Functor> pendingFunctors_;
        };
    }
}
