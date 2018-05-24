//
// Created by zhuanxu on 2018/5/23.
//

#ifndef PROJECT_TCPSERVER_H
#define PROJECT_TCPSERVER_H

#include <map>
#include <memory>
#include <boost/noncopyable.hpp>

#include <muduo/base/Types.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>

namespace muduo {
    namespace net {
        class EventLoop;
        class Acceptor;

        class TcpServer : boost::noncopyable {
        public:
            TcpServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg);

            ~TcpServer();

            const string &hostport() const { return hostport_; }

            const string &name() const { return name_; }

            void start();

            void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

            void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

        private:
            void newConnection(int sockfd, const InetAddress &peerAddr);

            void removeConnection(const TcpConnectionPtr&);

            typedef std::map<string, TcpConnectionPtr> ConnectionMap;

            EventLoop *loop_;
            // 处理tcp连接建立，内部自带socket
            std::unique_ptr<Acceptor> acceptor_;
            const string hostport_;
            const string name_;
            bool started_;
            int nextConnId_;
            ConnectionMap connections_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;

        };
    }
}


#endif //PROJECT_TCPSERVER_H
