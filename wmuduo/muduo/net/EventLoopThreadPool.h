//
// Created by zhuanxu on 2018/5/24.
//

#ifndef PROJECT_EVENTLOOPTHREADPOOL_H
#define PROJECT_EVENTLOOPTHREADPOOL_H


#include <boost/noncopyable.hpp>
#include <functional>
#include <boost/ptr_container/ptr_vector.hpp>

namespace muduo {
    namespace net {
        class EventLoop;
        class EventLoopThread;
        class EventLoopThreadPool : boost::noncopyable {
        public:
            typedef std::function<void(EventLoop*)> ThreadInitCallback;

            explicit EventLoopThreadPool(EventLoop*baseLoop);
            ~EventLoopThreadPool();

            void setThreadNum(int numThreads) { numThreads_ = numThreads; }
            void start(const ThreadInitCallback& cb = ThreadInitCallback());
            EventLoop* getNextLoop();

        private:
            EventLoop* baseLoop_;
            bool started_;
            int numThreads_;
            int next_;
            boost::ptr_vector<EventLoopThread> threads_;
            std::vector<EventLoop*> loops_;
        };
    }
}



#endif //PROJECT_EVENTLOOPTHREADPOOL_H
