set(os_c_code "
# if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#error cmake_OS Windows
# endif

# if defined(linux) || defined(__linux) || defined(__linux__)
#error cmake_OS Linux
# endif

# if defined(__NetBSD__)
#error cmake_OS NetBSD
# endif

# if defined(__OpenBSD__)
#error cmake_OS OpenBSD
# endif

# if (defined(__FreeBSD__) || defined(__DragonFly__) || defined(__FreeBSD_kernel__)) && !defined(FREEBSD)
#error cmake_OS FreeBSD
# endif

# if defined(sun) || defined(__sun)
#error cmake_OS Solaris
# endif

# if defined(macosx) || (defined(__APPLE__) && defined(__MACH__))
#error cmake_OS OSX
# endif

#error cmake_OS unknown
")

file(WRITE "${CMAKE_BINARY_DIR}/os.c" "${os_c_code}")

enable_language(C)

try_run(
    run_result_unused
    compile_result_unused
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_BINARY_DIR}/os.c"
    COMPILE_OUTPUT_VARIABLE OS
)

string(REGEX MATCH "cmake_OS ([a-zA-Z0-9_]+)" OS "${OS}")

string(REPLACE "cmake_OS " "" OS "${OS}")

if (NOT OS)
    set(OS unknown)
endif()