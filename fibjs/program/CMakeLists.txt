cmake_minimum_required(VERSION 2.6)

set(name "fibjs")
project(${name})

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/build_tools/cmake/option.cmake)

if(NOT "$ENV{USE_VENDER_DIST}" STREQUAL "")
	include(FetchContent)

	execute_process(
		COMMAND git describe --tags --abbrev=0
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../../vender"
		OUTPUT_VARIABLE VENDER_TAG
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if("${BUILD_OS}" STREQUAL "Linux")
		set(BUILD_TARGET "linux")
	elseif("${BUILD_OS}" STREQUAL "Alpine")
		set(BUILD_TARGET "alpine")
	elseif("${BUILD_OS}" STREQUAL "Darwin")
		set(BUILD_TARGET "darwin")
	elseif("${BUILD_OS}" STREQUAL "Windows")
		set(BUILD_TARGET "win32")
	elseif("${BUILD_OS}" STREQUAL "Android")
		set(BUILD_TARGET "android")
	elseif("${BUILD_OS}" STREQUAL "iPhone")
		set(BUILD_TARGET "iphone")
	endif()

	message("downloading: ${VENDER_TAG}/fibjs_vender-${BUILD_TARGET}-${BUILD_ARCH}-${BUILD_TYPE}.tar.gz\n")
	FetchContent_Declare(
		vender
		URL "https://github.com/fibjs/fibjs_vender/releases/download/${VENDER_TAG}/fibjs_vender-${BUILD_TARGET}-${BUILD_ARCH}-${BUILD_TYPE}.tar.gz"
	)

	FetchContent_GetProperties(vender)
	if (NOT vender_POPULATED)
  		FetchContent_Populate(vender)
	endif ()

	set(VENDER_PATH ${vender_SOURCE_DIR}/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE})
else()
	set(VENDER_PATH ${BIN_PATH})
endif()

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	if(NOT MSVC)
		# add quote to all paths in CMAKE_RC_COMPILE_OBJECT to make it robustic on Windows.
		#
		# BTW, sh.exe cannot deal with args of `llvm-rc.exe` correctly
		# @see https://stackoverflow.com/questions/34647591/passing-windows-slash-based-parameters-to-a-program-from-bash-script
		set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> //l 0x409 \"<SOURCE>\" //nologo //fo <OBJECT>")
		set(CMAKE_RC_COMPILER llvm-rc)
	endif()

	add_executable(${name} src/app.cpp res/fibjs.rc)
	add_executable(${name}_gui WIN32 src/app.cpp res/fibjs.rc)
else()
	add_executable(${name} src/app.cpp)
endif()

add_library(js SHARED src/so.cpp)

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/../include" "${PROJECT_SOURCE_DIR}/../../vender" "${PROJECT_SOURCE_DIR}/../../vender/v8" "${PROJECT_SOURCE_DIR}/../../vender/v8/include" "${PROJECT_SOURCE_DIR}/../../vender/mbedtls" "${PROJECT_SOURCE_DIR}/../../vender/zlib/include" "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(${name} "${BIN_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}fibjs${CMAKE_STATIC_LIBRARY_SUFFIX}")
target_link_libraries(js "${BIN_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}fibjs${CMAKE_STATIC_LIBRARY_SUFFIX}")

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(${name}_gui "${BIN_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}fibjs${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/libs.cmake)
foreach(lib ${libs})
	target_link_libraries(${name} "${VENDER_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")

	if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
		target_link_libraries(${name}_gui "${VENDER_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()

	if(NOT ${lib} STREQUAL "jemalloc")
		target_link_libraries(js "${VENDER_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()
endforeach()

setup_result_library(${name})
setup_result_library(js)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	setup_result_library(${name}_gui)
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	target_compile_definitions(${name} PUBLIC -D_CONSOLE)
	target_compile_definitions(${name}_gui PUBLIC -D_WINDOWS)
endif()
