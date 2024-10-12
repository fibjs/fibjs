/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/MenuItem.d.ts" />
/**
 * @description 菜单管理对象，用于窗口显示菜单
 * 
 *   Menu 可以使用以下方式创建：
 *   ```JavaScript
 *     var menu = gui.createMenu([
 *         { label: 'File', submenu: [
 *             { label: 'New', onclick: function() { console.log('New clicked'); } },
 *             { label: 'Open', onclick: function() { console.log('Open clicked'); } },
 *             { label: 'Save', onclick: function() { console.log('Save clicked'); } },
 *             { label: 'Save As', onclick: function() { console.log('Save As clicked'); } },
 *             { label: 'Close', onclick: function() { console.log('Close clicked'); } }
 *         ] },
 *         { label: 'Edit', submenu: [
 *             { label: 'Undo', onclick: function() { console.log('Undo clicked'); } },
 *             { label: 'Redo', onclick: function() { console.log('Redo clicked'); } },
 *             { type: 'separator' },
 *             { label: 'Cut', onclick: function() { console.log('Cut clicked'); } },
 *             { label: 'Copy', onclick: function() { console.log('Copy clicked'); } },
 *             { label: 'Paste', onclick: function() { console.log('Paste clicked'); } }
 *         ] },
 *         { label: 'Help', submenu: [
 *             { label: 'About', onclick: function() { console.log('About clicked'); } }
 *         ] }
 *     ]);
 *    ```
 * 
 *   或者在创建窗口时内置创建：
 *   ```JavaScript
 *     var win = gui.open({
 *         url: 'http://fibjs.org',
 *         menu: [
 *             { label: 'File', submenu: [
 *                 { label: 'New', onclick: function() { console.log('New clicked'); } },
 *                 { label: 'Open', onclick: function() { console.log('Open clicked'); } },
 *                 { label: 'Save', onclick: function() { console.log('Save clicked'); } },
 *                 { label: 'Save As', onclick: function() { console.log('Save As clicked'); } },
 *                 { label: 'Close', onclick: function() { console.log('Close clicked'); } }
 *             ] },
 *             { label: 'Edit', submenu: [
 *                 { label: 'Undo', onclick: function() { console.log('Undo clicked'); } },
 *                 { label: 'Redo', onclick: function() { console.log('Redo clicked'); } },
 *                 { type: 'separator' },
 *                 { label: 'Cut', onclick: function() { console.log('Cut clicked'); } },
 *                 { label: 'Copy', onclick: function() { console.log('Copy clicked'); } },
 *                 { label: 'Paste', onclick: function() { console.log('Paste clicked'); } }
 *             ] },
 *             { label: 'Help', submenu: [
 *                 { label: 'About', onclick: function() { console.log('About clicked'); } }
 *             ] }
 *         ]
 *     });         
 *   ```
 *  
 */
declare class Class_Menu extends Class_EventEmitter {
    /**
     * @description 添加菜单项，将一个菜单项添加到菜单中。
     *      @param item 菜单项对象
     *      
     */
    append(item: FIBJS.GeneralObject): void;

    /**
     * @description 插入菜单项，在指定位置插入一个菜单项。
     *      @param pos 插入位置的索引
     *      @param item 菜单项对象
     *      
     */
    insert(pos: number, item: FIBJS.GeneralObject): void;

    /**
     * ! @rief 移除菜单项，从菜单中移除指定位置的菜单项。
     *      @param pos 要移除的菜单项的索引
     *      
     */
    remove(pos: number): void;

    /**
     * @description 获取菜单项数量 
     */
    readonly length: number;

    /**
     * @description 获取菜单项，通过 id 获取菜单中的菜单项。
     *      @param id 菜单项的 id
     *      @return 菜单项对象，如果没有找到返回 null
     *      
     */
    getMenuItemById(id: string): Class_MenuItem;


}

