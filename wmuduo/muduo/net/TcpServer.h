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
        class EventLoopThreadPool;
        class Acceptor;

        class TcpServer : boost::noncopyable {
        public:
            typedef std::function<void(EventLoop*)> ThreadInitCallback;

            TcpServer(EventLoop *loop, const InetAddress &listenAddr, const string &nameArg);

            ~TcpServer();

            const string &hostport() const { return hostport_; }

            const string &name() const { return name_; }

            void start();

            void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

            void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }


            /// Set the number of threads for handling input.
            ///
            /// Always accepts new connection in loop's thread.
            /// Must be called before @c start
            /// @param numThreads
            /// - 0 means all I/O in loop's thread, no thread will created.
            ///   this is the default value.
            /// - 1 means all I/O in another thread.
            /// - N means a thread pool with N threads, new connections
            ///   are assigned on a round-robin basis.
            void setThreadNum(int numThreads);
        private:
            void newConnection(int sockfd, const InetAddress &peerAddr);

            void removeConnection(const TcpConnectionPtr&);
            /// Not thread safe, but in loop
            void removeConnectionInLoop(const TcpConnectionPtr& conn);

            typedef std::map<string, TcpConnectionPtr> ConnectionMap;

            EventLoop *loop_;
            // 处理tcp连接建立，内部自带socket
            std::unique_ptr<Acceptor> acceptor_;
            std::unique_ptr<EventLoopThreadPool> threadPool_;

            const string hostport_;
            const string name_;
            bool started_;
            int nextConnId_;
            ConnectionMap connections_;
            ConnectionCallback connectionCallback_;
            MessageCallback messageCallback_;
            ThreadInitCallback threadInitCallback_;	// IO线程池中的线程在进入事件循环前，会回调用此函数
        };
    }
}


#endif //PROJECT_TCPSERVER_H
