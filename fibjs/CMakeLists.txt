cmake_minimum_required(VERSION 2.6)

set(name "fibjs")
project(${name})

include(../vender/build_tools/cmake/option.cmake)

add_library(${name} ${src_list} ${src_platform_list})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" AND ${ARCH} STREQUAL "i386")
	add_definitions(-DFIBJS_DISABLE_GUI)
endif()

include_directories("${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/../vender"
	"${PROJECT_SOURCE_DIR}/../vender/v8"
	"${PROJECT_SOURCE_DIR}/../vender/v8/gen"
	"${PROJECT_SOURCE_DIR}/../vender/v8/include"
	"${PROJECT_SOURCE_DIR}/../vender/cef"
	"${PROJECT_SOURCE_DIR}/../vender/uv"
	"${PROJECT_SOURCE_DIR}/../vender/uv/include"
	"${PROJECT_SOURCE_DIR}/../vender/mbedtls"
	"${PROJECT_SOURCE_DIR}/../vender/zlib/include"
	"${PROJECT_SOURCE_DIR}/../vender/msgpack/include"
	"${CMAKE_CURRENT_BINARY_DIR}")

setup_result_library(${name})