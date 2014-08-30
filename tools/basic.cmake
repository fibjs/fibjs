include(../../tools/arch.cmake)
include(../../tools/os.cmake)

project(${name})

file(GLOB_RECURSE src_list "*.c*")
add_library(${name} ${src_list})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin/${OS}_${BUILD_TYPE}/vender)

set(flags "-fshort-wchar -fsigned-char -fmessage-length=0 -Wno-long-long -fdata-sections -Wno-strict-aliasing -ffunction-sections -fno-exceptions -D_FILE_OFFSET_BITS=64")
set(link_flags " ")

if(${OS} STREQUAL "Darwin")
	set(flags "${flags} -mmacosx-version-min=10.6")
endif()

if(${BUILD_TYPE} STREQUAL "Release32")
	set(flags "${flags} -m32")
	set(link_flags "${link_flags} -m32")
	set(BUILD_TYPE "Release")
endif()

if(${BUILD_TYPE} STREQUAL "Debug32")
	set(flags "${flags} -m32")
	set(link_flags "${link_flags} -m32")
	set(BUILD_TYPE "Debug")
endif()

if(${BUILD_TYPE} STREQUAL "Release")
	set(flags "${flags} -O3")
	add_definitions(-DNDEBUG=1)
endif()

if(${BUILD_TYPE} STREQUAL "Debug")
	set(flags "${flags} -g -O0")
	add_definitions(-DDEBUG=1)
endif()

set_target_properties(${name} PROPERTIES COMPILE_FLAGS ${flags})

if(link_flags)
	set_target_properties(${name} PROPERTIES LINK_FLAGS ${link_flags})
endif()

include_directories(${PROJECT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/../" "/usr/local/include/")
