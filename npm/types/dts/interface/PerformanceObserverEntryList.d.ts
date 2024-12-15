/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description PerformanceObserverEntryList 对象包含了 PerformanceObserver 观察到的 performance 记录的详细信息 
 */
declare class Class_PerformanceObserverEntryList extends Class_object {
    /**
     * @description 查询全部 performance 记录的详细信息。
     *      @return PerformanceEntry 对象数组
     *     
     */
    getEntries(): any[];

    /**
     * @description 根据名称查询 performance 记录的详细信息
     *      @param name 一个字符串，表示 performance 记录的名称。
     *      @param entryType 一个字符串，表示 performance 记录的类型。
     *      @return PerformanceEntry 对象数组
     *     
     */
    getEntriesByName(name: string, entryType?: string): any[];

    /**
     * @description 根据类型查询 performance 记录的详细信息
     *      @param entryType 一个字符串，表示 performance 记录的类型。
     *      @return PerformanceEntry 对象数组
     *     
     */
    getEntriesByType(entryType: string): any[];

}

