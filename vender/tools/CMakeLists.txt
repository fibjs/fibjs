cmake_minimum_required(VERSION 2.6)

include(../../tools/arch.cmake)
include(../../tools/os.cmake)

project(vender)

file(GLOB_RECURSE src_list "../src/*.c*")
add_library(vender ${src_list})

set(flags "-fshort-wchar -fsigned-char -fmessage-length=0 -Wno-long-long -fdata-sections -Wno-strict-aliasing -ffunction-sections -fno-exceptions -D_FILE_OFFSET_BITS=64 -DSQLITE_ENABLE_FTS4=1")
set(link_flags " ")

if(${OS} STREQUAL "Darwin")
	set(flags "${flags} -mmacosx-version-min=10.6")
endif()

if(${BUILD_TYPE} STREQUAL "Release")
	set(flags "${flags} -O3")
	add_definitions(-DNDEBUG=1)
endif()

if(${BUILD_TYPE} STREQUAL "Debug")
	set(flags "${flags} -g -O0")
	add_definitions(-DDEBUG=1 -DVERIFY_HEAP=1 -DOBJECT_PRINT=1 -DENABLE_DISASSEMBLER=1 -DENABLE_HANDLE_ZAPPING=1)
endif()

if(${BUILD_TYPE} STREQUAL "Release32")
	set(flags "${flags} -O3 -m32")
	set(link_flags "${link_flags} -m32")
	add_definitions(-DNDEBUG=1)
endif()

if(${BUILD_TYPE} STREQUAL "Debug32")
	set(flags "${flags} -g -O0 -m32")
	set(link_flags "${link_flags} -m32")
	add_definitions(-DDEBUG=1 -DVERIFY_HEAP=1 -DOBJECT_PRINT=1 -DENABLE_DISASSEMBLER=1 -DENABLE_HANDLE_ZAPPING=1)
endif()

set_target_properties(vender PROPERTIES COMPILE_FLAGS ${flags})

if(link_flags)
	set_target_properties(vender PROPERTIES LINK_FLAGS ${link_flags})
endif()

include_directories("${PROJECT_SOURCE_DIR}/../include/" "${PROJECT_SOURCE_DIR}/../src/v8/src/" "${PROJECT_SOURCE_DIR}/../src/v8/" "${PROJECT_SOURCE_DIR}/../src/leveldb/" "/usr/local/include/")
