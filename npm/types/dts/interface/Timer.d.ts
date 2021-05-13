/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 定时器处理器对象 
 */
declare class Class_Timer extends Class_object {
    /**
     * @description 维持 fibjs 进程不退出，在定时器等待期间阻止 fibjs 进程退出
     *      @return 返回定时器对象
     *     
     */
    ref(): Class_Timer;

    /**
     * @description 允许 fibjs 进程退出，在定时器等待期间允许 fibjs 进程退出
     *      @return 返回定时器对象
     *     
     */
    unref(): Class_Timer;

    /**
     * @description 取消当前定时器 
     */
    clear(): void;

    /**
     * @description 查询当前定时器是否已经终止 
     */
    readonly stopped: boolean;

}

