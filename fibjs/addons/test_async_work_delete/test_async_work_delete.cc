/*
 * Regression tests for the N-API async work life cycle.
 *
 * A fibjs async work object is both the N-API handle and the event queued in
 * the acPool thread pool (the pool stores a raw AsyncEvent* pointing at it), so
 * releasing the handle while the work is still queued or running used to leave
 * the pool holding a dangling pointer: the worker then called invoke() on freed
 * memory, read a garbage environment out of it and crashed in
 * Isolate::Unref() with SIGBUS(BUS_ADRALN)/SIGSEGV(SEGV_MAPERR) on native arm64
 * - on x86_64 and under qemu the same use-after-free only corrupted memory
 * silently, which is why the bug looked platform specific.
 *
 * The scenarios below attack the three properties a correct implementation
 * needs:
 *
 *  1. a work released while it is queued, or while a pool thread is executing
 *     it, must not be freed - the pool still owns a pointer to it;
 *  2. after the pool thread hands the object over to the JS job queue
 *     (post_task) it must not touch the object again: the JS thread runs the
 *     completion callback and may free it right away;
 *  3. a work released twice, or released before its completion callback runs,
 *     must not free the object underneath its own users.
 *
 * Detection does not rely on the unaligned atomic trap that made this look like
 * an arm64-only problem: after releasing a handle the tests take back the block
 * the runtime freed (if it freed one) from the JS thread and poison it, so any
 * later use of the object - the virtual call through its vtable, its
 * environment pointer, the reset of its V8 handle - is a wild access that
 * crashes on every platform.
 */

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include <node_api.h>
#include <uv.h>

#include "../common.h"

namespace {

// Widen the window between "a worker picked the work up" and "the object is
// used after the release" so the overlap does not depend on scheduling luck.
void BusyExecute(napi_env env, void* data)
{
    volatile int32_t sum = 0;
    for (int32_t i = 0; i < 20000; i++)
        sum += i;
    (void)sum;
}

void NoopExecute(napi_env env, void* data)
{
}

void NoopComplete(napi_env env, napi_status status, void* data)
{
}

std::atomic<int32_t> g_complete_count { 0 };
std::atomic<int32_t> g_complete_status { 0 };
std::atomic<int32_t> g_poison_complete_count { 0 };
std::atomic<int32_t> g_tail_complete_count { 0 };

void CountingComplete(napi_env env, napi_status status, void* data)
{
    g_complete_status = (int32_t)status;
    g_complete_count++;
}

void PoisonComplete(napi_env env, napi_status status, void* data){
    // Every work released while queued still has to report back: the runtime
    // must keep it alive until then.
    g_poison_complete_count++;
}

napi_value NewResult(napi_env env, int32_t value)
{
    napi_value result;
    NODE_API_CALL(env, napi_create_int32(env, value, &result));
    return result;
}

/*
 * PoisonBlocks(): allocate and poison one block per allocation size the work
 * object can live in, and keep them.
 *
 * A runtime that frees the work object while it is still in use hands its block
 * back to the allocator; the next same size allocation from this thread is then
 * very likely to be that very block (per size class free lists are LIFO), so it
 * comes back filled with 0x5a. Every later use of the object - the virtual
 * invoke() call, the environment pointer, the reset of its V8 handle - is then
 * a wild access, which is what makes the failure visible on any platform
 * instead of only where unaligned atomics trap (the arm64 report).
 */
std::vector<void*> PoisonBlocks()
{
    std::vector<void*> blocks;

    for (size_t size = 64; size <= 1024; size += 8) {
        void* p = malloc(size);
        if (p != nullptr) {
            memset(p, 0x5a, size);
            blocks.push_back(p);
        }
    }

    return blocks;
}

void FreeBlocks(std::vector<void*>& blocks)
{
    for (void* p : blocks)
        free(p);
    blocks.clear();
}

/*
 * queueAndDelete(count): create `count` async works, queue each one and delete
 * the handle right away. Every work is still in the acPool queue (or already
 * being picked up by a worker) when it is released.
 */
napi_value QueueAndDelete(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    int32_t count = 1000;
    if (argc >= 1)
        NODE_API_CALL(env, napi_get_value_int32(env, argv[0], &count));

    napi_value name;
    NODE_API_CALL(env,
        napi_create_string_utf8(env, "test_async_work_delete", NAPI_AUTO_LENGTH, &name));

    int32_t queued = 0;
    for (int32_t i = 0; i < count; i++) {
        napi_async_work work = nullptr;
        if (napi_create_async_work(env, nullptr, name, BusyExecute, NoopComplete, nullptr, &work) != napi_ok)
            break;

        if (napi_queue_async_work(env, work) != napi_ok) {
            napi_delete_async_work(env, work);
            break;
        }

        queued++;

        // The work is in flight here: releasing the handle must not free the
        // object while the pool or the job queue still owns it.
        if (napi_delete_async_work(env, work) != napi_ok)
            break;
    }

    return NewResult(env, queued);
}

/*
 * queueDeletePoison(count): like queueAndDelete, but reuse the block the
 * runtime may have freed right after every release.
 *
 * This is the platform independent version of the original report's trigger: a
 * work released while it is queued (or already being picked up), whose memory is
 * handed straight back to the addon and poisoned before a pool thread or the JS
 * job queue looks at it again.
 */
napi_value QueueDeletePoison(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    int32_t count = 100;
    if (argc >= 1)
        NODE_API_CALL(env, napi_get_value_int32(env, argv[0], &count));

    napi_value name;
    NODE_API_CALL(env,
        napi_create_string_utf8(env, "test_async_work_delete", NAPI_AUTO_LENGTH, &name));

    int32_t queued = 0;
    for (int32_t i = 0; i < count; i++) {
        napi_async_work work = nullptr;
        if (napi_create_async_work(env, nullptr, name, BusyExecute, PoisonComplete, nullptr, &work) != napi_ok)
            break;

        if (napi_queue_async_work(env, work) != napi_ok) {
            napi_delete_async_work(env, work);
            break;
        }

        queued++;

        if (napi_delete_async_work(env, work) != napi_ok)
            break;

        // The handle is released while the work is in flight: if the runtime
        // freed the object, take its block back poisoned before any pool thread
        // or the job queue touches it again.
        std::vector<void*> blocks = PoisonBlocks();
        FreeBlocks(blocks);
    }

    return NewResult(env, queued);
}

/*
 * cancelAndDelete(count): queue work, cancel it and release the handle. The
 * cancelled work still runs invoke() (with UV_ECANCELED) and is still delivered
 * to its complete callback, so the release overlaps the same in-flight phases.
 */
napi_value CancelAndDelete(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    int32_t count = 1000;
    if (argc >= 1)
        NODE_API_CALL(env, napi_get_value_int32(env, argv[0], &count));

    napi_value name;
    NODE_API_CALL(env,
        napi_create_string_utf8(env, "test_async_work_delete", NAPI_AUTO_LENGTH, &name));

    int32_t queued = 0;
    for (int32_t i = 0; i < count; i++) {
        napi_async_work work = nullptr;
        if (napi_create_async_work(env, nullptr, name, BusyExecute, NoopComplete, nullptr, &work) != napi_ok)
            break;

        if (napi_queue_async_work(env, work) != napi_ok) {
            napi_delete_async_work(env, work);
            break;
        }

        queued++;

        // napi_cancel_async_work() only flags the work, it is not a deletion,
        // and it may already have started (napi_generic_failure) - either way
        // the handle is released while the work is in flight.
        napi_cancel_async_work(env, work);

        if (napi_delete_async_work(env, work) != napi_ok)
            break;
    }

    return NewResult(env, queued);
}

/*
 * createAndDelete(count): create a work and release it without ever queueing
 * it. Nothing else references the object, so it has to be freed right away
 * (this is the no-op path that must not leak or double free).
 */
napi_value CreateAndDelete(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    int32_t count = 1000;
    if (argc >= 1)
        NODE_API_CALL(env, napi_get_value_int32(env, argv[0], &count));

    napi_value name;
    NODE_API_CALL(env,
        napi_create_string_utf8(env, "test_async_work_delete", NAPI_AUTO_LENGTH, &name));

    int32_t created = 0;
    for (int32_t i = 0; i < count; i++) {
        napi_async_work work = nullptr;
        if (napi_create_async_work(env, nullptr, name, NoopExecute, NoopComplete, nullptr, &work) != napi_ok)
            break;

        if (napi_delete_async_work(env, work) != napi_ok)
            break;

        created++;
    }

    return NewResult(env, created);
}

/*
 * queueDeleteInComplete(count, burn_ms): the shape real addons use - a CPU
 * heavy execute callback (napi-rs/spawn, PNG encoders) that releases the handle
 * from inside its completion callback, once per work.
 *
 * The pool thread may be descheduled right after it hand the object over to the
 * JS job queue; the completion callback runs on the JS thread and the runtime
 * frees the object when that callback is done, while the pool thread still has
 * to finish invoke(). Nothing on the pool thread may touch the object after the
 * hand-over, otherwise this pattern use-after-frees.
 */

struct TailCarrier {
    napi_async_work work;
    int32_t work_us;
};

void SpinUs(int32_t us)
{
    volatile int32_t sum = 0;
    uint64_t end = uv_hrtime() + (uint64_t)us * 1000;

    while (uv_hrtime() < end) {
        for (int32_t i = 0; i < 512; i++)
            sum += i;
    }
    (void)sum;
}

void TailExecute(napi_env env, void* data)
{
    TailCarrier* c = (TailCarrier*)data;
    SpinUs(c->work_us);
}

void TailComplete(napi_env env, napi_status status, void* data)
{
    TailCarrier* c = (TailCarrier*)data;

    // Rolling poisoning: the block of the work whose completion ran just before
    // this callback was freed at the end of that callback, so take it back
    // poisoned here. A pool thread that still has to finish invoke() for it
    // (i.e. one that was descheduled right after handing the object to the JS
    // job queue) then walks into poison instead of stale contents.
    std::vector<void*> blocks = PoisonBlocks();
    FreeBlocks(blocks);

    // Release the handle from the completion callback, like napi-rs and sharp do.
    napi_delete_async_work(env, c->work);
    free(c);

    g_tail_complete_count++;
}

napi_value QueueDeleteInComplete(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    int32_t count = 100;
    if (argc >= 1)
        NODE_API_CALL(env, napi_get_value_int32(env, argv[0], &count));

    int32_t work_us = 1000;
    if (argc >= 2)
        NODE_API_CALL(env, napi_get_value_int32(env, argv[1], &work_us));

    napi_value name;
    NODE_API_CALL(env,
        napi_create_string_utf8(env, "test_async_work_delete", NAPI_AUTO_LENGTH, &name));

    int32_t queued = 0;
    for (int32_t i = 0; i < count; i++) {
        TailCarrier* c = (TailCarrier*)malloc(sizeof(TailCarrier));
        if (c == nullptr)
            break;
        c->work = nullptr;
        c->work_us = work_us;

        if (napi_create_async_work(env, nullptr, name, TailExecute, TailComplete, c, &c->work) != napi_ok) {
            free(c);
            break;
        }

        if (napi_queue_async_work(env, c->work) != napi_ok) {
            napi_delete_async_work(env, c->work);
            free(c);
            break;
        }

        queued++;
    }

    return NewResult(env, queued);
}

/*
 * startBlocking()/isExecuting()/deleteWork()/releaseWork(): release the handle
 * while a pool thread is executing the work, i.e. strictly after the work has
 * left the queue and before invoke() has finished with the object.
 */

std::atomic<bool> g_executing { false };
std::atomic<bool> g_release { false };
napi_async_work g_work = nullptr;
napi_ref g_complete_cb = nullptr;

void BlockingExecute(napi_env env, void* data)
{
    g_executing = true;

    // Bounded wait (5s) so a broken test cannot hang the process forever.
    for (int32_t i = 0; i < 5000 && !g_release; i++)
        uv_sleep(1);

    g_executing = false;
}

void NotifyComplete(napi_env env, napi_status status, void* data)
{
    g_complete_status = (int32_t)status;
    g_complete_count++;

    if (g_complete_cb == nullptr)
        return;

    napi_value cb, global, arg, result;
    NODE_API_CALL_RETURN_VOID(env, napi_get_reference_value(env, g_complete_cb, &cb));
    NODE_API_CALL_RETURN_VOID(env, napi_get_global(env, &global));
    NODE_API_CALL_RETURN_VOID(env, napi_create_int32(env, (int32_t)status, &arg));
    NODE_API_CALL_RETURN_VOID(env, napi_call_function(env, global, cb, 1, &arg, &result));
}

napi_value StartBlocking(napi_env env, napi_callback_info info)
{
    if (g_work != nullptr) {
        napi_throw_error(env, nullptr, "a work is still in flight");
        return nullptr;
    }

    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc >= 1 && g_complete_cb == nullptr)
        NODE_API_CALL(env, napi_create_reference(env, argv[0], 1, &g_complete_cb));

    g_release = false;
    g_executing = false;

    napi_value name;
    NODE_API_CALL(env,
        napi_create_string_utf8(env, "test_async_work_delete", NAPI_AUTO_LENGTH, &name));

    NODE_API_CALL(env,
        napi_create_async_work(env, nullptr, name, BlockingExecute, NotifyComplete, nullptr, &g_work));
    NODE_API_CALL(env, napi_queue_async_work(env, g_work));

    return nullptr;
}

napi_value IsExecuting(napi_env env, napi_callback_info info)
{
    napi_value result;
    NODE_API_CALL(env, napi_get_boolean(env, g_executing, &result));
    return result;
}

napi_value DeleteWork(napi_env env, napi_callback_info info)
{
    if (g_work == nullptr)
        return nullptr;

    napi_async_work work = g_work;
    g_work = nullptr;

    NODE_API_CALL(env, napi_delete_async_work(env, work));

    // The pool thread is parked inside the execute callback right now: if the
    // runtime freed the object, it is poisoned memory from here on and the
    // worker walks into it as soon as releaseWork() lets it continue.
    std::vector<void*> blocks = PoisonBlocks();
    FreeBlocks(blocks);

    return nullptr;
}

napi_value ReleaseWork(napi_env env, napi_callback_info info)
{
    g_release = true;
    return nullptr;
}

napi_value CompleteCount(napi_env env, napi_callback_info info)
{
    return NewResult(env, g_complete_count);
}

napi_value PoisonCompleteCount(napi_env env, napi_callback_info info)
{
    return NewResult(env, g_poison_complete_count);
}

napi_value TailCompleteCount(napi_env env, napi_callback_info info)
{
    return NewResult(env, g_tail_complete_count);
}

napi_value CompleteStatus(napi_env env, napi_callback_info info)
{
    return NewResult(env, g_complete_status);
}

/*
 * churn(count): allocate and release C++ heap blocks. Called from the JS thread
 * between releasing an in-flight work and letting the pool thread resume, so
 * the allocator is very likely to hand the just released object straight back
 * out. On a runtime that frees the object early, the worker resumes on reused
 * memory instead of the old contents.
 */
napi_value Poison(napi_env env, napi_callback_info info)
{
    std::vector<void*> blocks = PoisonBlocks();
    FreeBlocks(blocks);
    return nullptr;
}

/*
 * startNoise(threads)/stopNoise(): background churn threads.
 *
 * Real addons (napi-rs, libvips, skia, pango) run worker threads that keep
 * allocating and freeing memory while async work completes. Those threads are
 * what makes the pool-thread hand-over race easy to hit in production: they
 * both put the CPU under pressure (the pool thread may be descheduled right
 * after it hands the object to the JS job queue) and take back the block of a
 * work object as soon as the runtime frees it. Reproducing it here means the
 * race can be exercised without a heavy real world workload.
 */
std::atomic<bool> g_noise_stop { false };
std::vector<std::thread> g_noise_threads;

void NoiseLoop()
{
    while (!g_noise_stop) {
        std::vector<void*> blocks;
        for (size_t size = 128; size <= 384; size += 16) {
            for (int32_t i = 0; i < 4; i++) {
                void* p = malloc(size);
                if (p != nullptr) {
                    memset(p, 0x5a, size);
                    blocks.push_back(p);
                }
            }
        }

        for (void* p : blocks)
            free(p);
    }
}

napi_value StartNoise(napi_env env, napi_callback_info info)
{
    int32_t count = 4;
    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc >= 1)
        NODE_API_CALL(env, napi_get_value_int32(env, argv[0], &count));

    g_noise_stop = false;
    g_noise_threads.clear();
    for (int32_t i = 0; i < count; i++)
        g_noise_threads.emplace_back(NoiseLoop);

    return nullptr;
}

napi_value StopNoise(napi_env env, napi_callback_info info)
{
    g_noise_stop = true;
    for (std::thread& t : g_noise_threads)
        t.join();
    g_noise_threads.clear();

    return nullptr;
}

napi_value Churn(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1];
    NODE_API_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    int32_t count = 1000;
    if (argc >= 1)
        NODE_API_CALL(env, napi_get_value_int32(env, argv[0], &count));

    std::vector<void*> blocks;
    blocks.reserve(count);

    for (int32_t i = 0; i < count; i++) {
        size_t size = 64 + (i % 8) * 32;
        void* p = malloc(size);
        if (p != nullptr) {
            memset(p, 0xa5, size);
            blocks.push_back(p);
        }
    }

    for (void* p : blocks)
        free(p);

    return nullptr;
}

} // namespace

NAPI_MODULE_INIT()
{
    napi_property_descriptor properties[] = {
        DECLARE_NODE_API_PROPERTY("queueAndDelete", QueueAndDelete),
        DECLARE_NODE_API_PROPERTY("queueDeletePoison", QueueDeletePoison),
        DECLARE_NODE_API_PROPERTY("cancelAndDelete", CancelAndDelete),
        DECLARE_NODE_API_PROPERTY("createAndDelete", CreateAndDelete),
        DECLARE_NODE_API_PROPERTY("queueDeleteInComplete", QueueDeleteInComplete),
        DECLARE_NODE_API_PROPERTY("startBlocking", StartBlocking),
        DECLARE_NODE_API_PROPERTY("isExecuting", IsExecuting),
        DECLARE_NODE_API_PROPERTY("deleteWork", DeleteWork),
        DECLARE_NODE_API_PROPERTY("releaseWork", ReleaseWork),
        DECLARE_NODE_API_PROPERTY("poison", Poison),
        DECLARE_NODE_API_PROPERTY("startNoise", StartNoise),
        DECLARE_NODE_API_PROPERTY("stopNoise", StopNoise),
        DECLARE_NODE_API_PROPERTY("completeCount", CompleteCount),
        DECLARE_NODE_API_PROPERTY("poisonCompleteCount", PoisonCompleteCount),
        DECLARE_NODE_API_PROPERTY("tailCompleteCount", TailCompleteCount),
        DECLARE_NODE_API_PROPERTY("completeStatus", CompleteStatus),
        DECLARE_NODE_API_PROPERTY("churn", Churn),
    };

    NODE_API_CALL(env, napi_define_properties(env, exports,
                           sizeof(properties) / sizeof(*properties), properties));

    return exports;
}
