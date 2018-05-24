//
// Created by zhuanxu on 2018/5/23.
//

#ifndef PROJECT_TCPCONNECTION_H
#define PROJECT_TCPCONNECTION_H

#include <boost/core/noncopyable.hpp>
#include <memory>
#include <muduo/base/Timestamp.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/Callbacks.h>

namespace muduo {
    namespace net {
        class Socket;

        class EventLoop;

        class Channel;

        class TcpConnection : boost::noncopyable,
                              public std::enable_shared_from_this<TcpConnection> {
        public:
            TcpConnection(EventLoop *loop,
                          const string &name,
                          int sockfd,
                          const InetAddress &localAddr,
                          const InetAddress &peerAddr);
            ~TcpConnection();

            EventLoop* getLoop() { return loop_;}
            const string& name() const { return name_; }
            const InetAddress& localAddress() { return localAddr_; }
            const InetAddress& peerAddress() { return peerAddr_; }
            bool connected() const { return state_ == kConnected; }



            void setConnectionCallback(const ConnectionCallback& cb)
            { connectionCallback_ = cb; }

            void setMessageCallback(const MessageCallback& cb)
            { messageCallback_ = cb; }
            /// Internal use only.
            void setCloseCallback(const CloseCallback& cb)
            { closeCallback_ = cb; }

            // called when TcpServer accepts a new connection
            // 被TcpServer调用
            void connectEstablished();   // should be called only once
            void connectDestroyed();
        private:
            enum StateE {
                kConnecting, kConnected,kDisconnecting,kDisconnected
            };
            void setState(StateE s) { state_ = s; }

            void handleRead(Timestamp receiveTime);
            void handleClose();
            void handleError();

            EventLoop *loop_;
            string name_;
            StateE state_;
            std::unique_ptr<Socket> socket_;
            std::unique_ptr<Channel> channel_;
            InetAddress localAddr_;
            InetAddress peerAddr_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            CloseCallback closeCallback_;
        };
    }
}


#endif //PROJECT_TCPCONNECTION_H
