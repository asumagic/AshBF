cmake_minimum_required(VERSION 2.8.11)
project(AshBF)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -fomit-frame-pointer -fstrict-aliasing -msse2 -std=c++14")

add_executable(AshBF
			   src/main.cpp
			   src/fileutils.cpp
			   src/bf/bcinterpreter.cpp src/bf/compiler.cpp src/bf/optimizer.cpp src/bf/linker.cpp)
