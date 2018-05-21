//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#include <poll.h>
#include <muduo/net/Channel.h>
#include <muduo/base/Logging.h>
#include <muduo/net/Poller.h>
#include <muduo/net/poller/PollPoller.h>

using namespace muduo;
using namespace muduo::net;

PollPoller::PollPoller(EventLoop *loop) : Poller(loop) {

}

PollPoller::~PollPoller() = default;

Timestamp PollPoller::poll(int timeouMs, Poller::ChannelList *activeChannels) {
    // 我们使用poll方法
//    return Timestamp();
// int poll(struct pollfd *fds, nfds_t nfds, int timeout);
    // a file descriptor becomes ready;
    // the call is interrupted by a signal handler; or
    // the timeout expires.
    int numEvents = ::poll(pollFdList_.data(), static_cast<nfds_t>(pollFdList_.size()), timeouMs);
    Timestamp now(Timestamp::now());
    // 返回值有3
    // >0 =0 <0
    if (numEvents > 0) {
        LOG_TRACE << numEvents << " events happended";
        // 表示已经ok的Channel，我们将ok的channel加入到 activeChannels
        fillActiveChannels(activeChannels, numEvents);
    } else if (numEvents == 0) {
        LOG_TRACE << " nothing happended";
    } else {
        LOG_SYSERR << "PollPoller::poll()";
    }
    return now;
}

void PollPoller::fillActiveChannels(ChannelList *activeChannels, int numEvents) {
    for (int i = 0; i < pollFdList_.size() && numEvents > 0; i++) {
        if (pollFdList_[i].revents > 0) {
            Channel *channel = channelMap_[(pollFdList_[i].fd)];
            channel->set_revents(pollFdList_[i].revents);
            activeChannels->push_back(channel);
            numEvents--;
        }
    }
}

void PollPoller::updateChannel(Channel *channel) {
    // 只有在io线程中调用才是线程安全的，否则需要枷锁
    assertInLoopThread();
    // 找到 channel 在 pollFdList_ 中的位置，并且更新
    if (channel->index()<0){
        // 标明是新的Channel
        struct pollfd pfd{};
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollFdList_.push_back(pfd);
        auto idx = static_cast<int>(pollFdList_.size() - 1);
        channel->set_index(idx);
        channelMap_[pfd.fd] = channel;
    }else {
        // 作为一个老的Channel
        int idx = channel->index();
        struct pollfd &pfd = pollFdList_[idx];
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()){
            pfd.fd = -pfd.fd - 1; // 负数表示不关心,-1是为了当fd=0的时候也可以满足
        }
    }

}

void PollPoller::removeChannel(Channel *channel) {
    assertInLoopThread();
    assert(channel->isNoneEvent());
    int idx = channel->index();
    // 去除 channelMap_ , 需要 fd
    channelMap_.erase(channel->fd());
    // 去除 pollFdList_
    if (idx == pollFdList_.size() -1 ){
        pollFdList_.pop_back();
    }else {
        // 将Channel 与 最后一个channel交换
        int channelFdAtEnd = pollFdList_.back().fd;
        std::iter_swap(pollFdList_.begin()+idx, pollFdList_.end()-1);
        pollFdList_.pop_back();
        // 万一 channelAtEnd < 0
        if(channelFdAtEnd < 0) {
            channelFdAtEnd = - channelFdAtEnd - 1;
        }
        channelMap_[channelFdAtEnd]->set_index(idx);
    }
}
