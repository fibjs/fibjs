cmake_minimum_required(VERSION 3.10)

# The library target is named fibjs_core so that it can share a build tree with
# the fibjs executable; the artifact name stays libfibjs.a.
set(name "fibjs_core")
project(${name})

include(../vender/v8/cmake/options.cmake)
include(../vender/build_tools/cmake/option.cmake)

add_library(${name} ${src_list} ${src_platform_list})

set_target_properties(${name} PROPERTIES OUTPUT_NAME fibjs)

if(TARGET bt_config)
	target_link_libraries(${name} bt_config)
endif()

# Vendored libraries: real targets in the in-tree build, IMPORTED targets in the
# dist build (-DBT_VENDER=dist).
#
# They are collected in an INTERFACE library instead of being linked to
# fibjs_core directly.  A static library is never linked against them - the
# headers it needs come from the include_directories below - but CMake turns
# every target_link_libraries entry that names a target into a build-order
# dependency (fibjs_core.dir/all: vender/v8.dir/all), which holds every fibjs
# object file back until the whole vendored tree, libv8 above all, has been
# built.  The archives are only needed by the final links, so the program and
# the js shared library link fibjs_vender themselves; the list stays in one
# place and the static link order is unchanged.
include(${CMAKE_CURRENT_LIST_DIR}/../vender/libs.cmake)

add_library(fibjs_vender INTERFACE)
target_link_libraries(fibjs_vender INTERFACE ${libs})

include_directories("${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/../vender"
	"${PROJECT_SOURCE_DIR}/../vender/uv"
	"${PROJECT_SOURCE_DIR}/../vender/uv/include"
	"${PROJECT_SOURCE_DIR}/../vender/zlib/include"
	"${PROJECT_SOURCE_DIR}/../vender/msgpack/include"
	"${PROJECT_SOURCE_DIR}/../vender/openssl/include"
	"${PROJECT_SOURCE_DIR}/../vender/nghttp2/include"
	"${PROJECT_SOURCE_DIR}/../vender/boost/preprocessor/include"
	"${PROJECT_SOURCE_DIR}/../vender/postject"
	"${PROJECT_SOURCE_DIR}/../vender/ada/include"
	"${CMAKE_CURRENT_BINARY_DIR}")

if("${BUILD_OS}" MATCHES "Windows|Darwin|Linux|Alpine")
	add_definitions(-DOS_DESKTOP)
endif()

# FIBJS_ASYNC_STATE_CHECK=1 (environment variable or -D at configure time)
# enables the AsyncState phase lock in AsyncCall.h.  It is deliberately
# independent of DEBUG/NDEBUG: the illegal dispatches it looks for only show up
# in optimized, high-concurrency builds.  The lock is a debugging aid only - it
# reports the violating caller with a stack trace and aborts; it never changes
# production behaviour.  "0" or an empty value keeps it disabled, so CI can
# enable it for branch/PR builds while tag(release) builds stay clean.
set(_fibjs_async_state_check "")
if(FIBJS_ASYNC_STATE_CHECK)
        set(_fibjs_async_state_check 1)
elseif(DEFINED ENV{FIBJS_ASYNC_STATE_CHECK} AND NOT "$ENV{FIBJS_ASYNC_STATE_CHECK}" STREQUAL "" AND NOT "$ENV{FIBJS_ASYNC_STATE_CHECK}" STREQUAL "0")
        set(_fibjs_async_state_check 1)
endif()
if(_fibjs_async_state_check)
	add_definitions(-DFIBJS_ASYNC_STATE_CHECK=1)
endif()

if("${BUILD_OS}" MATCHES "Linux|Alpine")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk+-3.0)

    include_directories(${GTK_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/../vender/webkitgtk")
endif()

setup_result_library(${name})
