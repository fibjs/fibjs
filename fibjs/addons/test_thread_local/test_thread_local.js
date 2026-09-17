/*
 * test_thread_local.js
 *
 *  Runnable reproduction of the fiber / OS thread affinity problem that breaks
 *  N-API addons which cache state in thread-local storage (TLS).
 *
 *  Run it with:
 *
 *      fibjs addons/test_thread_local/test_thread_local.js
 *      fibjs --no-js-thread-affinity addons/test_thread_local/test_thread_local.js
 *
 *  The first form runs with the default JS thread affinity (nothing is lost),
 *  the second reproduces the addon-visible thread-local storage loss.
 */

const path = require('path');
const fs = require('fs');
const coroutine = require('coroutine');

function load_binding() {
    // The addon is produced next to the fibjs binary by `sh build ci`.
    let locations = [
        path.join(path.dirname(process.execPath), 'test_thread_local.node'),
        path.join(__dirname, 'test_thread_local.node'),
    ];

    for (const location of locations) {
        if (fs.exists(location)) {
            const module = { exports: {} };
            process.dlopen(module, location);
            return module.exports;
        }
    }

    throw new Error('test_thread_local.node not found, run `sh build ci` first');
}

const binding = load_binding();

// ---------------------------------------------------------------------------
// 1. Where did module registration run?
// ---------------------------------------------------------------------------
const main_tid = binding.threadId();
const reg_tid = binding.registrationThreadId();

console.log('[1] module registration thread : ' + reg_tid);
console.log('[1] current (main fiber) thread : ' + main_tid);
console.log('[1] module registration count  : ' + binding.registrationCount());

// ---------------------------------------------------------------------------
// 2. TLS survives while the fiber stays on the thread, and is lost as soon as
//    the scheduler resumes the fiber somewhere else.
// ---------------------------------------------------------------------------
function suspend_and_check(iterations) {
    let migrated = 0;
    let lost = 0;
    let first_loss = null;

    for (let i = 0; i < iterations; i++) {
        // A unique marker per iteration, so a value left behind on a worker
        // thread by an earlier iteration cannot hide a loss.
        const marker = 0x5A5A0000 + i;
        const before = binding.threadId();
        binding.resetTls(marker);

        // Suspend the fiber. fibjs resumes it on whichever worker thread of the
        // isolate becomes free first.
        coroutine.sleep(1);

        const after = binding.threadId();
        const value = binding.getTls();

        if (after !== before) {
            migrated++;
            if (value !== marker) {
                lost++;
                if (first_loss === null)
                    first_loss = { iteration: i, from: before, to: after, value: value, expected: marker };
            }
        }
    }

    return { iterations, migrated, lost, first_loss };
}

const tls = suspend_and_check(20);

console.log('');
console.log('[2] thread-local value across coroutine.sleep():');
console.log('    iterations                    : ' + tls.iterations);
console.log('    resumed on another thread     : ' + tls.migrated);
console.log('    value lost after migration    : ' + tls.lost);
if (tls.first_loss)
    console.log('    first loss                    : iteration ' + tls.first_loss.iteration
        + ', t:' + tls.first_loss.from + ' -> t:' + tls.first_loss.to
        + ', read back ' + tls.first_loss.value + ' (expected ' + tls.first_loss.expected + ')');

// ---------------------------------------------------------------------------
// 3. The napi-rs failure mode: the constructor lookup goes through TLS.
// ---------------------------------------------------------------------------
function lookup_from_other_thread(attempts) {
    const results = { ok: 0, failed: 0, first_error: null, threads: {} };
    const observations = [];

    const fibers = [];
    for (let i = 0; i < attempts; i++) {
        fibers.push(coroutine.start(function () {
            // give the scheduler a chance to place us on another worker thread
            coroutine.sleep(1);

            const tid = binding.threadId();
            let error = null;
            try {
                binding.lookupClass('CanvasRenderingContext2D');
            } catch (e) {
                error = e;
            }

            observations.push({ tid: tid, error: error });
        }));
    }
    fibers.forEach(f => f.join());

    for (const observation of observations) {
        if (observation.error === null)
            results.ok++;
        else {
            results.failed++;
            if (results.first_error === null)
                results.first_error = observation.error;
        }
        results.threads[observation.tid] =
            (results.threads[observation.tid] || 0) + 1;
    }

    return results;
}

const lookup = lookup_from_other_thread(24);

console.log('');
console.log('[3] napi-rs style class registry lookup:');
console.log('    attempts                      : 24');
console.log('    succeeded                     : ' + lookup.ok);
console.log('    failed with InvalidArg        : ' + lookup.failed);
console.log('    registration thread           : ' + reg_tid);
console.log('    threads observed              : ' + Object.keys(lookup.threads).join(', '));
if (lookup.first_error)
    console.log('    error                         : ' + lookup.first_error.message
        + ' (code=' + lookup.first_error.code + ')');

// ---------------------------------------------------------------------------
// 4. Async callback paths must observe the same thread-local registry.
// ---------------------------------------------------------------------------
function check_async_callback(trigger) {
    const ev = new coroutine.Event();
    let result = null;

    trigger(function (thread, registry, registry_tid) {
        result = {
            thread: thread,
            registry: registry,
            registry_tid: registry_tid,
        };
        ev.set();
    });

    ev.wait();
    return result;
}

const asyncWork = check_async_callback(binding.runAsync);
const tsfn = check_async_callback(binding.runThreadsafe);

console.log('');
console.log('[4] async callback paths:');
console.log('    async work complete thread    : ' + asyncWork.thread);
console.log('    async work registry           : ' + asyncWork.registry);
console.log('    async work registry thread    : ' + asyncWork.registry_tid);
console.log('    TSFN call_js thread           : ' + tsfn.thread);
console.log('    TSFN registry                 : ' + tsfn.registry);
console.log('    TSFN registry thread          : ' + tsfn.registry_tid);

// ---------------------------------------------------------------------------
// 5. Contrast: process-global state is never affected.
// ---------------------------------------------------------------------------
let global_lost = 0;
for (let i = 0; i < 20; i++) {
    binding.setGlobal(0x1234);
    coroutine.sleep(1);
    if (binding.getGlobal() !== 0x1234)
        global_lost++;
}
console.log('');
console.log('[4] process-global value lost    : ' + global_lost + ' / 20');

// ---------------------------------------------------------------------------
// Summary
// ---------------------------------------------------------------------------
console.log('');
if (tls.lost === 0
    && lookup.failed === 0
    && asyncWork.thread === reg_tid
    && asyncWork.registry === 0x52454743
    && asyncWork.registry_tid === reg_tid
    && tsfn.thread === reg_tid
    && tsfn.registry === 0x52454743
    && tsfn.registry_tid === reg_tid) {
    console.log('RESULT: no thread-local loss observed in this run.');
    console.log('        (retry a few times, the scheduler placement is probabilistic)');
} else {
    console.log('RESULT: thread-local loss reproduced.');
    console.log('        ' + tls.lost + '/' + tls.iterations + ' sleeps lost the TLS value,');
    console.log('        ' + lookup.failed + '/24 class lookups failed with InvalidArg.');
}
