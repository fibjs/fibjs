cmake_minimum_required(VERSION 3.10)

# N-API test addons.
#
# They are built when -DFIBJS_ADDONS=ON is passed to the top-level configure;
# a single addon can always be built on demand:
#
#   cmake --build <build dir> --target test_buffer
#
# Each addon is an independent shared library (suffix .node) and links nothing
# from fibjs at build time (the symbols are resolved by the host process).

set(addons
    1_hello_world
    2_function_arguments
    3_callbacks
    4_object_factory
    5_function_factory
    6_object_wrap
    test_array
    test_async
    test_async_work_delete
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
    test_null_init
    test_number
    test_object
    test_promise
    test_properties
    test_reference
    test_string
    test_symbol
    test_thread_local
    test_threadsafe_function
    test_typedarray
)

foreach(addon ${addons})
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${addon}/CMakeLists.txt")
        add_subdirectory(${addon})
    endif()
endforeach()
