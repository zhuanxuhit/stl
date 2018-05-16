//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#pragma once

#include <boost/core/noncopyable.hpp>
#include <muduo/base/CurrentThread.h>

namespace muduo {
    namespace net {
        class EventLoop : boost::noncopyable {
        public:
            EventLoop();
            ~EventLoop();

            void loop();

            void assertInLoopThread() {
                if (!isInLoopThread()){
                    abortNotInLoopThread();
                }
            }
            bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

        private:
            void abortNotInLoopThread();
            bool looping_;
            const pid_t threadId_;

        };
    }
}
