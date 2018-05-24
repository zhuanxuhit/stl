//
// Created by zhuanxu on 2018/5/22.
//

#ifndef PROJECT_ACCEPTOR_H
#define PROJECT_ACCEPTOR_H

#include <boost/core/noncopyable.hpp>
#include <functional>
#include <muduo/net/InetAddress.h>
#include <muduo/net/Socket.h>
#include <muduo/net/Channel.h>

namespace muduo{
    namespace net {
        class EventLoop;
        class Acceptor : boost::noncopyable{
        public:
            typedef std::function<void (int fd,const InetAddress&)> NewConnectionCallback;

            Acceptor(EventLoop* loop, const InetAddress& listenAddr);
            ~Acceptor();
            void setNewConnectionCallback(const NewConnectionCallback& cb)
            { newConnectionCallback_ = cb; }

            bool listenning() const { return listening_; }
            void listen();

        private:
            void handleRead();
            NewConnectionCallback newConnectionCallback_;
            bool listening_;
            EventLoop* loop_;
            Socket acceptSocket_;
            Channel acceptChannel_;
            int idleFd_;
        };
    }
}




#endif //PROJECT_ACCEPTOR_H
