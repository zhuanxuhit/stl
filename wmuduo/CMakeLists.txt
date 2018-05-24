
project(muduo CXX)

#set(CMAKE_CXX_STANDARD 11)


set(CXX_FLAGS
        -g
        # -DVALGRIND
#        -DMUDUO_STD_STRING
        -D_FILE_OFFSET_BITS=64
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wno-unused-parameter
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
        -march=native
        # -MMD
        # -std=c++0x
        -rdynamic
        )
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_package(Boost REQUIRED)
find_package(Protobuf)
find_package(CURL)
find_path(CARES_INCLUDE_DIR ares.h)
find_library(CARES_LIBRARY NAMES cares)
find_path(MHD_INCLUDE_DIR microhttpd.h)
find_library(MHD_LIBRARY NAMES microhttpd)
find_library(BOOSTTEST_LIBRARY NAMES boost_unit_test_framework)

include_directories(${Boost_INCLUDE_DIRS})

# 最近的一个 project 目录
include_directories(${PROJECT_SOURCE_DIR})

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

#add_subdirectory(base)
#add_subdirectory(net)
add_subdirectory(muduo/base)
add_subdirectory(muduo/net)
#if(NOT CMAKE_BUILD_NO_EXAMPLES)
#  add_subdirectory(examples)
#else()
#  if(CARES_INCLUDE_DIR AND CARES_LIBRARY)
#    add_subdirectory(examples/cdns)
#  endif()
#endif()

#add_definitions(-DMUDUO_STD_STRING)
add_subdirectory(tests)