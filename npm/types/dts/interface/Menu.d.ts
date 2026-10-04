/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/MenuItem.d.ts" />
/**
 * @description Menu management object, used to display a menu in a window
 *
 *   A Menu can be created in the following ways:
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
 *   Or created inline when creating a window:
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
declare class Class_Menu extends Class_object {
    /**
     * @description Appends a menu item, adding a menu item to the menu.
     *      @param item menu item object
     *
     */
    append(item: FIBJS.GeneralObject): void;

    /**
     * @description Inserts a menu item, inserting a menu item at the specified position.
     *      @param pos the index of the insertion position
     *      @param item menu item object
     *
     */
    insert(pos: number, item: FIBJS.GeneralObject): void;

    /**
     * @description Removes a menu item, removing the menu item at the specified position from the menu.
     *      @param pos the index of the menu item to remove
     *
     */
    remove(pos: number): void;

    /**
     * @description Gets the number of menu items
     */
    readonly length: number;

    /**
     * @description Gets a menu item, retrieving the menu item with the specified id from the menu.
     *      @param id the id of the menu item
     *      @return menu item object, or null if not found
     *
     */
    getMenuItemById(id: string): Class_MenuItem;

    /**
     * @description Gets a menu item, retrieving the menu item at the specified index from the menu.
     *      @param index the index of the menu item
     *      @return menu item object
     *
     */
    [index: number]: Class_MenuItem;

}

