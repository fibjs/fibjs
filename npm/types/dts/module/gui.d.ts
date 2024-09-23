/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/WebView.d.ts" />
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
     * @description 打开一个窗口并访问指定网址
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "left": 100, // specify the left position of the window, default is system auto set
     *          "right": 100, // spcify the top position of the window, default is system auto set
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "frame": true, // specify whether the window has frame, default is true
     *          "caption": true, // specify whether the window has caption, default is true
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "debug": false, // specify whether the DevTools in WebView is enabled, default is false
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
     *          "left": 100, // specify the left position of the window, default is system auto set
     *          "right": 100, // spcify the top position of the window, default is system auto set
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "frame": true, // specify whether the window has frame, default is true
     *          "caption": true, // specify whether the window has caption, default is true
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "debug": false, // specify whether the DevTools in WebView is enabled, default is false
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
     *          "left": 100, // specify the left position of the window, default is system auto set
     *          "right": 100, // spcify the top position of the window, default is system auto set
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "frame": true, // specify whether the window has frame, default is true
     *          "caption": true, // specify whether the window has caption, default is true
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "debug": false, // specify whether the DevTools in WebView is enabled, default is false
     *      }
     *      ```
     *      当设定 width 和 height，而未设定 left 或 right 时，窗口将自动居中
     *      @param opt 打开窗口参数
     *      @return 返回打开的窗口对象
     * 	
     */
    function open(opt?: FIBJS.GeneralObject): Class_WebView;

}

