/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/PerformanceObserverEntryList.d.ts" />
/// <reference path="../interface/PerformanceEntry.d.ts" />
/**
 * @description The PerformanceObserver interface is an interface for observing performance entries
 *
 *  The PerformanceObserver interface is an interface for observing performance entries. It allows you to register a callback function, which is called when a new performance entry is added to the performance buffer of the browser. You can use the PerformanceObserver interface to observe specific types of performance entries, such as resource load time, user input delay, etc.
 *
 */
declare class Class_PerformanceObserver extends Class_object {
    /**
     * @description Constructor
     *
     *      @param callback the callback function called when a new performance entry is added to the performance buffer of the browser
     *
     */
    constructor(callback: (list: Class_PerformanceObserverEntryList)=>void);

    /**
     * @description Registers the observed resource types
     *
     *      options is an object containing the following properties:
     *        - type: the observed resource type
     *        - entryTypes: the list of observed resource types
     *
     *      @param options the observed resource types
     *
     */
    observe(options: FIBJS.GeneralObject): void;

    /**
     * @description Unregisters the observed resource types
     */
    disconnect(): void;

    /**
     * @description Gets the entries of the observed resource types
     *      @return returns the entries of the observed resource types
     *
     */
    takeRecords(): Class_PerformanceEntry[];

}

