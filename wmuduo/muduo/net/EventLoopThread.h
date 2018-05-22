//
// Created by Wang,Chao(EBDPRD02) on 2018/5/22.
//

#pragma once

#include <functional>
#include <boost/core/noncopyable.hpp>
#include <muduo/base/Thread.h>
#include <muduo/base/Mutex.h>
#include <muduo/base/Condition.h>

namespace muduo {
    namespace net {
        class EventLoop;
        class EventLoopThread : boost::noncopyable {
        public:
            typedef std::function<void(EventLoop*)> ThreadInitCallback;

            explicit EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
            ~EventLoopThread();
            EventLoop* startLoop();

        private:
            void threadFunc();
            EventLoop* loop_;
            bool exiting_;
            Thread thread_;
            MutexLock mutex_;
            Condition cond_;
            ThreadInitCallback callback_;
        };
    }
}





