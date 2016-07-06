cmake_minimum_required(VERSION 3.2)
project(MarketSaver)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
link_directories(./lib)
add_executable(MarketSaver ${SOURCE_FILES} lib/c.o)
target_link_libraries (MarketSaver pthread)