set(archdetect_c_code "
# if defined(i386) || defined(__i386__) || defined(_M_IX86) || defined(_X86_) || defined(x86)
#error cmake_ARCH I386
# endif

# if defined(__amd64) || defined(__x86_64__) || defined(_M_X64)
#error cmake_ARCH x64
# endif

# if defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
#error cmake_ARCH ia64
# endif

# if defined(__AARCH64EL__)
#error cmake_ARCH arm64
# endif

# if defined(__ARMEL__)
#error cmake_ARCH arm
# endif

#if defined(__ppc__) || defined(__ppc) || defined(__powerpc__) \\
      || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC)  \\
      || defined(_M_MPPC) || defined(_M_PPC)
#  if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
#error cmake_ARCH ppc64
#  else
#error cmake_ARCH ppc
#endif

#error cmake_ARCH unknown
")

file(WRITE "${CMAKE_BINARY_DIR}/arch.c" "${archdetect_c_code}")

enable_language(C)

try_run(
    run_result_unused
    compile_result_unused
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_BINARY_DIR}/arch.c"
    COMPILE_OUTPUT_VARIABLE ARCH
)

string(REGEX MATCH "cmake_ARCH ([a-zA-Z0-9_]+)" ARCH "${ARCH}")

string(REPLACE "cmake_ARCH " "" ARCH "${ARCH}")

if (NOT ARCH)
    set(ARCH unknown)
endif()

# check host bitwide
if(${CMAKE_SIZEOF_VOID_P} EQUAL "8")
	set(HOST_OS_64BIT TRUE BOOL INTERNAL)
	set(HOST_OS_BIT "64")
else()
	set(HOST_OS_32BIT TRUE BOOL INTERNAL)
	set(HOST_OS_BIT "32")
endif()
