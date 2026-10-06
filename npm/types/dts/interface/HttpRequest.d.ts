/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HttpMessage.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/HttpCollection.d.ts" />
/// <reference path="../interface/FormData.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/**
 * @description The HTTP request message: what a server receives and what a client sends
 *
 *  An HttpRequest carries the request line (method, address, query string), the header
 *  collection and the body, plus the live HttpResponse it belongs to. The body API, the
 *  header API and the connection metadata come from HttpMessage and Message. In an
 *  http.Server handler the first argument is an HttpRequest and its response is the object
 *  to answer with. Node.js exposes the server side as http.IncomingMessage and the client
 *  side as http.ClientRequest; fibjs uses one class for both (http.IncomingMessage is an
 *  alias) and adds the parsed query, cookies, form and href members.
 *
 *  Concepts:
 *
 *  - **Request line**: method is the verb, address is the path part (`/a/b`), queryString is
 *    the text after `?` and url joins the two; assigning url splits it at the first `?`.
 *    href rebuilds the absolute URL from X-Forwarded-Proto (or the TLS socket), the Host
 *    header and address/query, which is the fibjs equivalent of the WHATWG Request.url.
 *  - **Parsed request data**: query is a URLSearchParams built from queryString, cookies is
 *    an HttpCollection built from the Cookie header and form is a FormData built from the
 *    body according to Content-Type; each is parsed on first access and cached, so later
 *    changes of queryString or of the Cookie header do not refresh them.
 *  - **Body**: inherited from Message; read, readAll, text, json, pack, formData, bytes and
 *    blob consume the body in order and bodyUsed records that a streaming read happened. A
 *    body received from the network belongs to the connection; rewind req.body before
 *    reading it twice. The query/cookies/form getters parse without consuming the caller's
 *    view of the body.
 *  - **Server side**: the parsed request is reused for every keep-alive request on the same
 *    connection; req.response is the same HttpResponse the handler receives as its second
 *    argument, and req.socket/req.stream expose the connection.
 *  - **Client side**: http.request, http.get and the other asynchronous functions return the
 *    HttpRequest they queued; after completion its response property holds the same
 *    HttpResponse delivered to the callback. abort() cancels that request.
 *
 *  Obtained from:
 *  - the handler of an http.Server — the first argument of every handler form;
 *  - `new http.Request()` — an empty request, filled by readFrom() or by direct assignment;
 *  - `new http.Request(url, options)` — a Fetch-style request for http.fetch() and the
 *    http.Client methods; the whole URL is stored in address;
 *  - `http.request(...)`, `http.get(...)` and their siblings — the queued client request
 *    whose response property receives the answer.
 *
 *  Example 1 — build a request and inspect the derived members:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const req = new http.Request();
 *  req.method = 'POST';
 *  req.url = '/submit?tag=a&tag=b';
 *  req.setHeader('Host', 'example.com');
 *  req.appendHeader('Cookie', 'session=42');
 *
 *  console.log(req.address, req.queryString); // /submit tag=a&tag=b
 *  console.log(req.query.getAll('tag').join(',')); // a,b
 *  console.log(req.cookies.get('session')); // 42
 *  console.log(req.href); // http://example.com/submit?tag=a&tag=b
 *  ```
 *
 *  Example 2 — a handler reads the query, the cookies and an urlencoded body:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, (req) => {
 *      const res = req.response;
 *      if (req.address === '/echo') {
 *          res.json({
 *              q: req.query.get('q'),
 *              cookie: req.cookies.get('sid'),
 *              user: req.form.get('user')
 *          });
 *          return;
 *      }
 *      res.write('hi ' + req.query.get('name'));
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  console.log(http.getSync('http://127.0.0.1:' + port + '/?name=fibjs').text()); // hi fibjs
 *
 *  const echo = http.postSync('http://127.0.0.1:' + port + '/echo?q=1', {
 *      headers: { Cookie: 'sid=abc', 'Content-Type': 'application/x-www-form-urlencoded' },
 *      body: 'user=lion'
 *  });
 *  console.log(JSON.stringify(echo.json())); // {"q":"1","cookie":"abc","user":"lion"}
 *
 *  server.stop();
 *  ```
 *
 *  Example 3 — a Fetch-style request handed to http.fetch():
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, (req) => {
 *      req.response.write(req.method + ' ' + req.text());
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const req = new http.Request('http://127.0.0.1:' + port + '/api', {
 *      method: 'POST',
 *      body: 'payload'
 *  });
 *  const res = http.fetch(req);
 *  console.log(res.statusCode); // 200
 *  console.log(res.text()); // POST payload
 *
 *  server.stop();
 *  ```
 *
 *  Notes:
 *  - Headers parsed from the wire keep their original names; lookup through hasHeader,
 *    firstHeader and the Headers collection is case-insensitive.
 *  - A Request built from a URL keeps the whole URL in address; use readFrom() when the
 *    message comes from a stream.
 *
 */
declare class Class_HttpRequest extends Class_HttpMessage {
    /**
     * @description Creates an empty request
     *
     *      The message starts as a GET request for "/" with no query string, protocol HTTP/1.1
     *      and keepAlive on; the body is empty and the response object is created on first
     *      access. Assign the members directly, or fill the request from a stream with the
     *      inherited readFrom(). Use the URL or the copy constructor for the Web Request forms.
     *
     */
    constructor();

    /**
     * @description Copies an existing request, optionally overriding members (Fetch API)
     *
     *      The copy keeps method, address, query string, headers and body of the source; every
     *      member present in options overrides it. An explicit headers value replaces the whole
     *      header set of the copy (an empty object clears it) while undefined members keep the
     *      source values, following WebIDL. A GET or HEAD result must not carry a body and
     *      throws a TypeError (20024); a body is cloneable only when it is a seekable stream.
     *      Matches the WHATWG `new Request(request, init)` form.
     *
     *      @param request the existing HttpRequest object
     *      @param options the override options; method, headers, body and keepAlive are read
     *
     */
    constructor(request: Class_HttpRequest | Class_HttpRequestPromise, options?: FIBJS.GeneralObject);

    /**
     * @description Creates a client request from a URL and options (Fetch API)
     *
     *      The URL is stored in address exactly as given, because the client functions read the
     *      target from that property. method defaults to GET; options accepts headers (object
     *      or Headers), body, json, pack, keepAlive, timeout, redirect, signal, streaming and
     *      agent. A string body defaults to text/plain;charset=UTF-8 here, while the http
     *      client functions use application/x-www-form-urlencoded for strings. A body with GET
     *      or HEAD throws a TypeError (20024). Hand the request to http.fetch() or a Client.
     *
     *      @param url the request URL
     *      @param options the request options; see the detail for the keys that are read
     *
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description The HttpResponse bound to this request, read-only
     *
     *      Server side: created on first access and the same object the handler receives as its
     *      second argument, so either name writes the same reply. Client side: the asynchronous
     *      http functions fill it with the response and deliver the same object to the callback,
     *      so the property can be read after completion. A request built in memory and never
     *      sent simply owns an empty 200 response.
     *
     *      Example — the response property of an asynchronous client request:
     *      ```JavaScript
     *      const http = require('http');
     *      const coroutine = require('coroutine');
     *
     *      const server = new http.Server(0, (req) => { req.response.write('pong'); });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      let delivered = null;
     *      const req = http.get('http://127.0.0.1:' + port + '/ping', (res) => { delivered = res; });
     *      while (delivered === null)
     *          coroutine.sleep(10);
     *
     *      console.log(req.response === delivered); // true
     *      console.log(req.response.text()); // pong
     *
     *      server.stop();
     *      ```
     *
     */
    readonly response: Class_HttpResponse;

    /**
     * @description Queries and sets the request method
     *
     *      The value is stored verbatim: no uppercasing and no validation. The parser sets it
     *      from the request line and the constructors default it to GET. Routing, HEAD handling
     *      and the WebSocket upgrade check compare it case-insensitively. Node.js exposes the
     *      same value as IncomingMessage.method.
     *
     */
    method: string;

    /**
     * @description Queries and sets the request address, the path part of the request line
     *
     *      A parsed server request stores only the path (`/a/b`) here and routing maps match
     *      against it; the Fetch-style constructors store the whole URL instead because the
     *      client functions read the target from this property. Changing it does not touch
     *      queryString. Node.js IncomingMessage.url contains path and query together.
     *
     */
    address: string;

    /**
     * @description Queries and sets the path and query string together, as in /path?key=value
     *
     *      Reading joins address and '?' + queryString when the query is not empty; assigning
     *      splits the value at the first '?' and a value without '?' clears the query string.
     *      Nothing is normalized or encoded here. Node.js IncomingMessage.url has the same read
     *      shape but is not assignable.
     *
     */
    url: string;

    /**
     * @description The absolute URL of the request, read-only
     *
     *      Rebuilt on each access: the protocol comes from the X-Forwarded-Proto header when
     *      present, otherwise from the TLS socket (https) or plain TCP (http); the authority
     *      comes from the Host header, falling back to localhost, and the address and query
     *      string are appended. It is the fibjs equivalent of the WHATWG Request.url.
     *
     *      Example — href follows the forwarded protocol and the Host header:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const req = new http.Request();
     *      req.url = '/items?page=2';
     *      console.log(req.href); // http://localhost/items?page=2
     *
     *      req.setHeader('Host', 'api.example.com');
     *      req.setHeader('X-Forwarded-Proto', 'https');
     *      console.log(req.href); // https://api.example.com/items?page=2
     *      ```
     *
     */
    readonly href: string;

    /**
     * @description Queries and sets the raw query string, the text after the ? character
     *
     *      Stored without the leading '?', for example "a=1&b=2"; the parser fills it from the
     *      request line and the setters of url update it. Assigning it does not refresh an
     *      already parsed query object. Node.js keeps the same text inside
     *      IncomingMessage.url.
     *
     */
    queryString: string;

    /**
     * @description The request cookies as an HttpCollection, read-only
     *
     *      Parsed from the Cookie header on first access and cached; get(name) and all(name)
     *      read the values while duplicate names keep every value. Changing the Cookie header
     *      after the first access does not refresh the collection. Node.js does not parse
     *      cookies on IncomingMessage.
     *
     */
    readonly cookies: Class_HttpCollection;

    /**
     * @description The request body parsed as a FormData object, read-only
     *
     *      Parsed on first access according to Content-Type: application/x-www-form-urlencoded
     *      and multipart/form-data (with a boundary) are accepted. A missing Content-Type
     *      throws "Content-Type is missing", another type throws "unknown form format" and an
     *      empty body gives an empty FormData (error 20024). The parse rewinds the seekable
     *      body and does not set bodyUsed, so the body remains readable afterwards. Node.js has
     *      no built-in form parsing.
     *
     *      Example — read an urlencoded form posted to a server:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const req = new http.Request();
     *      req.method = 'POST';
     *      req.setHeader('Content-Type', 'application/x-www-form-urlencoded');
     *      req.write('name=lion&role=dev');
     *
     *      console.log(req.form.get('name')); // lion
     *      console.log(req.form.get('role')); // dev
     *      ```
     *
     */
    readonly form: Class_FormData;

    /**
     * @description The parsed query string as a URLSearchParams object, read-only
     *
     *      Built once from queryString on first access and cached; get, getAll, has and the
     *      iteration helpers read it, but changes made through the URLSearchParams object are
     *      not written back to the request. Change queryString or url before the first access to
     *      affect it. Node.js exposes no parsed query on IncomingMessage.
     *
     */
    readonly query: Class_URLSearchParams;

    /**
     * @description Aborts the request and closes the underlying connection
     *
     *      Closes the TCP socket or TLS stream the message was read from; a message built in
     *      memory has no socket and the call is a no-op. On an asynchronous client request it
     *      cancels the pending operation, which fails with error 20022 (Operation was aborted);
     *      on a server request it drops the connection to the client.
     *
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
     * @description Creates an empty request
     *
     *      The message starts as a GET request for "/" with no query string, protocol HTTP/1.1
     *      and keepAlive on; the body is empty and the response object is created on first
     *      access. Assign the members directly, or fill the request from a stream with the
     *      inherited readFrom(). Use the URL or the copy constructor for the Web Request forms.
     *
     */
    constructor();

    /**
     * @description Copies an existing request, optionally overriding members (Fetch API)
     *
     *      The copy keeps method, address, query string, headers and body of the source; every
     *      member present in options overrides it. An explicit headers value replaces the whole
     *      header set of the copy (an empty object clears it) while undefined members keep the
     *      source values, following WebIDL. A GET or HEAD result must not carry a body and
     *      throws a TypeError (20024); a body is cloneable only when it is a seekable stream.
     *      Matches the WHATWG `new Request(request, init)` form.
     *
     *      @param request the existing HttpRequest object
     *      @param options the override options; method, headers, body and keepAlive are read
     *
     */
    constructor(request: Class_HttpRequest | Class_HttpRequestPromise, options?: FIBJS.GeneralObject);

    /**
     * @description Creates a client request from a URL and options (Fetch API)
     *
     *      The URL is stored in address exactly as given, because the client functions read the
     *      target from that property. method defaults to GET; options accepts headers (object
     *      or Headers), body, json, pack, keepAlive, timeout, redirect, signal, streaming and
     *      agent. A string body defaults to text/plain;charset=UTF-8 here, while the http
     *      client functions use application/x-www-form-urlencoded for strings. A body with GET
     *      or HEAD throws a TypeError (20024). Hand the request to http.fetch() or a Client.
     *
     *      @param url the request URL
     *      @param options the request options; see the detail for the keys that are read
     *
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description The HttpResponse bound to this request, read-only
     *
     *      Server side: created on first access and the same object the handler receives as its
     *      second argument, so either name writes the same reply. Client side: the asynchronous
     *      http functions fill it with the response and deliver the same object to the callback,
     *      so the property can be read after completion. A request built in memory and never
     *      sent simply owns an empty 200 response.
     *
     *      Example — the response property of an asynchronous client request:
     *      ```JavaScript
     *      const http = require('http');
     *      const coroutine = require('coroutine');
     *
     *      const server = new http.Server(0, (req) => { req.response.write('pong'); });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      let delivered = null;
     *      const req = http.get('http://127.0.0.1:' + port + '/ping', (res) => { delivered = res; });
     *      while (delivered === null)
     *          coroutine.sleep(10);
     *
     *      console.log(req.response === delivered); // true
     *      console.log(req.response.text()); // pong
     *
     *      server.stop();
     *      ```
     *
     */
    readonly response: Class_HttpResponsePromise;

    /**
     * @description Queries and sets the request method
     *
     *      The value is stored verbatim: no uppercasing and no validation. The parser sets it
     *      from the request line and the constructors default it to GET. Routing, HEAD handling
     *      and the WebSocket upgrade check compare it case-insensitively. Node.js exposes the
     *      same value as IncomingMessage.method.
     *
     */
    method: string;

    /**
     * @description Queries and sets the request address, the path part of the request line
     *
     *      A parsed server request stores only the path (`/a/b`) here and routing maps match
     *      against it; the Fetch-style constructors store the whole URL instead because the
     *      client functions read the target from this property. Changing it does not touch
     *      queryString. Node.js IncomingMessage.url contains path and query together.
     *
     */
    address: string;

    /**
     * @description Queries and sets the path and query string together, as in /path?key=value
     *
     *      Reading joins address and '?' + queryString when the query is not empty; assigning
     *      splits the value at the first '?' and a value without '?' clears the query string.
     *      Nothing is normalized or encoded here. Node.js IncomingMessage.url has the same read
     *      shape but is not assignable.
     *
     */
    url: string;

    /**
     * @description The absolute URL of the request, read-only
     *
     *      Rebuilt on each access: the protocol comes from the X-Forwarded-Proto header when
     *      present, otherwise from the TLS socket (https) or plain TCP (http); the authority
     *      comes from the Host header, falling back to localhost, and the address and query
     *      string are appended. It is the fibjs equivalent of the WHATWG Request.url.
     *
     *      Example — href follows the forwarded protocol and the Host header:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const req = new http.Request();
     *      req.url = '/items?page=2';
     *      console.log(req.href); // http://localhost/items?page=2
     *
     *      req.setHeader('Host', 'api.example.com');
     *      req.setHeader('X-Forwarded-Proto', 'https');
     *      console.log(req.href); // https://api.example.com/items?page=2
     *      ```
     *
     */
    readonly href: string;

    /**
     * @description Queries and sets the raw query string, the text after the ? character
     *
     *      Stored without the leading '?', for example "a=1&b=2"; the parser fills it from the
     *      request line and the setters of url update it. Assigning it does not refresh an
     *      already parsed query object. Node.js keeps the same text inside
     *      IncomingMessage.url.
     *
     */
    queryString: string;

    /**
     * @description The request cookies as an HttpCollection, read-only
     *
     *      Parsed from the Cookie header on first access and cached; get(name) and all(name)
     *      read the values while duplicate names keep every value. Changing the Cookie header
     *      after the first access does not refresh the collection. Node.js does not parse
     *      cookies on IncomingMessage.
     *
     */
    readonly cookies: Class_HttpCollection;

    /**
     * @description The request body parsed as a FormData object, read-only
     *
     *      Parsed on first access according to Content-Type: application/x-www-form-urlencoded
     *      and multipart/form-data (with a boundary) are accepted. A missing Content-Type
     *      throws "Content-Type is missing", another type throws "unknown form format" and an
     *      empty body gives an empty FormData (error 20024). The parse rewinds the seekable
     *      body and does not set bodyUsed, so the body remains readable afterwards. Node.js has
     *      no built-in form parsing.
     *
     *      Example — read an urlencoded form posted to a server:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const req = new http.Request();
     *      req.method = 'POST';
     *      req.setHeader('Content-Type', 'application/x-www-form-urlencoded');
     *      req.write('name=lion&role=dev');
     *
     *      console.log(req.form.get('name')); // lion
     *      console.log(req.form.get('role')); // dev
     *      ```
     *
     */
    readonly form: Class_FormData;

    /**
     * @description The parsed query string as a URLSearchParams object, read-only
     *
     *      Built once from queryString on first access and cached; get, getAll, has and the
     *      iteration helpers read it, but changes made through the URLSearchParams object are
     *      not written back to the request. Change queryString or url before the first access to
     *      affect it. Node.js exposes no parsed query on IncomingMessage.
     *
     */
    readonly query: Class_URLSearchParams;

    /**
     * @description Aborts the request and closes the underlying connection
     *
     *      Closes the TCP socket or TLS stream the message was read from; a message built in
     *      memory has no socket and the call is a no-op. On an asynchronous client request it
     *      cancels the pending operation, which fails with error 20022 (Operation was aborted);
     *      on a server request it drops the connection to the client.
     *
     */
    abort(): void;

}


declare namespace Class_HttpRequest {
    const promises: FIBJS.GeneralObject;
}
