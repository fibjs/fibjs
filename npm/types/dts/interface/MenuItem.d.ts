/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/**
 * @description 菜单项接口，继承自 EventEmitter。 
 */
declare class Class_MenuItem extends Class_EventEmitter {
    /**
     * @description 菜单项的唯一标识符。 
     */
    id: string;

    /**
     * @description 菜单项的类型。 
     */
    readonly type: string;

    /**
     * @description 菜单项的图标。 
     */
    icon: string;

    /**
     * @description 菜单项的标签。 
     */
    label: string;

    /**
     * @description 菜单项的提示信息。 
     */
    tooltip: string;

    /**
     * @description 菜单项是否启用。 
     */
    enabled: boolean;

    /**
     * @description 菜单项是否被选中。 
     */
    checked: boolean;

    /**
     * @description 子菜单。 
     */
    readonly submenu: Class_Menu;

    /**
     * @description 菜单项的点击事件处理函数。 
     */
    onclick: (...args: any[])=>any;

}

