cmake_minimum_required(VERSION 2.6)

include(tools/arch.cmake)
include(tools/os.cmake)
include(tools/subdirs.cmake)

project(fibjs)

file(GLOB_RECURSE src_list "src/*.c*")
add_executable(fibjs ${src_list})

set(BIN_DIR ${PROJECT_SOURCE_DIR}/../bin/${OS}_${ARCH}_${BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})

include(CheckIncludeFiles)
check_include_files(iconv.h HAVE_ICONV_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/gitinfo.h.in ${CMAKE_CURRENT_BINARY_DIR}/gitinfo.h)

set(flags "-fsigned-char -fmessage-length=0 -fdata-sections -ffunction-sections -fno-exceptions -D_FILE_OFFSET_BITS=64")
set(ccflags "-std=c++11")
set(link_flags " ")

if(${OS} STREQUAL "Darwin")
	set(link_flags "${link_flags} -mmacosx-version-min=10.9")
	set(flags "${flags} -mmacosx-version-min=10.9")
	target_link_libraries(fibjs dl iconv stdc++)
endif()

if(${OS} STREQUAL "Linux")
	target_link_libraries(fibjs dl rt)
endif()

if(${OS} STREQUAL "FreeBSD")
	find_library(execinfo execinfo "/usr/local/lib" "/usr/lib")
	target_link_libraries(fibjs ${execinfo})
endif()

if(${BUILD_TYPE} STREQUAL "release")
	set(flags "${flags} -O3 ${BUILD_OPTION} -w -fomit-frame-pointer -fvisibility=hidden")
	set(link_flags "${link_flags} ${BUILD_OPTION}")
	add_definitions(-DNDEBUG=1)

	if(${OS} STREQUAL "Linux" AND ${ARCH} STREQUAL "amd64")
		set(link_flags "${link_flags} -Wl,--wrap=memcpy")
	endif()

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(link_flags "${link_flags} -static-libgcc -static-libstdc++ -Wl,--no-as-needed")
	endif()
endif()

if(${BUILD_TYPE} STREQUAL "debug")
	set(flags "${flags} -g -O0 ${BUILD_OPTION} -Wall -Wno-overloaded-virtual")
	set(link_flags "${link_flags} ${BUILD_OPTION}")
	add_definitions(-DDEBUG=1)

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		set(link_flags "${link_flags} -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=calloc -Wl,--wrap=realloc")
	endif()
endif()

set(CMAKE_C_FLAGS "${flags}")
set(CMAKE_CXX_FLAGS "${flags} ${ccflags}")

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/../vender" "${PROJECT_SOURCE_DIR}/../vender/v8" "${PROJECT_SOURCE_DIR}/../vender/v8/include" "${PROJECT_SOURCE_DIR}/../vender/zlib/include" "${CMAKE_CURRENT_BINARY_DIR}")

set(libs expat gumbo gd tiff jpeg png webp zlib leveldb snappy ev pcre sqlite mongo umysql uuid exif winiconv mbedtls jssdk v8 SpiderMonkey unzip editline exlib)

foreach(lib ${libs})
	target_link_libraries(fibjs "${BIN_DIR}/lib${lib}.a")
endforeach()

target_link_libraries(fibjs pthread)

set_target_properties(fibjs PROPERTIES LINK_FLAGS ${link_flags})
