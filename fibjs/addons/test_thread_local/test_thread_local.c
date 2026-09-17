/*
 * test_thread_local.c
 *
 *  Reproduction fixture for N-API addons that keep state in OS thread-local
 *  storage (TLS) while running on fibjs' multi-threaded fiber scheduler.
 *
 *  Background
 *  ----------
 *  fibjs runs every JS fiber of one Isolate on a pool of worker threads
 *  (`exlib::Service`, `cpus + 1` workers). A fiber that suspends on a blocking
 *  native call is resumed by whichever worker thread picks it up first, so the
 *  OS thread that executes a JS callback is not stable across a single `await`
 *  or a single synchronous `read()`.
 *
 *  Node.js never exposes this: one isolate == one event-loop thread, so an
 *  addon may safely assume "one JS thread per Env". napi-rs states that
 *  assumption explicitly and relies on it: the constructor of every `#[napi]`
 *  class is cached in a Rust `thread_local!` map at module registration:
 *
 *      // crates/napi/src/bindgen_runtime/module_register.rs
 *      thread_local! {
 *        static REGISTERED_CLASSES: LazyCell<RegisteredClasses> = ...;
 *      }
 *      pub fn get_class_constructor(js_name: &'static str) -> Option<napi_ref> {
 *        REGISTERED_CLASSES.with(|cell| cell.borrow_mut(|map| map.get(js_name).copied()))
 *      }
 *
 *  On fibjs, `require('@napi-rs/canvas')` fills that map on worker thread A.
 *  A later `createCanvas()` call that happens to be scheduled on worker
 *  thread B finds the map empty and fails with:
 *
 *      Error: Failed to get constructor of class `CanvasRenderingContext2D`
 *      { code: "InvalidArg" }
 *
 *  This fixture models the exact same pattern in plain C, without Rust or
 *  Skia, so the failure can be reproduced deterministically.
 *
 *  Exposed API
 *  -----------
 *    threadId()               current OS thread id
 *    registrationThreadId()   OS thread id that ran module registration
 *    resetTls(v)              write v into the thread-local slot, returns previous
 *    getTls()                 read the thread-local slot (0 on an unseen thread)
 *    tlsWriteCount()          number of writes performed on the current thread
 *    setGlobal(v) / getGlobal()   process-global slot, for contrast
 *    lookupClass(name)        napi-rs `get_class_constructor` analogue:
 *                             returns the opaque constructor handle registered
 *                             at module init, or throws InvalidArg when the
 *                             calling thread never ran module registration
 *    registryThreadId()       OS thread id the registry entry was written on
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <node_api.h>
#include <uv.h>
#include "../common.h"

#ifdef _WIN32
#include <windows.h>
#define TLS_SLOT __declspec(thread)
#else
#include <pthread.h>
#define TLS_SLOT _Thread_local
#endif

// Magic stored in the "class registry" at module registration time. Any other
// value means the current thread never ran module registration.
#define REGISTRY_MAGIC 0x52454743 /* "REGC" */

static int64_t os_thread_id()
{
#ifdef _WIN32
    return (int64_t)GetCurrentThreadId();
#else
    return (int64_t)(uintptr_t)pthread_self();
#endif
}

/*
 * napi-rs `REGISTERED_CLASSES` analogue: written once by module registration,
 * read again on every later call. A `thread_local` slot therefore looks like a
 * process-wide cache but actually is a per-thread cache.
 */
TLS_SLOT int64_t tls_registry;
TLS_SLOT int64_t tls_registry_tid;

/* Generic user visible slot, to show that any addon-owned TLS is lost. */
TLS_SLOT int64_t tls_value;
TLS_SLOT int64_t tls_writes;

/* Recorded at module registration. */
static int64_t s_registration_tid;
static int64_t s_registration_count;

/* Real process-global state, for contrast. */
static int64_t s_global_value;

typedef struct {
    napi_async_work work;
    napi_ref cb_ref;
} async_work_ctx;

typedef struct {
    napi_threadsafe_function tsfn;
    uv_thread_t thread;
} tsfn_ctx;

static napi_value create_int64(napi_env env, int64_t value)
{
    napi_value result;
    NODE_API_CALL(env, napi_create_int64(env, value, &result));
    return result;
}

static void create_thread_snapshot_argv(napi_env env, napi_value argv[3])
{
    argv[0] = create_int64(env, os_thread_id());
    argv[1] = create_int64(env, tls_registry);
    argv[2] = create_int64(env, tls_registry_tid);
}

static napi_value ThreadId(napi_env env, napi_callback_info info)
{
    return create_int64(env, os_thread_id());
}

static napi_value RegistrationThreadId(napi_env env, napi_callback_info info)
{
    return create_int64(env, s_registration_tid);
}

static napi_value RegistrationCount(napi_env env, napi_callback_info info)
{
    return create_int64(env, s_registration_count);
}

static napi_value ResetTls(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    int64_t value = 0;
    int64_t previous;

    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));
    if (argc > 0)
        NODE_API_CALL(env, napi_get_value_int64(env, args[0], &value));

    previous = tls_value;
    tls_value = value;
    tls_writes++;

    return create_int64(env, previous);
}

static napi_value GetTls(napi_env env, napi_callback_info info)
{
    return create_int64(env, tls_value);
}

static napi_value TlsWriteCount(napi_env env, napi_callback_info info)
{
    return create_int64(env, tls_writes);
}

static napi_value SetGlobal(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    int64_t value = 0;
    int64_t previous;

    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));
    if (argc > 0)
        NODE_API_CALL(env, napi_get_value_int64(env, args[0], &value));

    previous = s_global_value;
    s_global_value = value;

    return create_int64(env, previous);
}

static napi_value GetGlobal(napi_env env, napi_callback_info info)
{
    return create_int64(env, s_global_value);
}

static napi_value RegistryThreadId(napi_env env, napi_callback_info info)
{
    return create_int64(env, tls_registry_tid);
}

static void async_execute(napi_env env, void* data)
{
    (void)env;
    (void)data;
}

static void async_complete(napi_env env, napi_status status, void* data)
{
    async_work_ctx* ctx = (async_work_ctx*)data;
    napi_value cb;
    napi_value undefined;
    napi_value argv[3];

    if (status == napi_ok) {
        NODE_API_CALL_RETURN_VOID(env, napi_get_reference_value(env, ctx->cb_ref, &cb));
        NODE_API_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
        create_thread_snapshot_argv(env, argv);
        NODE_API_CALL_RETURN_VOID(env, napi_call_function(env, undefined, cb, 3, argv, NULL));
    }

    NODE_API_CALL_RETURN_VOID(env, napi_delete_reference(env, ctx->cb_ref));
    NODE_API_CALL_RETURN_VOID(env, napi_delete_async_work(env, ctx->work));
    free(ctx);
}

static napi_value RunAsync(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value resource_name;
    async_work_ctx* ctx = NULL;

    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));
    NODE_API_ASSERT(env, argc == 1, "RunAsync expects one callback argument");

    ctx = (async_work_ctx*)malloc(sizeof(async_work_ctx));
    NODE_API_ASSERT(env, ctx != NULL, "malloc async_work_ctx");

    NODE_API_CALL(env, napi_create_reference(env, args[0], 1, &ctx->cb_ref));
    NODE_API_CALL(env, napi_create_string_utf8(env,
                           "ThreadLocalAsyncWork", NAPI_AUTO_LENGTH, &resource_name));
    NODE_API_CALL(env, napi_create_async_work(env,
                           NULL,
                           resource_name,
                           async_execute,
                           async_complete,
                           ctx,
                           &ctx->work));
    NODE_API_CALL(env, napi_queue_async_work(env, ctx->work));

    return NULL;
}

static void tsfn_call_js(napi_env env, napi_value cb, void* context, void* data)
{
    (void)context;
    (void)data;

    if (env == NULL || cb == NULL)
        return;

    napi_value undefined;
    napi_value argv[3];
    NODE_API_CALL_RETURN_VOID(env, napi_get_undefined(env, &undefined));
    create_thread_snapshot_argv(env, argv);
    NODE_API_CALL_RETURN_VOID(env, napi_call_function(env, undefined, cb, 3, argv, NULL));
}

static void tsfn_thread_main(void* data)
{
    tsfn_ctx* ctx = (tsfn_ctx*)data;
    napi_status status = napi_call_threadsafe_function(ctx->tsfn, NULL, napi_tsfn_blocking);

    if (status != napi_ok && status != napi_closing) {
        napi_fatal_error("thread_local tsfn", NAPI_AUTO_LENGTH,
            "napi_call_threadsafe_function failed", NAPI_AUTO_LENGTH);
    }

    napi_release_threadsafe_function(ctx->tsfn, napi_tsfn_release);
}

static void tsfn_finalize(napi_env env, void* data, void* hint)
{
    tsfn_ctx* ctx = (tsfn_ctx*)data;
    (void)env;
    (void)hint;
    uv_thread_join(&ctx->thread);
    free(ctx);
}

static napi_value RunThreadsafe(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value resource_name;
    tsfn_ctx* ctx = NULL;

    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));
    NODE_API_ASSERT(env, argc == 1, "RunThreadsafe expects one callback argument");

    ctx = (tsfn_ctx*)malloc(sizeof(tsfn_ctx));
    NODE_API_ASSERT(env, ctx != NULL, "malloc tsfn_ctx");

    NODE_API_CALL(env, napi_create_string_utf8(env,
                           "ThreadLocalTSFN", NAPI_AUTO_LENGTH, &resource_name));
    NODE_API_CALL(env, napi_create_threadsafe_function(env,
                           args[0],
                           NULL,
                           resource_name,
                           0,
                           1,
                           ctx,
                           tsfn_finalize,
                           NULL,
                           tsfn_call_js,
                           &ctx->tsfn));

    NODE_API_ASSERT(env,
        uv_thread_create(&ctx->thread, tsfn_thread_main, ctx) == 0,
        "TSFN thread creation");

    return NULL;
}

/*
 * Faithful analogue of napi-rs' `get_class_constructor`: the lookup goes to the
 * thread-local registry and fails when the calling OS thread is not the one
 * that loaded the addon.
 */
static napi_value LookupClass(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    char name[128] = "Unnamed";
    size_t length = 0;

    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));
    if (argc > 0)
        napi_get_value_string_utf8(env, args[0], name, sizeof(name), &length);

    if (tls_registry != REGISTRY_MAGIC) {
        // Same error shape emitted by napi-rs on an empty thread-local map.
        char message[256];

        snprintf(message, sizeof(message),
            "Failed to get constructor of class `%s`", name);

        napi_throw_error(env, "InvalidArg", message);
        return NULL;
    }

    return create_int64(env, tls_registry);
}

static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NODE_API_PROPERTY("threadId", ThreadId),
        DECLARE_NODE_API_PROPERTY("registrationThreadId", RegistrationThreadId),
        DECLARE_NODE_API_PROPERTY("registrationCount", RegistrationCount),
        DECLARE_NODE_API_PROPERTY("resetTls", ResetTls),
        DECLARE_NODE_API_PROPERTY("getTls", GetTls),
        DECLARE_NODE_API_PROPERTY("tlsWriteCount", TlsWriteCount),
        DECLARE_NODE_API_PROPERTY("setGlobal", SetGlobal),
        DECLARE_NODE_API_PROPERTY("getGlobal", GetGlobal),
        DECLARE_NODE_API_PROPERTY("lookupClass", LookupClass),
        DECLARE_NODE_API_PROPERTY("registryThreadId", RegistryThreadId),
        DECLARE_NODE_API_PROPERTY("runAsync", RunAsync),
        DECLARE_NODE_API_PROPERTY("runThreadsafe", RunThreadsafe),
    };

    // Module registration runs on the thread that executed `require()`.
    // Everything written here stays on that thread only.
    s_registration_tid = os_thread_id();
    s_registration_count++;
    tls_registry = REGISTRY_MAGIC;
    tls_registry_tid = s_registration_tid;

    NODE_API_CALL(env, napi_define_properties(
                           env, exports,
                           sizeof(properties) / sizeof(*properties), properties));

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
