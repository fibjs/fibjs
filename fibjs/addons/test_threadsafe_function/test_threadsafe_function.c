#include <node_api.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include "../common.h"

#define ARRAY_LENGTH 10
#define MAX_QUEUE_SIZE 2

static uv_thread_t the_thread;
static napi_threadsafe_function ts_fn;

// Thread data
static int ints[ARRAY_LENGTH];

// JS marshaller callback: converts int* data to napi_value and calls JS
static void call_js(napi_env env, napi_value cb, void* hint, void* data) {
    if (!(env == NULL || cb == NULL)) {
        napi_value argv, undefined;
        NODE_API_CALL_RETURN_VOID(env,
            napi_create_int32(env, *(int*)data, &argv));
        NODE_API_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
        NODE_API_CALL_RETURN_VOID(env,
            napi_call_function(env, undefined, cb, 1, &argv, NULL));
    }
}

// Worker thread: pushes all data items then releases
static void data_source_thread(void* data) {
    napi_threadsafe_function fn = (napi_threadsafe_function)data;
    int index;

    for (index = ARRAY_LENGTH - 1; index >= 0; index--) {
        napi_status status =
            napi_call_threadsafe_function(fn, &ints[index], napi_tsfn_blocking);
        if (status == napi_closing)
            break;
        if (status != napi_ok) {
            napi_fatal_error("data_source_thread", NAPI_AUTO_LENGTH,
                "napi_call_threadsafe_function failed", NAPI_AUTO_LENGTH);
        }
    }

    napi_release_threadsafe_function(fn, napi_tsfn_release);
}

// Invoke finalize callback and join thread
static void join_the_thread(napi_env env, void* raw_data, void* hint) {
    napi_ref* ref = (napi_ref*)raw_data;
    napi_value js_cb, undefined;

    uv_thread_join(&the_thread);

    NODE_API_CALL_RETURN_VOID(env,
        napi_get_reference_value(env, *ref, &js_cb));
    NODE_API_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    NODE_API_CALL_RETURN_VOID(env,
        napi_call_function(env, undefined, js_cb, 0, NULL, NULL));
    NODE_API_CALL_RETURN_VOID(env, napi_delete_reference(env, *ref));
    free(ref);
}

// StartThread(callback, finalize_callback, max_queue_size[, initial_thread_count])
static napi_value StartThread(napi_env env, napi_callback_info info) {
    size_t argc = 4;
    napi_value argv[4];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));

    uint32_t max_queue_size;
    NODE_API_CALL(env, napi_get_value_uint32(env, argv[2], &max_queue_size));

    uint32_t initial_thread_count = 1;
    if (argc > 3) {
        NODE_API_CALL(env, napi_get_value_uint32(env, argv[3], &initial_thread_count));
    }

    napi_ref* ref = (napi_ref*)malloc(sizeof(napi_ref));
    NODE_API_CALL(env, napi_create_reference(env, argv[1], 1, ref));

    napi_value async_name;
    NODE_API_CALL(env, napi_create_string_utf8(env,
        "ThreadSafeTest", NAPI_AUTO_LENGTH, &async_name));

    NODE_API_CALL(env, napi_create_threadsafe_function(env,
        argv[0], NULL, async_name,
        max_queue_size, initial_thread_count,
        ref, join_the_thread,
        NULL, call_js,
        &ts_fn));

    NODE_API_ASSERT(env,
        uv_thread_create(&the_thread, data_source_thread, ts_fn) == 0,
        "Thread creation");

    return NULL;
}

// StopThread(abort)
static napi_value StopThread(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));

    bool abort;
    NODE_API_CALL(env, napi_get_value_bool(env, argv[0], &abort));

    NODE_API_ASSERT(env, ts_fn != NULL, "Existing threadsafe function");
    NODE_API_CALL(env,
        napi_release_threadsafe_function(ts_fn,
            abort ? napi_tsfn_abort : napi_tsfn_release));
    ts_fn = NULL;
    return NULL;
}

// Unref()
static napi_value Unref(napi_env env, napi_callback_info info) {
    NODE_API_ASSERT(env, ts_fn != NULL, "Existing threadsafe function");
    NODE_API_CALL(env, napi_unref_threadsafe_function(env, ts_fn));
    return NULL;
}

// StartThreadNoNative(callback, finalize_callback, max_queue_size)
// Uses default call_js (no custom marshaller)
static napi_value StartThreadNoNative(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value argv[3];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));

    uint32_t max_queue_size;
    NODE_API_CALL(env, napi_get_value_uint32(env, argv[2], &max_queue_size));

    napi_ref* ref = (napi_ref*)malloc(sizeof(napi_ref));
    NODE_API_CALL(env, napi_create_reference(env, argv[1], 1, ref));

    napi_value async_name;
    NODE_API_CALL(env, napi_create_string_utf8(env,
        "ThreadSafeTestNoNative", NAPI_AUTO_LENGTH, &async_name));

    NODE_API_CALL(env, napi_create_threadsafe_function(env,
        argv[0], NULL, async_name,
        max_queue_size, 1,
        ref, join_the_thread,
        NULL, NULL,
        &ts_fn));

    NODE_API_ASSERT(env,
        uv_thread_create(&the_thread, data_source_thread, ts_fn) == 0,
        "Thread creation");

    return NULL;
}

// Acquire and release from secondary thread
static napi_threadsafe_function acquire_ts_fn;

static void secondary_acquire_thread(void* data) {
    napi_threadsafe_function fn = (napi_threadsafe_function)data;
    napi_status status;

    status = napi_acquire_threadsafe_function(fn);
    if (status != napi_ok) {
        napi_fatal_error("secondary_acquire_thread", NAPI_AUTO_LENGTH,
            "napi_acquire_threadsafe_function failed", NAPI_AUTO_LENGTH);
    }

    // Push one item, then release  
    int value = 42;
    status = napi_call_threadsafe_function(fn, &value, napi_tsfn_blocking);
    if (status != napi_ok && status != napi_closing) {
        napi_fatal_error("secondary_acquire_thread", NAPI_AUTO_LENGTH,
            "napi_call_threadsafe_function failed", NAPI_AUTO_LENGTH);
    }

    napi_release_threadsafe_function(fn, napi_tsfn_release);
}

// GetContext test
static void* test_context_ptr = (void*)0xDEAD;

static napi_value TestGetContext(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));

    napi_value async_name;
    NODE_API_CALL(env, napi_create_string_utf8(env,
        "ContextTest", NAPI_AUTO_LENGTH, &async_name));

    napi_threadsafe_function fn;
    NODE_API_CALL(env, napi_create_threadsafe_function(env,
        argv[0], NULL, async_name,
        0, 1,
        NULL, NULL,
        test_context_ptr, NULL,
        &fn));

    void* context;
    NODE_API_CALL(env,
        napi_get_threadsafe_function_context(fn, &context));

    napi_value result;
    NODE_API_CALL(env,
        napi_get_boolean(env, context == test_context_ptr, &result));

    NODE_API_CALL(env, napi_release_threadsafe_function(fn, napi_tsfn_release));

    return result;
}

// Module init
static napi_value Init(napi_env env, napi_value exports) {
    size_t index;
    for (index = 0; index < ARRAY_LENGTH; index++) {
        ints[index] = index;
    }

    napi_value js_array_length, js_max_queue_size;
    napi_create_uint32(env, ARRAY_LENGTH, &js_array_length);
    napi_create_uint32(env, MAX_QUEUE_SIZE, &js_max_queue_size);

    napi_property_descriptor properties[] = {
        { "ARRAY_LENGTH", NULL, NULL, NULL, NULL, js_array_length,
          napi_enumerable, NULL },
        { "MAX_QUEUE_SIZE", NULL, NULL, NULL, NULL, js_max_queue_size,
          napi_enumerable, NULL },
        DECLARE_NODE_API_PROPERTY("StartThread", StartThread),
        DECLARE_NODE_API_PROPERTY("StartThreadNoNative", StartThreadNoNative),
        DECLARE_NODE_API_PROPERTY("StopThread", StopThread),
        DECLARE_NODE_API_PROPERTY("Unref", Unref),
        DECLARE_NODE_API_PROPERTY("TestGetContext", TestGetContext),
    };

    NODE_API_CALL(env, napi_define_properties(env, exports,
        sizeof(properties) / sizeof(properties[0]), properties));

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
