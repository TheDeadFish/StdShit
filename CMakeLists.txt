project(stdshit)
cmake_minimum_required(VERSION 3.16)
set(CD ${CMAKE_CURRENT_SOURCE_DIR})
file(MAKE_DIRECTORY ${CD}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CD}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CD}/bin)

# build stdshit library
file(GLOB_RECURSE GSRC src/*.cc src/windows/*.cpp)
add_library(stdshit STATIC ${GSRC}
	src/math.cpp src/memalloc.cpp src/string.cpp)
target_precompile_headers(stdshit PUBLIC bin/stdshit.h)

# build tests
add_executable(hello tests/hello.cc)
target_link_libraries(hello stdshit)
target_precompile_headers(hello REUSE_FROM stdshit)
