/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The PerformanceEntry interface provides the common properties of performance entries
 */
declare class Class_PerformanceEntry extends Class_object {
    /**
     * @description The name of the performance entry.
     */
    readonly name: string;

    /**
     * @description The type of the performance entry.
     */
    readonly entryType: string;

    /**
     * @description The start time of the performance entry.
     */
    readonly startTime: number;

    /**
     * @description The duration of the performance entry.
     */
    readonly duration: number;

}

