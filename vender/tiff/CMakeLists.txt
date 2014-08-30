cmake_minimum_required(VERSION 2.6)

set(name tiff)

set(flags "-Wno-int-to-void-pointer-cast -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast")
include_directories("${PROJECT_SOURCE_DIR}/../jpeg/include/" "${PROJECT_SOURCE_DIR}/../zlib/include/")

include(../../tools/basic.cmake)
