/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * @description HttpClient is a class library designed for HTTP client functionality, providing basic HTTP/HTTPS requests, proxy access, cookie management and other features
 *
 * With HttpClient you can easily access and operate web pages; here is a simple example that prints the source code of a web page:
 *
 * ```JavaScript
 * const http = require('http');
 *
 * const res = http.get('http://www.example.com/');
 *
 * console.log(res.body.readAll().toString());
 * ```
 *
 * In this example, the http module is introduced via require, then http.get is used to send a get request, where the url parameter specifies the requested web address. Because the http.get method returns an HttpResponse object, its body property can be used to access the body content returned by the request and convert it to a string with the toString method.
 *
 * When the requested url is of https type instead of http type, the code only needs to change http to https:
 *
 * ```JavaScript
 * const http = require('http');
 *
 * const res = http.get('https://www.example.com/');
 *
 * console.log(res.body.readAll().toString());
 * ```
 *
 * In addition, here is an example of sending a POST request directly through HttpClient and setting the User-Agent:
 *
 * ```JavaScript
 * const http = require('http');
 *
 * const httpClient = new http.Client();
 * httpClient.userAgent = 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36';
 * const res = httpClient.post('http://www.example.com/post', {
 *     json: { name: 'fibjs', version: '0.31.0' }
 * });
 * console.log(res.body.readAll().toString());
 * ```
 *
 * In this example, an HttpClient object httpClient is first created and its userAgent is set to a browser User-Agent. Then its post method is used to send a post request, where the name and version parameters specify the request body. Finally the body content of the return value is printed.
 *
 */
declare class Class_HttpClient extends Class_EventEmitter {
    /**
     * @description HttpClient constructor, creates a new HttpClient object
     */
    constructor();

    /**
     * @description HttpClient constructor, creates a new HttpClient object
     *      @param context the secure context used to create the HttpClient
     *
     */
    constructor(context: Class_SecureContext);

    /**
     * @description HttpClient constructor, creates a new HttpClient object
     *
     *      In addition to the properties used to create a SecureContext, options also needs to provide the following properties:
     *      - keepAlive: specifies whether to keep the connection alive
     *      - timeout: specifies the timeout
     *      - enableCookie: specifies whether to enable the cookie feature
     *      - autoRedirect: specifies whether to enable the automatic redirect feature
     *      - enableEncoding: specifies whether to enable the automatic decompression feature
     *      - enableH2: specifies whether to enable HTTP/2 automatic upgrade
     *      - maxHeadersCount: specifies the maximum number of request headers
     *      - maxHeaderSize: specifies the maximum request header size
     *      - maxBodySize: specifies the maximum body size
     *      - userAgent: specifies the browser identifier
     *      - poolTimeout: specifies the keep-alive cached connection timeout
     *      - proxyEnv: specifies the proxy configuration environment variables, including HTTP_PROXY, HTTPS_PROXY, NO_PROXY and their lowercase forms
     *
     *      @param options options required to create a secure context using tls.createSecureContext
     *
     */
    constructor(options: FIBJS.GeneralObject);

    /**
     * @description Returns the HttpCookie object list of the http client
     */
    readonly cookies: any[];

    /**
     * @description Queries and sets whether to keep the connection alive
     */
    keepAlive: boolean;

    /**
     * @description Queries and sets the timeout in milliseconds
     */
    timeout: number;

    /**
     * @description Cookie feature switch, enabled by default
     */
    enableCookie: boolean;

    /**
     * @description Automatic redirect feature switch, enabled by default
     */
    autoRedirect: boolean;

    /**
     * @description Automatic decompression feature switch, enabled by default
     */
    enableEncoding: boolean;

    /**
     * @description HTTP/2 automatic upgrade switch, disabled by default
     */
    enableH2: boolean;

    /**
     * @description Queries and sets the maximum number of request headers, default 128
     */
    maxHeadersCount: number;

    /**
     * @description Queries and sets the maximum request header size, default 8192
     */
    maxHeaderSize: number;

    /**
     * @description Queries and sets the maximum chunk size in MB, default 2
     */
    maxChunkSize: number;

    /**
     * @description Queries and sets the maximum body size in MB, default -1, no size limit
     */
    maxBodySize: number;

    /**
     * @description Queries and sets the browser identifier in http requests
     */
    userAgent: string;

    /**
     * @description Queries and sets the keep-alive cached connection timeout, default 10000 ms
     */
    poolTimeout: number;

    /**
     * @description Queries and sets the proxy configuration environment variables, supports HTTP_PROXY, HTTPS_PROXY, NO_PROXY and their lowercase forms
     */
    proxyEnv: FIBJS.GeneralObject;

    /**
     * @description Queries and sets the maximum number of connections per host, default unlimited
     */
    maxSockets: number;

    /**
     * @description Queries and sets the maximum total number of connections for all hosts, default unlimited
     */
    maxTotalSockets: number;

    /**
     * @description Queries and sets the maximum number of idle connections per host, default 256
     */
    maxFreeSockets: number;

    /**
     * @description Queries and sets the default port used by getName(), default 80
     */
    defaultPort: number;

    /**
     * @description Queries and sets the default protocol used by getName(), default "http:"
     */
    protocol: string;

    /**
     * @description Returns the map of idle connections keyed by host:port
     */
    readonly freeSockets: FIBJS.GeneralObject;

    /**
     * @description Returns the map of connections in use keyed by host:port
     */
    readonly sockets: FIBJS.GeneralObject;

    /**
     * @description Returns the total number of connections in use across all hosts
     */
    readonly totalSocketCount: number;

    /**
     * @description Returns a unique key for the given request options, used for the connection pool
     *      @param options request options
     *      @return returns the connection pool key string
     *
     */
    getName(options?: FIBJS.GeneralObject): string;

    /**
     * @description Destroys all connections currently in use
     */
    destroy(): void;

    /**
     * @description Sends an http request to the specified stream object and returns the result
     *      @param conn the stream object to process the request
     *      @param req the HttpRequest object to send
     *      @return returns the server response
     *
     */
    request(conn: Class_Stream, req: Class_HttpRequest): Class_HttpRequest;

    /**
     * @description Requests the specified url and returns the result
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
    requestSync(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

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
    requestSync(opts: FIBJS.GeneralObject): Class_HttpResponse;

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
    requestSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
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
    request(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the url specified by opts and returns an HttpRequest object
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
    request(opts: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url and returns an HttpRequest object
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
    request(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(method: string, url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the url specified by opts, registers a callback to receive the response, and returns an HttpRequest object
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(method: string, url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    getSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
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
    get(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    get(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    get(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    postSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
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
    post(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    post(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    post(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    delSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
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
    del(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    del(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    del(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    putSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
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
    put(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    put(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    put(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

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
    patchSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
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
    patch(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    patch(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    patch(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("PATCH", ...)
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
    headSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
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
    head(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    head(url: string, opts: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    head(url: string, callback: (...args: any[])=>any): Class_HttpRequest;

    /**
     * @description Sends a request using the Web Fetch standard and returns an HttpResponse object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpResponse object
     *
     */
    fetch(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    fetch(url: string, opts?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description Sends a request using the Web Fetch standard and returns an HttpResponse object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpResponse object
     *
     */
    fetchSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Sends a request using the Web Fetch standard and returns an HttpResponse object
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpResponse object
     *
     */
    fetchAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponse>;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *      opts can override the request fields in request; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "method": "GET", // overrides the request method in request
     *          "headers": {}, // merged with request.headers; headers with the same name in opts override those in request
     *          "body": SeekableStream | Buffer | String | {}, // overrides request.body
     *          "keepAlive": unknown, // overrides the keep-alive setting
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "redirect": "follow", // redirect mode: "follow" (default) | "error" | "manual"
     *          "signal": AbortSignal, // AbortSignal object used to cancel the request
     *          "streaming": false // whether to return the response body in streaming mode
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which overrides no information in request
     *      @param request request source object, provides basic information such as url, method, headers and body
     *      @param opts the additional information, can override the corresponding fields in request
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetch(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    fetch(request: Class_HttpRequest, opts?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *      opts can override the request fields in request; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "method": "GET", // overrides the request method in request
     *          "headers": {}, // merged with request.headers; headers with the same name in opts override those in request
     *          "body": SeekableStream | Buffer | String | {}, // overrides request.body
     *          "keepAlive": unknown, // overrides the keep-alive setting
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "redirect": "follow", // redirect mode: "follow" (default) | "error" | "manual"
     *          "signal": AbortSignal, // AbortSignal object used to cancel the request
     *          "streaming": false // whether to return the response body in streaming mode
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which overrides no information in request
     *      @param request request source object, provides basic information such as url, method, headers and body
     *      @param opts the additional information, can override the corresponding fields in request
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetchSync(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *      opts can override the request fields in request; the supported contents are as follows:
     *      ```JavaScript
     *      {
     *          "method": "GET", // overrides the request method in request
     *          "headers": {}, // merged with request.headers; headers with the same name in opts override those in request
     *          "body": SeekableStream | Buffer | String | {}, // overrides request.body
     *          "keepAlive": unknown, // overrides the keep-alive setting
     *          "timeout": 0, // request timeout in milliseconds, uses the client default settings by default
     *          "redirect": "follow", // redirect mode: "follow" (default) | "error" | "manual"
     *          "signal": AbortSignal, // AbortSignal object used to cancel the request
     *          "streaming": false // whether to return the response body in streaming mode
     *      }
     *      ```
     *      body, json and pack must not appear at the same time. Default is {}, which overrides no information in request
     *      @param request request source object, provides basic information such as url, method, headers and body
     *      @param opts the additional information, can override the corresponding fields in request
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetchAsync(request: Class_HttpRequest, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponse>;

}

