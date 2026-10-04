/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/Headers.d.ts" />
/// <reference path="../interface/HttpCookie.d.ts" />
/// <reference path="../interface/HttpServer.d.ts" />
/// <reference path="../interface/HttpClient.d.ts" />
/// <reference path="../interface/HttpsServer.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpHandler.d.ts" />
/// <reference path="../interface/HttpRepeater.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/**
 * The promise variant of the http module: async members return a Promise as their primary form.
 */
declare module 'http/promises' {
    /**
     * @description Creates an http request object, see HttpRequest
     */
    const Request: typeof Class_HttpRequest;

    /**
     * @description Compatibility alias, equivalent to HttpRequest
     */
    const IncomingMessage: typeof Class_HttpRequest;

    /**
     * @description Creates an http response object, see HttpResponse
     */
    const Response: typeof Class_HttpResponse;

    /**
     * @description Compatibility alias, equivalent to HttpResponse
     */
    const ServerResponse: typeof Class_HttpResponse;

    /**
     * @description Creates a Headers object, see Headers
     */
    const Headers: typeof Class_Headers;

    /**
     * @description Creates an http cookie object, see HttpCookie
     */
    const Cookie: typeof Class_HttpCookie;

    /**
     * @description Creates an http server, see HttpServer
     */
    const Server: typeof Class_HttpServer;

    /**
     * @description Creates an http client, see HttpClient
     */
    const Client: typeof Class_HttpClient;

    /**
     * @description Creates an http agent; HttpAgent is an alias of HttpClient
     */
    const Agent: typeof Class_HttpClient;

    /**
     * @description Creates an https server, see HttpsServer
     */
    const HttpsServer: typeof Class_HttpsServer;

    /**
     * @description Creates an http server
     *      @param hdlr request handler function, receives (req, res) parameters
     *      @return returns an HttpServer object that is not bound to a port; call listen() to start it
     *
     */
    function createServer(hdlr: Class_Handler | Class_HandlerPromise): Class_HttpServer;

    /**
     * @description Creates an https server
     *      @param options either the SecureContext object used for TLS configuration, or the TLS options object used to create one
     *      @param hdlr request handler function, receives (req, res) parameters
     *      @return returns an HttpsServer object that is not bound to a port; call listen() to start it
     *
     */
    function createServer(options: FIBJS.GeneralObject | Class_SecureContext | Class_SecureContextPromise, hdlr: Class_Handler | Class_HandlerPromise): Class_HttpServer;

    /**
     * @description Creates an http protocol handler object, see HttpHandler
     */
    const Handler: typeof Class_HttpHandler;

    /**
     * @description Creates an http request repeater object, see HttpRepeater
     */
    const Repeater: typeof Class_HttpRepeater;

    /**
     * @description Returns the collection of standard HTTP response status codes and their short descriptions.
     */
    const STATUS_CODES: FIBJS.GeneralObject;

    /**
     * @description Returns an array of all method names (in uppercase) supported by the HTTP protocol.
     */
    const METHODS: string[];

    /**
     * @description Returns the HttpCookie object list of the http client
     */
    const cookies: Class_HttpCookie[];

    /**
     * @description Queries and sets whether to keep the connection alive
     */
    var keepAlive: boolean;

    /**
     * @description Queries and sets the timeout
     */
    var timeout: number;

    /**
     * @description Cookie feature switch, enabled by default
     */
    var enableCookie: boolean;

    /**
     * @description Automatic redirect feature switch, enabled by default
     */
    var autoRedirect: boolean;

    /**
     * @description Automatic decompression feature switch, enabled by default
     */
    var enableEncoding: boolean;

    /**
     * @description HTTP/2 automatic upgrade switch, disabled by default
     */
    var enableH2: boolean;

    /**
     * @description Queries and sets the maximum number of request headers, default 128
     */
    var maxHeadersCount: number;

    /**
     * @description Queries and sets the maximum request header size, default 8192
     */
    var maxHeaderSize: number;

    /**
     * @description Queries and sets the maximum chunk size in MB, default 2
     */
    var maxChunkSize: number;

    /**
     * @description Queries and sets the maximum body size in MB, default -1, no size limit
     */
    var maxBodySize: number;

    /**
     * @description Queries and sets the browser identifier in http requests
     */
    var userAgent: string;

    /**
     * @description Queries and sets the keep-alive cached connection timeout, default 10000 ms
     */
    var poolTimeout: number;

    /**
     * @description Queries and sets the maximum number of idle connections per host, default 256
     */
    var maxFreeSockets: number;

    /**
     * @description Creates an http static file handler to respond to http messages with static files
     *
     *      fileHandler supports gzip pre-compression: when the request accepts gzip encoding and a filename.ext.gz file exists at the same path, this file is returned directly,
     *      thus avoiding server load caused by repeated compression.
     *      @param root file root path
     *      @param autoIndex whether browsing directory files is supported, default false, not supported
     *      @return returns a static file handler for processing http messages
     *
     */
    function fileHandler(root: string, autoIndex?: boolean): Class_Handler;

    /**
     * @description Creates an http static file handler to respond to http messages with static files
     *
     *      fileHandler supports gzip pre-compression: when the request accepts gzip encoding and a filename.ext.gz file exists at the same path, this file is returned directly,
     *      thus avoiding server load caused by repeated compression.
     *
     *      Meanings of the options fields:
     *      - autoIndex: Boolean, whether browsing directory files is supported, default false, not supported
     *      - maxAge: Integer, cache time in seconds, default 0, meaning cache-related response headers are not generated automatically
     *      - immutable: Boolean, when true, appends the immutable directive to the automatically generated Cache-Control, default false
     *      - cacheControl: Boolean, whether to generate Cache-Control automatically, default true; generated as public, max-age=N only when maxAge is greater than 0 and the response does not already carry Cache-Control
     *      - headers: Object, declares response headers by glob pattern, in the form { '<pattern>': { '<header-name>': '<value>' } }; patterns match request paths relative to root
     *        (without a leading /, directory requests match as index.html), with the same glob semantics as path.matchesGlob (* does not cross directories, ** can cross any level);
     *        rules match in declaration order, the first match takes effect
     *
     *      For example, to make the entry page non-cacheable and long-cache static assets with a hash:
     *      ```JavaScript
     *      http.fileHandler('/home/frontend/assets/', {
     *          maxAge: 31536000,
     *          immutable: true,
     *          headers: {
     *              'index.html': { 'Cache-Control': 'no-cache' },
     *              'sw.js': { 'Cache-Control': 'no-cache' }
     *          }
     *      })
     *      ```
     *      @param root file root path
     *      @param options configuration options, see above for the fields
     *      @return returns a static file handler for processing http messages
     *
     */
    function fileHandler(root: string, options?: FIBJS.GeneralObject): Class_Handler;

    /**
     * @description Sends an http request to the specified stream object and returns the result
     *      @param conn the stream object to process the request
     *      @param req the HttpRequest object to send
     *      @return returns the server response
     *
     */
    function request(conn: Class_Stream | Class_StreamPromise, req: Class_HttpRequest | Class_HttpRequestPromise): Class_HttpRequest;

    /**
     * @description Requests the url specified by opts and returns the result
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSync(opts: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the url specified by opts and returns the result
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSyncSync(opts: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the url specified by opts and returns the result
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSyncAsync(opts: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url and returns the result
     *
     *      opts contains additional request options; the supported contents are as follows:
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
     *          "path": "", // alias of pathname, used for the request option.
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSync(method: string, url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url and returns the result
     *
     *      opts contains additional request options; the supported contents are as follows:
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
     *          "path": "", // alias of pathname, used for the request option.
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSyncSync(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url and returns the result
     *
     *      opts contains additional request options; the supported contents are as follows:
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
     *          "path": "", // alias of pathname, used for the request option.
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {}
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function requestSyncAsync(method: string, url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the url specified by opts and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
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
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function request(opts: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the url specified by opts, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function request(opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function request(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
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
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function request(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function request(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function request(method: string, url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
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
     *          "path": "", // alias of pathname, used for the request option.
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function request(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function request(method: string, url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function getSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function getSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function getSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object sends the request automatically without calling `end()`; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function get(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object sends the request automatically without calling `end()`; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function get(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object sends the request automatically without calling `end()`; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function get(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function postSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function postSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function postSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function post(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function post(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function post(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function delSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function delSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function delSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function del(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function del(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function del(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function putSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function putSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function putSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function put(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function put(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function put(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function patchSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function patchSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function patchSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function patch(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "body": SeekableStream | Buffer | String | {},
     *          "json": {},
     *          "pack": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function patch(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object requires calling `end()` to send the request; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function patch(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function headSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function headSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *      opts contains additional request options; the supported contents are as follows:
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
     *      body, json and pack must not appear at the same time. Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    function headSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object sends the request automatically without calling `end()`; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function head(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object sends the request automatically without calling `end()`; the response is received through the callback; you can also listen to the `'response'` event of the returned object.
     *      opts contains additional request options; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "protocol": "http",
     *          "slashes": true,
     *          "username": "",
     *          "password": "",
     *          "hostname": "",
     *          "port": "",
     *          "pathname": "",
     *          "keepAlive": unknown, // If not specified, the default settings of the client will be used.
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "query": {},
     *          "headers": {},
     *          "signal": AbortSignal // AbortSignal object used to cancel the request
     *      }
     *      ```
     *      Default is {}, which contains no additional information
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    function head(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned HttpRequest object sends the request automatically without calling `end()`; the response is received through the callback.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    function head(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Dynamically configures proxy support from environment variables
     *      When this function is called, it reads the proxy configuration from the environment variables (HTTP_PROXY, HTTPS_PROXY, NO_PROXY and their lowercase forms) and applies it globally.
     *      Can be used to dynamically enable proxy support at runtime, as an alternative to the --use-env-proxy flag.
     *
     *      @param proxyEnv object containing the proxy configuration. If not provided, process.env is read.
     *               supported properties: HTTP_PROXY, http_proxy, HTTPS_PROXY, https_proxy, NO_PROXY, no_proxy
     *      @return a callable function used to restore the original proxy configuration
     *
     */
    function setGlobalProxyFromEnv(proxyEnv?: FIBJS.GeneralObject): ()=>any;

    /**
     * @description Sends a request using the Web Fetch standard and returns an HttpResponse object
     *
     *      request is the request source: an HttpRequest object, or the target URL of the request; when it
     *      is a URL string the URL-override fields of opts are honoured as well. opts overrides the request
     *      fields (`new Request(request, init)` semantics); the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "method": "GET", // overrides the request method; the method of an HttpRequest source is kept when not given
     *          "headers": {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          "body": SeekableStream | Buffer | String | {}, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          "keepAlive": unknown, // overrides the keep-alive setting
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "redirect": "follow", // redirect mode: "follow" (default) | "error" | "manual"
     *          "signal": AbortSignal, // AbortSignal object used to cancel the request
     *          "streaming": false // whether to return the response body in streaming mode
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which overrides no information in request.
     *      Following the Fetch standard a GET or HEAD request must not carry a body, a string body is sent as
     *      `text/plain;charset=UTF-8`, and `headers` replaces the headers of the request source instead of
     *      merging them
     *      @param request the request source: an HttpRequest object, or the target URL of the request
     *      @param opts the additional information, can override the corresponding fields in request
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    function fetch(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Sends a request using the Web Fetch standard and returns an HttpResponse object
     *
     *      request is the request source: an HttpRequest object, or the target URL of the request; when it
     *      is a URL string the URL-override fields of opts are honoured as well. opts overrides the request
     *      fields (`new Request(request, init)` semantics); the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "method": "GET", // overrides the request method; the method of an HttpRequest source is kept when not given
     *          "headers": {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          "body": SeekableStream | Buffer | String | {}, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          "keepAlive": unknown, // overrides the keep-alive setting
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "redirect": "follow", // redirect mode: "follow" (default) | "error" | "manual"
     *          "signal": AbortSignal, // AbortSignal object used to cancel the request
     *          "streaming": false // whether to return the response body in streaming mode
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which overrides no information in request.
     *      Following the Fetch standard a GET or HEAD request must not carry a body, a string body is sent as
     *      `text/plain;charset=UTF-8`, and `headers` replaces the headers of the request source instead of
     *      merging them
     *      @param request the request source: an HttpRequest object, or the target URL of the request
     *      @param opts the additional information, can override the corresponding fields in request
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    function fetchSync(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Sends a request using the Web Fetch standard and returns an HttpResponse object
     *
     *      request is the request source: an HttpRequest object, or the target URL of the request; when it
     *      is a URL string the URL-override fields of opts are honoured as well. opts overrides the request
     *      fields (`new Request(request, init)` semantics); the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "method": "GET", // overrides the request method; the method of an HttpRequest source is kept when not given
     *          "headers": {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          "body": SeekableStream | Buffer | String | {}, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          "keepAlive": unknown, // overrides the keep-alive setting
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "redirect": "follow", // redirect mode: "follow" (default) | "error" | "manual"
     *          "signal": AbortSignal, // AbortSignal object used to cancel the request
     *          "streaming": false // whether to return the response body in streaming mode
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which overrides no information in request.
     *      Following the Fetch standard a GET or HEAD request must not carry a body, a string body is sent as
     *      `text/plain;charset=UTF-8`, and `headers` replaces the headers of the request source instead of
     *      merging them
     *      @param request the request source: an HttpRequest object, or the target URL of the request
     *      @param opts the additional information, can override the corresponding fields in request
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    function fetchAsync(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

}


declare module "http" {
    const promises: typeof import("http/promises");
}
