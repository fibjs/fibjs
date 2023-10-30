cmake_minimum_required(VERSION 2.6)

set(name "fibjs")
project(${name})

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/build_tools/cmake/option.cmake)

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

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/libs.cmake)

set(libs fibjs ${libs})

foreach(lib ${libs})
	target_link_libraries(${name} "${BIN_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
		target_link_libraries(${name}_gui "${BIN_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()

	if(NOT ${lib} STREQUAL "jemalloc")
		target_link_libraries(js "${BIN_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()
endforeach()

setup_result_library(${name})
setup_result_library(js)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	setup_result_library(${name}_gui)
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	if(link_flags)
		set_target_properties(${name} PROPERTIES LINK_FLAGS ${link_flags})
		set_target_properties(${name}_gui PROPERTIES LINK_FLAGS ${link_flags})
		set_target_properties(js PROPERTIES LINK_FLAGS ${link_flags})
	endif()

	target_link_libraries(${name} comctl32)
	target_link_libraries(${name}_gui comctl32)
	target_link_libraries(js comctl32)

	target_compile_definitions(${name} PUBLIC -D_CONSOLE)
	target_compile_definitions(${name}_gui PUBLIC -D_WINDOWS)
else()
	if(link_flags)
		set_target_properties(${name} PROPERTIES LINK_FLAGS ${link_flags})
		set_target_properties(js PROPERTIES LINK_FLAGS ${link_flags})
	endif()
endif()
