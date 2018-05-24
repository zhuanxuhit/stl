//
// Created by Wang,Chao(EBDPRD02) on 2018/5/18.
//

#include <muduo/net/poller/EPollPoller.h>
#include <muduo/net/Channel.h>
//#include <sys/epoll.h>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

namespace {
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
}

EPollPoller::EPollPoller(EventLoop *loop) :
        Poller(loop),
        epollfd_(::epoll_create1(EPOLL_CLOEXEC)), // EPOLL_CLOEXEC 防止子进程有fd的引用
        eventList_(kInitEventListSize) {
    if (epollfd_ < 0) {
        LOG_SYSFATAL << "EPollPoller::EPollPoller";
    }
}

EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeouMs, ChannelList *activeChannels) {
    int numEvents = ::epoll_wait(epollfd_, eventList_.data(), eventList_.size(), timeouMs);
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        LOG_TRACE << numEvents << " events happended";
        fillActiveChannels(numEvents, activeChannels);
        if (implicit_cast<size_t>(numEvents) == eventList_.size())
        {
            eventList_.resize(eventList_.size()*2);
        }
    } else if (numEvents == 0) {
        LOG_TRACE << " nothing happended";
    } else {
        LOG_SYSERR << "EPollPoller::poll()";
    }

    return now;
}

void EPollPoller::updateChannel(Channel *channel) {
    assertInLoopThread();
    LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
    const int index = channel->index();

    if (index == kNew || index == kDeleted) {
        int fd = channel->fd();
        if (index == kNew) {
            channelMap_[fd] = channel;
        } else {
            // delete
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        // old channel
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel) {
    assertInLoopThread();
    assert(channel->isNoneEvent());
    int fd = channel->fd();
    LOG_TRACE << "fd = " << fd;
    int index = channel->index();
    channelMap_.erase(fd);
    if (index == kAdded){
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}


void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels)  {
    for (int i = 0; i < numEvents; i++) {
        struct epoll_event &event = eventList_[i];
        // update channel revents
        Channel *channel = static_cast<Channel *>(event.data.ptr);
        channel->set_revents(event.events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::update(int operation, Channel *channel) {
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_SYSERR << "epoll_ctl op=" << operation << " fd=" << fd;
        }
        else
        {
            LOG_SYSFATAL << "epoll_ctl op=" << operation << " fd=" << fd;
        }
    }
}
