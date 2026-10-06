/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/**
 * @description System tray icon with an optional menu, created by gui.createTray
 *
 *  A Tray shows an icon in the desktop status area: the notification area on
 *  Windows, the status bar on macOS and the indicator area on Linux. The icon file
 *  is read when the tray is created; the native icon is then created asynchronously
 *  on the GUI thread, so a desktop session is required.
 *
 *  Obtained from:
 *  - `gui.createTray(options)` — the only way to create a tray icon.
 *
 *  Concepts:
 *
 *  - **Icon and text**: `icon` is required and must be a PNG file. `title` and
 *    `tooltip` are optional and their effect is platform dependent: Linux uses the
 *    title as the indicator label and the tooltip as its title, macOS shows the
 *    title next to the icon and the tooltip on hover, and Windows ignores both
 *    (the icon always carries a fixed tooltip).
 *  - **Menu**: the `menu` option accepts a Menu object or a template array; the tray
 *    keeps the resulting Menu and `getMenu` returns it (null when no menu was
 *    given). The menu is attached to the tray and can no longer be edited.
 *  - **Lifetime**: createTray returns as soon as the object exists while the native
 *    icon is built asynchronously; `close` waits for it and then removes the icon.
 *    A second close throws "Tray: tray is closed", and after close the object
 *    cannot be shown again. The tray object does not expose the EventEmitter
 *    interface, so the internal click and close notifications of the native
 *    implementations cannot be observed from JavaScript.
 *
 *  Example 1 — create a tray icon with a menu (requires a desktop session):
 *  ```JavaScript
 *  // requires: long-running
 *  const gui = require('gui');
 *  const path = require('path');
 *
 *  const tray = gui.createTray({
 *      icon: path.join(__dirname, 'icon.png'),
 *      title: 'My App',
 *      tooltip: 'fibjs tray icon',
 *      menu: [
 *          { label: 'Show', onclick: function () { console.log('show'); } },
 *          { type: 'separator' },
 *          { label: 'Quit', onclick: function () { console.log('quit'); } }
 *      ]
 *  });
 *
 *  console.log(tray.getMenu().length); // 3
 *  tray.close();
 *  ```
 *
 *  Example 2 — the menu is optional and the icon can be removed at any time:
 *  ```JavaScript
 *  // requires: long-running
 *  const gui = require('gui');
 *  const path = require('path');
 *
 *  const tray = gui.createTray({ icon: path.join(__dirname, 'icon.png') });
 *
 *  console.log(tray.getMenu()); // null
 *
 *  tray.close();
 *  console.log('tray removed');
 *  ```
 *
 */
declare class Class_Tray extends Class_object {
    /**
     * @description Returns the menu of the tray icon, or null when it has none
     *
     *      The value is the Menu object kept by the tray (a template array given in
     *      the `menu` option is converted when the tray is created); the lookup does
     *      not depend on the native icon, so it also works after close. When a menu
     *      was given, `tray.getMenu().length` counts its top-level entries.
     *
     *      Example — read the menu back from a tray (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *      const path = require('path');
     *
     *      const tray = gui.createTray({
     *          icon: path.join(__dirname, 'icon.png'),
     *          menu: [{ label: 'Quit' }]
     *      });
     *
     *      console.log(tray.getMenu().length); // 1
     *      tray.close();
     *      ```
     *
     *      @return returns the menu of the tray icon
     *
     */
    getMenu(): Class_Menu;

    /**
     * @description Removes the icon from the status area and releases it
     *
     *      Waits for the native icon and then destroys it; a second close throws
     *      "Tray: tray is closed". The generated closeAsync form returns a Promise
     *      instead of blocking the calling fiber.
     *
     *      Example — remove the tray icon when the application exits (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *      const path = require('path');
     *
     *      const tray = gui.createTray({ icon: path.join(__dirname, 'icon.png') });
     *
     *      tray.close();
     *      console.log('tray removed');
     *      ```
     *
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Removes the icon from the status area and releases it
     *
     *      Waits for the native icon and then destroys it; a second close throws
     *      "Tray: tray is closed". The generated closeAsync form returns a Promise
     *      instead of blocking the calling fiber.
     *
     *      Example — remove the tray icon when the application exits (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *      const path = require('path');
     *
     *      const tray = gui.createTray({ icon: path.join(__dirname, 'icon.png') });
     *
     *      tray.close();
     *      console.log('tray removed');
     *      ```
     *
     */
    closeSync(): void;

    /**
     * @description Removes the icon from the status area and releases it
     *
     *      Waits for the native icon and then destroys it; a second close throws
     *      "Tray: tray is closed". The generated closeAsync form returns a Promise
     *      instead of blocking the calling fiber.
     *
     *      Example — remove the tray icon when the application exits (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *      const path = require('path');
     *
     *      const tray = gui.createTray({ icon: path.join(__dirname, 'icon.png') });
     *
     *      tray.close();
     *      console.log('tray removed');
     *      ```
     *
     */
    closeAsync(): Promise<void>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/**
 * The promise variant of the Tray class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TrayPromise extends Class_object {
    /**
     * @description Returns the menu of the tray icon, or null when it has none
     *
     *      The value is the Menu object kept by the tray (a template array given in
     *      the `menu` option is converted when the tray is created); the lookup does
     *      not depend on the native icon, so it also works after close. When a menu
     *      was given, `tray.getMenu().length` counts its top-level entries.
     *
     *      Example — read the menu back from a tray (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *      const path = require('path');
     *
     *      const tray = gui.createTray({
     *          icon: path.join(__dirname, 'icon.png'),
     *          menu: [{ label: 'Quit' }]
     *      });
     *
     *      console.log(tray.getMenu().length); // 1
     *      tray.close();
     *      ```
     *
     *      @return returns the menu of the tray icon
     *
     */
    getMenu(): Class_Menu;

    /**
     * @description Removes the icon from the status area and releases it
     *
     *      Waits for the native icon and then destroys it; a second close throws
     *      "Tray: tray is closed". The generated closeAsync form returns a Promise
     *      instead of blocking the calling fiber.
     *
     *      Example — remove the tray icon when the application exits (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *      const path = require('path');
     *
     *      const tray = gui.createTray({ icon: path.join(__dirname, 'icon.png') });
     *
     *      tray.close();
     *      console.log('tray removed');
     *      ```
     *
     */
    close(): Promise<void>;

    /**
     * @description Removes the icon from the status area and releases it
     *
     *      Waits for the native icon and then destroys it; a second close throws
     *      "Tray: tray is closed". The generated closeAsync form returns a Promise
     *      instead of blocking the calling fiber.
     *
     *      Example — remove the tray icon when the application exits (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *      const path = require('path');
     *
     *      const tray = gui.createTray({ icon: path.join(__dirname, 'icon.png') });
     *
     *      tray.close();
     *      console.log('tray removed');
     *      ```
     *
     */
    closeSync(): void;

    /**
     * @description Removes the icon from the status area and releases it
     *
     *      Waits for the native icon and then destroys it; a second close throws
     *      "Tray: tray is closed". The generated closeAsync form returns a Promise
     *      instead of blocking the calling fiber.
     *
     *      Example — remove the tray icon when the application exits (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *      const path = require('path');
     *
     *      const tray = gui.createTray({ icon: path.join(__dirname, 'icon.png') });
     *
     *      tray.close();
     *      console.log('tray removed');
     *      ```
     *
     */
    closeAsync(): Promise<void>;

}


declare namespace Class_Tray {
    const promises: FIBJS.GeneralObject;
}
