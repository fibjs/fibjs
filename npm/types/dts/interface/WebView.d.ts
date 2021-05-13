/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description 浏览器窗口对象
 *  WebView 是一个嵌入浏览器的窗口组件，目前仅支持 windows, macOS(10.10+). 在 windows 使用 IE/Edge 内核, 在 macOS 使用 WKWebView.
 *  
 *  由于 WebView 内的 JavaScript 程序与 fibjs 并不在同一个引擎内，所以如果需要与宿主程序进行通讯，需要通过消息进行。
 * 
 *  WebView 用于通讯的对象是 external，external 支持一个方法 postMessage 和两个事件 onmessage、onclose。
 * 
 *  一个简单的通讯示例代码如下：
 *  ```JavaScript 
 *  // index.js
 *  var gui = require('gui');
 *  var webview = gui.open('fs://index.html');
 * 
 *  webview.onmessage = msg => console.log(msg);
 *  
 *  webview.onload = evt => webview.postMessage("hello from fibjs");
 * 
 *  webview.wait();
 *  ```
 * 
 *  index.html 的内容如下：
 *  ```html
 *  <script>
 *      external.onclose = function() {
 *      }
 * 
 *      external.onmessage = function(msg){ 
 *          external.postMessage("send back: " + msg);
 *      };
 *  </script>
 *  ```
 *  
 * 在用户窗口关闭之前，会触发 external.onclose 事件，external.onclose 可以决定是否关闭。如果 external.onclose 返回 false，则此次操作取消，否则将关闭窗口。
 * 
 *  以下的例子，会在用户点关闭后等待 5 秒后再关闭窗口。
 *  ```html
 *  <script lang="JavaScript">
 *      var bClose = false;
 *      external.onclose = function () {
 *          if (!bClose) {
 *              setTimeout(function () {
 *                  bClose = true;
 *                  window.close();
 *              }, 5000);
 *              return false;
 *          }
 *      }
 *  </script>
 *  ```
 *  上面的代码中，因为 window.close 本身也会触发 onclose 事件，所以需要增加一个开关变量，用于识别是否需要处理此次事件。
 *  
 */
declare class Class_WebView extends Class_EventEmitter {
    /**
     * @description 加载指定 url 的页面
     * 	 @param url 指定的 url
     * 	 
     */
    loadUrl(url: string): void;

    loadUrl(url: string, callback: (err: Error | undefined | null)=>any): void;

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
     * @description 打印当前窗口文档
     * 	 @param mode 打印参数，0: 快速打印; 1: 标准打印; 2: 打印预览。缺省为 1
     * 	 
     */
    print(mode?: number): void;

    print(mode?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 打印当前窗口文档到 PDF 文件
     * 	 @param file 指定 pdf 路径
     * 	 
     */
    printToPDF(file: string): void;

    printToPDF(file: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 在当前窗口运行一段 JavaScript 代码
     * 	 @param code 指定要执行的 JavaScript 代码
     * 	 
     */
    executeJavaScript(code: string): void;

    executeJavaScript(code: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 在当前窗口执行 DevTools 命令，并返回结果
     * 	 @param method 指定要执行的 DevTools 命令
     *      @param params 指定命令的参数
     *      @return 返回执行的结果
     * 	 
     */
    executeDevToolsMethod(method: string, params?: FIBJS.GeneralObject): any;

    executeDevToolsMethod(method: string, params?: FIBJS.GeneralObject, callback?: (err: Error | undefined | null, retVal: any)=>any): void;

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
     * @description 当前窗口的 WebView 引擎 
     */
    readonly type: string;

    /**
     * @description DevTools 访问对象，调用接口参见：https://chromedevtools.github.io/devtools-protocol/ 
     */
    readonly dev: any;

    /**
     * @description 查询和绑定加载成功事件，相当于 on("open", func); 
     */
    onopen: (...args: any[])=>any;

    /**
     * @description 查询和绑定加载成功事件，相当于 on("load", func); 
     */
    onload: (...args: any[])=>any;

    /**
     * @description 查询和绑定页面地址变化事件，相当于 on("address", func); 
     */
    onaddress: (...args: any[])=>any;

    /**
     * @description 查询和绑定页面标题改变事件，相当于 on("title", func); 
     */
    ontitle: (...args: any[])=>any;

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
     * @description 查询和绑定窗口关闭事件，WebView 关闭后会触发此时间，相当于 on("closed", func); 
     */
    onclosed: (...args: any[])=>any;

    /**
     * @description 查询和绑定接受 webview 内 postMessage 消息事件，相当于 on("message", func); 
     */
    onmessage: (...args: any[])=>any;

}

