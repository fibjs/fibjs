cmake_minimum_required(VERSION 3.10)

set(name "fibjs")
project(${name})

include(../vender/v8/cmake/options.cmake)
include(../vender/build_tools/cmake/option.cmake)

add_library(${name} ${src_list} ${src_platform_list})

include_directories("${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/../vender"
	"${PROJECT_SOURCE_DIR}/../vender/uv"
	"${PROJECT_SOURCE_DIR}/../vender/uv/include"
	"${PROJECT_SOURCE_DIR}/../vender/zlib/include"
	"${PROJECT_SOURCE_DIR}/../vender/msgpack/include"
	"${PROJECT_SOURCE_DIR}/../vender/openssl/include"
	"${PROJECT_SOURCE_DIR}/../vender/boost/preprocessor/include"
	"${PROJECT_SOURCE_DIR}/../vender/postject"
	"${PROJECT_SOURCE_DIR}/../vender/ada/include"
	"${CMAKE_CURRENT_BINARY_DIR}")

if("${BUILD_OS}" MATCHES "Windows|Darwin|Linux|Alpine")
	add_definitions(-DOS_DESKTOP)
endif()

if("${BUILD_OS}" MATCHES "Linux|Alpine")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk+-3.0)

    include_directories(${GTK_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/../vender/webkitgtk")
endif()

setup_result_library(${name})
