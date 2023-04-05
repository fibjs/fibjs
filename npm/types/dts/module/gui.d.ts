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
     * @description WebView ie 模拟版本，指定 ie7 
     */
    export const IE7: 7000;

    /**
     * @description WebView ie 模拟版本，指定 ie8 
     */
    export const IE8: 8000;

    /**
     * @description WebView ie 模拟版本，指定 ie9 
     */
    export const IE9: 9000;

    /**
     * @description WebView ie 模拟版本，指定 ie10 
     */
    export const IE10: 10000;

    /**
     * @description WebView ie 模拟版本，指定 ie11 
     */
    export const IE11: 11000;

    /**
     * @description WebView ie 模拟版本，指定 edge 
     */
    export const EDGE: 11001;

    /**
     *  设置 WebView 内 ie 最高模拟版本，当系统 ie 版本低于此版本时，将模拟系统安装版本
     *      @param ver 指定 ie 模拟版本
     *     
     */
    function setVersion(ver: number): void;

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
     *          "border": true, // specify whether the window has border, default is true
     *          "caption": true, // specify whether the window has caption, default is true
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "debug": true, // specify whether the WebView output error and console info, default is true
     *      }
     *      ```
     *      当设定 width 和 height，而未设定 left 或 right 时，窗口将自动居中
     *      @param url 指定的网址，，可以使用 fs:path 访问本地文件系统
     *      @param opt 打开窗口参数
     *      @return 返回打开的窗口对象
     * 	
     */
    function open(url: string, opt?: FIBJS.GeneralObject): Class_WebView;

    /**
     * @description 打开一个空的浏览器窗口
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "left": 100, // specify the left position of the window, default is system auto set
     *          "right": 100, // spcify the top position of the window, default is system auto set
     *          "width": 100, // specify the width of the window, default is system auto set
     *          "height": 100, // specify the height of the window, default is system auto set
     *          "border": true, // specify whether the window has border, default is true
     *          "caption": true, // specify whether the window has caption, default is true
     *          "resizable": true, // specify whether the window is resizable, default is true
     *          "maximize": false, // specify whether the window is maximized, default is false
     *          "fullscreen": false, // specify whether the window is fullscreen, default is false
     *          "debug": true, // specify whether the WebView output error and console info, default is true
     *      }
     *      ```
     *      当设定 width 和 height，而未设定 left 或 right 时，窗口将自动居中
     *      @param opt 打开窗口参数
     *      @return 返回打开的窗口对象
     * 	
     */
    function open(opt?: FIBJS.GeneralObject): Class_WebView;

}

