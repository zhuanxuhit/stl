//
// Created by Wang,Chao(EBDPRD02) on 2018/5/16.
//

#ifndef PROJECT_CHANNEL_H
#define PROJECT_CHANNEL_H


#include <functional>
#include <boost/core/noncopyable.hpp>
#include <muduo/base/Types.h>
#include <muduo/base/Timestamp.h>
#include <memory>

namespace muduo {
    namespace net {
        class EventLoop;

        class Channel : boost::noncopyable {
        public:

            typedef std::function<void(Timestamp)> ReadEventCallback;
            typedef std::function<void()> EventCallback;


            Channel(EventLoop *loop, int fd);

            ~Channel();

            void set_revents(int revt) { revents_ = revt; } // used by pollers

            int index() const { return index_; }

            int fd() const { return fd_; }

            int events() const { return events_; }

            void set_index(int idx) { index_ = idx; }

            bool isNoneEvent() const { return events_ == kNoneEvent; }

            string reventsToString()const;

            EventLoop* ownerLoop() const;
            void remove();

            void handleEvent(Timestamp receiveTime);
            void setReadCallback(const ReadEventCallback& cb)
            { readCallback_ = cb; }
            void setWriteCallback(const EventCallback& cb)
            { writeCallback_ = cb; }
            void setCloseCallback(const EventCallback& cb)
            { closeCallback_ = cb; }
            void setErrorCallback(const EventCallback& cb)
            { errorCallback_ = cb; }

            void enableReading() { events_ |= kReadEvent; update(); }
            // void disableReading() { events_ &= ~kReadEvent; update(); }
            void enableWriting() { events_ |= kWriteEvent; update(); }
            void disableWriting() { events_ &= ~kWriteEvent; update(); }
            void disableAll() { events_ = kNoneEvent; update(); }
            bool isWriting() const { return events_ & kWriteEvent; }

            /// Tie this channel to the owner object managed by shared_ptr,
            /// prevent the owner object being destroyed in handleEvent.
            void tie(const std::shared_ptr<void>&);
        private:
            void update();


            ReadEventCallback readCallback_;
            EventCallback writeCallback_;
            EventCallback closeCallback_;
            EventCallback errorCallback_;



            EventLoop *loop_;
            const int fd_;
            int events_;
            int revents_;
            int index_; // 标明 Channel 在 poller vector 中的位置
            bool tied_;
            // 弱引用其他指针
            std::weak_ptr<void> tie_;

            static const int kNoneEvent;
            static const int kReadEvent;
            static const int kWriteEvent;
        };
    }
}


#endif //PROJECT_CHANNEL_H
