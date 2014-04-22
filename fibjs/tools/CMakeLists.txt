cmake_minimum_required(VERSION 2.6)

project(fibjs)

file(GLOB_RECURSE src_list "../src/*.c*")
add_executable(fibjs ${src_list})


set(flags "-c -fshort-wchar -fmessage-length=0 -Wno-long-long -fdata-sections -Wno-strict-aliasing -ffunction-sections")

set(link_flags " ")

if(${OS_TYPE} STREQUAL "Darwin")
	target_link_libraries(fibjs pthread dl)
endif()

if(${OS_TYPE} STREQUAL "Linux")
	set(link_flags "-static-libgcc -static-libstdc++ -Wl,--no-as-needed -Wl,--wrap=memcpy")
	target_link_libraries(fibjs pthread dl rt)
endif()

if(${OS_TYPE} STREQUAL "FreeBSD")
	find_library(execinfo execinfo "/usr/local/lib" "/usr/lib")
	target_link_libraries(fibjs ${execinfo})
	target_link_libraries(fibjs pthread kvm)
endif()


if(${BUILD_TYPE} STREQUAL "Release")
	set(flags "${flags} -O3 -m64")
	set(link_flags "${link_flags} -m64")
	add_definitions("-DNDEBUG=1")
endif()

if(${BUILD_TYPE} STREQUAL "Release32")
	set(flags "${flags} -O3 -m32")
	set(link_flags "${link_flags} -m32")
	add_definitions("-DNDEBUG=1")
endif()

if(${BUILD_TYPE} STREQUAL "Debug")
	set(flags "${flags} -g -O0 -m64")
	set(link_flags "${link_flags} -m64")
endif()

if(${BUILD_TYPE} STREQUAL "Debug32")
	set(flags "${flags} -g -O0 -m32")
	set(link_flags "${link_flags} -m32")
endif()

set_target_properties(fibjs PROPERTIES COMPILE_FLAGS ${flags})

if(DEFINED SVN_INFO)
	add_definitions("-DSVN_INFO=${SVN_INFO}")
endif()

include_directories("${PROJECT_SOURCE_DIR}/../include" "${PROJECT_SOURCE_DIR}/../../vender/include")

set_target_properties(fibjs PROPERTIES LINK_FLAGS ${link_flags})

find_library(lib_list vender "${PROJECT_SOURCE_DIR}/../../vender/${BUILD_TYPE}")
target_link_libraries(fibjs ${lib_list})
