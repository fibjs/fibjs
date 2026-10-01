/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/// <reference path="../interface/File.d.ts" />
/// <reference path="../interface/Headers.d.ts" />
/// <reference path="../interface/FormData.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/TextDecoder.d.ts" />
/// <reference path="../interface/TextEncoder.d.ts" />
/// <reference path="../interface/AbortController.d.ts" />
/// <reference path="../interface/AbortSignal.d.ts" />
/// <reference path="../interface/DOMEvent.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/MessageEvent.d.ts" />
/// <reference path="../interface/MessagePort.d.ts" />
/// <reference path="../interface/MessageChannel.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/// <reference path="../interface/CryptoKey.d.ts" />
/// <reference path="../interface/DOMParser.d.ts" />
/// <reference path="../interface/CSSStyleDeclaration.d.ts" />
/// <reference path="../interface/DOMStringMap.d.ts" />
/// <reference path="../interface/XMLSerializer.d.ts" />
/// <reference path="../interface/XmlDocument.d.ts" />
/// <reference path="../interface/WebSocket.d.ts" />
/// <reference path="../module/console.d.ts" />
/// <reference path="../module/process.d.ts" />
/// <reference path="../module/performance.d.ts" />
/// <reference path="../interface/PerformanceObserver.d.ts" />
/// <reference path="../module/webcrypto.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description The global object, the base object every script can access
 *
 *  The global object provides:
 *
 *  - **Web standard objects**: `Buffer`, `URL`, `URLSearchParams`, `Blob`, `File`, `Headers`, `FormData`, `Request`, `Response`, `TextDecoder`, `TextEncoder`, `AbortController`, `AbortSignal`, `Event`, `EventTarget`, `MessagePort`, `MessageChannel`, `Worker`, `WebSocket`, `DOMParser`, `XMLSerializer` and more;
 *  - **Core modules**: `console`, `process`, `performance`, `crypto`;
 *  - **Module loading**: `require` loads modules, `run` runs scripts;
 *  - **Timers**: `setTimeout`, `setInterval`, `setImmediate` and so on, behaving like the same-named functions of the timers module;
 *  - **Helpers**: `btoa`/`atob` encoding, `structuredClone` deep copy, `fetch` requests, `queueMicrotask` micro-task scheduling.
 */
declare module 'global' {
    /**
     * @description The binary data buffer object used for io reads and writes, see the Buffer object.
     */
    const Buffer: typeof Class_Buffer;

    /**
     * @description Creates a URLSearchParams object, see URLSearchParams
     */
    const URLSearchParams: typeof Class_URLSearchParams;

    /**
     * @description Creates a UrlObject object, see UrlObject
     */
    const URL: typeof Class_UrlObject;

    /**
     * @description Creates a Blob object, see Blob
     */
    const Blob: typeof Class_Blob;

    /**
     * @description Creates a File object, see File
     */
    const File: typeof Class_File;

    /**
     * @description Creates a Headers object, see Headers
     */
    const Headers: typeof Class_Headers;

    /**
     * @description Creates a FormData object, see FormData
     */
    const FormData: typeof Class_FormData;

    /**
     * @description Creates an http request object, see HttpRequest
     */
    const Request: typeof Class_HttpRequest;

    /**
     * @description Creates a Fetch API response object, see HttpResponse
     */
    const Response: typeof Class_HttpResponse;

    /**
     * @description The TextDecoder object, see the TextDecoder object.
     */
    const TextDecoder: typeof Class_TextDecoder;

    /**
     * @description The TextEncoder object, see the TextEncoder object.
     */
    const TextEncoder: typeof Class_TextEncoder;

    /**
     * @description The controller object used to abort one or more Web requests on demand, see the AbortController object.
     */
    const AbortController: typeof Class_AbortController;

    /**
     * @description The signal object used to communicate with and abort asynchronous operations, see the AbortSignal object.
     */
    const AbortSignal: typeof Class_AbortSignal;

    /**
     * @description The DOM event object, representing a W3C standard event
     */
    const Event: typeof Class_DOMEvent;

    /**
     * @description The DOM event target object, providing Web standard event listening and dispatching
     */
    const EventTarget: typeof Class_EventEmitter;

    /**
     * @description The MessageEvent object, representing a message received by a target object
     */
    const MessageEvent: typeof Class_MessageEvent;

    /**
     * @description The MessagePort object, representing one end of a message channel
     */
    const MessagePort: typeof Class_MessagePort;

    /**
     * @description The MessageChannel object, providing a pair of connected MessagePort objects
     */
    const MessageChannel: typeof Class_MessageChannel;

    /**
     * @description The Worker object, used to create child threads
     *
     *    The same class as `worker_threads.Worker` with identical semantics; equivalent to `require('worker_threads').Worker`:
     *
     *    ```JavaScript
     *    const worker = new Worker(__dirname + '/worker.js');
     *    worker.on('message', (msg) => console.log(msg));
     *    worker.postMessage('hello');
     *    ```
     *
     */
    const Worker: typeof Class_Worker;

    /**
     * @description The CryptoKey class represents symmetric or asymmetric keys, each kind exposing different capabilities
     */
    const CryptoKey: typeof Class_CryptoKey;

    /**
     * @description The DOMParser interface parses strings into DOM documents, see the DOMParser object
     */
    const DOMParser: typeof Class_DOMParser;

    /**
     * @description The CSSStyleDeclaration interface represents the CSS declaration block of the style attribute of an element, see the CSSStyleDeclaration object
     */
    const CSSStyleDeclaration: typeof Class_CSSStyleDeclaration;

    /**
     * @description The DOMStringMap interface represents the key-value map of the data-* attributes of an element, see the DOMStringMap object
     */
    const DOMStringMap: typeof Class_DOMStringMap;

    /**
     * @description The XMLSerializer interface serializes DOM nodes into strings, see the XMLSerializer object
     */
    const XMLSerializer: typeof Class_XMLSerializer;

    /**
     * @description The XMLDocument interface represents an XML document, the same as XmlDocument
     */
    const XMLDocument: typeof Class_XmlDocument;

    /**
     * @description The WebSocket class creates and manages WebSocket connections, see the WebSocket object
     */
    const WebSocket: typeof Class_WebSocket;

    /**
     * @description The console access object
     */
    const console: typeof import ('console');

    /**
     * @description The process object
     */
    const process: typeof import ('process');

    /**
     * @description The basic performance monitoring module
     */
    const performance: typeof import ('performance');

    /**
     * @description The PerformanceObserver interface observes performance records
     */
    const PerformanceObserver: typeof Class_PerformanceObserver;

    /**
     * @description The w3c webcrypto standard crypto module
     */
    const crypto: typeof import ('webcrypto');

    /**
     * @description The global object
     */
    const global: FIBJS.GeneralObject;

    /**
     * @description The global object
     */
    const globalThis: FIBJS.GeneralObject;

    /**
     * @description Runs a script
     *      @param fname the path of the script to run
     *
     */
    function run(fname: string): void;

    /**
     * @description Loads a module and returns the module object, see @ref module for more information
     *
     *      require can load both internal modules and file modules.
     *
     *      Internal modules are initialized when the sandbox is created; they are referenced by their id, for example require("net").
     *
     *      File modules are user-defined modules, referenced by a relative path starting with ./ or ../. File modules support .js, .jsc and .json files.
     *
     *      File modules also support the package.json format. When the module is a directory, require first looks up main in package.json, and when it is missing, tries index.js, index.jsc or index.json under the path.
     *
     *      When the referenced path does not start with ./ or ../ and is not an internal module, require searches node_modules under the path of the current module, walking up the parent directories.
     *
     *      The basic flow is as follows:
     *
     *      ```dot
     *         digraph{
     *             node [fontname = "Helvetica,sans-Serif", fontsize = 10];
     *             edge [fontname = "Helvetica,sans-Serif", fontsize = 10];
     *
     *             start [label="start"];
     *             resolve [label="path.resolve" shape="rect"];
     *             search [label="recursive lookup\nnode_modules\nfrom the current path" shape="rect"];
     *             load [label="load" shape="rect"];
     *             end [label="end" shape="doublecircle"];
     *
     *             is_native [label="is internal module?" shape="diamond"];
     *             is_mod [label="is module?" shape="diamond"];
     *             is_abs [label="is absolute?" shape="diamond"];
     *             has_file [label="module exists?" shape="diamond"];
     *             has_ext [label="module.js exists?" shape="diamond"];
     *             has_package [label="/package.json\nexists?" shape="diamond"];
     *             has_main [label="main exists?" shape="diamond"];
     *             has_index [label="index.js exists?" shape="diamond"];
     *
     *             start -> is_native;
     *             is_native -> end [label="Yes"];
     *             is_native -> is_mod [label="No"];
     *             is_mod -> search [label="Yes"];
     *             search -> has_file;
     *             is_mod -> is_abs [label="No"];
     *             is_abs -> has_file [label="Yes"];
     *             is_abs -> resolve [label="No"];
     *             resolve -> has_file;
     *             has_file -> load [label="Yes"];
     *             has_file -> has_ext [label="No"];
     *             has_ext -> load [label="Yes"];
     *             has_ext -> has_package [label="No"];
     *             has_package -> has_main [label="Yes"];
     *             has_package -> has_index [label="No"];
     *             has_main -> load [label="Yes"];
     *             has_main -> has_index [label="No"];
     *             has_index -> load [label="Yes"];
     *             has_index -> end [label="No"];
     *             load -> end;
     *         }
     *      ```
     *
     *      @param id the name of the module to load
     *      @return the exported object of the loaded module
     *
     */
    function require(id: string): any;

    /**
     * @description Calls a function after the given time, behaving like the same-named function of the timers module
     *     @param callback the callback function
     *     @param timeout the delay in milliseconds, 1 by default; values below 1 or above 2^31-1 are treated as 1ms.
     *     @param args extra arguments passed to the callback, optional.
     *     @return the timer object
     *
     */
    function setTimeout(callback: (...args: any[])=>any, timeout?: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the given timer
     *      @param t the timer to clear
     *
     */
    function clearTimeout(t: any): void;

    /**
     * @description Calls a function after every given interval, behaving like the same-named function of the timers module
     *      @param callback the callback function
     *      @param timeout the interval in milliseconds; values below 1 or above 2^31-1 are treated as 1ms.
     *      @param args extra arguments passed to the callback, optional.
     *      @return the timer object
     *
     */
    function setInterval(callback: (...args: any[])=>any, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the given timer
     *      @param t the timer to clear
     *
     */
    function clearInterval(t: any): void;

    /**
     * @description Calls a function after every given interval; this is a high-precision timer that interrupts the running JavaScript script
     *      Since the setHrInterval timer interrupts running code to execute the callback, do not modify data that may affect other modules inside the callback, and do not call any API marked as async in the callback, otherwise unpredictable results may occur. For example:
     *      ```JavaScript
     *         var timers = require('timers');
     *
     *         var cnt = 0;
     *         timers.setHrInterval(() => {
     *             cnt++;
     *         }, 100);
     *
     *         while (cnt < 10);
     *
     *         console.error("===============================> done");
     *      ```
     *      In this code, the loop on line 8 will not end when cnt changes, because when optimizing the code JavaScript assumes that cnt will not change during the loop.
     *      @param callback the callback function
     *      @param timeout the interval in milliseconds; values below 1 or above 2^31-1 are treated as 1ms.
     *      @param args extra arguments passed to the callback, optional.
     *      @return the timer object
     *
     */
    function setHrInterval(callback: (...args: any[])=>any, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the given timer
     *      @param t the timer to clear
     *
     */
    function clearHrInterval(t: any): void;

    /**
     * @description Calls the callback as soon as the next idle moment arrives
     *      @param callback the callback function
     *      @param args extra arguments passed to the callback, optional.
     *      @return the timer object
     *
     */
    function setImmediate(callback: (...args: any[])=>any, ...args: any[]): Class_Timer;

    /**
     * @description Clears the given timer
     *      @param t the timer to clear
     *
     */
    function clearImmediate(t: any): void;

    /**
     * @description Encodes data in base64
     * 	 @param data the data to encode
     * 	 @return the encoded string
     *
     */
    function btoa(data: string): string;

    /**
     * @description Decodes a string into binary data in base64
     * 	 @param data the string to decode
     * 	 @return the decoded binary data
     *
     */
    function atob(data: string): string;

    /**
     * @description Creates a deep copy of a value
     *      Creates a deep copy of the given value using the structured clone algorithm. Circular references are supported.
     *
     *      The transfer option specifies the list of transferable objects (such as ArrayBuffer) to move instead of clone. Once transferred, the original objects become unusable.
     *
     *      @param value the value to clone
     *      @param options optional options object containing the transfer array
     *      @return the cloned value
     *
     */
    function structuredClone(value: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description Requests the given url and returns the result, the same as http.request(url, ...)
     *      opts contains extra request options; the supported fields are:
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      body, json and pack must not be used together. The default is {}, with no extra information
     *      @param url the url to request, which must be a complete url including the host
     *      @param opts extra options
     *      @return the server response
     *
     */
    function fetch(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponse>;

    /**
     * @description Requests the given url and returns the result, the same as http.request(url, ...)
     *      opts contains extra request options; the supported fields are:
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      body, json and pack must not be used together. The default is {}, with no extra information
     *      @param url the url to request, which must be a complete url including the host
     *      @param opts extra options
     *      @return the server response
     *
     */
    function fetchSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the given url and returns the result, the same as http.request(url, ...)
     *      opts contains extra request options; the supported fields are:
     *      ```JavaScript
     *      {
     *          "method": "GET", // specify the http request method: GET, POST, etc, default: GET.
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      body, json and pack must not be used together. The default is {}, with no extra information
     *      @param url the url to request, which must be a complete url including the host
     *      @param opts extra options
     *      @return the server response
     *
     */
    function fetchAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponse>;

    /**
     * @description Sends a Fetch request given a Request object
     *      @param request the Request object
     *      @param opts request options (may override the fields of request)
     *      @return the server response object
     *
     */
    function fetch(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponse>;

    /**
     * @description Sends a Fetch request given a Request object
     *      @param request the Request object
     *      @param opts request options (may override the fields of request)
     *      @return the server response object
     *
     */
    function fetchSync(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Sends a Fetch request given a Request object
     *      @param request the Request object
     *      @param opts request options (may override the fields of request)
     *      @return the server response object
     *
     */
    function fetchAsync(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponse>;

    /**
     * @description Queues a micro-task for execution
     *      The callback runs after the current task completes and before the next task starts.
     *
     *      @param callback the function to queue as a micro-task
     *
     */
    function queueMicrotask(callback: (...args: any[])=>any): void;

}

