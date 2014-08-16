cmake_minimum_required(VERSION 2.6)

include(../../tools/arch.cmake)
include(../../tools/os.cmake)
include(../../tools/buildtype.cmake)

include(../../tools/check_zlib.cmake)

project(fibjs)

file(GLOB_RECURSE src_list "../src/*.c*")
add_executable(fibjs ${src_list})

include(CheckIncludeFiles)
check_include_files(iconv.h HAVE_ICONV_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gitinfo.h.in ${CMAKE_CURRENT_BINARY_DIR}/gitinfo.h)

set(flags "-c -fshort-wchar -fsigned-char -fmessage-length=0 -Wno-long-long -fdata-sections -Wno-strict-aliasing -ffunction-sections -fno-exceptions -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(link_flags " ")

include(./select_zlib.cmake)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(link_flags "${link_flags} -static-libgcc -static-libstdc++ -Wl,--no-as-needed")
endif()

if(${OS} STREQUAL "Darwin")
	set(link_flags "${link_flags} -mmacosx-version-min=10.6")
	set(flags "${flags} -mmacosx-version-min=10.6")
	target_link_libraries(fibjs dl iconv pthread)
endif()

if(${OS} STREQUAL "Linux")
	target_link_libraries(fibjs dl rt pthread)
endif()

if(${OS} STREQUAL "FreeBSD")
	find_library(execinfo execinfo "/usr/local/lib" "/usr/lib")
	target_link_libraries(fibjs ${execinfo})

	target_link_libraries(fibjs pthread)
endif()


if(${BUILD_TYPE} STREQUAL "Release")
	set(flags "${flags} -O3")
	add_definitions("-DNDEBUG=1")

	if(${OS} STREQUAL "Linux" AND ${ARCH} STREQUAL "x64")
		set(link_flags "${link_flags} -Wl,--wrap=memcpy")
	endif()
endif()

if(${BUILD_TYPE} STREQUAL "Debug")
	set(flags "${flags} -g -O0")
endif()

if(${BUILD_TYPE} STREQUAL "Release32")
	set(flags "${flags} -O3 -m32")
	set(link_flags "${link_flags} -m32")
	add_definitions("-DNDEBUG=1")
endif()

if(${BUILD_TYPE} STREQUAL "Debug32")
	set(flags "${flags} -g -O0 -m32")
	set(link_flags "${link_flags} -m32")
endif()

set_target_properties(fibjs PROPERTIES COMPILE_FLAGS ${flags})

include_directories("${PROJECT_SOURCE_DIR}/../include" "${PROJECT_SOURCE_DIR}/../../vender/include" "${CMAKE_CURRENT_BINARY_DIR}")

set_target_properties(fibjs PROPERTIES LINK_FLAGS ${link_flags})

find_library(lib_list vender "${PROJECT_SOURCE_DIR}/../../vender/out/${OS}_${BUILD_TYPE}")
target_link_libraries(fibjs ${lib_list})
