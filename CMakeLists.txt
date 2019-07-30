cmake_minimum_required(VERSION 3.9)
project(ashbf)

option(js "Generate a JavaScript library using emscripten and WebAssembly" off)

set(core_files
	"src/bf/compiler.cpp"
	"src/bf/disasm.cpp"
	"src/bf/linker.cpp"
	"src/bf/logger.cpp"
	"src/bf/optimizer.cpp"
	"src/bf/vm.cpp"
	"src/bf/codegen/asm-x86-64.cpp"
	"src/bf/codegen/c.cpp"
	"src/bf/codegen/llvm.cpp")

set(core_flags
	"-Wall"
	"-Wextra"
	"-fno-exceptions"
	"-std=c++1z"
	"-ggdb"
	$<$<CONFIG:RELEASE>:-O3 -DNDEBUG>
	$<$<CONFIG:DEBUG>:-Og -g>)

if (js)
	message("Generating a WebAssembly build.")

	set(CMAKE_CXX_COMPILER "em++")
	set(CMAKE_C_COMPILER "emcc")

	add_executable(ashbf.js
		${core_files}
		"src/bfjswrapper.cpp")

	set(CMAKE_CXX_FLAGS "-s WASM=1 --bind -s MODULARIZE=1 -s 'EXPORT_NAME=\"ashbfInitialize\"'")
	target_compile_options(ashbf.js PRIVATE ${core_flags} "-s WASM=1")
	target_compile_definitions(ashbf.js PRIVATE WEB=1)
else()
	add_executable(ashbf
		${core_files}
		"src/main.cpp"
		"src/cli.cpp")

	target_compile_options(ashbf PRIVATE ${core_flags})
endif()

# TODO: support ashbf.js again
target_link_libraries(ashbf PRIVATE
	fmt
	LLVM
)
