/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/PerformanceEntry.d.ts" />
/**
 * @description The PerformanceObserverEntryList object contains the detailed information of the performance entries observed by the PerformanceObserver
 */
declare class Class_PerformanceObserverEntryList extends Class_object {
    /**
     * @description Queries the detailed information of all performance entries.
     *      @return an array of PerformanceEntry objects
     *
     */
    getEntries(): Class_PerformanceEntry[];

    /**
     * @description Queries the detailed information of performance entries by name
     *      @param name a string, representing the name of the performance entry.
     *      @param entryType a string, representing the type of the performance entry.
     *      @return an array of PerformanceEntry objects
     *
     */
    getEntriesByName(name: string, entryType?: string): Class_PerformanceEntry[];

    /**
     * @description Queries the detailed information of performance entries by type
     *      @param entryType a string, representing the type of the performance entry.
     *      @return an array of PerformanceEntry objects
     *
     */
    getEntriesByType(entryType: string): Class_PerformanceEntry[];

}

