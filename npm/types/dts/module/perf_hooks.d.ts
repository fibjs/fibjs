/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/PerformanceObserver.d.ts" />
/// <reference path="../module/performance.d.ts" />
/**
 * @description Node.js compatibility entry point that exposes the performance measurement API
 *
 *  `require('perf_hooks')` returns a module with exactly two members, `performance` and
 *  `PerformanceObserver`; both are the same objects as the globals `performance` and
 *  `PerformanceObserver`, so the module exists only so that code written for Node.js can keep its
 *  `require('perf_hooks')` destructuring.
 *
 *  Concepts:
 *
 *  - **What is shared**: the module object itself is stateless; marks, measures and observers live on
 *    the isolate, so every reference to `performance` (module, global, sandbox) sees the same
 *    timeline.
 *  - **Not provided**: Node.js also exports `Performance`, `PerformanceEntry`, `PerformanceMark`,
 *    `PerformanceMeasure`, `PerformanceObserverEntryList`, `PerformanceResourceTiming`,
 *    `monitorEventLoopDelay`, `eventLoopUtilization`, `timerify`, `createHistogram` and `constants`;
 *    fibjs exposes none of them. The entry classes exist internally but are not reachable by name,
 *    and resource timing is a no-op (see the performance module).
 *
 *  Import:
 *  ```JavaScript
 *  const { performance, PerformanceObserver } = require('perf_hooks');
 *  ```
 *
 *  Example 1 — the module members are the globals:
 *  ```JavaScript
 *  const perf_hooks = require('perf_hooks');
 *
 *  console.log(perf_hooks.performance === global.performance);                 // true
 *  console.log(perf_hooks.PerformanceObserver === global.PerformanceObserver); // true
 *  console.log(Object.keys(perf_hooks).sort().join(', ')); // PerformanceObserver, performance
 *  ```
 *
 *  Example 2 — a minimal Node.js style measurement:
 *  ```JavaScript
 *  const { performance, PerformanceObserver } = require('perf_hooks');
 *
 *  performance.clearMarks();
 *  performance.mark('boot');
 *  performance.mark('ready');
 *  const observer = new PerformanceObserver(() => {});
 *  observer.observe({ entryTypes: ['measure'] });
 *  performance.measure('boot-time', 'boot', 'ready');
 *
 *  const measure = observer.takeRecords()[0];
 *  console.log(measure.name, measure.entryType); // boot-time measure
 *  observer.disconnect();
 *  ```
 *
 */
declare module 'perf_hooks' {
    /**
     * @description The PerformanceObserver class, identical to the global of the same name, see PerformanceObserver
     *
     *      Use it to subscribe to marks and measures as they are recorded:
     *      `new perf_hooks.PerformanceObserver(callback)`.
     *
     */
    const PerformanceObserver: typeof Class_PerformanceObserver;

    /**
     * @description The performance timeline object, identical to the global `performance`, see performance
     *
     *      Carries the `now`, `mark`, `measure` and `getEntries*` members described in the performance
     *      module.
     *
     */
    const performance: typeof import ('performance');

}

