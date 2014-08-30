cmake_minimum_required(VERSION 2.6)

set(name gd)

include_directories("${PROJECT_SOURCE_DIR}/../tiff/include/" "${PROJECT_SOURCE_DIR}/../png/include/" "${PROJECT_SOURCE_DIR}/../jpeg/include/" "${PROJECT_SOURCE_DIR}/../zlib/include/")

include(../../tools/basic.cmake)