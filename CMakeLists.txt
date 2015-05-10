cmake_minimum_required(VERSION 2.8)

project(FastLogger C CXX)

file(GLOB CPP_FILES lib/*.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(${PROJECT_NAME} STATIC ${CPP_FILES})
