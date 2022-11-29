cmake_minimum_required(VERSION 3.0)

if(NOT "${CLEAN_BUILD}" STREQUAL "true")
    include(../../vender/build_tools/cmake-scripts/get_env.cmake)

    set(WORK_ROOT $ENV{WORK_ROOT})
    if(NOT WORK_ROOT)
        set(WORK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    endif()

    build("${CMAKE_CURRENT_SOURCE_DIR}" "${WORK_ROOT}" "program")
endif()
