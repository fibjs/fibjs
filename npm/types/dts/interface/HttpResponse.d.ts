/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpCookie.d.ts" />
/**
 * @description HttpResponse is an HTTP response object; use the HttpRequest.response object to complete the Http server-side data response, or use http.request to request and return the server's response data
 *
 * The following example shows how to use it in http.Server; the sample code is as follows:
 * ```
 * const http = require('http');
 *
 * const server = new http.Server(8080, (request) => {
 *   // retreive the response object
 *   const response = request.response;
 *   // set the status code
 *   response.statusCode = 200;
 *   // set the content type to text/plain
 *   response.setHeader('Content-Type', 'text/plain');
 *   // write the response body
 *   response.write('ok');
 * });
 *
 * server.start();
 * ```
 *
 */
declare class Class_HttpResponse extends Class_HttpMessage {
    /**
     * @description HttpResponse constructor, creates a new HttpResponse object
     */
    constructor();

    /**
     * @description HttpResponse constructor, creates a new HttpResponse object (Web API compatible)
     *
     *      Supports the Web standard Response construction style, for example:
     *      ```JavaScript
     *      const response = new http.Response("Hello World", {
     *          status: 200,
     *          statusText: "OK",
     *          headers: { "Content-Type": "text/plain" }
     *      });
     *      ```
     *      @param body the response body content, which can be a string, Buffer or null
     *      @param options the options object, supporting the status, statusText and headers properties
     *
     */
    constructor(body: any, options?: FIBJS.GeneralObject);

    /**
     * @description queries and sets the return status of the response message
     */
    statusCode: number;

    /**
     * @description queries and sets the return message of the response message
     */
    statusMessage: string;

    /**
     * @description queries and sets the return message of the response message, same as statusMessage (Web API compatible)
     */
    statusText: string;

    /**
     * @description queries and sets the return status of the response message, same as statusCode
     */
    status: number;

    /**
     * @description queries whether the current response is ok
     */
    readonly ok: boolean;

    /**
     * @description sets the return status of the response message and adds response headers
     *      @param statusCode specifies the return status of the response message
     *      @param headers specifies the response headers to add to the response message
     *
     */
    writeHead(statusCode: number, headers?: FIBJS.GeneralObject): void;

    /**
     * @description sets the return status and return message of the response message, and adds response headers
     *      @param statusCode specifies the return status of the response message
     *      @param statusMessage specifies the return message of the response message
     *      @param headers specifies the response headers to add to the response message
     *
     */
    writeHead(statusCode: number, statusMessage: string, headers?: FIBJS.GeneralObject): void;

    /**
     * @description returns the list of HttpCookie objects of the current message
     */
    readonly cookies: Class_HttpCookie[];

    /**
     * @description adds an HttpCookie object to cookies
     *
     *      cookie may be an HttpCookie object, or an options object the HttpCookie constructor
     *      accepts (name, value, path, domain, ...).
     *      @param cookie the cookie to add
     *
     */
    addCookie(cookie: Class_HttpCookie | FIBJS.GeneralObject): void;

    /**
     * @description sends a redirect to the client
     *      @param url the redirect address
     *
     */
    redirect(url: string): void;

    /**
     * @description sends a redirect to the client
     *      @param statusCode specifies the return status of the response message; the accepted statuses are: 301, 302, 307
     *      @param url the redirect address
     *
     */
    redirect(statusCode: number, url: string): void;

    /**
     * @description the final URL of the Fetch API response (the address after redirections)
     */
    readonly url: string;

    /**
     * @description whether it has been redirected
     */
    readonly redirected: boolean;

    /**
     * @description response type ("basic", "cors", "error", etc.), overrides Message.type
     */
    readonly type: string;

    /**
     * @description writes the given data encoded as JSON, and can set the response status and headers at the same time
     *      @param data the data to serialize to JSON
     *      @param options the options object, supporting status, statusText and headers
     *      @return this method does not return data
     *
     */
    json(data: any, options?: FIBJS.GeneralObject): any;

    json(data: any, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description writes the given data encoded as JSON, and can set the response status and headers at the same time
     *      @param data the data to serialize to JSON
     *      @param options the options object, supporting status, statusText and headers
     *      @return this method does not return data
     *
     */
    jsonSync(data: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description writes the given data encoded as JSON, and can set the response status and headers at the same time
     *      @param data the data to serialize to JSON
     *      @param options the options object, supporting status, statusText and headers
     *      @return this method does not return data
     *
     */
    jsonAsync(data: any, options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    json(): any;

    json(callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    jsonSync(): any;

    /**
     * @description parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    jsonAsync(): Promise<any>;

    /**
     * @description creates a JSON response (static factory)
     *      @param data the data to serialize to JSON
     *      @param options the options object, supporting status, statusText and headers
     *      @return returns a new HttpResponse object
     *
     */
    static json(data: any, options?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description creates a redirect response (static factory)
     *      @param url the redirect target URL
     *      @param status the redirect status code, default is 302
     *      @return returns a new HttpResponse object
     *
     */
    static redirect(url: string, status?: number): Class_HttpResponse;

    /**
     * @description creates an error response (static factory)
     *      @return returns a new HttpResponse object with type="error"
     *
     */
    static error(): Class_HttpResponse;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpCookie.d.ts" />
/**
 * The promise variant of the HttpResponse class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpResponsePromise extends Class_HttpMessagePromise {
    /**
     * @description HttpResponse constructor, creates a new HttpResponse object
     */
    constructor();

    /**
     * @description HttpResponse constructor, creates a new HttpResponse object (Web API compatible)
     *
     *      Supports the Web standard Response construction style, for example:
     *      ```JavaScript
     *      const response = new http.Response("Hello World", {
     *          status: 200,
     *          statusText: "OK",
     *          headers: { "Content-Type": "text/plain" }
     *      });
     *      ```
     *      @param body the response body content, which can be a string, Buffer or null
     *      @param options the options object, supporting the status, statusText and headers properties
     *
     */
    constructor(body: any, options?: FIBJS.GeneralObject);

    /**
     * @description queries and sets the return status of the response message
     */
    statusCode: number;

    /**
     * @description queries and sets the return message of the response message
     */
    statusMessage: string;

    /**
     * @description queries and sets the return message of the response message, same as statusMessage (Web API compatible)
     */
    statusText: string;

    /**
     * @description queries and sets the return status of the response message, same as statusCode
     */
    status: number;

    /**
     * @description queries whether the current response is ok
     */
    readonly ok: boolean;

    /**
     * @description sets the return status of the response message and adds response headers
     *      @param statusCode specifies the return status of the response message
     *      @param headers specifies the response headers to add to the response message
     *
     */
    writeHead(statusCode: number, headers?: FIBJS.GeneralObject): void;

    /**
     * @description sets the return status and return message of the response message, and adds response headers
     *      @param statusCode specifies the return status of the response message
     *      @param statusMessage specifies the return message of the response message
     *      @param headers specifies the response headers to add to the response message
     *
     */
    writeHead(statusCode: number, statusMessage: string, headers?: FIBJS.GeneralObject): void;

    /**
     * @description returns the list of HttpCookie objects of the current message
     */
    readonly cookies: Class_HttpCookie[];

    /**
     * @description adds an HttpCookie object to cookies
     *
     *      cookie may be an HttpCookie object, or an options object the HttpCookie constructor
     *      accepts (name, value, path, domain, ...).
     *      @param cookie the cookie to add
     *
     */
    addCookie(cookie: Class_HttpCookie | FIBJS.GeneralObject): void;

    /**
     * @description sends a redirect to the client
     *      @param url the redirect address
     *
     */
    redirect(url: string): void;

    /**
     * @description sends a redirect to the client
     *      @param statusCode specifies the return status of the response message; the accepted statuses are: 301, 302, 307
     *      @param url the redirect address
     *
     */
    redirect(statusCode: number, url: string): void;

    /**
     * @description the final URL of the Fetch API response (the address after redirections)
     */
    readonly url: string;

    /**
     * @description whether it has been redirected
     */
    readonly redirected: boolean;

    /**
     * @description response type ("basic", "cors", "error", etc.), overrides Message.type
     */
    readonly type: string;

    /**
     * @description writes the given data encoded as JSON, and can set the response status and headers at the same time
     *      @param data the data to serialize to JSON
     *      @param options the options object, supporting status, statusText and headers
     *      @return this method does not return data
     *
     */
    json(data: any, options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description writes the given data encoded as JSON, and can set the response status and headers at the same time
     *      @param data the data to serialize to JSON
     *      @param options the options object, supporting status, statusText and headers
     *      @return this method does not return data
     *
     */
    jsonSync(data: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description writes the given data encoded as JSON, and can set the response status and headers at the same time
     *      @param data the data to serialize to JSON
     *      @param options the options object, supporting status, statusText and headers
     *      @return this method does not return data
     *
     */
    jsonAsync(data: any, options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    json(): Promise<any>;

    /**
     * @description parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    jsonSync(): any;

    /**
     * @description parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    jsonAsync(): Promise<any>;

    /**
     * @description creates a JSON response (static factory)
     *      @param data the data to serialize to JSON
     *      @param options the options object, supporting status, statusText and headers
     *      @return returns a new HttpResponse object
     *
     */
    static json(data: any, options?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description creates a redirect response (static factory)
     *      @param url the redirect target URL
     *      @param status the redirect status code, default is 302
     *      @return returns a new HttpResponse object
     *
     */
    static redirect(url: string, status?: number): Class_HttpResponse;

    /**
     * @description creates an error response (static factory)
     *      @return returns a new HttpResponse object with type="error"
     *
     */
    static error(): Class_HttpResponse;

}


declare namespace Class_HttpResponse {
    const promises: {
        readonly json: (data: any, options?: FIBJS.GeneralObject)=>Class_HttpResponse;
        readonly redirect: (url: string, status?: number)=>Class_HttpResponse;
        readonly error: ()=>Class_HttpResponse;
    };

}

