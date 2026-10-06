/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/WebView.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/// <reference path="../interface/Tray.d.ts" />
/**
 * The promise variant of the gui module: async members return a Promise as their primary form.
 */
declare module 'gui/promises' {
    /**
     * @description The WebView class, used for type checks of window objects
     *
     *      The property exposes the class itself, not an instance: `gui.open` and
     *      `gui.openFile` return objects of this type, and `win instanceof gui.WebView`
     *      is true. The constructor is not usable (`new gui.WebView()` throws "not a
     *      constructor"); see the WebView interface for the window API.
     *
     */
    const WebView: typeof Class_WebView;

    /**
     * @description Opens a window and visits the specified url
     *
     *      The native window is created asynchronously on the GUI thread and the
     *      returned WebView is usable immediately; the members that need the window
     *      wait for it in the calling fiber. An explicit url overrides the `url` and
     *      `file` properties of the options object, and openFile is the form that
     *      clears the options `url` and loads a local file. With width and height but
     *      without left/top the window is centered; without a size the platform
     *      chooses it.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "url": "about:blank",        // initial page when open(options) is used
     *          "file": "",                  // local file or archive.zip$/dir/page.html
     *          "icon": "/path/to/file.png", // window icon, read when the window is created
     *          "left": 100,                 // window position, centered when omitted
     *          "top": 100,                  // window position, centered when omitted
     *          "width": 640,                // window size, decided by the platform when omitted
     *          "height": 480,
     *          "visible": true,             // show the window when it is created
     *          "hideOnClose": false,        // hide instead of closing on the close button
     *          "minWidth": 0,               // minimum size, 0 means no limit
     *          "minHeight": 0,
     *          "maxWidth": 2000,            // maximum size, unset means no limit
     *          "maxHeight": 2000,
     *          "frame": true,               // draw the window frame and title bar
     *          "titlebar": "show",          // "show" | "hide" | "transparent" or { style, height }
     *          "resizable": true,           // allow the user to resize the window
     *          "maximize": false,           // start maximized
     *          "fullscreen": false,         // start fullscreen
     *          "devtools": false,           // enable the engine developer tools
     *          "menu": null,                // a Menu object or a menu template array
     *          "app": {},                   // object exposed as window.app in the page
     *          "onloading": null,           // shortcut for win.on("loading", fn)
     *          "onload": null,              // shortcut for win.on("load", fn)
     *          "onclose": null,             // shortcut for win.on("close", fn)
     *          "onmove": null,              // shortcut for win.on("move", fn)
     *          "onresize": null,            // shortcut for win.on("resize", fn)
     *          "onfocus": null,             // shortcut for win.on("focus", fn)
     *          "onblur": null,              // shortcut for win.on("blur", fn)
     *          "onmessage": null            // shortcut for win.on("message", fn)
     *      })
     *      ```
     *
     *      Malformed options throw before the window is created: an unknown `titlebar`
     *      style or height, a missing icon file (ENOENT) and wrong property types
     *      (TypeError 20005). The titlebar and window icon options are ignored on
     *      platforms that do not support them (gtk4 ignores the icon and the initial
     *      position). See the WebView interface for the events and the app bridge.
     *
     *      Example — open a hidden window and close it when the page has loaded
     *      (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200, visible: false });
     *
     *      win.on('load', function () {
     *          win.close();
     *      });
     *
     *      win.loadUrl('data:text/html;charset=utf-8,<title>Ready</title>');
     *      win.waitFor();
     *      ```
     *
     *      @param url the url to visit
     *      @param opt window opening parameters
     *      @return returns the opened window object
     *
     */
    function open(url: string, opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description Opens a window with the content selected by the options object
     *
     *      Without a url argument the initial content comes from the `url` or `file`
     *      property of the options, defaulting to about:blank (`url` wins when both are
     *      given). The options are the same as for the url form, see open(url, opt) for
     *      the full list and the platform notes.
     *
     *      @param opt window opening parameters
     *      @return returns the opened window object
     *
     */
    function open(opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description Opens a window and loads the specified local file
     *
     *      The file becomes the initial content instead of the options `url`, which is
     *      discarded. A path inside a zip archive mounted with fs.setZipFS is written as
     *      `archive.zip$/dir/page.html`. The options are the same as for the url form,
     *      see open(url, opt) for the full list and the platform notes.
     *
     *      @param file the file to load
     *      @param opt window opening parameters
     *      @return returns the opened window object
     *
     */
    function openFile(file: string, opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description Creates a Menu from an array of item descriptors
     *
     *      Every element is a plain object descriptor (or an existing MenuItem); the
     *      accepted properties, the type inference and the validation rules are
     *      described in MenuItem. Item icons are read when the menu is created, so a
     *      missing icon file fails here with ENOENT. The array may be empty; submenu
     *      arrays and nested Menu objects are converted recursively.
     *
     *      Example — build a menu with all item types:
     *      ```JavaScript
     *      const gui = require('gui');
     *
     *      const menu = gui.createMenu([
     *          { id: 'open', label: 'Open' },
     *          { label: 'Auto save', checked: true },
     *          { label: 'Recent', submenu: [{ label: 'notes.txt' }] },
     *          { type: 'separator' }
     *      ]);
     *
     *      console.log(menu.length); // 4
     *      console.log(menu.getMenuItemById('open').label); // Open
     *      console.log(menu[1].type + ' checked=' + menu[1].checked); // checkbox checked=true
     *      ```
     *
     *      @param items menu item array
     *      @return returns the created menu object
     *
     */
    function createMenu(items?: FIBJS.GeneralObject[]): Class_Menu;

    /**
     * @description Creates a Tray icon from the given options
     *
     *      The icon file is required and read immediately (a PNG file); the native icon
     *      is created asynchronously on the GUI thread, so a desktop session is needed.
     *      The menu option accepts a Menu object or a template array and the tray keeps
     *      the resulting object; see the Tray interface for the platform differences of
     *      title and tooltip and for the tray lifecycle.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "icon": "/path/to/file.png", // required; icon file, must be a PNG
     *          "title": "",                 // optional text next to the icon, not on Windows
     *          "tooltip": "",               // optional hover text, not on Windows
     *          "menu": null                 // a Menu object or a template array
     *      })
     *      ```
     *
     *      @param opt tray creation parameters
     *      @return returns the created tray icon object
     *
     */
    function createTray(opt?: FIBJS.GeneralObject): Class_Tray;

    /**
     * @description Pops up a modal message box
     *
     *      Blocks the calling fiber until the user dismisses the dialog; the generated
     *      alertAsync form returns a Promise instead. The one-argument form uses an
     *      empty window title. Requires a desktop session.
     *
     *      @param message message content
     *
     */
    function alert(message: string): Promise<void>;

    /**
     * @description Pops up a modal message box
     *
     *      Blocks the calling fiber until the user dismisses the dialog; the generated
     *      alertAsync form returns a Promise instead. The one-argument form uses an
     *      empty window title. Requires a desktop session.
     *
     *      @param message message content
     *
     */
    function alertSync(message: string): void;

    /**
     * @description Pops up a modal message box
     *
     *      Blocks the calling fiber until the user dismisses the dialog; the generated
     *      alertAsync form returns a Promise instead. The one-argument form uses an
     *      empty window title. Requires a desktop session.
     *
     *      @param message message content
     *
     */
    function alertAsync(message: string): Promise<void>;

    /**
     * @description Pops up a modal message box with the given title
     *
     *      The blocking behaviour, the Promise form and the desktop requirement are
     *      described on alert(message).
     *
     *      @param title message title
     *      @param message message content
     *
     */
    function alert(title: string, message: string): Promise<void>;

    /**
     * @description Pops up a modal message box with the given title
     *
     *      The blocking behaviour, the Promise form and the desktop requirement are
     *      described on alert(message).
     *
     *      @param title message title
     *      @param message message content
     *
     */
    function alertSync(title: string, message: string): void;

    /**
     * @description Pops up a modal message box with the given title
     *
     *      The blocking behaviour, the Promise form and the desktop requirement are
     *      described on alert(message).
     *
     *      @param title message title
     *      @param message message content
     *
     */
    function alertAsync(title: string, message: string): Promise<void>;

    /**
     * @description Pops up a modal confirmation box
     *
     *      Returns true when the user confirms with OK and false for Cancel or a closed
     *      dialog; blocks the calling fiber until the dialog is dismissed. Requires a
     *      desktop session.
     *
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirm(message: string): Promise<boolean>;

    /**
     * @description Pops up a modal confirmation box
     *
     *      Returns true when the user confirms with OK and false for Cancel or a closed
     *      dialog; blocks the calling fiber until the dialog is dismissed. Requires a
     *      desktop session.
     *
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirmSync(message: string): boolean;

    /**
     * @description Pops up a modal confirmation box
     *
     *      Returns true when the user confirms with OK and false for Cancel or a closed
     *      dialog; blocks the calling fiber until the dialog is dismissed. Requires a
     *      desktop session.
     *
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirmAsync(message: string): Promise<boolean>;

    /**
     * @description Pops up a modal confirmation box with the given title
     *
     *      See confirm(message) for the result, the blocking behaviour and the Promise
     *      form.
     *
     *      @param title message title
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirm(title: string, message: string): Promise<boolean>;

    /**
     * @description Pops up a modal confirmation box with the given title
     *
     *      See confirm(message) for the result, the blocking behaviour and the Promise
     *      form.
     *
     *      @param title message title
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirmSync(title: string, message: string): boolean;

    /**
     * @description Pops up a modal confirmation box with the given title
     *
     *      See confirm(message) for the result, the blocking behaviour and the Promise
     *      form.
     *
     *      @param title message title
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirmAsync(title: string, message: string): Promise<boolean>;

    /**
     * @description Pops up a modal input box
     *
     *      The password argument masks the entered text. Cancelling the dialog returns
     *      undefined instead of a string; the dialog blocks the calling fiber until it
     *      is dismissed. Requires a desktop session.
     *
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function input(message: string, password?: boolean): Promise<string>;

    /**
     * @description Pops up a modal input box
     *
     *      The password argument masks the entered text. Cancelling the dialog returns
     *      undefined instead of a string; the dialog blocks the calling fiber until it
     *      is dismissed. Requires a desktop session.
     *
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function inputSync(message: string, password?: boolean): string;

    /**
     * @description Pops up a modal input box
     *
     *      The password argument masks the entered text. Cancelling the dialog returns
     *      undefined instead of a string; the dialog blocks the calling fiber until it
     *      is dismissed. Requires a desktop session.
     *
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function inputAsync(message: string, password?: boolean): Promise<string>;

    /**
     * @description Pops up a modal input box with the given title
     *
     *      See input(message, password) for the behaviour, the cancel result and the
     *      Promise form.
     *
     *      @param title message title
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function input(title: string, message: string, password?: boolean): Promise<string>;

    /**
     * @description Pops up a modal input box with the given title
     *
     *      See input(message, password) for the behaviour, the cancel result and the
     *      Promise form.
     *
     *      @param title message title
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function inputSync(title: string, message: string, password?: boolean): string;

    /**
     * @description Pops up a modal input box with the given title
     *
     *      See input(message, password) for the behaviour, the cancel result and the
     *      Promise form.
     *
     *      @param title message title
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function inputAsync(title: string, message: string, password?: boolean): Promise<string>;

    /**
     * @description Pops up a modal file chooser and returns the selected paths
     *
     *      Cancelling the dialog returns undefined. The result is always an array, also
     *      for a single selection; saveFile returns at most one path, openFile and
     *      openDirectory return one or more paths according to multiSelections. Requires
     *      a desktop session.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "title": "",              // dialog title
     *          "type": "openFile",       // "openFile" | "openDirectory" | "saveFile"
     *          "defaultPath": "",        // directory shown when the dialog opens
     *          "multiSelections": false, // allow several files, ignored by saveFile
     *          "filters": null           // [{ name: "Images", extensions: ["png", "jpg"] }]
     *      })
     *      ```
     *      Filter extensions are written without the leading dot. An unknown type and,
     *      on Windows, an invalid defaultPath throw an Error.
     *
     *      Example — pick one or more images (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const files = gui.chooseFile({
     *          title: 'Select images',
     *          type: 'openFile',
     *          multiSelections: true,
     *          filters: [{ name: 'Images', extensions: ['png', 'jpg'] }]
     *      });
     *
     *      console.log(files ? files.length : 'cancelled');
     *      ```
     *
     *      @param options file chooser dialog parameters
     *      @return returns the array of files chosen by the user
     *
     */
    function chooseFile(options: FIBJS.GeneralObject): Promise<any[]>;

    /**
     * @description Pops up a modal file chooser and returns the selected paths
     *
     *      Cancelling the dialog returns undefined. The result is always an array, also
     *      for a single selection; saveFile returns at most one path, openFile and
     *      openDirectory return one or more paths according to multiSelections. Requires
     *      a desktop session.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "title": "",              // dialog title
     *          "type": "openFile",       // "openFile" | "openDirectory" | "saveFile"
     *          "defaultPath": "",        // directory shown when the dialog opens
     *          "multiSelections": false, // allow several files, ignored by saveFile
     *          "filters": null           // [{ name: "Images", extensions: ["png", "jpg"] }]
     *      })
     *      ```
     *      Filter extensions are written without the leading dot. An unknown type and,
     *      on Windows, an invalid defaultPath throw an Error.
     *
     *      Example — pick one or more images (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const files = gui.chooseFile({
     *          title: 'Select images',
     *          type: 'openFile',
     *          multiSelections: true,
     *          filters: [{ name: 'Images', extensions: ['png', 'jpg'] }]
     *      });
     *
     *      console.log(files ? files.length : 'cancelled');
     *      ```
     *
     *      @param options file chooser dialog parameters
     *      @return returns the array of files chosen by the user
     *
     */
    function chooseFileSync(options: FIBJS.GeneralObject): any[];

    /**
     * @description Pops up a modal file chooser and returns the selected paths
     *
     *      Cancelling the dialog returns undefined. The result is always an array, also
     *      for a single selection; saveFile returns at most one path, openFile and
     *      openDirectory return one or more paths according to multiSelections. Requires
     *      a desktop session.
     *
     *      options supports the following options:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "title": "",              // dialog title
     *          "type": "openFile",       // "openFile" | "openDirectory" | "saveFile"
     *          "defaultPath": "",        // directory shown when the dialog opens
     *          "multiSelections": false, // allow several files, ignored by saveFile
     *          "filters": null           // [{ name: "Images", extensions: ["png", "jpg"] }]
     *      })
     *      ```
     *      Filter extensions are written without the leading dot. An unknown type and,
     *      on Windows, an invalid defaultPath throw an Error.
     *
     *      Example — pick one or more images (requires a desktop session):
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const files = gui.chooseFile({
     *          title: 'Select images',
     *          type: 'openFile',
     *          multiSelections: true,
     *          filters: [{ name: 'Images', extensions: ['png', 'jpg'] }]
     *      });
     *
     *      console.log(files ? files.length : 'cancelled');
     *      ```
     *
     *      @param options file chooser dialog parameters
     *      @return returns the array of files chosen by the user
     *
     */
    function chooseFileAsync(options: FIBJS.GeneralObject): Promise<any[]>;

}


declare module "gui" {
    const promises: typeof import("gui/promises");
}
