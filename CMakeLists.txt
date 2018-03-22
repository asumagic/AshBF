cmake_minimum_required(VERSION 2.8.11)
project(ashbf)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++1z -Winline -fno-exceptions")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -fomit-frame-pointer -flto")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

file(GLOB_RECURSE headers "*.hpp")
#add_custom_target(headers SOURCES ${headers})

file(GLOB_RECURSE sources "*.cpp")
add_executable(${PROJECT_NAME} ${sources})

set(CMAKE_CXX_COMPILER "g++")
