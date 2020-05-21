if (NOT DEFINED VENDER_ROOT)
    set(VENDER_ROOT "${CMAKE_SOURCE_DIR}/../../")
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    # keep same name format with Unix
    set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
    set(CMAKE_STATIC_LIBRARY_PREFIX_C "lib")
    set(CMAKE_STATIC_LIBRARY_PREFIX_CXX "lib")

    if(NOT DEFINED CXX_STD_VER_FLAG)
        set(CXX_STD_VER_FLAG "-std=c++14")
    endif()

    if(${ARCH} STREQUAL "amd64")
        set(CLANG_TARGET x86_64-pc-windows-msvc)
    elseif(${ARCH} STREQUAL "i386")
        set(CLANG_TARGET i686-pc-windows-msvc)
    endif()

    if(FIBJS_CMAKE_BUILD_VERBOSE)
        message("[cmake::platform] OS is ${OS}")
        message("[cmake::platform] CMAKE_GENERATOR is ${CMAKE_GENERATOR}")
        message("[cmake::platform] CMAKE_C_COMPILER is ${CMAKE_C_COMPILER}")
        message("[cmake::platform] CMAKE_CXX_COMPILER is ${CMAKE_CXX_COMPILER}")
        message("[cmake::platform] CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
    endif()

    SET(flags "${flags} -target ${CLANG_TARGET}")
endif()

if(NOT DEFINED USED_BY_FIBJS)
    set(USED_BY_FIBJS "")
endif()

if((${BUILD_TYPE}) STREQUAL "release")
    set(CMAKE_BUILD_TYPE "Release")
elseif((${BUILD_TYPE}) STREQUAL "debug")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

if(NOT DEFINED CXX_STD_VER_FLAG)
    set(CXX_STD_VER_FLAG "-std=c++11")
endif()