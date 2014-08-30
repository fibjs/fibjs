cmake_minimum_required(VERSION 2.6)

set(name v8)

include(../../tools/basic.cmake)

if(${BUILD_TYPE} STREQUAL "Debug")
	add_definitions(-DVERIFY_HEAP=1 -DOBJECT_PRINT=1 -DENABLE_DISASSEMBLER=1 -DENABLE_HANDLE_ZAPPING=1 -DENABLE_EXTRA_CHECKS=1)
endif()
