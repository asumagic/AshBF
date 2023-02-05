cmake_minimum_required(VERSION 3.9)
project(ashbf)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

set(core_files
	"src/bf/compiler.cpp"
	"src/bf/disasm.cpp"
	"src/bf/linker.cpp"
	"src/bf/logger.cpp"
	"src/bf/optimizer.cpp"
	"src/bf/vm.cpp"
	"src/bf/codegen/asm-x86-64.cpp"
	"src/bf/codegen/c.cpp")

set(core_flags
	"-Wall"
	"-Wextra"
	"-fno-exceptions"
	"-std=c++20"
	"-ggdb"
	"-fnew-infallible"
	"-mllvm" "-align-all-nofallthru-blocks=7"
	$<$<CONFIG:RELEASE>:-O3 -DNDEBUG -flto=thin>
	$<$<CONFIG:DEBUG>:-Og -g>)

add_executable(ashbf
	${core_files}
	"src/main.cpp"
	"src/cli.cpp"
)

target_compile_options(ashbf PRIVATE ${core_flags} "-fno-pic")
target_link_options(ashbf PRIVATE
	"-no-pie"
	"-flto=thin"
	"-fuse-ld=lld"
	"-Wl,--thinlto-cache-dir=${PROJECT_BINARY_DIR}/lto.cache"
)

target_precompile_headers(ashbf PRIVATE
	"src/pch.hpp"
)

target_link_libraries(ashbf PRIVATE
	fmt
)
