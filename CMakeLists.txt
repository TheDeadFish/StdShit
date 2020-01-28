project(stdshit)
cmake_minimum_required(VERSION 3.16)
set(CD ${CMAKE_CURRENT_SOURCE_DIR})
file(MAKE_DIRECTORY ${CD}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CD}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CD}/bin)

# build stdshit.h
file(GLOB_RECURSE combine_dep src/*.h)
add_custom_command(
	OUTPUT ${CD}/bin/stdshit.h
	COMMAND ${CD}/combine.exe noasm sect endi comp utf8
	DEPENDS ${CD}/combine.exe ${combine_dep}
)

# build stdshit library
file(GLOB_RECURSE GSRC src/*.cc src/windows/*.cpp)
add_library(stdshit STATIC ${GSRC}
	src/math.cpp src/memalloc.cpp src/string.cpp bin/stdshit.h)
target_precompile_headers(stdshit PUBLIC bin/stdshit.h)
