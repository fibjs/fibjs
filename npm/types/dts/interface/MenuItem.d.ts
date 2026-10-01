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
     */
    on(event: "click", listener: ()=>void): this;

}

