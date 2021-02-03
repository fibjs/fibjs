cmake_minimum_required(VERSION 2.6)

include(../vender/tools/option.cmake)
include(../vender/tools/subdirs.cmake)

project(fibjs)

file(GLOB_RECURSE src_list "src/*.c*")
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" AND ${ARCH} STREQUAL "amd64")
	file(GLOB_RECURSE src_platform_list "src/*.m*")
else()
	file(GLOB_RECURSE src_platform_list "")
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin" AND NOT ${ARCH} STREQUAL "amd64")
	add_definitions(-DFIBJS_DISABLE_GUI=1)
endif()

add_library(fibjs ${src_list} ${src_platform_list})

set(BIN_DIR ${PROJECT_SOURCE_DIR}/../bin/${CMAKE_HOST_SYSTEM_NAME}_${ARCH}_${BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH ${BIN_DIR})

include(CheckIncludeFiles)
include(CheckCSourceCompiles)

check_include_files(iconv.h HAVE_ICONV_H)

check_c_source_compiles("void posix_spawnp();
	__asm__(\".symver posix_spawnp,posix_spawnp@GLIBC_2.2.5\");
	void main(void){posix_spawnp();}" HAVE_GLIB_C_225_H)

check_c_source_compiles("void posix_spawnp();
	__asm__(\".symver posix_spawnp,posix_spawnp@GLIBC_2.2\");
	void main(void){posix_spawnp();}" HAVE_GLIB_C_22_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.2.5\");
	void main(void){fcntl();}" HAVE_GLIB_C_FCNTL_225_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.0\");
	void main(void){fcntl();}" HAVE_GLIB_C_FCNTL_2_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.4\");
	void main(void){fcntl();}" HAVE_GLIB_C_FCNTL_24_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.17\");
	void main(void){fcntl();}" HAVE_GLIB_C_FCNTL_217_H)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tools/gitinfo.h.in ${CMAKE_CURRENT_BINARY_DIR}/gitinfo.h)

set(flags "-fsigned-char -fmessage-length=0 -fdata-sections -ffunction-sections -D_FILE_OFFSET_BITS=64")
set(ccflags "${ccflags} ${CXX_STD_VER_FLAG}")

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
	set(flags "${flags} -Wno-nullability-completeness")
	set(cflags "${cflags} -DOBJC_OLD_DISPATCH_PROTOTYPES=1")
	set(ccflags "${ccflags} -DOBJC_OLD_DISPATCH_PROTOTYPES=1")
	set(link_flags "${link_flags} -framework WebKit -framework Cocoa")
	
	set_source_files_properties(${src_platform_list} PROPERTIES COMPILE_FLAGS "-x objective-c++")
	set(flags "${flags} -mmacosx-version-min=10.9")

	if(${ARCH} STREQUAL "amd64")
		set(flags "${flags} -m64")
	elseif(${ARCH} STREQUAL "i386")
		set(flags "${flags} -m32")
	endif()
endif()

if(${BUILD_TYPE} STREQUAL "release")
	set(flags "${flags} -O3 -s ${BUILD_OPTION} -w -fvisibility=hidden")

	if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "FreeBSD")
		set(flags "${flags} -fno-omit-frame-pointer")
	else()
		set(flags "${flags} -fomit-frame-pointer")
	endif()

	add_definitions(-DNDEBUG=1)
endif()

if(${BUILD_TYPE} STREQUAL "debug")
	set(flags "${flags} -g -O0 ${BUILD_OPTION} -Wall -Wno-overloaded-virtual")
	add_definitions(-DDEBUG=1)
endif()

set(CMAKE_C_FLAGS "${flags} ${cflags}")
set(CMAKE_CXX_FLAGS "${flags} ${ccflags}")

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
