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
#include <string.h>

#include <node_api.h>
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

static napi_value create_int64(napi_env env, int64_t value)
{
    napi_value result;
    NODE_API_CALL(env, napi_create_int64(env, value, &result));
    return result;
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
