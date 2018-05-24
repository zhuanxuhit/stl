//
// Created by zhuanxu on 2018/5/22.
//

#ifndef PROJECT_INETADDRESS_H
#define PROJECT_INETADDRESS_H

#include <muduo/base/copyable.h>
#include <muduo/base/StringPiece.h>

#include <netinet/in.h>

namespace muduo {
    namespace net {
        class InetAddress : public muduo::copyable {
        public:
            explicit InetAddress(uint16_t port);

            /// Constructs an endpoint with given ip and port.
            /// @c ip should be "1.2.3.4"
            InetAddress(const StringPiece& ip, uint16_t port);

            InetAddress(const struct sockaddr_in& addr)
                    : addr_(addr)
            { }
            string toIp() const;
            string toIpPort() const;

            // __attribute__ ((deprecated)) 表示该函数是过时的，被淘汰的
            // 这样使用该函数，在编译的时候，会发出警告
            string toHostPort() const __attribute__ ((deprecated))
            { return toIpPort(); }

            // default copy/assignment are Okay

            const struct sockaddr_in& getSockAddrInet() const { return addr_; }
            void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }

            uint32_t ipNetEndian() const { return addr_.sin_addr.s_addr; }
            uint16_t portNetEndian() const { return addr_.sin_port; }

        private:
            struct sockaddr_in addr_;
        };
    }
}




#endif //PROJECT_INETADDRESS_H
