cmake_minimum_required(VERSION 2.6)

include(../tools/arch.cmake)
include(../tools/os.cmake)
include(../tools/subdirs.cmake)

project(fibjs)

file(GLOB_RECURSE src_list "src/*.c*")
add_executable(fibjs ${src_list})

set(BIN_DIR ${PROJECT_SOURCE_DIR}/../bin/${OS}_${BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})

include(CheckIncludeFiles)
check_include_files(iconv.h HAVE_ICONV_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/gitinfo.h.in ${CMAKE_CURRENT_BINARY_DIR}/gitinfo.h)

set(flags "-std=gnu++0x -Wall -Wno-overloaded-virtual -Wno-strict-aliasing -fshort-wchar -fsigned-char -fmessage-length=0 -fdata-sections -ffunction-sections -fno-exceptions -D_FILE_OFFSET_BITS=64")
set(link_flags " ")

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
	set(flags "${flags} -O3 -fvisibility=hidden")
	add_definitions(-DNDEBUG=1)

	if(${OS} STREQUAL "Linux" AND ${ARCH} STREQUAL "x64")
		set(link_flags "${link_flags} -Wl,--wrap=memcpy")
	endif()

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(link_flags "${link_flags} -static-libgcc -static-libstdc++ -Wl,--no-as-needed")
	endif()
endif()

if(${BUILD_TYPE} STREQUAL "Debug")
	set(flags "${flags} -g -O0")
	add_definitions(-DDEBUG=1)

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(link_flags "${link_flags} -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=calloc -Wl,--wrap=realloc")
	endif()
endif()

if(${BUILD_TYPE} STREQUAL "Release32")
	set(flags "${flags} -O3 -m32 -fvisibility=hidden")
	set(link_flags "${link_flags} -m32")
	add_definitions(-DNDEBUG=1)

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(link_flags "${link_flags} -static-libgcc -static-libstdc++ -Wl,--no-as-needed")
	endif()
endif()

if(${BUILD_TYPE} STREQUAL "Debug32")
	set(flags "${flags} -g -O0 -m32")
	set(link_flags "${link_flags} -m32")
	add_definitions(-DDEBUG=1)

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(link_flags "${link_flags} -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=calloc -Wl,--wrap=realloc")
	endif()
endif()

set_target_properties(fibjs PROPERTIES COMPILE_FLAGS ${flags})

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/../vender" "${PROJECT_SOURCE_DIR}/../vender/v8" "${CMAKE_CURRENT_BINARY_DIR}")

set(libs exlib expat gumbo gd tiff jpeg png zlib leveldb snappy ev pcre sqlite mongo umysql uuid exif winiconv mbedtls v8)

foreach(lib ${libs})
	target_link_libraries(fibjs "${BIN_DIR}/vender/lib${lib}.a")
endforeach()

set_target_properties(fibjs PROPERTIES LINK_FLAGS ${link_flags})
