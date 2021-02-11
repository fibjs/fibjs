cmake_minimum_required(VERSION 2.6)

set(name "installer")
project(${name})

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/tools/option.cmake)

add_executable(${name} "src/installer.cpp")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../../bin/${CMAKE_HOST_SYSTEM_NAME}_${ARCH}_${BUILD_TYPE})

include_directories("${EXECUTABLE_OUTPUT_PATH}")

link_libraries(${name})