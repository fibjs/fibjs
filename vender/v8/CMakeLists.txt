cmake_minimum_required(VERSION 2.6)

set(name v8)

set(flags "-std=gnu++0x")

include(../../tools/basic.cmake)

add_definitions(-DV8_NO_FAST_TLS=1)
if(${BUILD_TYPE} STREQUAL "Debug")
	add_definitions(-DVERIFY_HEAP=1 -DOBJECT_PRINT=1 -DENABLE_DISASSEMBLER=1 -DENABLE_HANDLE_ZAPPING=1 -DENABLE_EXTRA_CHECKS=1)
endif()
