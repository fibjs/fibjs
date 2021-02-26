cmake_minimum_required(VERSION 2.6)

set(name "fibjs")
project(${name})

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/tools/option.cmake)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	if(NOT MSVC)
		# add quote to all paths in CMAKE_RC_COMPILE_OBJECT to make it robustic on Windows.
		#
		# BTW, sh.exe cannot deal with args of `llvm-rc.exe` correctly
		# @see https://stackoverflow.com/questions/34647591/passing-windows-slash-based-parameters-to-a-program-from-bash-script
		set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> //l 0x409 \"<SOURCE>\" //nologo //fo <OBJECT>")
		set(CMAKE_RC_COMPILER llvm-rc)
	endif()

	add_executable(${name} ${src_list} res/fibjs.rc)
	add_executable(${name}_gui WIN32 ${src_list} res/fibjs.rc)
else()
	add_executable(${name} ${src_list})
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" AND ${ARCH} STREQUAL "i386")
	add_definitions(-DFIBJS_DISABLE_GUI)
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../../bin/${CMAKE_HOST_SYSTEM_NAME}_${ARCH}_${BUILD_TYPE})

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/../include" "${PROJECT_SOURCE_DIR}/../../vender" "${PROJECT_SOURCE_DIR}/../../vender/v8" "${PROJECT_SOURCE_DIR}/../../vender/v8/include" "${PROJECT_SOURCE_DIR}/../../vender/mbedtls" "${PROJECT_SOURCE_DIR}/../../vender/zlib/include" "${CMAKE_CURRENT_BINARY_DIR}")

set(libs fibjs cef expat gumbo gd tiff jpeg png webp zlib leveldb snappy msgpack pcre sqlite mongo umysql uuid exif mbedtls v8 unzip uv exlib)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	list(APPEND libs winiconv)
else()
	list(APPEND libs ev editline jemalloc)
endif()

foreach(lib ${libs})
	target_link_libraries(${name} "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
		target_link_libraries(${name}_gui "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()
endforeach()

setup_result_library(${name})

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	setup_result_library(${name}_gui)
endif()

if(HAVE_GLIB_C_225_H)
	set(link_flags "${link_flags} -Wl,--wrap=memcpy")
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	if(link_flags)
		set_target_properties(${name} PROPERTIES LINK_FLAGS ${link_flags})
		set_target_properties(${name}_gui PROPERTIES LINK_FLAGS ${link_flags})
	endif()

	target_link_libraries(${name} comctl32)
	target_link_libraries(${name}_gui comctl32)

	target_compile_definitions(${name} PUBLIC -D_CONSOLE)
	target_compile_definitions(${name}_gui PUBLIC -D_WINDOWS)
else()
	if(link_flags)
		set_target_properties(${name} PROPERTIES LINK_FLAGS ${link_flags})
	endif()
endif()
