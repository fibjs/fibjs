/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/**
 * @description 系统状态图标，用于在系统托盘中显示一个图标 
 */
declare class Class_Tray extends Class_EventEmitter {
    /**
     * @description 查询状态图标的菜单
     *      @return 返回状态图标的菜单
     *     
     */
    getMenu(): Class_Menu;

    /**
     * @description 关闭状态图标 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

}

