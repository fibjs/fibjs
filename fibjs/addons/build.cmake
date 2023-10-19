cmake_minimum_required(VERSION 3.0)

if(NOT "${CLEAN_BUILD}" STREQUAL "true")
    include(../../vender/build_tools/cmake-scripts/get_env.cmake)

    set(WORK_ROOT $ENV{WORK_ROOT})
    if(NOT WORK_ROOT)
        set(WORK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    endif()

    set(addons
        1_hello_world
        2_function_arguments
        3_callbacks
        4_object_factory
        5_function_factory
        6_object_wrap
        test_array
        test_bigint
        test_buffer
        test_cannot_run_js
        test_constructor
        test_conversions
        test_dataview
        test_date
        test_error
        test_exception
        test_function
        test_general
        test_handle_scope
        test_init_order
        test_new_target
        test_number
        test_object
        test_promise
        test_properties
        test_reference
        test_string
        test_symbol
        test_typedarray
    )

    foreach(addon ${addons})
        build("${CMAKE_CURRENT_SOURCE_DIR}/${addon}" "${WORK_ROOT}" "${addon}")
    endforeach()
endif()
