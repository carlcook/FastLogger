cmake_minimum_required(VERSION 2.8.11)

project(FastLogger C CXX)

file(GLOB MY_SRCS "lib/*.cpp" "lib/*.h")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(${PROJECT_NAME} STATIC ${MY_SRCS})

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/lib")


