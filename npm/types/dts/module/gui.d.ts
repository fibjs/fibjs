/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/WebView.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/// <reference path="../interface/Tray.d.ts" />
/**
 * @description gui 模块
 * 
 * 使用方法：
 * ```JavaScript
 * var gui = require('gui');
 * ```
 *  
 */
declare module 'gui' {
    /**
     * @description 浏览器窗口对象，WebView 是一个嵌入浏览器的窗口组件 
     */
    const WebView: typeof Class_WebView;

    /**
     * @description 打开一个窗口并访问指定网址
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "icon": "/path/to/file.png", // specify the icon of the window, not work in gtk4
     *          "left": 100, // specify the left position of the window, default position is center of the screen, not work in gtk4
     *          "right": 100, // spcify the top position of the window, default position is center of the screen, not work in gtk4
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "frame": true, // specify whether the window has frame, default is true
     *          "caption": true, // specify whether the window has caption, default is true
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "menu": menu, // specify the menu of the window, can be a Menu object or a menu item array, default is null
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "devtools": false, // specify whether the DevTools in WebView is enabled, default is false
     *      }
     *      ```
     *      当设定 width 和 height，而未设定 left 或 right 时，窗口将自动居中
     *      @param url 指定的网址
     *      @param opt 打开窗口参数
     *      @return 返回打开的窗口对象
     * 	
     */
    function open(url: string, opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description 打开一个窗口并访问指定文件
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "icon": "/path/to/file.png", // specify the icon of the window, not work in gtk4
     *          "left": 100, // specify the left position of the window, default position is center of the screen, not work in gtk4
     *          "right": 100, // spcify the top position of the window, default position is center of the screen, not work in gtk4
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "frame": true, // specify whether the window has frame, default is true
     *          "caption": true, // specify whether the window has caption, default is true
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "menu": menu, // specify the menu of the window, can be a Menu object or a menu item array, default is null
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "devtools": false, // specify whether the DevTools in WebView is enabled, default is false
     *      }
     *      ```
     *      当设定 width 和 height，而未设定 left 或 right 时，窗口将自动居中
     *      @param file 指定的文件
     *      @param opt 打开窗口参数
     *      @return 返回打开的窗口对象
     * 	
     */
    function openFile(file: string, opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description 打开一个浏览器窗口，如果指定 url 或者 file 则加载指定资源
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "url": , // specify the url of the window, default is about:blank
     *          "file": , // specify the file of the window
     *          "icon": "/path/to/file.png", // specify the icon of the window, not work in gtk4
     *          "left": 100, // specify the left position of the window, default position is center of the screen, not work in gtk4
     *          "right": 100, // spcify the top position of the window, default position is center of the screen, not work in gtk4
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "frame": true, // specify whether the window has frame, default is true
     *          "caption": true, // specify whether the window has caption, default is true
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "menu": menu, // specify the menu of the window, can be a Menu object or a menu item array, default is null
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "devtools": false, // specify whether the DevTools in WebView is enabled, default is false
     *      }
     *      ```
     *      当设定 width 和 height，而未设定 left 或 right 时，窗口将自动居中
     *      @param opt 打开窗口参数
     *      @return 返回打开的窗口对象
     * 	
     */
    function open(opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description 创建一个菜单对象
     * 
     *     菜单项支持以下类型：
     *     - normal
     *         - type: "normal"
     *         - label: 必需
     *         - tooltip, icon, enabled: 可选
     *         - 不能有 submenu 或 checked
     *     - checkbox
     *         - type: "checkbox"
     *         - label: 必需
     *         - checked: 可选
     *         - tooltip, icon, enabled: 可选
     *         - 不能有 submenu
     *     - submenu
     *         - type: "submenu"
     *         - label, submenu: 必需
     *         - tooltip, icon, enabled: 可选
     *         - 不能有 checked
     *     - separator
     *         - type: "separator"
     *         - 不能有 label、submenu、checked、icon 或 tooltip
     * 
     *     如果菜单项未指定 type，则根据其它属性自动判断类型。识别策略如下：
     *     - 如果存在 submenu 属性，则将 type 设置为 "submenu"。
     *     - 如果存在 checked 属性，则将 type 设置为 "checkbox"。
     *     - 如果传入的对象为空，则将 type 设置为 "separator"。
     *     - 如果以上条件都不满足，则将 type 设置为 "normal"。
     * 
     *      @param items 菜单项数组
     *      @return 返回创建的菜单对象
     *     
     */
    function createMenu(items?: any[]): Class_Menu;

    /**
     * @description 创建一个状态图标对象
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "icon": "/path/to/file.png", // specify the icon of the tray, must be a png file
     *          "title": "", // specify the title of the tray, if not set, it will not be displayed
     *          "tooltip": "", // specify the tooltip of the tray, if not set, it will not be displayed
     *          "menu": menu, // specify the menu of the tray, default is null
     *      }
     *      ```
     *      @param opt 创建状态图标参数
     *      @return 返回创建的状态图标对象
     *     
     */
    function createTray(opt?: FIBJS.GeneralObject): Class_Tray;

}

