cmake_minimum_required(VERSION 3.9)
project(ashbf)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_compile_definitions(
    FMT_USE_FLOAT=0
    FMT_USE_DOUBLE=0
    FMT_USE_LONG_DOUBLE=0
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=thin -fno-pic -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables")

add_subdirectory(vendor/fmt)

add_executable(ashbf
	"src/bf/compiler.cpp"
	"src/bf/disasm.cpp"
	"src/bf/linker.cpp"
	"src/bf/logger.cpp"
	"src/bf/optimizer.cpp"
	"src/bf/vm.cpp"
	"src/bf/codegen/asm-x86-64.cpp"
	"src/bf/codegen/c.cpp"
	"src/main.cpp"
	"src/cli.cpp"
)

target_compile_options(ashbf PRIVATE
	"-Wall"
	"-Wextra"
	"-std=c++20"
    "-gsplit-dwarf"
	"-ggdb"
	$<$<CONFIG:RELEASE>:-O3 -DNDEBUG>
	$<$<CONFIG:DEBUG>:-Og -g>
)

target_link_options(ashbf PRIVATE
	"-no-pie"
	"-flto=thin"
	"-fuse-ld=lld"
    "-gsplit-dwarf"
	"-Wl,--thinlto-cache-dir=${PROJECT_BINARY_DIR}/lto.cache"
    "-Wl,--icf=all"
    "-Wl,--gc-sections"
)

target_precompile_headers(ashbf PRIVATE
	"src/pch.hpp"
)

target_link_libraries(ashbf PRIVATE
	fmt
)
