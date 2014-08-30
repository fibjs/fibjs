cmake_minimum_required(VERSION 2.6)

set(name sqlite)

add_definitions(-DSQLITE_ENABLE_FTS4=1)

include(../../tools/basic.cmake)