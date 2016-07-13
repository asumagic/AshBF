cmake_minimum_required(VERSION 2.8.11)
project(AshBF)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -fstrict-aliasing -fomit-frame-pointer -msse2 -std=c++11 -flto -fwhole-program")

add_executable(AshBF
			   src/main.cpp
			   src/fileutils.hpp src/fileutils.cpp
			   src/bf/bf.hpp src/bf/bcinterpreter.cpp src/bf/compiler.cpp)
