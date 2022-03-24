cmake_minimum_required(VERSION 3.0)

if(NOT "${CLEAN_BUILD}" STREQUAL "true")
    include(../vender/build_tools/cmake-scripts/get_env.cmake)

    set(WORK_ROOT $ENV{WORK_ROOT})
    if(NOT WORK_ROOT)
        set(WORK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
    endif()

    set(OUT_PATH "${WORK_ROOT}/out/${CMAKE_HOST_SYSTEM_NAME}_${BUILD_ARCH}_${BUILD_TYPE}")
    set(BIN_PATH "${WORK_ROOT}/bin/${CMAKE_HOST_SYSTEM_NAME}_${BUILD_ARCH}_${BUILD_TYPE}")
    build("${CMAKE_CURRENT_SOURCE_DIR}" "${OUT_PATH}/fibjs" ${BIN_PATH})
endif()
