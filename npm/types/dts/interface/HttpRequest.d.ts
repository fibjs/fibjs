/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/// <reference path="../interface/FormData.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/**
 * @description HttpRequest is the class used to handle HTTP requests; it allows you to create HTTP requests and interact with servers. You can use it to send GET, POST and other types of HTTP requests to a Web server
 *
 * Suppose we have a query parameter whose key is name; we return different results according to this parameter: if the parameter is empty, return "Hello world!"; if the parameter is "fibjs", return "Hello fibjs!"; otherwise return "Hello some body!".
 *
 * The implementation is as follows:
 * ```JavaScript
 * const http = require('http');
 *
 * var svr = new http.Server(8080, (req) => {
 *   var name = req.query.get('name');
 *   var msg = name ? `Hello ${name}!` : 'Hello world!';
 *
 *   req.response.write(msg);
 * });
 *
 * svr.start();
 * ```
 *
 * Here we use `req.query`, a Collection type, which represents the query parameters in the HTTP request URL.
 *
 * When we access http://127.0.0.1:8080/?name=fibjs from a browser, the server response we get is `Hello fibjs!`.
 *
 */
declare class Class_HttpRequest extends Class_HttpMessage {
    /**
     * @description HttpRequest constructor, creates a new HttpRequest object
     */
    constructor();

    /**
     * @description HttpRequest constructor, copies from an existing Request object and can override options (Fetch API)
     *      @param request the existing HttpRequest object
     *      @param options the override options, which can contain fields such as method, headers and body
     *
     */
    constructor(request: Class_HttpRequest | Class_HttpRequestPromise, options?: FIBJS.GeneralObject);

    /**
     * @description HttpRequest constructor, creates a request object from a URL string and options (Fetch API)
     *      @param url the request URL
     *      @param options the request options, which can contain fields such as method, headers and body
     *
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description gets the response message object
     */
    readonly response: Class_HttpResponse;

    /**
     * @description queries and sets the request method
     */
    method: string;

    /**
     * @description queries and sets the request address
     */
    address: string;

    /**
     * @description queries and sets the URL path and query string of the request, for example /path?key=value
     */
    url: string;

    /**
     * @description gets the complete URL of the request, including protocol, host, path and query string
     */
    readonly href: string;

    /**
     * @description queries and sets the request query string
     */
    queryString: string;

    /**
     * @description gets the container holding the message cookies
     */
    readonly cookies: Class_HttpCollection;

    /**
     * @description gets the container holding the message form
     */
    readonly form: Class_FormData;

    /**
     * @description gets the container holding the message query
     */
    readonly query: Class_URLSearchParams;

    /**
     * @description aborts the request and closes the underlying connection
     */
    abort(): void;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/// <reference path="../interface/FormData.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/**
 * The promise variant of the HttpRequest class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpRequestPromise extends Class_HttpMessagePromise {
    /**
     * @description HttpRequest constructor, creates a new HttpRequest object
     */
    constructor();

    /**
     * @description HttpRequest constructor, copies from an existing Request object and can override options (Fetch API)
     *      @param request the existing HttpRequest object
     *      @param options the override options, which can contain fields such as method, headers and body
     *
     */
    constructor(request: Class_HttpRequest | Class_HttpRequestPromise, options?: FIBJS.GeneralObject);

    /**
     * @description HttpRequest constructor, creates a request object from a URL string and options (Fetch API)
     *      @param url the request URL
     *      @param options the request options, which can contain fields such as method, headers and body
     *
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description gets the response message object
     */
    readonly response: Class_HttpResponsePromise;

    /**
     * @description queries and sets the request method
     */
    method: string;

    /**
     * @description queries and sets the request address
     */
    address: string;

    /**
     * @description queries and sets the URL path and query string of the request, for example /path?key=value
     */
    url: string;

    /**
     * @description gets the complete URL of the request, including protocol, host, path and query string
     */
    readonly href: string;

    /**
     * @description queries and sets the request query string
     */
    queryString: string;

    /**
     * @description gets the container holding the message cookies
     */
    readonly cookies: Class_HttpCollection;

    /**
     * @description gets the container holding the message form
     */
    readonly form: Class_FormData;

    /**
     * @description gets the container holding the message query
     */
    readonly query: Class_URLSearchParams;

    /**
     * @description aborts the request and closes the underlying connection
     */
    abort(): void;

}


declare namespace Class_HttpRequest {
    const promises: FIBJS.GeneralObject;
}
