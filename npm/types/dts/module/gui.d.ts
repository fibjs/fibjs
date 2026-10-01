/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/WebView.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/// <reference path="../interface/Tray.d.ts" />
/**
 * @description gui module
 *
 * Usage:
 * ```JavaScript
 * var gui = require('gui');
 * ```
 *
 */
declare module 'gui' {
    /**
     * @description Browser window object; WebView is a window component with an embedded browser
     */
    const WebView: typeof Class_WebView;

    /**
     * @description Opens a window and visits the specified url
     *
     *      The following parameters are supported:
     *      ```JavaScript
     *      {
     *          "icon": "/path/to/file.png", // specify the icon of the window, not work in gtk4
     *          "left": 100, // specify the left position of the window, default position is center of the screen, not work in gtk4
     *          "right": 100, // spcify the top position of the window, default position is center of the screen, not work in gtk4
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "visible": true, // specify whether the window is visible, default is true
     *          "hideOnClose": false, // specify whether the window is hidden when closed, default is false
     *          "minWidth": 0, // specify the minimum width of the window, default is 0
     *          "minHeight": 0, // specify the minimum height of the window, default is 0
     *          "maxWidth": 0, // specify the maximum width of the window, default is no limit
     *          "maxHeight": 0, // specify the maximum height of the window, default is no limit
     *          "frame": true, // specify whether the window has frame, default is true
     *          "titlebar": "show" | {  // specify the titlebar style: "show" (default), "hide", "transparent"
     *             "style": "show", // specify the titlebar style: "show" (default), "hide", "transparent"
     *             "height": "nprmal" // specify the titlebar height: "normal" (default), "tall", not work in macos
     *          },
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "menu": menu, // specify the menu of the window, can be a Menu object or a menu item array, default is null
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "devtools": false, // specify whether the DevTools in WebView is enabled, default is false
     *          "app": {}, // specify the app object that can be remote call in WebView, default is undefined
     *      }
     *      ```
     *      When width and height are set but left or right is not set, the window is automatically centered
     *      @param url the url to visit
     *      @param opt window opening parameters
     *      @return returns the opened window object
     *
     */
    function open(url: string, opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description Opens a browser window; if url or file is specified, the specified resource is loaded
     *
     *      The following parameters are supported:
     *      ```JavaScript
     *      {
     *          "url": , // specify the url of the window, default is about:blank
     *          "file": , // specify the file of the window
     *          "icon": "/path/to/file.png", // specify the icon of the window, not work in gtk4
     *          "left": 100, // specify the left position of the window, default position is center of the screen, not work in gtk4
     *          "right": 100, // spcify the top position of the window, default position is center of the screen, not work in gtk4
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "visible": true, // specify whether the window is visible, default is true
     *          "hideOnClose": false, // specify whether the window is hidden when closed, default is false
     *          "minWidth": 0, // specify the minimum width of the window, default is 0
     *          "minHeight": 0, // specify the minimum height of the window, default is 0
     *          "maxWidth": 0, // specify the maximum width of the window, default is no limit
     *          "maxHeight": 0, // specify the maximum height of the window, default is no limit
     *          "frame": true, // specify whether the window has frame, default is true
     *          "titlebar": "show" | {  // specify the titlebar style: "show" (default), "hide", "transparent"
     *             "style": "show", // specify the titlebar style: "show" (default), "hide", "transparent"
     *             "height": "nprmal" // specify the titlebar height: "normal" (default), "tall", not work in macos
     *          },
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "menu": menu, // specify the menu of the window, can be a Menu object or a menu item array, default is null
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "devtools": false, // specify whether the DevTools in WebView is enabled, default is false
     *          "app": {}, // specify the app object that can be remote call in WebView, default is undefined
     *      }
     *      ```
     *      When width and height are set but left or right is not set, the window is automatically centered
     *      @param opt window opening parameters
     *      @return returns the opened window object
     *
     */
    function open(opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description Opens a window and visits the specified file
     *
     *      The following parameters are supported:
     *      ```JavaScript
     *      {
     *          "icon": "/path/to/file.png", // specify the icon of the window, not work in gtk4
     *          "left": 100, // specify the left position of the window, default position is center of the screen, not work in gtk4
     *          "right": 100, // spcify the top position of the window, default position is center of the screen, not work in gtk4
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "visible": true, // specify whether the window is visible, default is true
     *          "hideOnClose": false, // specify whether the window is hidden when closed, default is false
     *          "minWidth": 0, // specify the minimum width of the window, default is 0
     *          "minHeight": 0, // specify the minimum height of the window, default is 0
     *          "maxWidth": 0, // specify the maximum width of the window, default is no limit
     *          "maxHeight": 0, // specify the maximum height of the window, default is no limit
     *          "frame": true, // specify whether the window has frame, default is true
     *          "titlebar": "show" | {  // specify the titlebar style: "show" (default), "hide", "transparent"
     *             "style": "show", // specify the titlebar style: "show" (default), "hide", "transparent"
     *             "height": "nprmal" // specify the titlebar height: "normal" (default), "tall", not work in macos
     *          },
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "menu": menu, // specify the menu of the window, can be a Menu object or a menu item array, default is null
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "devtools": false, // specify whether the DevTools in WebView is enabled, default is false
     *          "app": {}, // specify the app object that can be remote call in WebView, default is undefined
     *      }
     *      ```
     *      When width and height are set but left or right is not set, the window is automatically centered
     *      @param file the file to load
     *      @param opt window opening parameters
     *      @return returns the opened window object
     *
     */
    function openFile(file: string, opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description Creates a menu object
     *
     *     The following menu item types are supported:
     *     - normal
     *         - type: "normal"
     *         - label: required
     *         - tooltip, icon, enabled: optional
     *         - cannot have submenu or checked
     *     - checkbox
     *         - type: "checkbox"
     *         - label: required
     *         - checked: optional
     *         - tooltip, icon, enabled: optional
     *         - cannot have submenu
     *     - submenu
     *         - type: "submenu"
     *         - label, submenu: required
     *         - tooltip, icon, enabled: optional
     *         - cannot have checked
     *     - separator
     *         - type: "separator"
     *         - cannot have label, submenu, checked, icon or tooltip
     *
     *     If a menu item does not specify type, its type is inferred from the other properties. The inference rules are:
     *     - If the submenu property exists, type is set to "submenu".
     *     - If the checked property exists, type is set to "checkbox".
     *     - If the passed object is empty, type is set to "separator".
     *     - If none of the above conditions is met, type is set to "normal".
     *
     *      @param items menu item array
     *      @return returns the created menu object
     *
     */
    function createMenu(items?: FIBJS.GeneralObject[]): Class_Menu;

    /**
     * @description Creates a tray icon object
     *
     *      The following parameters are supported:
     *      ```JavaScript
     *      {
     *          "icon": "/path/to/file.png", // specify the icon of the tray, must be a png file
     *          "title": "", // specify the title of the tray, if not set, it will not be displayed
     *          "tooltip": "", // specify the tooltip of the tray, if not set, it will not be displayed
     *          "menu": menu, // specify the menu of the tray, default is null
     *      }
     *      ```
     *      @param opt tray creation parameters
     *      @return returns the created tray icon object
     *
     */
    function createTray(opt?: FIBJS.GeneralObject): Class_Tray;

    /**
     * @description Pops up a message box
     *      @param message message content
     *
     */
    function alert(message: string): void;

    function alert(message: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Pops up a message box
     *      @param message message content
     *
     */
    function alertSync(message: string): void;

    /**
     * @description Pops up a message box
     *      @param message message content
     *
     */
    function alertAsync(message: string): Promise<void>;

    /**
     * @description Pops up a message box
     *      @param title message title
     *      @param message message content
     *
     */
    function alert(title: string, message: string): void;

    function alert(title: string, message: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Pops up a message box
     *      @param title message title
     *      @param message message content
     *
     */
    function alertSync(title: string, message: string): void;

    /**
     * @description Pops up a message box
     *      @param title message title
     *      @param message message content
     *
     */
    function alertAsync(title: string, message: string): Promise<void>;

    /**
     * @description Pops up a confirmation box
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirm(message: string): boolean;

    function confirm(message: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Pops up a confirmation box
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirmSync(message: string): boolean;

    /**
     * @description Pops up a confirmation box
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirmAsync(message: string): Promise<boolean>;

    /**
     * @description Pops up a confirmation box
     *      @param title message title
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirm(title: string, message: string): boolean;

    function confirm(title: string, message: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Pops up a confirmation box
     *      @param title message title
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirmSync(title: string, message: string): boolean;

    /**
     * @description Pops up a confirmation box
     *      @param title message title
     *      @param message message content
     *      @return returns the user's choice
     *
     */
    function confirmAsync(title: string, message: string): Promise<boolean>;

    /**
     * @description Pops up an input box
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function input(message: string, password?: boolean): string;

    function input(message: string, password?: boolean, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Pops up an input box
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function inputSync(message: string, password?: boolean): string;

    /**
     * @description Pops up an input box
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function inputAsync(message: string, password?: boolean): Promise<string>;

    /**
     * @description Pops up an input box
     *      @param title message title
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function input(title: string, message: string, password?: boolean): string;

    function input(title: string, message: string, password?: boolean, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Pops up an input box
     *      @param title message title
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function inputSync(title: string, message: string, password?: boolean): string;

    /**
     * @description Pops up an input box
     *      @param title message title
     *      @param message message content
     *      @param password whether this is a password input, default is false
     *      @return returns the content entered by the user
     *
     */
    function inputAsync(title: string, message: string, password?: boolean): Promise<string>;

    /**
     * @description Pops up a file chooser dialog
     *
     *      options supports the following parameters:
     *       - title: dialog title
     *       - type: dialog type, "openFile", "openDirectory" or "saveFile", default is "openFile"
     *       - defaultPath: the default path to open
     *       - multiple: whether multiple selection is allowed, default is false
     *       - filters: file filter array; each element is an object containing the name and extensions properties, where extensions is an array of extensions
     *
     *      @param options file chooser dialog parameters
     *      @return returns the array of files chosen by the user
     *
     */
    function chooseFile(options: FIBJS.GeneralObject): any[];

    function chooseFile(options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Pops up a file chooser dialog
     *
     *      options supports the following parameters:
     *       - title: dialog title
     *       - type: dialog type, "openFile", "openDirectory" or "saveFile", default is "openFile"
     *       - defaultPath: the default path to open
     *       - multiple: whether multiple selection is allowed, default is false
     *       - filters: file filter array; each element is an object containing the name and extensions properties, where extensions is an array of extensions
     *
     *      @param options file chooser dialog parameters
     *      @return returns the array of files chosen by the user
     *
     */
    function chooseFileSync(options: FIBJS.GeneralObject): any[];

    /**
     * @description Pops up a file chooser dialog
     *
     *      options supports the following parameters:
     *       - title: dialog title
     *       - type: dialog type, "openFile", "openDirectory" or "saveFile", default is "openFile"
     *       - defaultPath: the default path to open
     *       - multiple: whether multiple selection is allowed, default is false
     *       - filters: file filter array; each element is an object containing the name and extensions properties, where extensions is an array of extensions
     *
     *      @param options file chooser dialog parameters
     *      @return returns the array of files chosen by the user
     *
     */
    function chooseFileAsync(options: FIBJS.GeneralObject): Promise<any[]>;

}

