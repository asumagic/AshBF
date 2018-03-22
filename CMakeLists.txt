cmake_minimum_required(VERSION 3.9)
project(ashbf)

add_executable(ashbf
	"src/main.cpp"
	"src/commandline.cpp"
	"src/bf/logger.cpp"
	"src/bf/compiler.cpp"
	"src/bf/il.cpp"
	"src/bf/linker.cpp"
	"src/bf/optimizer.cpp"
	"src/bf/vm.cpp")

target_compile_options(ashbf PRIVATE -Wall -fno-exceptions -std=c++17 $<$<CONFIG:RELEASE>:-Os>)
