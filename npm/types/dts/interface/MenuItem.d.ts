/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/**
 * @description Menu item interface, inherits from EventEmitter.
 */
declare class Class_MenuItem extends Class_EventEmitter {
    /**
     * @description Unique identifier of the menu item.
     */
    id: string;

    /**
     * @description Type of the menu item.
     */
    readonly type: string;

    /**
     * @description Icon of the menu item.
     */
    icon: string;

    /**
     * @description Label of the menu item.
     */
    label: string;

    /**
     * @description Tooltip of the menu item.
     */
    tooltip: string;

    /**
     * @description Whether the menu item is enabled.
     */
    enabled: boolean;

    /**
     * @description Whether the menu item is checked.
     */
    checked: boolean;

    /**
     * @description Submenu.
     */
    readonly submenu: Class_Menu;

    /**
     * @description Click event handler of the menu item.
     *      @param ev the event object, carrying the clicked menu item
     *
     */
    on(event: "click", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "click", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "click", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "click", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "click", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "click", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "click", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "click", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "click", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Click event handler of the menu item.
     *      @param ev the event object, carrying the clicked menu item
     *
     */
    onclick: ((ev: FIBJS.GeneralObject)=>void) | null;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}

