//
// Created by Wang,Chao(EBDPRD02) on 2018/5/18.
//

#ifndef PROJECT_EPOLLPOLLER_H
#define PROJECT_EPOLLPOLLER_H

#include <muduo/net/Poller.h>
#include <map>
#include <sys/epoll.h>

namespace muduo {
    namespace net {
        class EPollPoller : public Poller{
        public:
            explicit EPollPoller(EventLoop *loop);
            ~EPollPoller() override ;

            Timestamp poll(int timeouMs, ChannelList *activeChannels) override;

            void updateChannel(Channel *channel) override;

            void removeChannel(Channel *channel) override;

        private:
            static const int kInitEventListSize = 16;
            void fillActiveChannels(int numEvents,
                                    ChannelList* activeChannels);
            void update(int operation, Channel* channel);

            typedef std::vector<struct epoll_event> EventList;
            typedef std::map<int, Channel*> ChannelMap;

            EventList eventList_;
            ChannelMap channelMap_;

            int epollfd_;
        };
    }
}




#endif //PROJECT_EPOLLPOLLER_H
