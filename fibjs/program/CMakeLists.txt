cmake_minimum_required(VERSION 3.10)

set(name "fibjs")
project(${name})

include(../../vender/v8/cmake/options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../vender/build_tools/cmake/option.cmake)

include_directories("${PROJECT_SOURCE_DIR}/../../vender/uv/include")

# The vendored libraries are linked as CMake targets below: real targets in the
# in-tree build, IMPORTED targets in the dist build (-DBT_VENDER=dist).
# The dist handling itself is part of build_tools
# (vender/build_tools/cmake/vender_dist.cmake).

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	if(NOT MSVC)
		# add quote to all paths in CMAKE_RC_COMPILE_OBJECT to make it robustic on Windows.
		#
		# BTW, sh.exe cannot deal with args of `llvm-rc.exe` correctly
		# @see https://stackoverflow.com/questions/34647591/passing-windows-slash-based-parameters-to-a-program-from-bash-script
		set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> //l 0x409 \"<SOURCE>\" //nologo //fo <OBJECT>")
		set(CMAKE_RC_COMPILER llvm-rc)
	endif()

	add_executable(${name} src/app.cpp res/app.rc)
else()
	add_executable(${name} src/app.cpp)
endif()

add_library(js SHARED src/so.cpp)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/../include"
	"${PROJECT_SOURCE_DIR}/../../vender"
	"${PROJECT_SOURCE_DIR}/../../vender/v8"
	"${PROJECT_SOURCE_DIR}/../../vender/v8/include"
	"${PROJECT_SOURCE_DIR}/../../vender/zlib/include"
	"${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(${name} fibjs_core)
target_link_libraries(js fibjs_core)

if(NOT ${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	set(link_flags "${link_flags} -rdynamic")
endif()

setup_result_library(${name})
setup_result_library(js)