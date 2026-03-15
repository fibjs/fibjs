/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description DOMEvent 表示一个 W3C DOM 事件对象
 * 
 *  DOMEvent 实现了标准的 Web Event 接口，提供事件类型、冒泡、取消等标准事件属性。
 * 
 *  ```JavaScript
 *  const ev = new Event('click', { bubbles: true, cancelable: true });
 *  console.log(ev.type);       // 'click'
 *  console.log(ev.bubbles);    // true
 *  console.log(ev.cancelable); // true
 *  ```
 *  
 */
declare class Class_DOMEvent extends Class_object {
    /**
     * @description DOMEvent 构造函数
     *      @param type 事件类型
     *      @param eventInitDict 可选的事件初始化字典
     *      
     */
    constructor(type: string, eventInitDict?: FIBJS.GeneralObject);

    /**
     * @description 事件类型 
     */
    readonly type: string;

    /**
     * @description 事件是否冒泡 
     */
    readonly bubbles: boolean;

    /**
     * @description 事件是否可取消 
     */
    readonly cancelable: boolean;

    /**
     * @description 事件是否可穿越 Shadow DOM 边界 
     */
    readonly composed: boolean;

    /**
     * @description 是否已调用 preventDefault() 
     */
    readonly defaultPrevented: boolean;

    /**
     * @description 事件目标 
     */
    readonly target: any;

    /**
     * @description 当前事件目标 
     */
    readonly currentTarget: any;

    /**
     * @description 事件创建时间戳 
     */
    readonly timeStamp: number;

    /**
     * @description 阻止事件的进一步传播 
     */
    stopPropagation(): void;

    /**
     * @description 阻止同一事件的其他监听器被调用 
     */
    stopImmediatePropagation(): void;

    /**
     * @description 如果事件可取消，则取消该事件 
     */
    preventDefault(): void;

}

