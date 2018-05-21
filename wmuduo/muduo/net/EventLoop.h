//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#pragma once

#include <boost/core/noncopyable.hpp>
#include <vector>
#include <bits/unique_ptr.h>
#include <muduo/base/Timestamp.h>
#include <muduo/base/CurrentThread.h>

namespace muduo {
    namespace net {
        // 前向声明
        class Poller;

        class Channel;

        class EventLoop : boost::noncopyable {
        public:
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

        private:
            void printActiveChannels() const;

            void abortNotInLoopThread();

            bool looping_;
            bool quit_;
            bool eventHandling_;
            const pid_t threadId_;

            typedef std::vector<Channel *> ChannelList;
            ChannelList activeChannels_;
            Timestamp pollerReturnTime_;
            Channel* currentActiveChannel_;
            // auto delete
            std::unique_ptr<Poller> poller_;
        };
    }
}
