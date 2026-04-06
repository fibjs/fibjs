/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description performance 基础性能监控模块
 * 
 *  引用方法：
 *  ```JavaScript
 *  var performance = require('perf_hooks').performance;
 *  ```
 *  
 */
declare module 'performance' {
    /**
     * @description 清除所有性能标记
     *      @param name 标记名称，如果为空则清除所有标记
     *      
     */
    function clearMarks(name?: string): void;

    /**
     * @description 清除所有性能测量
     *      @param name 测量名称，如果为空则清除所有测量
     *      
     */
    function clearMeasures(name?: string): void;

    /**
     * @description 创建一个性能标记
     * 
     *      options 为一个对象，包含以下属性：
     *       - detail: 附加信息
     *       - startTime: 开始时间，如果为空则使用当前时间
     * 
     *      @param name 标记名称
     *      @param options 附加选项
     *     
     */
    function mark(name: string, options?: FIBJS.GeneralObject): void;

    /**
     * @description 创建一个性能测量
     *      @param name 测量名称
     *      @param startMark 开始标记名称，如果为空则使用进程起始时间
     *      @param endMark 结束标记名称，如果为空则使用当前时间
     *     
     */
    function measure(name: string, startMark?: string, endMark?: string): void;

    /**
     * @description 创建一个性能测量
     * 
     *      options 为一个对象，包含以下属性：
     *       - detail: 附加信息
     *       - duration: 持续时间
     *       - end: 如果为 Number 类型，则表示结束时间，如果是 String 类型，则表示结束标记名称
     *       - start: 如果为 Number 类型，则表示开始时间，如果是 String 类型，则表示开始标记名称
     * 
     *      @param name 测量名称
     *      @param options 附加选项
     *     
     */
    function measure(name: string, options?: FIBJS.GeneralObject): void;

    /**
     * @description 获取所有性能记录
     *      @return 返回所有性能记录
     */
    function getEntries(): any[];

    /**
     * @description 获取所有性能记录
     *      @param type 记录类型
     *      @return 返回所有性能记录
     */
    function getEntriesByType(type: string): any[];

    /**
     * @description 获取所有性能记录
     *      @param name 记录名称
     *      @param type 记录类型
     *      @return 返回所有性能记录
     */
    function getEntriesByName(name: string, type?: string): any[];

    /**
     * @description 标记资源时间（兼容性空实现）
     *      @param timingInfo 时间信息对象
     *      @param requestedUrl 请求的 URL
     *      @param initiatorType 发起者类型
     *      @param global 全局对象
     *      @param cacheState 缓存状态
     *      @param bodyInfo 请求体信息
     *      @param responseStatus 响应状态码
     *      
     */
    function markResourceTiming(timingInfo: any, requestedUrl: string, initiatorType: string, global: any, cacheState: string, bodyInfo: any, responseStatus: number): void;

    /**
     * @description 查询当前进程时间 
     *      @return 返回当前进程时间
     */
    function now(): number;

}

