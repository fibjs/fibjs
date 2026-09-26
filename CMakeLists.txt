cmake_minimum_required(VERSION 3.10)

# ============================================================================
# fibjs - single CMake build tree
#
#   vender (optional, in-tree or prebuilt dist)
#     + fibjs library
#     + program (fibjs executable, js shared library)
#     + addons (optional)
#     + installer (release only)
#
# Configure:
#   cmake -DBUILD_ARCH=x64 -DBUILD_TYPE=release -DBT_VENDER=in-tree \
#         -DFIBJS_ADDONS=ON -S . -B out/Linux_x64_release
# Build:
#   cmake --build out/Linux_x64_release -- -j8
#
# See plans/single-cmake-build-refactor-plan.md
# ============================================================================

# config.cmake comes first: it detects the platform/arch and the compiler
# (CC/CXX) that project() is about to use.
include(${CMAKE_CURRENT_LIST_DIR}/vender/build_tools/cmake/config.cmake)

project(fibjs C CXX)

# Feature checks (glibc/iconv/C++20) and the generated configuration headers,
# once for the whole tree.
include(${CMAKE_CURRENT_LIST_DIR}/vender/build_tools/cmake/option_config.cmake)
bt_config_target()

option(FIBJS_ADDONS "build the N-API test addons" OFF)
set(BT_VENDER "in-tree" CACHE STRING "source of the vendored libraries: in-tree | dist")

if(BT_VENDER STREQUAL "dist")
    # Prebuilt libraries from a fibjs_vender release (CI path); the handling is
    # part of build_tools, next to the rest of the build infrastructure.
    set(BT_VENDER_DIR "${CMAKE_CURRENT_LIST_DIR}/vender")
    include(${CMAKE_CURRENT_LIST_DIR}/vender/build_tools/cmake/vender_dist.cmake)
else()
    add_subdirectory(vender)
endif()

add_subdirectory(fibjs)
add_subdirectory(fibjs/program)

if(FIBJS_ADDONS)
    add_subdirectory(fibjs/addons)
endif()

# The installer (and the release packaging targets) only exist for release builds.
if("${BUILD_TYPE}" STREQUAL "release")
    add_subdirectory(fibjs/installer)
endif()
