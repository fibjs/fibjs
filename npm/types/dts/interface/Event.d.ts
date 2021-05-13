/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * @description 事件对象
 * 
 * 通过一个事件达到对一组纤程进行控制的目的（事件对象的状态为bool类型）
 * 
 *  
 */
declare class Class_Event extends Class_Lock {
    /**
     * @description 事件对象构造函数
     *      @param value 指定是否等待，为 true 时等待，缺省为 false
     *      
     */
    constructor(value?: boolean);

    /**
     * @description 判断事件对象是否为真
     *      @return 如果事件为真，返回 true
     *      
     */
    isSet(): boolean;

    /**
     * @description 激活事件（将事件状态改为true），并调用pulse() 
     */
    set(): void;

    /**
     * @description 激活等待该事件的所有纤程 
     */
    pulse(): void;

    /**
     * @description 重置事件（将事件状态改为false） 
     */
    clear(): void;

    /**
     * @description 等待一个事件 
     */
    wait(): void;

}

