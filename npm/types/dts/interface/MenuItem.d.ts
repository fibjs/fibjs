/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/**
 * @description Menu item, one row of a native Menu, created from a plain object descriptor
 *
 *  A menu template is written as plain objects; fibjs converts every object to a
 *  MenuItem when the menu is created, so MenuItem instances are never constructed
 *  directly (`new MenuItem()` throws "not a constructor"). The same descriptor
 *  shape is accepted by the `menu` option of gui.open, gui.openFile and
 *  gui.createTray.
 *
 *  Obtained from:
 *  - the items of a menu built by `gui.createMenu` or the `menu` option of
 *    gui.open/gui.openFile/gui.createTray, addressed by index (`menu[index]`) or
 *    by id (`menu.getMenuItemById(id)`);
 *  - `Menu#append` and `Menu#insert` — items added to an existing menu;
 *  - the `submenu` property of a submenu item — the nested Menu whose items are
 *    again MenuItem objects.
 *
 *  Concepts:
 *
 *  - **Item types**: `type` is fixed at creation. Without an explicit type it is
 *    inferred from the descriptor: a `submenu` property makes the item a "submenu",
 *    a `checked` property a "checkbox", otherwise a non-empty label makes it
 *    "normal" and an object without label a "separator". Each type accepts only its
 *    own properties: a separator cannot have label, tooltip, icon, submenu or
 *    checked; a normal item cannot have submenu or checked; a checkbox cannot have
 *    submenu; a submenu item cannot have checked. Invalid descriptors and unknown
 *    types throw when the item is created (see MenuItem#type).
 *  - **Item state**: `id`, `label`, `tooltip`, `enabled` and `checked` can be
 *    changed after creation, and label, tooltip, enabled and checked refresh the
 *    native menu entry when it exists. Optional properties that the descriptor did
 *    not provide stay undefined and writes to them are silently ignored, so an icon
 *    can only be set when it was given at creation. `type` and `submenu` are
 *    read-only.
 *  - **Activation**: the native menu emits `click` when the user activates a normal
 *    or checkbox item; separators and submenu parents are not clickable. The
 *    `onclick` property of the descriptor is registered as the first handler of the
 *    item and is called with the item as `this`. The native widget toggles the
 *    checked state before it emits the click event on every platform.
 *  - **Serialization**: toJSON returns a descriptor that can be passed back to
 *    gui.createMenu. It omits `enabled` while the item is enabled and always emits
 *    `checked: true` for items that have a checked state, so an unchecked checkbox
 *    does not round-trip through JSON.
 *
 *  Example 1 — inspect the items created from a template:
 *  ```JavaScript
 *  const gui = require('gui');
 *
 *  const menu = gui.createMenu([
 *      { label: 'Open' },
 *      { label: 'Auto save', checked: true },
 *      { label: 'Recent files', submenu: [{ label: 'notes.txt' }] },
 *      { type: 'separator' }
 *  ]);
 *
 *  for (let i = 0; i < menu.length; i++)
 *      console.log(i + ': ' + menu[i].type);
 *  // 0: normal
 *  // 1: checkbox
 *  // 2: submenu
 *  // 3: separator
 *
 *  console.log(menu[1].checked); // true
 *  console.log(menu[2].submenu.length); // 1
 *  ```
 *
 *  Example 2 — handle a click and read the state of the clicked item (requires a desktop session):
 *  ```JavaScript
 *  // requires: long-running
 *  const gui = require('gui');
 *
 *  const win = gui.open({
 *      menu: [
 *          {
 *              label: 'Say hello',
 *              onclick: function () {
 *                  console.log(this.type + ' ' + this.label + ' clicked');
 *              }
 *          }
 *      ]
 *  });
 *
 *  win.waitFor();
 *  ```
 *
 *  Notes:
 *
 *  - `click` is the only event declared on MenuItem, but the class also inherits
 *    the EventEmitter members, so `on`, `off` and `listenerCount` work on items.
 *    Replacing `item.onclick` after creation does not change the registered
 *    listener; use `on`/`off` to add or remove handlers instead.
 *
 */
declare class Class_MenuItem extends Class_EventEmitter {
    /**
     * @description Identifier used by Menu#getMenuItemById, an empty string by default
     *
     *      The id is not displayed and does not need to be unique: the lookup returns
     *      the first match in depth-first order. Assigning a new value takes effect
     *      immediately because the search walks the live items.
     *
     */
    id: string;

    /**
     * @description Type of the item: "normal", "checkbox", "submenu" or "separator"
     *
     *      The type is fixed when the item is created and inferred from the descriptor
     *      when it is not given explicitly; see the class documentation for the
     *      inference and validation rules. Unknown types are rejected with
     *      "MenuItem: Invalid menu item type: <type>". The property is read-only and
     *      later assignments are ignored.
     *
     */
    readonly type: string;

    /**
     * @description Icon file of the item, read when the item is created
     *
     *      The property is undefined when the descriptor had no icon, and writes to an
     *      undefined icon are ignored, so an icon cannot be added to an existing item.
     *      Assigning a new path replaces the stored value but does not refresh the
     *      native menu entry; recreate the menu to make the new icon visible. A missing
     *      file is reported with ENOENT when the item is created.
     *
     */
    icon: string;

    /**
     * @description Text of the item, required for normal, checkbox and submenu items
     *
     *      The property is undefined on separators, and writes to an undefined label
     *      are ignored. Setting the label of an item that is already shown refreshes
     *      the native menu entry.
     *
     */
    label: string;

    /**
     * @description Tooltip of the item, shown when the pointer rests on the entry
     *
     *      Optional: the property is undefined when the descriptor had no tooltip, and
     *      writes to an undefined tooltip are ignored. Setting the tooltip of an item
     *      that is already shown refreshes the native menu entry; Windows menu items do
     *      not display tooltips.
     *
     */
    tooltip: string;

    /**
     * @description Whether the item can be activated, true by default
     *
     *      A disabled item is grayed out and does not emit click. Setting the property
     *      of an item that is already shown refreshes the native menu entry.
     *
     */
    enabled: boolean;

    /**
     * @description Checked state of a checkbox item, undefined for other item types
     *
     *      The native menu toggles the state when the user activates the item. Writes
     *      are ignored when the item is not a checkbox or was created without the
     *      checked property.
     *
     */
    checked: boolean;

    /**
     * @description Nested Menu of a submenu item, null for the other item types
     *
     *      The nested menu is built from the submenu array or Menu object of the
     *      descriptor and is attached to the parent item; it cannot be replaced later.
     *
     */
    readonly submenu: Class_Menu;

    /**
     * @description Emitted when the user activates a normal or checkbox item
     *
     *      `ev.target` is the clicked MenuItem and `ev.type` is "click". The `onclick`
     *      property of the descriptor is registered as the first handler of this event,
     *      and handlers added with `on`/`addEventListener` are called as well.
     *      Separators and submenu parents are not clickable and do not emit it.
     *
     *      Example — register a click handler when the menu is built:
     *      ```JavaScript
     *      const gui = require('gui');
     *
     *      const menu = gui.createMenu([{ id: 'ping', label: 'Ping' }]);
     *
     *      menu[0].on('click', function (ev) {
     *          console.log('clicked ' + ev.target.id); // clicked ping
     *      });
     *
     *      console.log(menu[0].listenerCount('click')); // 1
     *      ```
     *
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
     * @description Emitted when the user activates a normal or checkbox item
     *
     *      `ev.target` is the clicked MenuItem and `ev.type` is "click". The `onclick`
     *      property of the descriptor is registered as the first handler of this event,
     *      and handlers added with `on`/`addEventListener` are called as well.
     *      Separators and submenu parents are not clickable and do not emit it.
     *
     *      Example — register a click handler when the menu is built:
     *      ```JavaScript
     *      const gui = require('gui');
     *
     *      const menu = gui.createMenu([{ id: 'ping', label: 'Ping' }]);
     *
     *      menu[0].on('click', function (ev) {
     *          console.log('clicked ' + ev.target.id); // clicked ping
     *      });
     *
     *      console.log(menu[0].listenerCount('click')); // 1
     *      ```
     *
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

