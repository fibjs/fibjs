cmake_minimum_required(VERSION 2.6)

set(name "fibjs")
project(${name})

file(GLOB_RECURSE src_list "src/*.c*")
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" AND ${ARCH} STREQUAL "amd64")
	file(GLOB_RECURSE src_platform_list "src/*.m*")
else()
	file(GLOB_RECURSE src_platform_list "")
endif()

add_library(${name} ${src_list} ${src_platform_list})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" AND NOT ${ARCH} STREQUAL "amd64")
	add_definitions(-DFIBJS_DISABLE_GUI=1)
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../../bin/${CMAKE_HOST_SYSTEM_NAME}_${ARCH}_${BUILD_TYPE})

include(../vender/tools/option.cmake)

include_directories("${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/../vender"
	"${PROJECT_SOURCE_DIR}/../vender/v8"
	"${PROJECT_SOURCE_DIR}/../vender/v8/include"
	"${PROJECT_SOURCE_DIR}/../vender/uv"
	"${PROJECT_SOURCE_DIR}/../vender/uv/include"
	"${PROJECT_SOURCE_DIR}/../vender/mbedtls"
	"${PROJECT_SOURCE_DIR}/../vender/zlib/include"
	"${PROJECT_SOURCE_DIR}/../vender/msgpack/include"
	"${CMAKE_CURRENT_BINARY_DIR}")
