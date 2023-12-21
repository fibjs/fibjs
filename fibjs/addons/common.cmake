get_filename_component(name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${name})

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/build_tools/cmake/option.cmake)

file(GLOB src_list "*.c*" "../*.c*")
add_library(${name} SHARED ${src_list})

include_directories(
    "${PROJECT_SOURCE_DIR}/../../include/addons/node_api"
    "${PROJECT_SOURCE_DIR}/../../../vender/uv/include"
    "${PROJECT_SOURCE_DIR}/../"
    "${CMAKE_CURRENT_BINARY_DIR}")

if(MSVC)
    target_link_libraries(${name} "${PROJECT_SOURCE_DIR}/../lib/node_${BUILD_ARCH}.lib")
    set(link_flags "${link_flags} /DELAYLOAD:node.exe")
else()
    set(link_flags "${link_flags} -Wl,-undefined,dynamic_lookup")
endif()

setup_result_library(${name})

set_target_properties(${name} PROPERTIES PREFIX "")
set_target_properties(${name} PROPERTIES SUFFIX ".node")
