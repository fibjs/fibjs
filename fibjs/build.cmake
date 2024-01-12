cmake_minimum_required(VERSION 3.0)

set(WORK_ROOT $ENV{WORK_ROOT})
if(NOT WORK_ROOT)
    set(WORK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
endif()

if("${CLEAN_BUILD}" STREQUAL "true")
    file(REMOVE_RECURSE ${WORK_ROOT}/out)
    file(REMOVE_RECURSE ${WORK_ROOT}/bin)
else()
    include(../vender/build_tools/cmake-scripts/get_env.cmake)

    build("${CMAKE_CURRENT_SOURCE_DIR}" "${WORK_ROOT}" "fibjs")
endif()
