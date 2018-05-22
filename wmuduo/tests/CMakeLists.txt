add_executable(reactor_test01 Reactor_test01.cpp)
target_link_libraries(reactor_test01 muduo_net)

add_executable(reactor_test02 Reactor_test02.cpp)
target_link_libraries(reactor_test02 muduo_net)


add_executable(reactor_test03 Reactor_test03.cpp)
target_link_libraries(reactor_test03 muduo_net)

add_executable(reactor_test04 Reactor_test04.cpp)
target_link_libraries(reactor_test04 muduo_net)

add_executable(rvo_test rvo_test.cpp)
add_executable(set_test set_test.cpp)