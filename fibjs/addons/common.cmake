get_filename_component(name ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${name})

include(${CMAKE_CURRENT_LIST_DIR}/../../vender/build_tools/cmake/option.cmake)

file(GLOB_RECURSE src_list "*.c*")

add_library(${name} SHARED ${src_list})

include_directories("${PROJECT_SOURCE_DIR}/../../include/addons" "${PROJECT_SOURCE_DIR}/../")

if(MSVC)
    target_link_libraries(${name} "${BIN_PATH}/fibjs.lib")
else()
    set(link_flags "${link_flags} ${link_flags} -Wl,-undefined,dynamic_lookup")
endif()

setup_result_library(${name})

set_target_properties(${name} PROPERTIES PREFIX "")
set_target_properties(${name} PROPERTIES SUFFIX ".node")

set_target_properties(${name} PROPERTIES LINK_FLAGS ${link_flags})
