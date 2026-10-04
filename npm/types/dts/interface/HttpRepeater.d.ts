/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/HttpClient.d.ts" />
/**
 * @description HttpRepeater is an HTTP request forwarder that can forward HTTP requests to a specified backend server and obtain responses. It is often used in complex systems where the front end interacts with multiple servers, or for load balancing
 *
 * Using HttpRepeater is very simple; just provide the URL of the backend server or an array of load-balancing URLs when creating the instance.
 *
 * The following is an example using a single backend:
 * ```JavaScript
 * var http = require('http');
 * var serverUrl = 'http://localhost:' + actualPort + '/example'
 * var repeater = new http.Repeater(serverUrl)
 *
 * var server = new http.Server(8081, repeater);
 * server.start();
 * ```
 * The following is an example using a URL array to implement load balancing:
 * ```JavaScript
 * var serverURLs = [
 *   'http://server1.example.com',
 *   'http://server2.example.com',
 *   'http://server3.example.com'
 * ]
 * var repeater = new http.Repeater(serverURLs)
 *
 * var server = new http.Server(8081, repeater);
 * server.start();
 * ```
 *
 */
declare class Class_HttpRepeater extends Class_Handler {
    /**
     * @description HttpRepeater constructor, creates a new HttpRepeater object
     *      @param url specifies a backend server url
     *
     */
    constructor(url: string);

    /**
     * @description HttpRepeater constructor, creates a new HttpRepeater object
     *      @param urls specifies a group of backend server urls
     *
     */
    constructor(urls: string[]);

    /**
     * @description loads a new group of backend urls
     *      @param urls specifies a group of backend server urls
     *
     */
    load(urls: string[]): void;

    /**
     * @description queries the current list of backend server urls
     */
    readonly urls: string[];

    /**
     * @description the HttpClient object used internally by the request forwarding handler
     */
    readonly client: Class_HttpClient;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/HttpClient.d.ts" />
/**
 * The promise variant of the HttpRepeater class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpRepeaterPromise extends Class_HandlerPromise {
    /**
     * @description HttpRepeater constructor, creates a new HttpRepeater object
     *      @param url specifies a backend server url
     *
     */
    constructor(url: string);

    /**
     * @description HttpRepeater constructor, creates a new HttpRepeater object
     *      @param urls specifies a group of backend server urls
     *
     */
    constructor(urls: string[]);

    /**
     * @description loads a new group of backend urls
     *      @param urls specifies a group of backend server urls
     *
     */
    load(urls: string[]): void;

    /**
     * @description queries the current list of backend server urls
     */
    readonly urls: string[];

    /**
     * @description the HttpClient object used internally by the request forwarding handler
     */
    readonly client: Class_HttpClientPromise;

}


declare namespace Class_HttpRepeater {
    const promises: FIBJS.GeneralObject;
}
