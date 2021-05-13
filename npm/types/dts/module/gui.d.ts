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
     * @description 设置 gui 全局参数，config 只对 cef gui 有效，不会影响 native webview
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "cache_path": "", // 设置数据持久化目录，缺省为空，退出进程后，客户端临时数据将不会被保存
     *          "cef_path": "", // 设置 runtime 路径，缺省时在当前目录寻找 runtime
     *          "debug": true, // 全局设置是否输出 WebView 内的错误和 console 信息，缺省显示
     *          "popup": true, // 全局设置是否允许弹出新的网页，缺省允许
     *          "menu": true, // 全局设置是否允许右键菜单，缺省允许
     *          "headless": false, // 设置 gui 全局无窗口，缺省不使用无窗口，当命令行指定 --headless 时，此属性将被忽略
     *          "backend": { // 设置 cef 后端处理器，可用于处理 WebView 内部请求
     *              "http://app1/": handler1,
     *              "fibjs://app2/": handler2,
     *          },
     *          "proxy": { // 全局设置 cef 访问代理
     *              "mode": "fixed_servers|pac_script|auto_detect|system|direct", // 指定代理模式
     *              "server": "localhost:8888" // 指定代理服务器地址，当 mode 为 pac_script 时，指定 pac url
     *          }
     *      }
     *      ```
     *      @param opt 指定设置的参数
     * 	
     */
    function config(opt?: FIBJS.GeneralObject): void;

    /**
     * @description 打开一个窗口并访问指定网址
     * 
     *      支持以下参数:
     *      ```JavaScript
     *      {
     *          "type": "cef", // 指定 WenView 引擎，允许值为 "native" 和 "cef"，缺省为 "cef"
     *          "left": 100, // 窗口左上角 x，缺省系统自动设定
     *          "right": 100, // 窗口左上角 y，缺省系统自动设定
     *          "width": 100, // 窗口宽度，缺省系统自动设定
     *          "height": 100, // 窗口高度，缺省系统自动设定
     *          "border": true, // 是否有边框，缺省有边框
     *          "caption": true, // 是否有标题栏，缺省有标题栏
     *          "resizable": true, // 是否可改变尺寸，缺省可以改变
     *          "maximize": false, // 是否最大化显示，缺省不最大化
     *          "fullscreen": false, // 是否显示全屏窗口，缺省不全屏
     *          "debug": true, // 是否输出 WebView 内的错误和 console 信息，缺省显示或继承全局设置
     * 
     *          // 以下配置仅用于 cef 模式
     *          "popup": true, // 是否允许弹出新的网页，缺省允许或继承全局设置
     *          "menu": true, // 是否允许右键菜单，缺省允许或继承全局设置
     *          "headless": false, // 是否使用无窗口模式加载页面，缺省不使用无窗口，当命令行指定 --headless 时，此属性将被忽略
     *          "proxy": { // 设置访问代理，为设置时继承s全局设置
     *              "mode": "fixed_servers|pac_script|auto_detect|system|direct", // 指定代理模式
     *              "server": "localhost:8888" // 指定代理服务器地址，当 mode 为 pac_script 时，指定 pac url
     *          }
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
     *          "left": 100, // 窗口左上角 x，缺省系统自动设定
     *          "right": 100, // 窗口左上角 y，缺省系统自动设定
     *          "width": 100, // 窗口宽度，缺省系统自动设定
     *          "height": 100, // 窗口高度，缺省系统自动设定
     *          "border": true, // 是否有边框，缺省有边框
     *          "caption": true, // 是否有标题栏，缺省有标题栏
     *          "resizable": true, // 是否可改变尺寸，缺省可以改变
     *          "maximize": false, // 是否最大化显示，缺省不最大化
     *          "fullscreen": false, // 是否显示全屏窗口，缺省不全屏
     *          "debug": true, // 是否输出 WebView 内的错误和 console 信息，缺省显示或继承全局设置
     * 
     *          // 以下配置仅用于 cef 模式
     *          "popup": true, // 是否允许弹出新的网页，缺省允许或继承全局设置
     *          "menu": true, // 是否允许右键菜单，缺省允许或继承全局设置
     *          "headless": false, // 是否使用无窗口模式加载页面，缺省不使用无窗口，当命令行指定 --headless 时，此属性将被忽略
     *          "proxy": { // 设置访问代理，为设置时继承s全局设置
     *              "mode": "fixed_servers|pac_script|auto_detect|system|direct", // 指定代理模式
     *              "server": "localhost:8888" // 指定代理服务器地址，当 mode 为 pac_script 时，指定 pac url
     *          }
     *      }
     *      ```
     *      当设定 width 和 height，而未设定 left 或 right 时，窗口将自动居中
     *      @param opt 打开窗口参数
     *      @return 返回打开的窗口对象
     * 	
     */
    function open(opt?: FIBJS.GeneralObject): Class_WebView;

}

