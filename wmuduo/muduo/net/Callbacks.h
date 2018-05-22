//
// Created by Wang,Chao(EBDPRD02) on 2018/5/21.
//

#pragma once

#include  <functional>

namespace muduo {
    namespace net {
        typedef std::function<void()> TimerCallback;
    }
}
