//
// Created by Wang,Chao(EBDPRD02) on 2018/5/21.
//

#pragma once

#include <muduo/base/copyable.h>
#include <cstdint>

namespace muduo {
    namespace net {
        class Timer;
        class TimerId : public muduo::copyable {
        public:
            TimerId()
                    : timer_(nullptr),
                      sequence_(0) {
            }

            TimerId(Timer *timer, int64_t seq)
                    : timer_(timer),
                      sequence_(seq) {
            }

            // default copy-ctor, dtor and assignment are okay

            friend class TimerQueue;

        private:
            Timer *timer_;
            int64_t sequence_;
        };
    }
}

