cmake_minimum_required(VERSION 3.9)
#project(stl)

set(CMAKE_CXX_STANDARD 11)

#add_executable(stl main.cpp iterator/skip_iterator.h iterator/matrix.h iterator/accumulate_iterator.h iterator/combination_iterator.h iterator/iterator.cpp)
add_subdirectory(iterator)
add_subdirectory(c++11)
#add_subdirectory(tag)
add_subdirectory(wmuduo)
add_subdirectory(memory)
add_subdirectory(hq)
add_subdirectory(rpc/proto)