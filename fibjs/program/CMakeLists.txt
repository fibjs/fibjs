cmake_minimum_required(VERSION 2.6)

set(name "fibjs")
project(${name})

file(GLOB_RECURSE src_list "src/*.c*")

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	# add quote to all paths in CMAKE_RC_COMPILE_OBJECT to make it robustic on Windows.
	#
	# BTW, sh.exe cannot deal with args of `llvm-rc.exe` correctly
	# @see https://stackoverflow.com/questions/34647591/passing-windows-slash-based-parameters-to-a-program-from-bash-script
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> //l 0x409 \"<SOURCE>\" //nologo //fo <OBJECT>")
	set(CMAKE_RC_COMPILER llvm-rc)

	add_executable(${name} ${src_list} res/fibjs.rc)
	add_executable(${name}_gui WIN32 ${src_list} res/fibjs.rc)
else()
	add_executable(${name} ${src_list})
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../../bin/${CMAKE_HOST_SYSTEM_NAME}_${ARCH}_${BUILD_TYPE})

include(../../vender/tools/option.cmake)

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/../include" "${PROJECT_SOURCE_DIR}/../../vender" "${PROJECT_SOURCE_DIR}/../../vender/v8" "${PROJECT_SOURCE_DIR}/../../vender/v8/include" "${PROJECT_SOURCE_DIR}/../../vender/mbedtls" "${PROJECT_SOURCE_DIR}/../../vender/zlib/include" "${CMAKE_CURRENT_BINARY_DIR}")

set(libs fibjs expat gumbo gd tiff jpeg png webp zlib leveldb snappy msgpack pcre sqlite mongo umysql uuid exif mbedtls v8 unzip uv exlib)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	list(APPEND libs winiconv)
else()
	list(APPEND libs ev editline jemalloc)
endif()

foreach(lib ${libs})
	target_link_libraries(${name} "${EXECUTABLE_OUTPUT_PATH}/lib${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
		target_link_libraries(${name}_gui "${EXECUTABLE_OUTPUT_PATH}/lib${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()
endforeach()

include(../../vender/tools/option_libs.cmake)

if(HAVE_GLIB_C_225_H)
	set(link_flags "${link_flags} -Wl,--wrap=memcpy")
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
	target_compile_definitions(${name} PUBLIC -D_CONSOLE)
	set_target_properties(${name} PROPERTIES LINK_FLAGS "${link_flags} -Xlinker //SUBSYSTEM:CONSOLE")
	
	target_compile_definitions(${name}_gui PUBLIC -D_WINDOWS)
	set_target_properties(${name}_gui PROPERTIES LINK_FLAGS "${link_flags} -Xlinker //SUBSYSTEM:WINDOWS")
else()
	set_target_properties(${name} PROPERTIES LINK_FLAGS ${link_flags})
endif()

