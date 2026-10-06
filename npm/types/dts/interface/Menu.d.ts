/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/MenuItem.d.ts" />
/**
 * @description Ordered list of MenuItem objects, shown as a window menu bar or a tray menu
 *
 *  A Menu is created from a template array with `gui.createMenu`, or implicitly by
 *  passing a template array or Menu object in the `menu` option of `gui.open`,
 *  `gui.openFile` or `gui.createTray`. The window or tray keeps the resulting
 *  object: `WebView#getMenu` and `Tray#getMenu` return the same instance that was
 *  passed in.
 *
 *  Concepts:
 *
 *  - **Hierarchy**: a Menu is a flat list of MenuItem objects. A submenu item owns
 *    the nested Menu in its `submenu` property, so any depth is expressed with
 *    nested descriptors; `length` and `operator[]` address the top level only.
 *  - **Templates**: every element of a template is a descriptor object or an
 *    existing MenuItem; the accepted properties and validation rules are described
 *    in MenuItem.
 *  - **Attachment**: the native menu is built when the object is first attached to
 *    a window or tray, and `append`, `insert` and `remove` then throw
 *    "Menu: Menu is already attached to a window" (the check is on the native
 *    handle, so tray menus behave the same). Build the whole menu before attaching
 *    it.
 *  - **Lookup**: `getMenuItemById` checks the top-level items in order and then
 *    recurses into every submenu, so an id anywhere in the tree can be reached from
 *    the root menu.
 *
 *  Example 1 — build and edit a menu:
 *  ```JavaScript
 *  const gui = require('gui');
 *
 *  const menu = gui.createMenu([
 *      { id: 'file', label: 'File' },
 *      { id: 'edit', label: 'Edit' }
 *  ]);
 *
 *  menu.append({ id: 'help', label: 'Help' });
 *  menu.insert(1, { id: 'view', label: 'View' });
 *  menu.remove(0);
 *
 *  console.log(menu.length); // 3
 *  console.log(menu[0].label + ' ' + menu[1].label + ' ' + menu[2].label); // View Edit Help
 *  console.log(menu.getMenuItemById('view').label); // View
 *  ```
 *
 *  Example 2 — attach a menu to a window and read it back (requires a desktop session):
 *  ```JavaScript
 *  // requires: long-running
 *  const gui = require('gui');
 *
 *  const menu = gui.createMenu([{ label: 'File', submenu: [{ label: 'Quit' }] }]);
 *  const win = gui.open({ width: 320, height: 200, menu: menu });
 *
 *  console.log(win.getMenu() === menu); // true
 *  win.waitFor();
 *  ```
 *
 */
declare class Class_Menu extends Class_object {
    /**
     * @description Appends an item at the end of the menu
     *
     *      item is a descriptor object or an existing MenuItem. The length and all item
     *      indexes are updated immediately. Throws when the menu is already attached to
     *      a window or tray, and when the descriptor is invalid (see MenuItem).
     *
     *      Example — build a menu with append:
     *      ```JavaScript
     *      const gui = require('gui');
     *
     *      const menu = gui.createMenu();
     *
     *      menu.append({ label: 'First' });
     *      menu.append({ label: 'Second' });
     *
     *      console.log(menu.length); // 2
     *      console.log(menu[1].label); // Second
     *      ```
     *
     *      @param item menu item object
     *
     */
    append(item: FIBJS.GeneralObject): void;

    /**
     * @description Inserts an item at the given position
     *
     *      pos must be between 0 and length (inclusive); values outside this range
     *      throw RangeError 20006. Throws when the menu is already attached to a window
     *      or tray, and when the descriptor is invalid (see MenuItem).
     *
     *      @param pos the index of the insertion position
     *      @param item menu item object
     *
     */
    insert(pos: number, item: FIBJS.GeneralObject): void;

    /**
     * @description Removes the item at the given position
     *
     *      pos must be between 0 and length - 1; values outside this range throw
     *      RangeError 20006. Throws when the menu is already attached to a window or
     *      tray; disable the entry of an attached menu with `item.enabled` instead.
     *
     *      @param pos the index of the menu item to remove
     *
     */
    remove(pos: number): void;

    /**
     * @description Number of items at the top level of the menu
     *
     *      The contents of submenus are not counted; use `menu[i].submenu.length` for a
     *      nested menu.
     *
     */
    readonly length: number;

    /**
     * @description Returns the first item with the given id, or null when no item matches
     *
     *      The search is depth-first: each top-level item is checked before its submenu
     *      and before the next sibling. Ids default to an empty string and are not
     *      required to be unique.
     *
     *      Example — find an item inside a submenu:
     *      ```JavaScript
     *      const gui = require('gui');
     *
     *      const menu = gui.createMenu([
     *          { id: 'file', label: 'File' },
     *          { label: 'Edit', submenu: [{ id: 'copy', label: 'Copy' }] }
     *      ]);
     *
     *      console.log(menu.getMenuItemById('copy').label); // Copy
     *      console.log(menu.getMenuItemById('missing')); // null
     *      ```
     *
     *      @param id the id of the menu item
     *      @return menu item object, or null if not found
     *
     */
    getMenuItemById(id: string): Class_MenuItem;

    /**
     * @description Returns the item at the given index
     *
     *      index must be less than length; an integer outside this range throws
     *      RangeError 20006. Negative indexes and other properties are ordinary
     *      JavaScript properties rather than menu items, so they read as undefined.
     *
     *      @return menu item at the given index
     *
     */
    [index: number]: Class_MenuItem;

}

