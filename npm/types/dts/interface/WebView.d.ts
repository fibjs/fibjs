/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description WebView object, an embedded browser window component.
 *
 *  WebView is a window component with an embedded browser. Since the JavaScript code inside a WebView does not run in the same engine as fibjs, they communicate through messages.
 *
 *  Inside a WebView, you can communicate with fibjs through window; the postMessage method and the message event are supported.
 *
 *  The following is a simple communication example:
 *  ```JavaScript
 *  // index.js
 *  var gui = require('gui');
 *  var webview = gui.open('https://fibjs.org/index.html');
 *
 *  webview.addEventListener("message", function (msg) { console.log(msg); });
 *
 *  webview.postMessage("hello from fibjs");
 *  ```
 *
 *  The content of index.html is as follows:
 *  ```html
 *  <script>
 *      window.addEventListener("message", function (msg) {
 *          window.postMessage("send back: " + msg);
 *      });
 *  </script>
 *  ```
 *
 *  WebView also supports a more convenient app API interface. The object used for API calls inside a WebView is window.app; you can specify the API interface through the app parameter when creating the WebView, and its methods can be called inside the WebView with await window.app.<method>.
 *
 *  The following is a simple call example:
 *  ```JavaScript
 *  const gui = require('gui');
 *  const coroutine = require('coroutine');
 *
 *  const win = gui.open({
 *      devtools: true,
 *      app: {
 *          test: async function (a, b, c, d) {
 *              console.log('test', a, b, c, d);
 *              await coroutine.sleepAsync(1000);
 *              return a + b + c + d + 1000;
 *          },
 *          test1: {
 *              test2: function (a, b, c, d) {
 *                  console.log('test2', a, b, c, d);
 *                  coroutine.sleep(1000);
 *                  return a + b + c + d + 2000;
 *              }
 *          }
 *      }
 *  });
 *
 *  win.eval(`
 *  (async function test() {
 *      console.log("test(1,2,3,4): " + await window.app.test(1,2,3,4));
 *      console.log("test1.test2(1,2,3,4): " + await window.app.test1.test2(1,2,3,4));
 *      console.log('test');
 *  })();`);
 *  ```
 *
 *  If you need to close the window from inside the WebView, call window.close. Note that a fullscreen window on macOS is prevented from closing by the macOS mechanism.
 *  ```html
 *  <script lang="JavaScript">
 *     document.getElementById('close').addEventListener('click', function () {
 *         window.close();
 *     });
 *  </script>
 *  ```
 *  In some applications, you may need to implement window dragging inside the WebView; this can be done with the following code:
 *  ```html
 *  <script>
 *     document.getElementById('dragRegion').addEventListener('mousedown', function (event) {
 *         if (event.button === 0) { // Check if left button is pressed
 *             window.drag();
 *         }
 *     });
 *  </script>
 *  ```
 *
 */
declare class Class_WebView extends Class_EventEmitter {
    /**
     * @description Loads the page at the specified url
     * 	 @param url the url to load
     *
     */
    loadUrl(url: string): void;

    loadUrl(url: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Loads the page at the specified url
     * 	 @param url the url to load
     *
     */
    loadUrlSync(url: string): void;

    /**
     * @description Loads the page at the specified url
     * 	 @param url the url to load
     *
     */
    loadUrlAsync(url: string): Promise<void>;

    /**
     * @description Loads the page of the specified file
     *      @param file the file to load
     *
     */
    loadFile(file: string): void;

    loadFile(file: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Loads the page of the specified file
     *      @param file the file to load
     *
     */
    loadFileSync(file: string): void;

    /**
     * @description Loads the page of the specified file
     *      @param file the file to load
     *
     */
    loadFileAsync(file: string): Promise<void>;

    /**
     * @description Queries the url of the current page
     * 	 @return returns the url of the current page
     *
     */
    getUrl(): string;

    getUrl(callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Queries the url of the current page
     * 	 @return returns the url of the current page
     *
     */
    getUrlSync(): string;

    /**
     * @description Queries the url of the current page
     * 	 @return returns the url of the current page
     *
     */
    getUrlAsync(): Promise<string>;

    /**
     * @description Sets the page html of the webview
     * 	 @param html the html to set
     *
     */
    setHtml(html: string): void;

    setHtml(html: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets the page html of the webview
     * 	 @param html the html to set
     *
     */
    setHtmlSync(html: string): void;

    /**
     * @description Sets the page html of the webview
     * 	 @param html the html to set
     *
     */
    setHtmlAsync(html: string): Promise<void>;

    /**
     * @description Gets the page html of the webview
     *      @return returns the page html of the webview
     *
     */
    getHtml(): string;

    getHtml(callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Gets the page html of the webview
     *      @return returns the page html of the webview
     *
     */
    getHtmlSync(): string;

    /**
     * @description Gets the page html of the webview
     *      @return returns the page html of the webview
     *
     */
    getHtmlAsync(): Promise<string>;

    /**
     * @description Queries whether the current page has finished loading
     *      @return returns whether the current page has finished loading
     *
     */
    isReady(): boolean;

    isReady(callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Queries whether the current page has finished loading
     *      @return returns whether the current page has finished loading
     *
     */
    isReadySync(): boolean;

    /**
     * @description Queries whether the current page has finished loading
     *      @return returns whether the current page has finished loading
     *
     */
    isReadyAsync(): Promise<boolean>;

    /**
     * @description Waits for the current page to finish loading
     *      @param url the url to wait for; empty means waiting for the current page
     *
     */
    waitFor(url?: string): void;

    waitFor(url?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Waits for the current page to finish loading
     *      @param url the url to wait for; empty means waiting for the current page
     *
     */
    waitForSync(url?: string): void;

    /**
     * @description Waits for the current page to finish loading
     *      @param url the url to wait for; empty means waiting for the current page
     *
     */
    waitForAsync(url?: string): Promise<void>;

    /**
     * @description Refreshes the current page
     */
    reload(): void;

    reload(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Refreshes the current page
     */
    reloadSync(): void;

    /**
     * @description Refreshes the current page
     */
    reloadAsync(): Promise<void>;

    /**
     * @description Goes back to the previous page
     */
    goBack(): void;

    goBack(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Goes back to the previous page
     */
    goBackSync(): void;

    /**
     * @description Goes back to the previous page
     */
    goBackAsync(): Promise<void>;

    /**
     * @description Goes forward to the next page
     */
    goForward(): void;

    goForward(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Goes forward to the next page
     */
    goForwardSync(): void;

    /**
     * @description Goes forward to the next page
     */
    goForwardAsync(): Promise<void>;

    /**
     * @description Runs a piece of JavaScript code in the current window
     * 	 @param code the JavaScript code to execute
     *      @return returns the execution result
     *
     */
    eval(code: string): any;

    eval(code: string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Runs a piece of JavaScript code in the current window
     * 	 @param code the JavaScript code to execute
     *      @return returns the execution result
     *
     */
    evalSync(code: string): any;

    /**
     * @description Runs a piece of JavaScript code in the current window
     * 	 @param code the JavaScript code to execute
     *      @return returns the execution result
     *
     */
    evalAsync(code: string): Promise<any>;

    /**
     * @description Sets the title of the window
     *      @param title the title of the window
     *
     */
    setTitle(title: string): void;

    setTitle(title: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets the title of the window
     *      @param title the title of the window
     *
     */
    setTitleSync(title: string): void;

    /**
     * @description Sets the title of the window
     *      @param title the title of the window
     *
     */
    setTitleAsync(title: string): Promise<void>;

    /**
     * @description Queries the title of the window
     *      @return returns the title of the window
     *
     */
    getTitle(): string;

    getTitle(callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Queries the title of the window
     *      @return returns the title of the window
     *
     */
    getTitleSync(): string;

    /**
     * @description Queries the title of the window
     *      @return returns the title of the window
     *
     */
    getTitleAsync(): Promise<string>;

    /**
     * @description Sets whether the window is visible
     *      @return returns whether the window is visible
     *
     */
    isVisible(): boolean;

    isVisible(callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Sets whether the window is visible
     *      @return returns whether the window is visible
     *
     */
    isVisibleSync(): boolean;

    /**
     * @description Sets whether the window is visible
     *      @return returns whether the window is visible
     *
     */
    isVisibleAsync(): Promise<boolean>;

    /**
     * @description Shows the window
     */
    show(): void;

    show(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Shows the window
     */
    showSync(): void;

    /**
     * @description Shows the window
     */
    showAsync(): Promise<void>;

    /**
     * @description Hides the window
     */
    hide(): void;

    hide(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Hides the window
     */
    hideSync(): void;

    /**
     * @description Hides the window
     */
    hideAsync(): Promise<void>;

    /**
     * @description Sets the size of the window
     *      @param width the width of the window
     *      @param height the height of the window
     *
     */
    setSize(width: number, height: number): void;

    setSize(width: number, height: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets the size of the window
     *      @param width the width of the window
     *      @param height the height of the window
     *
     */
    setSizeSync(width: number, height: number): void;

    /**
     * @description Sets the size of the window
     *      @param width the width of the window
     *      @param height the height of the window
     *
     */
    setSizeAsync(width: number, height: number): Promise<void>;

    /**
     * @description Queries the size of the window
     *      @return returns the size of the window as an array whose first element is the width and second element is the height
     *
     */
    getSize(): any[];

    getSize(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Queries the size of the window
     *      @return returns the size of the window as an array whose first element is the width and second element is the height
     *
     */
    getSizeSync(): any[];

    /**
     * @description Queries the size of the window
     *      @return returns the size of the window as an array whose first element is the width and second element is the height
     *
     */
    getSizeAsync(): Promise<any[]>;

    /**
     * @description Sets the position of the window
     *      @param left the x coordinate of the top-left corner of the window
     *      @param top the y coordinate of the top-left corner of the window
     *
     */
    setPosition(left: number, top: number): void;

    setPosition(left: number, top: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets the position of the window
     *      @param left the x coordinate of the top-left corner of the window
     *      @param top the y coordinate of the top-left corner of the window
     *
     */
    setPositionSync(left: number, top: number): void;

    /**
     * @description Sets the position of the window
     *      @param left the x coordinate of the top-left corner of the window
     *      @param top the y coordinate of the top-left corner of the window
     *
     */
    setPositionAsync(left: number, top: number): Promise<void>;

    /**
     * @description Queries the position of the window
     *      @return returns the position of the window as an array whose first element is the x coordinate and second element is the y coordinate
     *
     */
    getPosition(): any[];

    getPosition(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Queries the position of the window
     *      @return returns the position of the window as an array whose first element is the x coordinate and second element is the y coordinate
     *
     */
    getPositionSync(): any[];

    /**
     * @description Queries the position of the window
     *      @return returns the position of the window as an array whose first element is the x coordinate and second element is the y coordinate
     *
     */
    getPositionAsync(): Promise<any[]>;

    /**
     * @description Queries whether the window is the active window
     *      @return returns whether the window is the active window
     *
     */
    isActived(): boolean;

    isActived(callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Queries whether the window is the active window
     *      @return returns whether the window is the active window
     *
     */
    isActivedSync(): boolean;

    /**
     * @description Queries whether the window is the active window
     *      @return returns whether the window is the active window
     *
     */
    isActivedAsync(): Promise<boolean>;

    /**
     * @description Activates the window
     */
    active(): void;

    active(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Activates the window
     */
    activeSync(): void;

    /**
     * @description Activates the window
     */
    activeAsync(): Promise<void>;

    /**
     * @description Queries the menu of the window
     *      @return returns the menu of the window
     *
     */
    getMenu(): Class_Menu;

    /**
     * @description Captures an image of the current window
     *
     *      Page capture works for most pages, but for lazily loaded pages the full content may not be captured. It is recommended to test on the page to be captured, and to actively adjust the window size when necessary to trigger page loading.
     *      @param fullPage whether to capture the whole page; the default is false, which captures only the visible area
     *      @return returns the captured image
     *
     */
    takeScreenshot(fullPage?: boolean): Class_Buffer;

    takeScreenshot(fullPage?: boolean, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Captures an image of the current window
     *
     *      Page capture works for most pages, but for lazily loaded pages the full content may not be captured. It is recommended to test on the page to be captured, and to actively adjust the window size when necessary to trigger page loading.
     *      @param fullPage whether to capture the whole page; the default is false, which captures only the visible area
     *      @return returns the captured image
     *
     */
    takeScreenshotSync(fullPage?: boolean): Class_Buffer;

    /**
     * @description Captures an image of the current window
     *
     *      Page capture works for most pages, but for lazily loaded pages the full content may not be captured. It is recommended to test on the page to be captured, and to actively adjust the window size when necessary to trigger page loading.
     *      @param fullPage whether to capture the whole page; the default is false, which captures only the visible area
     *      @return returns the captured image
     *
     */
    takeScreenshotAsync(fullPage?: boolean): Promise<Class_Buffer>;

    /**
     * @description Closes the current window
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the current window
     */
    closeSync(): void;

    /**
     * @description Closes the current window
     */
    closeAsync(): Promise<void>;

    /**
     * @description Sends a message into the webview
     *      postMessage must be sent after the window has finished loading; messages sent before that are lost. Therefore it is recommended to call this method only after the onload event fires.
     * 	 @param msg the message to send
     *
     */
    postMessage(msg: string): void;

    postMessage(msg: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sends a message into the webview
     *      postMessage must be sent after the window has finished loading; messages sent before that are lost. Therefore it is recommended to call this method only after the onload event fires.
     * 	 @param msg the message to send
     *
     */
    postMessageSync(msg: string): void;

    /**
     * @description Sends a message into the webview
     *      postMessage must be sent after the window has finished loading; messages sent before that are lost. Therefore it is recommended to call this method only after the onload event fires.
     * 	 @param msg the message to send
     *
     */
    postMessageAsync(msg: string): Promise<void>;

    /**
     * @description Queries and binds the window load start event, equivalent to on("loading", func);
     */
    on(event: "loading", listener: ()=>void): this;

    once(event: "loading", listener: ()=>void): this;

    off(event: "loading", listener: ()=>void): this;

    addListener(event: "loading", listener: ()=>void): this;

    removeListener(event: "loading", listener: ()=>void): this;

    addEventListener(event: "loading", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "loading", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "loading", listener: ()=>void): this;

    prependOnceListener(event: "loading", listener: ()=>void): this;

    /**
     * @description Queries and binds the window load start event, equivalent to on("loading", func);
     */
    onloading: (()=>void) | null;

    /**
     * @description Queries and binds the window load completed event, equivalent to on("load", func);
     */
    on(event: "load", listener: ()=>void): this;

    once(event: "load", listener: ()=>void): this;

    off(event: "load", listener: ()=>void): this;

    addListener(event: "load", listener: ()=>void): this;

    removeListener(event: "load", listener: ()=>void): this;

    addEventListener(event: "load", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "load", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "load", listener: ()=>void): this;

    prependOnceListener(event: "load", listener: ()=>void): this;

    /**
     * @description Queries and binds the window load completed event, equivalent to on("load", func);
     */
    onload: (()=>void) | null;

    /**
     * @description Queries and binds the window move event, equivalent to on("move", func);
     *
     * 	 The following example outputs the top-left corner coordinates of the window when it moves:
     * 	 ```JavaScript
     * 	 var gui = require('gui');
     * 	 var webview = gui.open('fs://index.html');
     *
     * 	 webview.onmove = evt => console.log(evt.left, evt.top);
     * 	 ```
     *
     */
    on(event: "move", listener: ()=>void): this;

    once(event: "move", listener: ()=>void): this;

    off(event: "move", listener: ()=>void): this;

    addListener(event: "move", listener: ()=>void): this;

    removeListener(event: "move", listener: ()=>void): this;

    addEventListener(event: "move", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "move", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "move", listener: ()=>void): this;

    prependOnceListener(event: "move", listener: ()=>void): this;

    /**
     * @description Queries and binds the window move event, equivalent to on("move", func);
     *
     * 	 The following example outputs the top-left corner coordinates of the window when it moves:
     * 	 ```JavaScript
     * 	 var gui = require('gui');
     * 	 var webview = gui.open('fs://index.html');
     *
     * 	 webview.onmove = evt => console.log(evt.left, evt.top);
     * 	 ```
     *
     */
    onmove: (()=>void) | null;

    /**
     * @description Queries and binds the window size change event, equivalent to on("size", func);
     *
     *      The following example outputs the size of the window when it is resized:
     *      ```JavaScript
     *      var gui = require('gui');
     *      var webview = gui.open('fs://index.html');
     *
     *      webview.onresize = evt => console.log(evt.width, evt.height);
     *      ```
     *
     */
    on(event: "resize", listener: ()=>void): this;

    once(event: "resize", listener: ()=>void): this;

    off(event: "resize", listener: ()=>void): this;

    addListener(event: "resize", listener: ()=>void): this;

    removeListener(event: "resize", listener: ()=>void): this;

    addEventListener(event: "resize", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "resize", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "resize", listener: ()=>void): this;

    prependOnceListener(event: "resize", listener: ()=>void): this;

    /**
     * @description Queries and binds the window size change event, equivalent to on("size", func);
     *
     *      The following example outputs the size of the window when it is resized:
     *      ```JavaScript
     *      var gui = require('gui');
     *      var webview = gui.open('fs://index.html');
     *
     *      webview.onresize = evt => console.log(evt.width, evt.height);
     *      ```
     *
     */
    onresize: (()=>void) | null;

    /**
     * @description Queries and binds the window focus event, equivalent to on("focus", func);
     */
    on(event: "focus", listener: ()=>void): this;

    once(event: "focus", listener: ()=>void): this;

    off(event: "focus", listener: ()=>void): this;

    addListener(event: "focus", listener: ()=>void): this;

    removeListener(event: "focus", listener: ()=>void): this;

    addEventListener(event: "focus", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "focus", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "focus", listener: ()=>void): this;

    prependOnceListener(event: "focus", listener: ()=>void): this;

    /**
     * @description Queries and binds the window focus event, equivalent to on("focus", func);
     */
    onfocus: (()=>void) | null;

    /**
     * @description Queries and binds the window blur event, equivalent to on("blur", func);
     */
    on(event: "blur", listener: ()=>void): this;

    once(event: "blur", listener: ()=>void): this;

    off(event: "blur", listener: ()=>void): this;

    addListener(event: "blur", listener: ()=>void): this;

    removeListener(event: "blur", listener: ()=>void): this;

    addEventListener(event: "blur", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "blur", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "blur", listener: ()=>void): this;

    prependOnceListener(event: "blur", listener: ()=>void): this;

    /**
     * @description Queries and binds the window blur event, equivalent to on("blur", func);
     */
    onblur: (()=>void) | null;

    /**
     * @description Queries and binds the window close event, which fires after the WebView is closed, equivalent to on("closed", func);
     */
    on(event: "close", listener: ()=>void): this;

    once(event: "close", listener: ()=>void): this;

    off(event: "close", listener: ()=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    /**
     * @description Queries and binds the window close event, which fires after the WebView is closed, equivalent to on("closed", func);
     */
    onclose: (()=>void) | null;

    /**
     * @description Queries and binds the event for receiving postMessage messages from inside the webview, equivalent to on("message", func);
     */
    on(event: "message", listener: ()=>void): this;

    once(event: "message", listener: ()=>void): this;

    off(event: "message", listener: ()=>void): this;

    addListener(event: "message", listener: ()=>void): this;

    removeListener(event: "message", listener: ()=>void): this;

    addEventListener(event: "message", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: ()=>void): this;

    prependOnceListener(event: "message", listener: ()=>void): this;

    /**
     * @description Queries and binds the event for receiving postMessage messages from inside the webview, equivalent to on("message", func);
     */
    onmessage: (()=>void) | null;

    /**
     * @description Keeps the fibjs process alive; prevents the fibjs process from exiting while the object is bound
     *      @return returns the current object
     *
     */
    ref(): Class_WebView;

    /**
     * @description Allows the fibjs process to exit; permits the fibjs process to exit while the object is bound
     *      @return returns the current object
     *
     */
    unref(): Class_WebView;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the WebView class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_WebViewPromise extends Class_EventEmitter {
    /**
     * @description Loads the page at the specified url
     * 	 @param url the url to load
     *
     */
    loadUrl(url: string): Promise<void>;

    /**
     * @description Loads the page at the specified url
     * 	 @param url the url to load
     *
     */
    loadUrlSync(url: string): void;

    /**
     * @description Loads the page at the specified url
     * 	 @param url the url to load
     *
     */
    loadUrlAsync(url: string): Promise<void>;

    /**
     * @description Loads the page of the specified file
     *      @param file the file to load
     *
     */
    loadFile(file: string): Promise<void>;

    /**
     * @description Loads the page of the specified file
     *      @param file the file to load
     *
     */
    loadFileSync(file: string): void;

    /**
     * @description Loads the page of the specified file
     *      @param file the file to load
     *
     */
    loadFileAsync(file: string): Promise<void>;

    /**
     * @description Queries the url of the current page
     * 	 @return returns the url of the current page
     *
     */
    getUrl(): Promise<string>;

    /**
     * @description Queries the url of the current page
     * 	 @return returns the url of the current page
     *
     */
    getUrlSync(): string;

    /**
     * @description Queries the url of the current page
     * 	 @return returns the url of the current page
     *
     */
    getUrlAsync(): Promise<string>;

    /**
     * @description Sets the page html of the webview
     * 	 @param html the html to set
     *
     */
    setHtml(html: string): Promise<void>;

    /**
     * @description Sets the page html of the webview
     * 	 @param html the html to set
     *
     */
    setHtmlSync(html: string): void;

    /**
     * @description Sets the page html of the webview
     * 	 @param html the html to set
     *
     */
    setHtmlAsync(html: string): Promise<void>;

    /**
     * @description Gets the page html of the webview
     *      @return returns the page html of the webview
     *
     */
    getHtml(): Promise<string>;

    /**
     * @description Gets the page html of the webview
     *      @return returns the page html of the webview
     *
     */
    getHtmlSync(): string;

    /**
     * @description Gets the page html of the webview
     *      @return returns the page html of the webview
     *
     */
    getHtmlAsync(): Promise<string>;

    /**
     * @description Queries whether the current page has finished loading
     *      @return returns whether the current page has finished loading
     *
     */
    isReady(): Promise<boolean>;

    /**
     * @description Queries whether the current page has finished loading
     *      @return returns whether the current page has finished loading
     *
     */
    isReadySync(): boolean;

    /**
     * @description Queries whether the current page has finished loading
     *      @return returns whether the current page has finished loading
     *
     */
    isReadyAsync(): Promise<boolean>;

    /**
     * @description Waits for the current page to finish loading
     *      @param url the url to wait for; empty means waiting for the current page
     *
     */
    waitFor(url?: string): Promise<void>;

    /**
     * @description Waits for the current page to finish loading
     *      @param url the url to wait for; empty means waiting for the current page
     *
     */
    waitForSync(url?: string): void;

    /**
     * @description Waits for the current page to finish loading
     *      @param url the url to wait for; empty means waiting for the current page
     *
     */
    waitForAsync(url?: string): Promise<void>;

    /**
     * @description Refreshes the current page
     */
    reload(): Promise<void>;

    /**
     * @description Refreshes the current page
     */
    reloadSync(): void;

    /**
     * @description Refreshes the current page
     */
    reloadAsync(): Promise<void>;

    /**
     * @description Goes back to the previous page
     */
    goBack(): Promise<void>;

    /**
     * @description Goes back to the previous page
     */
    goBackSync(): void;

    /**
     * @description Goes back to the previous page
     */
    goBackAsync(): Promise<void>;

    /**
     * @description Goes forward to the next page
     */
    goForward(): Promise<void>;

    /**
     * @description Goes forward to the next page
     */
    goForwardSync(): void;

    /**
     * @description Goes forward to the next page
     */
    goForwardAsync(): Promise<void>;

    /**
     * @description Runs a piece of JavaScript code in the current window
     * 	 @param code the JavaScript code to execute
     *      @return returns the execution result
     *
     */
    eval(code: string): Promise<any>;

    /**
     * @description Runs a piece of JavaScript code in the current window
     * 	 @param code the JavaScript code to execute
     *      @return returns the execution result
     *
     */
    evalSync(code: string): any;

    /**
     * @description Runs a piece of JavaScript code in the current window
     * 	 @param code the JavaScript code to execute
     *      @return returns the execution result
     *
     */
    evalAsync(code: string): Promise<any>;

    /**
     * @description Sets the title of the window
     *      @param title the title of the window
     *
     */
    setTitle(title: string): Promise<void>;

    /**
     * @description Sets the title of the window
     *      @param title the title of the window
     *
     */
    setTitleSync(title: string): void;

    /**
     * @description Sets the title of the window
     *      @param title the title of the window
     *
     */
    setTitleAsync(title: string): Promise<void>;

    /**
     * @description Queries the title of the window
     *      @return returns the title of the window
     *
     */
    getTitle(): Promise<string>;

    /**
     * @description Queries the title of the window
     *      @return returns the title of the window
     *
     */
    getTitleSync(): string;

    /**
     * @description Queries the title of the window
     *      @return returns the title of the window
     *
     */
    getTitleAsync(): Promise<string>;

    /**
     * @description Sets whether the window is visible
     *      @return returns whether the window is visible
     *
     */
    isVisible(): Promise<boolean>;

    /**
     * @description Sets whether the window is visible
     *      @return returns whether the window is visible
     *
     */
    isVisibleSync(): boolean;

    /**
     * @description Sets whether the window is visible
     *      @return returns whether the window is visible
     *
     */
    isVisibleAsync(): Promise<boolean>;

    /**
     * @description Shows the window
     */
    show(): Promise<void>;

    /**
     * @description Shows the window
     */
    showSync(): void;

    /**
     * @description Shows the window
     */
    showAsync(): Promise<void>;

    /**
     * @description Hides the window
     */
    hide(): Promise<void>;

    /**
     * @description Hides the window
     */
    hideSync(): void;

    /**
     * @description Hides the window
     */
    hideAsync(): Promise<void>;

    /**
     * @description Sets the size of the window
     *      @param width the width of the window
     *      @param height the height of the window
     *
     */
    setSize(width: number, height: number): Promise<void>;

    /**
     * @description Sets the size of the window
     *      @param width the width of the window
     *      @param height the height of the window
     *
     */
    setSizeSync(width: number, height: number): void;

    /**
     * @description Sets the size of the window
     *      @param width the width of the window
     *      @param height the height of the window
     *
     */
    setSizeAsync(width: number, height: number): Promise<void>;

    /**
     * @description Queries the size of the window
     *      @return returns the size of the window as an array whose first element is the width and second element is the height
     *
     */
    getSize(): Promise<any[]>;

    /**
     * @description Queries the size of the window
     *      @return returns the size of the window as an array whose first element is the width and second element is the height
     *
     */
    getSizeSync(): any[];

    /**
     * @description Queries the size of the window
     *      @return returns the size of the window as an array whose first element is the width and second element is the height
     *
     */
    getSizeAsync(): Promise<any[]>;

    /**
     * @description Sets the position of the window
     *      @param left the x coordinate of the top-left corner of the window
     *      @param top the y coordinate of the top-left corner of the window
     *
     */
    setPosition(left: number, top: number): Promise<void>;

    /**
     * @description Sets the position of the window
     *      @param left the x coordinate of the top-left corner of the window
     *      @param top the y coordinate of the top-left corner of the window
     *
     */
    setPositionSync(left: number, top: number): void;

    /**
     * @description Sets the position of the window
     *      @param left the x coordinate of the top-left corner of the window
     *      @param top the y coordinate of the top-left corner of the window
     *
     */
    setPositionAsync(left: number, top: number): Promise<void>;

    /**
     * @description Queries the position of the window
     *      @return returns the position of the window as an array whose first element is the x coordinate and second element is the y coordinate
     *
     */
    getPosition(): Promise<any[]>;

    /**
     * @description Queries the position of the window
     *      @return returns the position of the window as an array whose first element is the x coordinate and second element is the y coordinate
     *
     */
    getPositionSync(): any[];

    /**
     * @description Queries the position of the window
     *      @return returns the position of the window as an array whose first element is the x coordinate and second element is the y coordinate
     *
     */
    getPositionAsync(): Promise<any[]>;

    /**
     * @description Queries whether the window is the active window
     *      @return returns whether the window is the active window
     *
     */
    isActived(): Promise<boolean>;

    /**
     * @description Queries whether the window is the active window
     *      @return returns whether the window is the active window
     *
     */
    isActivedSync(): boolean;

    /**
     * @description Queries whether the window is the active window
     *      @return returns whether the window is the active window
     *
     */
    isActivedAsync(): Promise<boolean>;

    /**
     * @description Activates the window
     */
    active(): Promise<void>;

    /**
     * @description Activates the window
     */
    activeSync(): void;

    /**
     * @description Activates the window
     */
    activeAsync(): Promise<void>;

    /**
     * @description Queries the menu of the window
     *      @return returns the menu of the window
     *
     */
    getMenu(): Class_Menu;

    /**
     * @description Captures an image of the current window
     *
     *      Page capture works for most pages, but for lazily loaded pages the full content may not be captured. It is recommended to test on the page to be captured, and to actively adjust the window size when necessary to trigger page loading.
     *      @param fullPage whether to capture the whole page; the default is false, which captures only the visible area
     *      @return returns the captured image
     *
     */
    takeScreenshot(fullPage?: boolean): Promise<Class_Buffer>;

    /**
     * @description Captures an image of the current window
     *
     *      Page capture works for most pages, but for lazily loaded pages the full content may not be captured. It is recommended to test on the page to be captured, and to actively adjust the window size when necessary to trigger page loading.
     *      @param fullPage whether to capture the whole page; the default is false, which captures only the visible area
     *      @return returns the captured image
     *
     */
    takeScreenshotSync(fullPage?: boolean): Class_Buffer;

    /**
     * @description Captures an image of the current window
     *
     *      Page capture works for most pages, but for lazily loaded pages the full content may not be captured. It is recommended to test on the page to be captured, and to actively adjust the window size when necessary to trigger page loading.
     *      @param fullPage whether to capture the whole page; the default is false, which captures only the visible area
     *      @return returns the captured image
     *
     */
    takeScreenshotAsync(fullPage?: boolean): Promise<Class_Buffer>;

    /**
     * @description Closes the current window
     */
    close(): Promise<void>;

    /**
     * @description Closes the current window
     */
    closeSync(): void;

    /**
     * @description Closes the current window
     */
    closeAsync(): Promise<void>;

    /**
     * @description Sends a message into the webview
     *      postMessage must be sent after the window has finished loading; messages sent before that are lost. Therefore it is recommended to call this method only after the onload event fires.
     * 	 @param msg the message to send
     *
     */
    postMessage(msg: string): Promise<void>;

    /**
     * @description Sends a message into the webview
     *      postMessage must be sent after the window has finished loading; messages sent before that are lost. Therefore it is recommended to call this method only after the onload event fires.
     * 	 @param msg the message to send
     *
     */
    postMessageSync(msg: string): void;

    /**
     * @description Sends a message into the webview
     *      postMessage must be sent after the window has finished loading; messages sent before that are lost. Therefore it is recommended to call this method only after the onload event fires.
     * 	 @param msg the message to send
     *
     */
    postMessageAsync(msg: string): Promise<void>;

    /**
     * @description Queries and binds the window load start event, equivalent to on("loading", func);
     */
    onloading: (()=>void) | null;

    /**
     * @description Queries and binds the window load completed event, equivalent to on("load", func);
     */
    onload: (()=>void) | null;

    /**
     * @description Queries and binds the window move event, equivalent to on("move", func);
     *
     * 	 The following example outputs the top-left corner coordinates of the window when it moves:
     * 	 ```JavaScript
     * 	 var gui = require('gui');
     * 	 var webview = gui.open('fs://index.html');
     *
     * 	 webview.onmove = evt => console.log(evt.left, evt.top);
     * 	 ```
     *
     */
    onmove: (()=>void) | null;

    /**
     * @description Queries and binds the window size change event, equivalent to on("size", func);
     *
     *      The following example outputs the size of the window when it is resized:
     *      ```JavaScript
     *      var gui = require('gui');
     *      var webview = gui.open('fs://index.html');
     *
     *      webview.onresize = evt => console.log(evt.width, evt.height);
     *      ```
     *
     */
    onresize: (()=>void) | null;

    /**
     * @description Queries and binds the window focus event, equivalent to on("focus", func);
     */
    onfocus: (()=>void) | null;

    /**
     * @description Queries and binds the window blur event, equivalent to on("blur", func);
     */
    onblur: (()=>void) | null;

    /**
     * @description Queries and binds the window close event, which fires after the WebView is closed, equivalent to on("closed", func);
     */
    onclose: (()=>void) | null;

    /**
     * @description Queries and binds the event for receiving postMessage messages from inside the webview, equivalent to on("message", func);
     */
    onmessage: (()=>void) | null;

    /**
     * @description Keeps the fibjs process alive; prevents the fibjs process from exiting while the object is bound
     *      @return returns the current object
     *
     */
    ref(): Class_WebView;

    /**
     * @description Allows the fibjs process to exit; permits the fibjs process to exit while the object is bound
     *      @return returns the current object
     *
     */
    unref(): Class_WebView;

}


declare namespace Class_WebView {
    const promises: FIBJS.GeneralObject;
}
