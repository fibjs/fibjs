/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description PerformanceEntry 接口提供了 performance 记录的通用属性 
 */
declare class Class_PerformanceEntry extends Class_object {
    /**
     * @description performance 记录的名称. 
     */
    readonly name: string;

    /**
     * @description performance 记录的类型. 
     */
    readonly entryType: string;

    /**
     * @description performance 记录的开始时间. 
     */
    readonly startTime: number;

    /**
     * @description performance 记录的持续时间. 
     */
    readonly duration: number;

}

