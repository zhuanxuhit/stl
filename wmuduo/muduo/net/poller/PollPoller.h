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
        class PollPoller : Poller{
        public:
            PollPoller(EventLoop *loop);

            virtual ~PollPoller();

            virtual Timestamp poll(int timeouMs, ChannelList *activeChannels);

            virtual void updateChannel(Channel *channel);

            virtual void removeChannel(Channel *channel);

        private:
            typedef std::vector<struct pollfd> PollFdList;
            typedef std::map<int,Channel*> ChannelMap;

            PollFdList pollFdList_;
            ChannelMap channelMap_;

            void fillActiveChannels(ChannelList *activeChannels, int numEvents) const;
        };
    }
}



#endif //PROJECT_POLLPOLLER_H
