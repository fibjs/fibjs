/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 浏览器窗口对象，WebView 是一个嵌入浏览器的窗口组件.
 *  
 *  ### 消息通信
 *  由于 WebView 内的 JavaScript 程序与 fibjs 并不在同一个引擎内，所以如果需要与宿主程序进行通讯，需要通过消息进行。
 * 
 *  WebView 用于通讯的对象是 window，支持方法 postMessage 和 message 事件。
 * 
 *  一个简单的通讯示例代码如下：
 *  ```JavaScript 
 *  // index.js
 *  var gui = require('gui');
 *  var webview = gui.open('https://fibjs.org/index.html');
 * 
 *  webview.addEventListener("message", function (msg) { console.log(msg); });
 *  
 *  webview.postMessage("hello from fibjs");
 * 
 *  webview.wait();
 *  ```
 * 
 *  index.html 的内容如下：
 *  ```html
 *  <script>
 *      window.addEventListener("message", function (msg) { 
 *          window.postMessage("send back: " + msg);
 *      });
 *  </script>
 *  ```
 *  ### 关闭窗口
 *  如果需要在 WebView 内关闭窗口，可以调用 window.close。
 *  ```html
 *  <script lang="JavaScript">
 *     document.getElementById('close').addEventListener('click', function () {
 *         window.close();
 *     });
 *  </script>
 *  ```
 *  ### 拖动窗口
 *  在有些应用里，需要在 WebView 内实现拖动窗口的功能，可以通过以下代码实现：
 *  ```html
 *  <script>
 *     document.getElementById('dragRegion').addEventListener('mousedown', function (event) {
 *         if (event.button === 0) { // Check if the left mouse button is pressed
 *             window.drag();
 *         }
 *     });
 *  </script>
 *  ```
 *  
 */
declare class Class_WebView extends Class_EventEmitter {
    /**
     * @description 加载指定 url 的页面
     * 	 @param url 指定的 url
     * 	 
     */
    loadURL(url: string): void;

    loadURL(url: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 加载指定文件的页面
     *      @param file 指定的文件
     *      
     */
    loadFile(file: string): void;

    loadFile(file: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询当前页面的 url
     * 	 @return 返回当前页面的 url
     * 	 
     */
    getUrl(): string;

    getUrl(callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 设置 webview 的页面 html
     * 	 @param html 设置的 html
     * 	 
     */
    setHtml(html: string): void;

    setHtml(html: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 获取 webview 的页面 html
     *      @return 返回 webview 的页面 html
     *     
     */
    getHtml(): string;

    getHtml(callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 查询当前页面是否加载完成
     *      @return 返回当前页面是否加载完成
     *     
     */
    isReady(): boolean;

    isReady(callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 刷新当前页面 
     */
    reload(): void;

    reload(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 退回到上一个页面 
     */
    goBack(): void;

    goBack(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 前进到下一个页面 
     */
    goForward(): void;

    goForward(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 在当前窗口运行一段 JavaScript 代码
     * 	 @param code 指定要执行的 JavaScript 代码
     *      @return 返回执行结果
     * 	 
     */
    eval(code: string): any;

    eval(code: string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 设置窗口的标题
     *      @param title 指定窗口的标题
     *     
     */
    setTitle(title: string): void;

    setTitle(title: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询窗口的标题
     *      @return 返回窗口的标题
     *     
     */
    getTitle(): string;

    getTitle(callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 设置窗口的尺寸
     *      @param width 指定窗口的宽度
     *      @param height 指定窗口的高度
     *     
     */
    setSize(width: number, height: number): void;

    setSize(width: number, height: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询窗口的尺寸
     *      @return 返回窗口的尺寸，返回值为一个数组，第一个元素为宽度，第二个元素为高度
     *     
     */
    getSize(): any[];

    getSize(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 设置窗口的位置
     *      @param left 指定窗口的左上角 x 坐标
     *      @param top 指定窗口的左上角 y 坐标
     *     
     */
    setPosition(left: number, top: number): void;

    setPosition(left: number, top: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询窗口的位置
     *      @return 返回窗口的位置，返回值为一个数组，第一个元素为 x 坐标，第二个元素为 y 坐标
     *     
     */
    getPosition(): any[];

    getPosition(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 查询窗口是否是激活窗口
     *      @return 返回窗口是否是激活窗口
     *     
     */
    isActived(): boolean;

    isActived(callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 激活窗口 
     */
    active(): void;

    active(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询窗口的菜单
     *      @return 返回窗口的菜单
     *     
     */
    getMenu(): Class_Menu;

    /**
     * @description 截取当前窗口的图像
     *      @return 返回截取的图像
     *     
     */
    capturePage(): Class_Buffer;

    capturePage(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 关闭当前窗口 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 向 webview 内发送消息
     *      postMessage 需要在窗口加载完成后发送消息，在此之前发送的消息会丢失。因此建议在 onload 事件触发后再调用此方法。
     * 	 @param msg 要发送的消息
     * 	
     */
    postMessage(msg: string): void;

    postMessage(msg: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询和绑定窗口开始加载事件，相当于 on("loading", func); 
     */
    onloading: (...args: any[])=>any;

    /**
     * @description 查询和绑定窗口加载 完成事件，相当于 on("load", func); 
     */
    onload: (...args: any[])=>any;

    /**
     * @description 查询和绑定窗口移动事件，相当于 on("move", func); 
     * 
     * 	 以下示例会在窗口移动时输出窗口的左上角坐标：
     * 	 ```JavaScript 
     * 	 var gui = require('gui');
     * 	 var webview = gui.open('fs://index.html');
     * 
     * 	 webview.onmove = evt => console.log(evt.left, evt.top);
     * 	 ```
     * 	
     */
    onmove: (...args: any[])=>any;

    /**
     * @description 查询和绑定窗口尺寸改变事件，相当于 on("size", func);
     * 
     *      以下示例会在窗口改变大小时输出窗口的尺寸：
     *      ```JavaScript
     *      var gui = require('gui');
     *      var webview = gui.open('fs://index.html');
     * 
     *      webview.onresize = evt => console.log(evt.width, evt.height);
     *      ```
     * 	
     */
    onresize: (...args: any[])=>any;

    /**
     * @description 查询和绑定窗口获得焦点事件，相当于 on("focus", func); 
     */
    onfocus: (...args: any[])=>any;

    /**
     * @description 查询和绑定窗口失去焦点事件，相当于 on("blur", func); 
     */
    onblur: (...args: any[])=>any;

    /**
     * @description 查询和绑定窗口关闭事件，WebView 关闭后会触发此时间，相当于 on("closed", func); 
     */
    onclose: (...args: any[])=>any;

    /**
     * @description 查询和绑定接受 webview 内 postMessage 消息事件，相当于 on("message", func); 
     */
    onmessage: (...args: any[])=>any;

}

