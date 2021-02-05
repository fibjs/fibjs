cmake_minimum_required(VERSION 3.10)

if(NOT "${CLEAN_BUILD}" STREQUAL "true")
    include(../vender/tools/get_env.cmake)

    set(OUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../out/${CMAKE_HOST_SYSTEM_NAME}_${BUILD_ARCH}_${BUILD_TYPE}")
    
    execute_process(WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND git describe --tags --always
        OUTPUT_VARIABLE GIT_INFO
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    build("${CMAKE_CURRENT_SOURCE_DIR}" "${OUT_PATH}/fibjs" "-DGIT_INFO=${GIT_INFO}")
endif()
