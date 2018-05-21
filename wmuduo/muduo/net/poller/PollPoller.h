//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#ifndef PROJECT_POLLPOLLER_H
#define PROJECT_POLLPOLLER_H

#include <muduo/net/Poller.h>
#include <map>
#include <vector>

namespace muduo {
    namespace net {
        class PollPoller : public Poller{
        public:
            explicit PollPoller(EventLoop *loop);

            ~PollPoller() override;

            Timestamp poll(int timeouMs, ChannelList *activeChannels) override;

            void updateChannel(Channel *channel) override;

            void removeChannel(Channel *channel) override;

        private:
            typedef std::vector<struct pollfd> PollFdList;
            typedef std::map<int,Channel*> ChannelMap;

            PollFdList pollFdList_;
            ChannelMap channelMap_;

            void fillActiveChannels(ChannelList *activeChannels, int numEvents);
        };
    }
}



#endif //PROJECT_POLLPOLLER_H
