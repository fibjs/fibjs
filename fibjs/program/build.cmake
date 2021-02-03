cmake_minimum_required(VERSION 3.12)

include(../../vender/tools/get_env.cmake)

set(OUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../out/${CMAKE_HOST_SYSTEM_NAME}_${BUILD_ARCH}_${BUILD_TYPE}")
build("${CMAKE_CURRENT_SOURCE_DIR}" "${OUT_PATH}/program")
