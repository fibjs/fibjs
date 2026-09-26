cmake_minimum_required(VERSION 3.10)

# ============================================================================
# fibjs - single CMake build tree
#
#   vender (optional, in-tree or prebuilt dist)
#     + fibjs library
#     + program (fibjs executable, js shared library)
#     + addons (N-API test addons)
#     + installer (release only)
#
# Configure:
#   cmake -DBUILD_ARCH=x64 -DBUILD_TYPE=release -DBT_VENDER=in-tree \
#         -S . -B out/Linux_x64_release
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

# The N-API test addons are part of the default build: each one is an
# independent shared library, so they compile in parallel with the rest of the
# tree instead of extending the critical path.
add_subdirectory(fibjs/addons)

# The installer (and the release packaging targets) only exist for release builds.
if("${BUILD_TYPE}" STREQUAL "release")
    add_subdirectory(fibjs/installer)
endif()
