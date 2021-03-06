set(net_SRCS
        EventLoop.cpp
        Poller.cpp
        Channel.cpp
        TimerQueue.cpp
        InetAddress.cpp
        EventLoopThreadPool.cpp
        Socket.cpp
        Acceptor.cpp
        TcpServer.cpp
        TcpConnection.cpp
        SocketsOps.cpp
        Timer.cpp
        EventLoopThread.cpp
        poller/PollPoller.cpp
        poller/DefaultPoller.cpp
        poller/EPollPoller.cpp
  )

add_library(muduo_net ${net_SRCS})
target_link_libraries(muduo_net muduo_base)

install(TARGETS muduo_net DESTINATION lib)
set(HEADERS
        Acceptor.h
        Channel.h
        Endian.h
        EventLoop.h
        EventLoopThread.h
        EventLoopThreadPool.h
        InetAddress.h
        TimerId.h
        TcpConnection
  )
install(FILES ${HEADERS} DESTINATION include/muduo/net)

#add_subdirectory(http)
#add_subdirectory(inspect)
#
#if(NOT CMAKE_BUILD_NO_EXAMPLES)
#  add_subdirectory(tests)
#endif()
#
#if(PROTOBUF_FOUND)
#  add_subdirectory(protorpc)
#else()
#  add_subdirectory(protorpc EXCLUDE_FROM_ALL)
#endif()
