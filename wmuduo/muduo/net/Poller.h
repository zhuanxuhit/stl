//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#pragma once

#include <vector>
#include <muduo/base/Timestamp.h>
#include <muduo/net/EventLoop.h>

namespace muduo {
    namespace net {
        class Channel;

        class Poller : boost::noncopyable {
        public:
            typedef std::vector<Channel *> ChannelList;

            Poller(EventLoop* loop);


            virtual ~Poller();

            // 因为是抽象类，需要子类继承，此处设置为0，表示的是必须子类实现
            virtual Timestamp poll(int timeouMs, ChannelList *activeChannels)=0;

            virtual void updateChannel(Channel *channel)=0;

            virtual void removeChannel(Channel *channel)=0;

            void assertInLoopThread() {
                ownerLoop_->assertInLoopThread();
            }

            // 工厂方法
            static Poller *newDefaultPoller(EventLoop *loop);

        private:
            EventLoop *ownerLoop_;
        };
    }
}
