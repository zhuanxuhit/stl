//
// Created by Wang,Chao(EBDPRD02) on 2018/5/18.
//

#include <muduo/net/poller/EPollPoller.h>
#include <sys/epoll.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

EPollPoller::EPollPoller(EventLoop *loop) :
        Poller(loop),
        epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
        eventList_(kInitEventListSize){
    if (epollfd_ < 0)
    {
        LOG_SYSFATAL << "EPollPoller::EPollPoller";
    }
}

Timestamp EPollPoller::poll(int timeouMs, ChannelList *activeChannels) {
    int numEvents =  ::epoll_wait(epollfd_,eventList_.data(),eventList_.size(),timeouMs);
    Timestamp now(Timestamp::now());
    if (numEvents>0){
        LOG_TRACE << numEvents << " events happended";
        fillActiveChannels(numEvents, activeChannels);
    }


    return Timestamp();
}

void EPollPoller::updateChannel(Channel *channel) {

}

void EPollPoller::removeChannel(Channel *channel) {

}

EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
    for(int i=0;i<numEvents;i++){
        struct epoll_event &event = eventList_[i];
        // update channel revents
        Channel* channel =  static_cast<Channel*>(event.data.ptr);
        channel->set_revents(event.events);
        activeChannels->push_back(channel);
    }
}
