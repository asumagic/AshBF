cmake_minimum_required(VERSION 2.8.11)
project(AshBF)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -ffast-math -fomit-frame-pointer -fstrict-aliasing -msse2 -std=c++14 -flto -fwhole-program")

add_executable(AshBF
			   src/main.cpp
			   src/fileutils.hpp src/fileutils.cpp
			   src/bf/bf.hpp src/bf/bcinterpreter.cpp src/bf/compiler.cpp src/bf/optimizer.cpp src/bf/linker.cpp)
