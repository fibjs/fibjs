/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/PerformanceEntry.d.ts" />
/**
 * @description Provides the performance timeline API: a monotonic clock plus named marks and measures for measuring and instrumenting application timings
 *
 *  Main capabilities:
 *
 *  - **Clock**: `now` reads the monotonic high-resolution clock;
 *  - **Marks**: `mark` records a named timestamp and `clearMarks` removes them;
 *  - **Measures**: `measure` computes a duration; `clearMeasures` is a Node.js compatibility no-op;
 *  - **Timeline queries**: `getEntries`, `getEntriesByType` and `getEntriesByName` read the marks;
 *  - **Observation**: `PerformanceObserver` receives marks and measures as they are recorded;
 *  - **Resource timing**: `markResourceTiming` is a compatibility stub that records nothing.
 *
 *  The module is available as `require('perf_hooks').performance` and as the global `performance`;
 *  both names refer to the same object.
 *
 *  Concepts:
 *
 *  - **Monotonic clock**: `now()` returns the milliseconds elapsed since the fibjs runtime started,
 *    as a floating-point number with sub-millisecond resolution. It never goes backwards and has no
 *    relation to the wall clock (use Date for calendar time); only a difference between two readings
 *    is meaningful. Node.js additionally provides `timeOrigin`, `toJSON`, `nodeTiming` and
 *    `eventLoopUtilization`, none of which exist here.
 *  - **Entry types**: only `mark` and `measure` entries are ever produced; there is no navigation,
 *    resource, paint or user-timing buffer, and an observer registered for another type never fires.
 *  - **Marks**: a mark is a named timestamp that can carry an arbitrary `detail` and whose
 *    `startTime` can be overridden. Marks are keyed by name, so marking the same name again replaces
 *    the previous entry (Node.js keeps both).
 *  - **Measures**: a measure is a computed duration derived from two mark names, from explicit
 *    numbers, or from `start` plus `duration`. Measures are **not stored** in the timeline:
 *    `getEntries`/`getEntriesByType`/`getEntriesByName` only ever return marks and `clearMeasures`
 *    does nothing; a measure reaches the program exclusively through a PerformanceObserver. Node.js
 *    retains measures and can query them.
 *  - **Observers vs polling**: `getEntries*` is a snapshot of the marks recorded so far, while an
 *    observer is notified about new entries. Records are queued synchronously, so `takeRecords()`
 *    can drain them before the callback runs; the callback itself runs when the current fiber yields.
 *  - **Profiling**: this module only records user-defined marks and measures. For CPU profiling use
 *    the `--prof` family of CLI options; for heap inspection use the v8 module.
 *
 *  Import:
 *  ```JavaScript
 *  const { performance, PerformanceObserver } = require('perf_hooks');
 *  // the same objects are available as the globals `performance` and `PerformanceObserver`
 *  ```
 *
 *  Example 1 — time a synchronous section with marks:
 *  ```JavaScript
 *  const { performance } = require('perf_hooks');
 *
 *  performance.clearMarks();
 *  performance.mark('section-start');
 *  let sum = 0;
 *  for (let i = 0; i < 100000; i++)
 *      sum += i;
 *  performance.mark('section-end', { detail: sum });
 *  performance.measure('section', 'section-start', 'section-end');
 *
 *  const entry = performance.getEntriesByName('section-end')[0];
 *  console.log(entry.entryType, entry.duration, entry.detail); // mark 0 4999950000
 *  console.log(performance.getEntriesByType('measure').length); // 0, measures are not stored
 *  ```
 *
 *  Example 2 — compute a measure from explicit timestamps and observe it:
 *  ```JavaScript
 *  const { performance, PerformanceObserver } = require('perf_hooks');
 *
 *  performance.clearMarks();
 *  const observer = new PerformanceObserver(() => {});
 *  observer.observe({ entryTypes: ['measure'] });
 *
 *  performance.measure('startup', { start: 0, duration: 12.5, detail: { phase: 'boot' } });
 *
 *  const measure = observer.takeRecords()[0];
 *  console.log(measure.name, measure.entryType); // startup measure
 *  console.log(measure.startTime, measure.duration); // 0 12.5
 *  console.log(measure.detail.phase); // boot
 *  observer.disconnect();
 *  ```
 *
 *  Example 3 — receive entries in the observer callback:
 *  ```JavaScript
 *  const { performance, PerformanceObserver } = require('perf_hooks');
 *
 *  (async () => {
 *      performance.clearMarks();
 *      let names = [];
 *      const observer = new PerformanceObserver((list) => {
 *          names = list.getEntries().map((entry) => entry.name);
 *      });
 *      observer.observe({ entryTypes: ['mark', 'measure'] });
 *
 *      performance.mark('request-start');
 *      performance.measure('request', 'request-start', 'request-start');
 *
 *      // the callback runs when the current fiber yields
 *      await new Promise((resolve) => setTimeout(resolve, 0));
 *      console.log(names.join(', ')); // request-start, request
 *      observer.disconnect();
 *  })();
 *  ```
 *
 *  Notes:
 *
 *  - The timeline is per isolate: marks recorded inside a worker are not visible to the main isolate.
 *  - getEntries returns the marks in an unspecified order; sort the array when a stable order is
 *    needed.
 *  - A missing mark in measure fails with Error [20024] and messages such as
 *    "startMark 'x' not found"; Node.js throws a DOMException instead.
 *
 */
declare module 'performance' {
    /**
     * @description Clears all marks, or a single named mark
     *
     *      Clears the mark timeline; the same names can be marked again afterwards, and clearing a name
     *      that does not exist is a no-op. Measures already delivered to observers are unaffected, but a
     *      measure that references a cleared mark can no longer be created.
     *
     *      @param name the mark name; if empty, clears all marks
     *
     */
    function clearMarks(name?: string): void;

    /**
     * @description Compatibility no-op kept for Node.js API parity
     *
     *      fibjs does not retain measures in the timeline, so there is nothing to remove and the call
     *      always returns undefined. It exists so code written for Node.js, where the member clears
     *      named or anonymous measures, keeps working; observer delivery is not affected.
     *
     *      @param name the measure name; ignored
     *
     */
    function clearMeasures(name?: string): void;

    /**
     * @description Records a named timestamp in the performance timeline
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "startTime": 0,   // the timestamp to record in ms; defaults to performance.now()
     *          "detail": null    // any value attached to the mark; defaults to undefined
     *      });
     *      ```
     *
     *      Marking a name that already exists replaces the previous entry, so the timeline holds at most
     *      one mark per name. The mark is observable through getEntries/getEntriesByType/getEntriesByName
     *      and is delivered to observers registered for the `mark` entry type. `startTime` must be a
     *      number: a string or boolean fails with TypeError [20005], while null is treated as absent.
     *
     *      Example — mark a phase with a custom offset and detail:
     *      ```JavaScript
     *      const { performance } = require('perf_hooks');
     *
     *      performance.clearMarks();
     *      performance.mark('cache-warm', { startTime: 42.5, detail: { entries: 3 } });
     *
     *      const mark = performance.getEntriesByName('cache-warm')[0];
     *      console.log(mark.entryType, mark.startTime, mark.duration); // mark 42.5 0
     *      console.log(mark.detail.entries); // 3
     *      ```
     *
     *      @param name the mark name
     *      @param options the additional options
     *
     */
    function mark(name: string, options?: FIBJS.GeneralObject): void;

    /**
     * @description Creates a measure from options, or between two mark names
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "start": 0,       // a mark name or a timestamp in ms; defaults to 0
     *          "end": null,      // a mark name or a timestamp in ms; defaults to performance.now()
     *          "duration": null, // the duration in ms; when given, end must be omitted
     *          "detail": null    // any value attached to the measure; defaults to undefined
     *      });
     *      ```
     *
     *      The second overload, measure(name, startMark, endMark), takes mark names directly: an empty
     *      startMark means timestamp 0 and an empty endMark means the current time. Both overloads
     *      compute duration as end - start; a negative duration is accepted (Node.js rejects an
     *      explicitly negative `duration` option and keeps end < start as negative). A string start or
     *      end that names a missing mark fails with Error [20024] and "startMark 'x' not found" or
     *      "endMark 'x' not found"; `duration` together with `end` fails with "end must not be specified
     *      when duration is specified"; a start or end that is neither string nor number fails with
     *      "start must be a string or number" or "end must be a string or number". Combining a
     *      mark-name `start` with `duration` is a known deviation: the end is computed before the mark
     *      is resolved, so the recorded duration is `duration - mark.startTime`; use `end` with a mark
     *      name, or a numeric `start`, instead.
     *
     *      Measures are not stored in the timeline: they are delivered only to PerformanceObserver
     *      instances registered for `measure` and never appear in getEntries/getEntriesByType/
     *      getEntriesByName.
     *
     *      Example — measure a span between two marks with an explicit detail:
     *      ```JavaScript
     *      const { performance, PerformanceObserver } = require('perf_hooks');
     *
     *      performance.clearMarks();
     *      performance.mark('begin', { startTime: 10 });
     *      performance.mark('end', { startTime: 15 });
     *      const observer = new PerformanceObserver(() => {});
     *      observer.observe({ type: 'measure' });
     *
     *      performance.measure('slice', { start: 'begin', end: 'end', detail: 'fast' });
     *
     *      const measure = observer.takeRecords()[0];
     *      console.log(measure.name, measure.startTime, measure.duration); // slice 10 5
     *      console.log(measure.detail); // fast
     *      observer.disconnect();
     *      ```
     *
     *      @param name the measure name
     *      @param options the additional options, or the start mark name in the string overload
     *
     */
    function measure(name: string, options?: FIBJS.GeneralObject): void;

    /**
     * @description Creates a measure between two mark names
     *
     *      Calls measure(name, startMark, endMark) with mark names: both marks must exist in the timeline
     *      or the call fails with Error [20024]. An empty startMark is interpreted as timestamp 0 and an
     *      empty endMark as the current time, mirroring the first overload; see the options overload for
     *      the full duration rules and error messages.
     *
     *      @param name the measure name
     *      @param startMark the start mark name; if empty, uses timestamp 0
     *      @param endMark the end mark name; if empty, uses the current time
     *
     */
    function measure(name: string, startMark?: string, endMark?: string): void;

    /**
     * @description Returns a snapshot of the marks in the timeline
     *
     *      The result is a new array with one PerformanceMark per name; measures are never included, so
     *      only clearing the marks empties it. The underlying collection is unordered: sort the array
     *      when a stable order is needed. The entries are the same objects that observers receive.
     *
     *      Example — list the current marks in a stable order:
     *      ```JavaScript
     *      const { performance } = require('perf_hooks');
     *
     *      performance.clearMarks();
     *      performance.mark('a');
     *      performance.mark('b');
     *      console.log(performance.getEntries().map((entry) => entry.name).sort().join(', ')); // a, b
     *      ```
     *
     *      @return returns all performance entries
     */
    function getEntries(): Class_PerformanceEntry[];

    /**
     * @description Returns the marks matching an entry type
     *
     *      The only type that can match is `mark`, because measures are not retained: `measure` or any
     *      unknown type returns an empty array without raising an error.
     *
     *      Example — `mark` matches while `measure` is always empty:
     *      ```JavaScript
     *      const { performance } = require('perf_hooks');
     *
     *      performance.clearMarks();
     *      performance.mark('tick');
     *      console.log(performance.getEntriesByType('mark').length);    // 1
     *      console.log(performance.getEntriesByType('measure').length); // 0
     *      ```
     *
     *      @param type the entry type
     *      @return returns all performance entries
     */
    function getEntriesByType(type: string): Class_PerformanceEntry[];

    /**
     * @description Returns the marks matching a name and an optional entry type
     *
     *      Filters the timeline by name; when type is given it must match too. Mark names are unique in
     *      the timeline, so the result holds at most one entry.
     *
     *      Example — look a mark up by name, with and without a type filter:
     *      ```JavaScript
     *      const { performance } = require('perf_hooks');
     *
     *      performance.clearMarks();
     *      performance.mark('load', { startTime: 3 });
     *
     *      console.log(performance.getEntriesByName('load').length);            // 1
     *      console.log(performance.getEntriesByName('load', 'mark').length);    // 1
     *      console.log(performance.getEntriesByName('load', 'measure').length); // 0
     *      ```
     *
     *      @param name the entry name
     *      @param type the entry type
     *      @return returns all performance entries
     */
    function getEntriesByName(name: string, type?: string): Class_PerformanceEntry[];

    /**
     * @description Compatibility stub of the Node.js resource timing hook
     *
     *      Accepts the Node.js markResourceTiming(timingInfo, requestedUrl, initiatorType, global,
     *      cacheState, bodyInfo, responseStatus) call and does nothing: no resource entry is created and
     *      observers registered for `resource` never fire. The arguments are still type-checked, so all
     *      seven must be supplied.
     *
     *      Example — the call is accepted and records nothing:
     *      ```JavaScript
     *      const { performance } = require('perf_hooks');
     *
     *      const before = performance.getEntries().length;
     *      performance.markResourceTiming({}, 'https://example.com/', 'fetch', global, 'local', {}, 200);
     *      console.log(performance.getEntries().length - before); // 0
     *      ```
     *
     *      @param timingInfo the timing information object
     *      @param requestedUrl the requested URL
     *      @param initiatorType the initiator type
     *      @param global the global object; any value is accepted and ignored
     *      @param cacheState the cache state
     *      @param bodyInfo the request body information
     *      @param responseStatus the response status code
     *
     */
    function markResourceTiming(timingInfo: FIBJS.GeneralObject, requestedUrl: string, initiatorType: string, global: any, cacheState: string, bodyInfo: FIBJS.GeneralObject, responseStatus: number): void;

    /**
     * @description Reads the monotonic high-resolution clock
     *
     *      Returns the milliseconds elapsed since the fibjs runtime started as a floating-point number
     *      with sub-millisecond resolution. The reading is monotonic and unrelated to the wall clock: use
     *      Date for calendar time and compare two readings to measure a duration. The first reading of a
     *      process is a small positive number, not an epoch timestamp.
     *
     *      Example — measure an interval between two readings:
     *      ```JavaScript
     *      const { performance } = require('perf_hooks');
     *
     *      const start = performance.now();
     *      const end = performance.now();
     *      console.log(typeof start, end >= start); // number true
     *      ```
     *
     *      @return returns the current process time
     */
    function now(): number;

}

