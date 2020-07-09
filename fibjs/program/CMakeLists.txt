cmake_minimum_required(VERSION 2.6)

include(../tools/arch.cmake)
include(../tools/os.cmake)
include(../tools/platform.cmake)
include(../tools/subdirs.cmake)

project(fibjs)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

file(GLOB_RECURSE src_list "src/*.c*")

if (${OS} STREQUAL "Windows")
	# add quote to all paths in CMAKE_RC_COMPILE_OBJECT to make it robustic on Windows.
	#
	# BTW, sh.exe cannot deal with args of `llvm-rc.exe` correctly
	# @see https://stackoverflow.com/questions/34647591/passing-windows-slash-based-parameters-to-a-program-from-bash-script
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> //l 0x409 \"<SOURCE>\" //nologo //fo <OBJECT>")
	set(CMAKE_RC_COMPILER llvm-rc)

	add_executable(fibjs ${src_list} res/fibjs.rc)
	add_executable(fibjs_gui WIN32 ${src_list} res/fibjs.rc)
else()
	add_executable(fibjs ${src_list})
endif()

set(BIN_DIR ${PROJECT_SOURCE_DIR}/../../bin/${OS}_${ARCH}_${BUILD_TYPE})
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})

include(CheckIncludeFiles)
include(CheckCSourceCompiles)

set(CMAKE_C_FLAGS "${BUILD_OPTION}")

check_include_files(iconv.h HAVE_ICONV_H)

check_c_source_compiles("void posix_spawnp();
	__asm__(\".symver posix_spawnp,posix_spawnp@GLIBC_2.2.5\");
	int main(void){posix_spawnp();return 0;}" HAVE_GLIB_C_225_H)

check_c_source_compiles("void posix_spawnp();
	__asm__(\".symver posix_spawnp,posix_spawnp@GLIBC_2.2\");
	int main(void){posix_spawnp();return 0;}" HAVE_GLIB_C_22_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.2.5\");
	int main(void){fcntl();return 0;}" HAVE_GLIB_C_FCNTL_225_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.0\");
	int main(void){fcntl();return 0;}" HAVE_GLIB_C_FCNTL_2_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.4\");
	int main(void){fcntl();return 0;}" HAVE_GLIB_C_FCNTL_24_H)

check_c_source_compiles("void fcntl();
	__asm__(\".symver fcntl,fcntl@GLIBC_2.17\");
	int main(void){fcntl();return 0;}" HAVE_GLIB_C_FCNTL_217_H)

check_c_source_compiles("#include <stdatomic.h>
	atomic_int a;
	int main(void){atomic_load(&a);return 0;}" HAVE_GLIB_C_ATOMIC_H)

set(flags "-fsigned-char -fmessage-length=0 -fdata-sections -ffunction-sections -D_FILE_OFFSET_BITS=64")
set(ccflags "${ccflags} ${CXX_STD_VER_FLAG}")
set(link_flags " ")

if(${BUILD_TYPE} STREQUAL "release")
	set(flags "${flags} -O3 -s ${BUILD_OPTION} -w -fvisibility=hidden")

	if(${OS} STREQUAL "FreeBSD")
		set(flags "${flags} -fno-omit-frame-pointer")
	else()
		set(flags "${flags} -fomit-frame-pointer")
	endif()

	set(link_flags "${link_flags} ${BUILD_OPTION} -static-libstdc++")
	add_definitions(-DNDEBUG=1)

	if(${OS} STREQUAL "Linux")
		set(link_flags "${link_flags} ${link_flags} -static-libgcc")
	endif()

	if(HAVE_GLIB_C_225_H)
		set(link_flags "${link_flags} -Wl,--wrap=memcpy")
	endif()
endif()

if(${BUILD_TYPE} STREQUAL "debug")
	set(flags "${flags} -g -O0 ${BUILD_OPTION} -Wall -Wno-overloaded-virtual")
	set(link_flags "${link_flags} ${BUILD_OPTION}")
	add_definitions(-DDEBUG=1)
endif()

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/../include" "${PROJECT_SOURCE_DIR}/../../vender" "${PROJECT_SOURCE_DIR}/../../vender/v8" "${PROJECT_SOURCE_DIR}/../../vender/v8/include" "${PROJECT_SOURCE_DIR}/../../vender/mbedtls" "${PROJECT_SOURCE_DIR}/../../vender/zlib/include" "${CMAKE_CURRENT_BINARY_DIR}")

set(libs fibjs expat gumbo gd tiff jpeg png webp zlib leveldb snappy msgpack pcre sqlite mongo umysql uuid exif mbedtls v8 unzip uv exlib)

if(${OS} STREQUAL "Windows")
	list(APPEND libs winiconv)
else()
	list(APPEND libs ev editline jemalloc)
endif()

foreach(lib ${libs})
	target_link_libraries(fibjs "${BIN_DIR}/lib${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	if(${OS} STREQUAL "Windows")
		target_link_libraries(fibjs_gui "${BIN_DIR}/lib${lib}${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()
endforeach()

if(NOT ${OS} STREQUAL "Windows")
	target_link_libraries(fibjs pthread)
endif()

if(${OS} STREQUAL "Darwin")
	set(DARWIN_LIBS "-framework WebKit -framework Cocoa")

	set(cflags "${cflags} -DOBJC_OLD_DISPATCH_PROTOTYPES=1")
	set(ccflags "${ccflags} -DOBJC_OLD_DISPATCH_PROTOTYPES=1")
	set(link_flags "${link_flags} -mmacosx-version-min=10.9 -framework Carbon -framework IOKit ${DARWIN_LIBS}")

	set(flags "${flags} -mmacosx-version-min=10.9")
	target_link_libraries(fibjs dl iconv stdc++)
endif()

if(${OS} STREQUAL "Windows")
	add_definitions(-DWIN32 -D_LIB -D_CRT_SECURE_NO_WARNINGS -D_CRT_RAND_S -DNOMINMAX)
	set(flags "${flags} -fms-extensions -fmsc-version=1910 -frtti")
	list(APPEND win_libs winmm ws2_32 psapi dbghelp shlwapi urlmon userenv advapi32 kernel32)
	foreach(lib ${win_libs})
		set(link_flags "${link_flags} -l${lib}")
	endforeach()
	
	list(APPEND uv_libraries psapi iphlpapi userenv ws2_32)
	foreach(lib ${uv_libraries})
		set(link_flags "${link_flags} -l${lib}")
	endforeach()
	
	set(link_flags "${link_flags} -Xlinker //OPT:ICF -Xlinker //ERRORREPORT:PROMPT -Xlinker //NOLOGO -Xlinker //TLBID:1")

	if(${ARCH} STREQUAL "amd64")
		set(flags "${flags} -m64")
	elseif(${ARCH} STREQUAL "i386")
		set(flags "${flags} -m32")
	endif()
endif()
set(CMAKE_C_FLAGS "${flags}")
set(CMAKE_CXX_FLAGS "${flags} ${ccflags}")

if(${OS} STREQUAL "Linux")
	target_link_libraries(fibjs dl rt)
endif()

if(${OS} STREQUAL "FreeBSD")
	find_library(execinfo execinfo "/usr/local/lib" "/usr/lib")
	target_link_libraries(fibjs ${execinfo})
endif()

if((NOT ${OS} STREQUAL "Windows") AND (NOT HAVE_GLIB_C_ATOMIC_H))
	target_link_libraries(fibjs atomic)
endif()

if(${OS} STREQUAL "Windows")
	target_compile_definitions(fibjs PUBLIC -D_CONSOLE)
	set_target_properties(fibjs PROPERTIES LINK_FLAGS "${link_flags} -Xlinker //SUBSYSTEM:CONSOLE")
	
	target_compile_definitions(fibjs_gui PUBLIC -D_WINDOWS)
	set_target_properties(fibjs_gui PROPERTIES LINK_FLAGS "${link_flags} -Xlinker //SUBSYSTEM:WINDOWS")
else()
	set_target_properties(fibjs PROPERTIES LINK_FLAGS ${link_flags})
endif()