/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/PerformanceEntry.d.ts" />
/**
 * @description performance basic performance monitoring module
 *
 *  Reference method:
 *  ```JavaScript
 *  var performance = require('perf_hooks').performance;
 *  ```
 *
 */
declare module 'performance' {
    /**
     * @description Clears all performance marks
     *      @param name the mark name; if empty, clears all marks
     *
     */
    function clearMarks(name?: string): void;

    /**
     * @description Clears all performance measures
     *      @param name the measure name; if empty, clears all measures
     *
     */
    function clearMeasures(name?: string): void;

    /**
     * @description Creates a performance mark
     *
     *      options is an object containing the following properties:
     *       - detail: additional information
     *       - startTime: start time; if empty, uses the current time
     *
     *      @param name the mark name
     *      @param options the additional options
     *
     */
    function mark(name: string, options?: FIBJS.GeneralObject): void;

    /**
     * @description Creates a performance measure
     *
     *      options is an object containing the following properties:
     *       - detail: additional information
     *       - duration: the duration
     *       - end: if of Number type, represents the end time; if of String type, represents the end mark name
     *       - start: if of Number type, represents the start time; if of String type, represents the start mark name
     *
     *      @param name the measure name
     *      @param options the additional options
     *
     */
    function measure(name: string, options?: FIBJS.GeneralObject): void;

    /**
     * @description Creates a performance measure
     *      @param name the measure name
     *      @param startMark the start mark name; if empty, uses the process start time
     *      @param endMark the end mark name; if empty, uses the current time
     *
     */
    function measure(name: string, startMark?: string, endMark?: string): void;

    /**
     * @description Gets all performance entries
     *      @return returns all performance entries
     */
    function getEntries(): Class_PerformanceEntry[];

    /**
     * @description Gets all performance entries
     *      @param type the entry type
     *      @return returns all performance entries
     */
    function getEntriesByType(type: string): Class_PerformanceEntry[];

    /**
     * @description Gets all performance entries
     *      @param name the entry name
     *      @param type the entry type
     *      @return returns all performance entries
     */
    function getEntriesByName(name: string, type?: string): Class_PerformanceEntry[];

    /**
     * @description Marks resource timing (compatibility no-op)
     *      @param timingInfo the timing information object
     *      @param requestedUrl the requested URL
     *      @param initiatorType the initiator type
     *      @param global the global object
     *      @param cacheState the cache state
     *      @param bodyInfo the request body information
     *      @param responseStatus the response status code
     *
     */
    function markResourceTiming(timingInfo: any, requestedUrl: string, initiatorType: string, global: any, cacheState: string, bodyInfo: any, responseStatus: number): void;

    /**
     * @description Queries the current process time
     *      @return returns the current process time
     */
    function now(): number;

}

