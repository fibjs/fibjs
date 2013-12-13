cmake_minimum_required(VERSION 2.8)

project(vender)

file(GLOB_RECURSE src_list "../src/*.c*")
add_library(vender ${src_list})

set(flags "-c -fshort-wchar -fmessage-length=0 -Wno-long-long -fdata-sections -Wno-strict-aliasing -ffunction-sections")
set(link_flags "")

if(${BUILD_TYPE} STREQUAL "Release")
	set(flags "${flags} -O3")
	add_definitions("-DNDEBUG=1")
endif()

if(${BUILD_TYPE} STREQUAL "Release32")
	set(flags "${flags} -O3 -m32")
	set(link_flags "-m32")
	add_definitions("-DNDEBUG=1")
endif()

if(${BUILD_TYPE} STREQUAL "Debug")
	set(flags "${flags} -g -O0")
endif()

if(${BUILD_TYPE} STREQUAL "Debug32")
	set(flags "${flags} -g -O0 -m32")
	set(link_flags "-m32")
endif()

set_target_properties(vender PROPERTIES COMPILE_FLAGS ${flags})
add_definitions("-DENABLE_DEBUGGER_SUPPORT=1")

if(link_flags)
	set_target_properties(vender PROPERTIES LINK_FLAGS ${link_flags})
endif()

include_directories("${PROJECT_SOURCE_DIR}/../include/" "${PROJECT_SOURCE_DIR}/../src/v8/src/" "/usr/local/include/")
