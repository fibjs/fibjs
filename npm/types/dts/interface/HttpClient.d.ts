/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpCookie.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * @description The HttpClient class provides an independent HTTP/HTTPS client: its own connection pool, cookie jar, defaults and optional TLS identity
 *
 *  HttpClient is the object behind every client function of the http module. It owns a pool of
 *  keep-alive connections, a cookie jar and the settings (`keepAlive`, `timeout`, `autoRedirect`,
 *  `enableEncoding`, `enableH2`, `userAgent`, the size limits and the proxy environment) used for
 *  its requests. The module-level functions (`http.getSync`, `http.request`, ...) share one hidden
 *  client configured by the module properties; create an HttpClient when you need different
 *  settings, an isolated cookie jar, a client certificate or simply a separate connection pool.
 *
 *  Obtained from:
 *  - `new http.Client(options)` — the class is exported as `http.Client`; `http.Agent` is the
 *    same object, provided for Node.js compatibility;
 *  - `new http.Client(secureContext)` — creates the client directly from a tls.SecureContext;
 *  - `new http.Client({ ...tls options, keepAlive, timeout, ... })` — the TLS options build the
 *    secure context and the remaining properties set the client defaults.
 *
 *  Concepts:
 *
 *  - **Connection pool**: an HttpClient pools finished keep-alive connections in one idle list and
 *    reuses them for later requests to the same host. `poolTimeout` (default 10000 ms) expires an
 *    idle connection and `maxFreeSockets` (default 256) caps the number of idle connections kept
 *    by the client as a whole, not per host as in Node.js. `maxSockets` and `maxTotalSockets` are
 *    accepted for Node.js compatibility but are not enforced. `freeSockets`, `sockets` and
 *    `totalSocketCount` are compatibility accessors: fibjs returns an empty object or 0 instead of
 *    exposing the pool contents, and `destroy()` releases the pooled connections.
 *  - **Cookies**: the client keeps its own cookie jar (`cookies`). With `enableCookie` enabled
 *    (the default) the Set-Cookie headers of every response are stored and matching cookies are
 *    sent with later requests; two clients never share cookies.
 *  - **Redirects**: with `autoRedirect` enabled (the default) 301, 302, 303, 307 and 308 responses
 *    are followed; 303 switches to GET and drops the body. There is no redirect count limit, but a
 *    URL seen twice in one chain raises a cyclic redirect error; `redirect: 'manual'` on fetch
 *    returns the redirect response instead.
 *  - **Timeouts and cancellation**: `timeout` (default 0, no timeout) is the maximum time of one
 *    request in milliseconds and a per-request `timeout` option overrides it. An `AbortSignal`
 *    passed with the request cancels it and fails it with an AbortError, or a TimeoutError when it
 *    comes from AbortSignal.timeout.
 *  - **TLS and proxy**: the constructor options are passed to tls.createSecureContext (for example
 *    `ca`, `cert`, `key`, `passphrase`, `ciphers`, `secureProtocol` and `rejectUnauthorized`), so
 *    an HTTPS client can trust a private CA or present a client certificate. `proxyEnv` routes
 *    HTTP/HTTPS requests through a proxy; connections to localhost, 127.0.0.1 and ::1 always
 *    bypass it.
 *  - **HTTP/2**: with `enableH2` enabled (the default) HTTPS connections negotiate HTTP/2 through
 *    ALPN. HTTP/2 sessions are cached process-wide by origin, proxy, SNI and TLS identity, so
 *    clients with the same identity share them; `destroy()` clears that cache for every client.
 *  - **Node.js differences**: Node.js splits this role between `Agent` and `globalAgent` and has no
 *    cookie jar, no automatic decompression and no automatic redirects; fibjs has `getName`
 *    returning `protocol//host:port[:localAddress]` (Node returns `host:port:localAddress`), and
 *    `defaultPort`/`protocol` only affect `getName()`.
 *
 *  Example 1 — a client with its own defaults:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const client = new http.Client({ timeout: 2000, userAgent: 'my-app/1.0' });
 *
 *  const server = new http.Server(0, (req) => {
 *      req.response.write(req.firstHeader('user-agent'));
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const resp = client.getSync('http://127.0.0.1:' + port + '/');
 *  console.log(resp.text()); // my-app/1.0
 *
 *  client.destroy();
 *  server.stop();
 *  ```
 *
 *  Example 2 — cookie jar and connection reuse in one client:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, (req) => {
 *      req.response.appendHeader('Set-Cookie', 'sid=42; path=/');
 *      req.response.write(req.firstHeader('cookie') || 'no cookie');
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *  const url = 'http://127.0.0.1:' + port + '/';
 *
 *  const client = new http.Client();
 *  console.log(client.getSync(url).text()); // no cookie
 *  console.log(client.getSync(url).text()); // sid=42
 *
 *  client.destroy();
 *  server.stop();
 *  ```
 *
 *  Example 3 — disable redirects and inspect the response:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, (req) => {
 *      if (req.address === '/old') {
 *          req.response.redirect('/new');
 *      } else {
 *          req.response.write('target');
 *      }
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *  const url = 'http://127.0.0.1:' + port;
 *
 *  const client = new http.Client({ autoRedirect: false });
 *  const resp = client.getSync(url + '/old');
 *  console.log(resp.statusCode, resp.firstHeader('location')); // 302 /new
 *  console.log(client.getSync(url + '/new').text()); // target
 *
 *  client.destroy();
 *  server.stop();
 *  ```
 *
 *  Example 4 — timeout and per-request override:
 *  ```JavaScript
 *  const http = require('http');
 *  const coroutine = require('coroutine');
 *
 *  const server = new http.Server(0, (req) => {
 *      if (req.address === '/slow') {
 *          coroutine.sleep(500);
 *      }
 *      req.response.write('done');
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *  const url = 'http://127.0.0.1:' + port;
 *
 *  const client = new http.Client({ timeout: 100 });
 *  try {
 *      client.getSync(url + '/slow');
 *  } catch (e) {
 *      console.log('timed out', e.number); // timed out 20021
 *  }
 *  // the per-request option overrides the client timeout
 *  console.log(client.getSync(url + '/slow', { timeout: 1000 }).text()); // done
 *
 *  client.destroy();
 *  server.stop();
 *  ```
 *
 */
declare class Class_HttpClient extends Class_EventEmitter {
    /**
     * @description HttpClient constructor, creates a new HttpClient object
     *
     *      The default options are keepAlive true, timeout 0, enableCookie/autoRedirect/enableEncoding/
     *      enableH2 true, maxHeadersCount 128, maxHeaderSize 8192, maxChunkSize 2, maxBodySize -1,
     *      poolTimeout 10000, maxFreeSockets 256, userAgent 'curl/8.14.1' and an empty proxyEnv.
     *
     */
    constructor();

    /**
     * @description HttpClient constructor, creates a new HttpClient object
     *
     *      options may be the options object used to create the secure context (the same object
     *      tls.createSecureContext accepts: `ca`, `cert`, `key`, `passphrase`, `ciphers`,
     *      `secureProtocol`, `rejectUnauthorized`, ...), or the SecureContext object itself.
     *
     *      In addition to the properties used to create a SecureContext, options also accepts the
     *      client properties listed below; each one has the same meaning and default as the instance
     *      property of the same name:
     *      - keepAlive: specifies whether to keep the connection alive (default true)
     *      - timeout: specifies the request timeout in milliseconds (default 0, no timeout)
     *      - enableCookie: specifies whether to enable the cookie feature (default true)
     *      - autoRedirect: specifies whether to enable the automatic redirect feature (default true)
     *      - enableEncoding: specifies whether to enable the automatic decompression feature (default true)
     *      - enableH2: specifies whether to enable HTTP/2 automatic upgrade (default true)
     *      - maxHeadersCount: specifies the maximum number of request headers (default 128)
     *      - maxHeaderSize: specifies the maximum request header size in bytes (default 8192)
     *      - maxChunkSize: specifies the maximum chunk size in MB (default 2)
     *      - maxBodySize: specifies the maximum body size in MB (default -1, no limit)
     *      - userAgent: specifies the browser identifier (default 'curl/8.14.1')
     *      - poolTimeout: specifies the keep-alive cached connection timeout in ms (default 10000)
     *      - maxFreeSockets: specifies the maximum number of idle connections (default 256)
     *      - proxyEnv: specifies the proxy configuration environment variables, including HTTP_PROXY, HTTPS_PROXY, NO_PROXY and their lowercase forms
     *
     *      Example — a client that trusts a private CA and identifies itself:
     *      ```JavaScript
     *      // fragment: options
     *      new http.Client({
     *          ca: fs.readFileSync('ca.pem'),
     *          cert: fs.readFileSync('client.pem'),
     *          key: fs.readFileSync('client-key.pem'),
     *          rejectUnauthorized: true,
     *          timeout: 5000,
     *          userAgent: 'my-service/1.0'
     *      })
     *      ```
     *      @param options the secure context or the options used to create one
     *
     */
    constructor(options: Class_SecureContext | Class_SecureContextPromise | FIBJS.GeneralObject);

    /**
     * @description Returns the HttpCookie object list of the http client
     *
     *      The jar of this client only: cookies collected from its responses are sent back on later
     *      requests to matching domains and paths. Two clients never share cookies; entries are updated
     *      in place when the same cookie is set again.
     *
     */
    readonly cookies: Class_HttpCookie[];

    /**
     * @description Queries and sets whether this client keeps connections alive
     *
     *      Default true: a finished connection is kept in the client pool and reused for later requests
     *      to the same host until `poolTimeout` expires. Set to false to open one connection per
     *      request. A per-request `keepAlive` option overrides this setting.
     *
     */
    keepAlive: boolean;

    /**
     * @description Queries and sets the timeout in milliseconds
     *
     *      Default 0, which means no timeout. The timeout covers a whole request; when it expires the
     *      request fails with error number 20021. A per-request `timeout` option overrides it, and
     *      `poolTimeout` separately controls idle pooled connections.
     *
     */
    timeout: number;

    /**
     * @description Cookie feature switch, enabled by default
     *
     *      When enabled, the Set-Cookie headers of responses are stored in `cookies` and matching
     *      cookies are sent with later requests. Set to false to ignore cookies completely.
     *
     */
    enableCookie: boolean;

    /**
     * @description Automatic redirect feature switch, enabled by default
     *
     *      When enabled, 301, 302, 303, 307 and 308 responses are followed automatically; 303 switches
     *      the request to GET and drops the body. There is no redirect count limit, but a URL visited
     *      twice raises a cyclic redirect error. When disabled, the redirect response itself is
     *      returned. Node.js never follows redirects automatically.
     *
     */
    autoRedirect: boolean;

    /**
     * @description Automatic decompression feature switch, enabled by default
     *
     *      When enabled, requests send `Accept-Encoding: gzip, deflate` and responses compressed with
     *      gzip or deflate are decompressed transparently, removing the Content-Encoding and
     *      Content-Length headers. When disabled, the raw compressed bytes are returned.
     *
     */
    enableEncoding: boolean;

    /**
     * @description HTTP/2 automatic upgrade switch, enabled by default
     *
     *      When enabled, an HTTPS request negotiates the protocol through ALPN and uses HTTP/2 when
     *      the server supports it; the switch has no effect on plain HTTP requests. HTTP/2 sessions
     *      are cached process-wide and shared by origin, proxy, SNI and TLS identity; `destroy()`
     *      clears the cache. Set to false to use HTTP/1.1 only.
     *
     */
    enableH2: boolean;

    /**
     * @description Queries and sets the maximum number of request headers, default 128
     *
     *      Applies to the messages parsed and generated by this client; a message can override it
     *      through its own `maxHeadersCount` property. Node.js defaults to 1000.
     *
     */
    maxHeadersCount: number;

    /**
     * @description Queries and sets the maximum request header size in bytes, default 8192
     *
     *      A response whose headers exceed the limit is rejected. Node.js defaults to 16384 bytes.
     *
     */
    maxHeaderSize: number;

    /**
     * @description Queries and sets the maximum chunk size in MB, default 2
     *
     *      Limits one chunk of a chunked request or response body; a chunk larger than the limit is
     *      rejected. Not a Node.js option.
     *
     */
    maxChunkSize: number;

    /**
     * @description Queries and sets the maximum body size in MB, default -1, no size limit
     *
     *      A response whose body exceeds the limit fails with error number 20024; 0 rejects every
     *      body. A HEAD response is exempt because it has no body. Not a Node.js option.
     *
     */
    maxBodySize: number;

    /**
     * @description Queries and sets the browser identifier in http requests
     *
     *      Default 'curl/8.14.1'. Sent as the User-Agent header when the request does not set one;
     *      assign an empty string to omit the header. Node.js sends no User-Agent by default.
     *
     */
    userAgent: string;

    /**
     * @description Queries and sets the keep-alive cached connection timeout, default 10000 ms
     *
     *      An idle pooled connection older than this is closed when the client looks for a free
     *      connection or stores one. Setting it to 0 disables connection reuse.
     *
     */
    poolTimeout: number;

    /**
     * @description Queries and sets the proxy configuration environment variables, supports HTTP_PROXY, HTTPS_PROXY, NO_PROXY and their lowercase forms
     *
     *      Default is an empty object, which means no proxy. Assigning an object routes HTTP/HTTPS
     *      requests through the given proxy (`http_proxy`/`HTTP_PROXY` and `https_proxy`/`HTTPS_PROXY`),
     *      with `no_proxy`/`NO_PROXY` listing hosts that must connect directly. Connections to
     *      localhost, 127.0.0.1 and ::1 always bypass the proxy. Equivalent to the proxyEnv constructor
     *      option; see the module `setGlobalProxyFromEnv` for the matching env parser.
     *
     */
    proxyEnv: FIBJS.GeneralObject;

    /**
     * @description Queries and sets the maximum number of connections per host, default unlimited
     *
     *      Accepted and stored for Node.js compatibility, but fibjs does not enforce it: concurrent
     *      requests are not queued when the limit is reached. Node.js queues the excess requests.
     *
     */
    maxSockets: number;

    /**
     * @description Queries and sets the maximum total number of connections, default unlimited
     *
     *      Accepted and stored for Node.js compatibility, but fibjs does not enforce it; only the
     *      number of idle pooled connections (`maxFreeSockets`) is limited.
     *
     */
    maxTotalSockets: number;

    /**
     * @description Queries and sets the maximum number of idle connections, default 256
     *
     *      This client keeps one idle list for all hosts and drops the oldest entries beyond this
     *      number; Node.js applies maxFreeSockets per host instead. Ignored when `keepAlive` is false.
     *
     */
    maxFreeSockets: number;

    /**
     * @description Queries and sets the default port used by getName(), default 80
     *
     *      Only used when building the connection pool key in getName(); it does not change the port
     *      of requests, which always comes from their URL.
     *
     */
    defaultPort: number;

    /**
     * @description Queries and sets the default protocol used by getName(), default "http:"
     *
     *      Only used when building the connection pool key in getName(); it does not change the
     *      protocol of requests, which always comes from their URL.
     *
     */
    protocol: string;

    /**
     * @description Returns the map of idle connections keyed by host:port
     *
     *      Node.js compatibility accessor: fibjs always returns an empty object because the internal
     *      pool is not exposed. Use `destroy()` to release the real pooled connections.
     *
     */
    readonly freeSockets: FIBJS.GeneralObject;

    /**
     * @description Returns the map of connections in use keyed by host:port
     *
     *      Node.js compatibility accessor: fibjs always returns an empty object because the internal
     *      pool is not exposed.
     *
     */
    readonly sockets: FIBJS.GeneralObject;

    /**
     * @description Returns the total number of connections in use across all hosts
     *
     *      Node.js compatibility accessor: fibjs always returns 0 because the internal pool is not
     *      exposed.
     *
     */
    readonly totalSocketCount: number;

    /**
     * @description Returns a unique key for the given request options, used for the connection pool
     *
     *      Reads `host` (default empty), `port` (default `defaultPort`) and `localAddress` from options
     *      and returns `protocol//host:port`, with `:localAddress` appended when one is given; the
     *      default result is therefore `http://:80`. This differs from Node.js, which returns
     *      `host:port:localAddress` without the protocol.
     *
     *      Example — the pool key of a host:
     *      ```JavaScript
     *      const http = require('http');
     *      const client = new http.Client({ protocol: 'https:', defaultPort: 443 });
     *      console.log(client.getName({ host: 'example.com', port: 8443 })); // https://example.com:8443
     *      ```
     *      @param options request options
     *      @return returns the connection pool key string
     *
     */
    getName(options?: FIBJS.GeneralObject): string;

    /**
     * @description Destroys all connections currently in use
     *
     *      Clears the idle connection pool of this client and destroys every cached HTTP/2 session;
     *      the HTTP/2 cache is process-wide, so this also ends sessions being used by other clients.
     *      Call it when the client is no longer needed to release the sockets immediately.
     *
     */
    destroy(): void;

    /**
     * @description Sends an HttpRequest over an existing stream and returns it with the response
     *
     *      This low-level form writes `req` to `conn` — any connected Stream such as a net.Socket or a
     *      TLSSocket — instead of creating a connection from a URL; it blocks until the response is
     *      received and returns the same HttpRequest object whose `response` property holds the reply.
     *      The request is sent with this client's settings, cookie jar and proxy configuration.
     *
     *      The option-based overloads below are the usual entry points:
     *      - `request(opts)`, `request(url, opts)` and `request(method, url, opts)` return an HttpRequest
     *        without sending it; `end()` sends it and the response arrives through the callback or the
     *        `'response'` event;
     *      - the forms taking a callback register it before returning the request.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // request method, used by the opts-only form
     *          protocol: 'http', // URL override fields: protocol/host/hostname/port/pathname/path/query/auth
     *          hostname: '', port: 80, pathname: '/', query: {},
     *          headers: {}, // Headers object or plain object, added to the generated headers
     *          body: null, // SeekableStream | Buffer | String | Object | FormData | URLSearchParams | Blob
     *          json: null, // encoded as JSON, Content-Type: application/json
     *          pack: null, // encoded as msgpack, Content-Type: application/msgpack
     *          keepAlive: undefined, // overrides the client keepAlive for this request
     *          timeout: undefined, // request timeout in ms, overrides the client timeout
     *          signal: null, // AbortSignal used to cancel the request
     *          agent: null // HttpClient that sends this request instead of this one
     *      })
     *      ```
     *      body, json and pack are mutually exclusive; `query` replaces the query string of the URL
     *      instead of merging with it. A string body is sent as application/x-www-form-urlencoded, a
     *      Buffer as application/octet-stream, a plain object or FormData as multipart/form-data with a
     *      generated boundary, and URLSearchParams as application/x-www-form-urlencoded.
     *
     *      Example — an event-style request through this client:
     *      ```JavaScript
     *      const http = require('http');
     *      const coroutine = require('coroutine');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.write('received: ' + (req.body ? req.body.readAll().toString() : ''));
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const done = new coroutine.Event();
     *      const req = client.request('POST', 'http://127.0.0.1:' + port + '/echo', (resp) => {
     *          console.log(resp.text()); // received: hello
     *          done.set();
     *      });
     *      req.end('hello'); // request() does not send before end()
     *      done.wait();
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param conn the stream object to process the request
     *      @param req the HttpRequest object to send
     *      @return returns req, whose response property receives the server response
     *
     */
    request(conn: Class_Stream | Class_StreamPromise, req: Class_HttpRequest | Class_HttpRequestPromise): Class_HttpRequest;

    /**
     * @description Requests the url specified by opts and returns the result
     *
     *      The request is sent through this client and blocks the current fiber until the response is
     *      received; the returned HttpResponse has its body ready to read. The request uses this
     *      client's defaults, cookie jar and proxy configuration, and all URL fields can be given in
     *      opts instead of a url argument.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // request method, used by the opts-only form
     *          protocol: 'http', // URL override fields: protocol/host/hostname/port/pathname/path/query/auth
     *          hostname: '', port: 80, pathname: '/', query: {},
     *          headers: {}, // Headers object or plain object, added to the generated headers
     *          body: null, // SeekableStream | Buffer | String | Object | FormData | URLSearchParams | Blob
     *          json: null, // encoded as JSON, Content-Type: application/json
     *          pack: null, // encoded as msgpack, Content-Type: application/msgpack
     *          keepAlive: undefined, // overrides the client keepAlive for this request
     *          timeout: undefined, // request timeout in ms, overrides the client timeout
     *          signal: null, // AbortSignal used to cancel the request
     *          agent: null // HttpClient that sends this request instead of this one
     *      })
     *      ```
     *      body, json and pack are mutually exclusive; `query` replaces the query string of the URL
     *      instead of merging with it. Without body/json/pack the request carries no body.
     *
     *      Example — a sync request through an independent client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.requestSync('http://127.0.0.1:' + port + '/status');
     *      console.log(resp.json()); // { path: '/status' }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSync(opts: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the url specified by opts and returns the result
     *
     *      The request is sent through this client and blocks the current fiber until the response is
     *      received; the returned HttpResponse has its body ready to read. The request uses this
     *      client's defaults, cookie jar and proxy configuration, and all URL fields can be given in
     *      opts instead of a url argument.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // request method, used by the opts-only form
     *          protocol: 'http', // URL override fields: protocol/host/hostname/port/pathname/path/query/auth
     *          hostname: '', port: 80, pathname: '/', query: {},
     *          headers: {}, // Headers object or plain object, added to the generated headers
     *          body: null, // SeekableStream | Buffer | String | Object | FormData | URLSearchParams | Blob
     *          json: null, // encoded as JSON, Content-Type: application/json
     *          pack: null, // encoded as msgpack, Content-Type: application/msgpack
     *          keepAlive: undefined, // overrides the client keepAlive for this request
     *          timeout: undefined, // request timeout in ms, overrides the client timeout
     *          signal: null, // AbortSignal used to cancel the request
     *          agent: null // HttpClient that sends this request instead of this one
     *      })
     *      ```
     *      body, json and pack are mutually exclusive; `query` replaces the query string of the URL
     *      instead of merging with it. Without body/json/pack the request carries no body.
     *
     *      Example — a sync request through an independent client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.requestSync('http://127.0.0.1:' + port + '/status');
     *      console.log(resp.json()); // { path: '/status' }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncSync(opts: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the url specified by opts and returns the result
     *
     *      The request is sent through this client and blocks the current fiber until the response is
     *      received; the returned HttpResponse has its body ready to read. The request uses this
     *      client's defaults, cookie jar and proxy configuration, and all URL fields can be given in
     *      opts instead of a url argument.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // request method, used by the opts-only form
     *          protocol: 'http', // URL override fields: protocol/host/hostname/port/pathname/path/query/auth
     *          hostname: '', port: 80, pathname: '/', query: {},
     *          headers: {}, // Headers object or plain object, added to the generated headers
     *          body: null, // SeekableStream | Buffer | String | Object | FormData | URLSearchParams | Blob
     *          json: null, // encoded as JSON, Content-Type: application/json
     *          pack: null, // encoded as msgpack, Content-Type: application/msgpack
     *          keepAlive: undefined, // overrides the client keepAlive for this request
     *          timeout: undefined, // request timeout in ms, overrides the client timeout
     *          signal: null, // AbortSignal used to cancel the request
     *          agent: null // HttpClient that sends this request instead of this one
     *      })
     *      ```
     *      body, json and pack are mutually exclusive; `query` replaces the query string of the URL
     *      instead of merging with it. Without body/json/pack the request carries no body.
     *
     *      Example — a sync request through an independent client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.requestSync('http://127.0.0.1:' + port + '/status');
     *      console.log(resp.json()); // { path: '/status' }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncAsync(opts: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body; the opts fields documented on requestSync(opts) apply here as well, with `url`
     *      providing protocol, host, port and path.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body; the opts fields documented on requestSync(opts) apply here as well, with `url`
     *      providing protocol, host, port and path.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body; the opts fields documented on requestSync(opts) apply here as well, with `url`
     *      providing protocol, host, port and path.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url and returns the result
     *
     *      Blocks the current fiber and returns the HttpResponse directly. method selects the request
     *      method (default GET) and opts carries the fields documented on requestSync(opts).
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSync(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url and returns the result
     *
     *      Blocks the current fiber and returns the HttpResponse directly. method selects the request
     *      method (default GET) and opts carries the fields documented on requestSync(opts).
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncSync(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url and returns the result
     *
     *      Blocks the current fiber and returns the HttpResponse directly. method selects the request
     *      method (default GET) and opts carries the fields documented on requestSync(opts).
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncAsync(method: string, url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the url specified by opts and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end()` to send it. The response arrives through the
     *      callback given here or registered later, or through the `'response'` event; it is also
     *      stored in the `response` property. All opts fields are documented on request(Stream,
     *      HttpRequest), the first request overload; unlike `get`, this function does not send
     *      automatically.
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    request(opts: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the url specified by opts, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The callback is called with the HttpResponse when the response arrives. The returned request
     *      must still be sent with `end()`; the request uses this client's settings and cookie jar.
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The method defaults to GET. The returned request must still be sent with `end()`; the
     *      callback receives the HttpResponse when the response arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end()` to send it; the response is delivered to the
     *      callback (when given), to the `'response'` event and to the `response` property. This form
     *      is the async counterpart of requestSync(url, opts) on this client; use `get(url, opts)` when
     *      the method is GET and the request should be sent automatically.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    request(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The method defaults to GET and the returned request must still be sent with `end()`; the
     *      callback is called with the HttpResponse. See the first request overload for the opts fields.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback is called with the
     *      HttpResponse when it arrives.
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(method: string, url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The general option form of the request family on this client; the returned request must
     *      still be sent with `end()`. See the first request overload for the opts fields.
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    request(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback is called with the
     *      HttpResponse. See the first request overload for the opts fields.
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(method: string, url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body, so body/json/pack are not accepted; the other opts fields of requestSync(opts) apply.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    getSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body, so body/json/pack are not accepted; the other opts fields of requestSync(opts) apply.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    getSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body, so body/json/pack are not accepted; the other opts fields of requestSync(opts) apply.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    getSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The request is sent automatically without calling `end()`; the callback receives the
     *      HttpResponse when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    get(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method and returns an HttpRequest object
     *
     *      The returned request is sent automatically without calling `end()`; the response is
     *      delivered to the callback or the `'response'` event and stored in the `response` property.
     *      A GET request carries no body; the opts fields are the same as getSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    get(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The request is sent automatically (no `end()` needed); the callback receives the HttpResponse
     *      when it arrives. See the get(url, opts) overload for the options.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    get(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. The request body is given
     *      by body, json or pack; a string body is sent as application/x-www-form-urlencoded, a plain
     *      object or FormData as multipart/form-data. See requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    postSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. The request body is given
     *      by body, json or pack; a string body is sent as application/x-www-form-urlencoded, a plain
     *      object or FormData as multipart/form-data. See requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    postSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. The request body is given
     *      by body, json or pack; a string body is sent as application/x-www-form-urlencoded, a plain
     *      object or FormData as multipart/form-data. See requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    postSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse
     *      when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    post(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end(data)` or `end()` to send it (the body may also
     *      be given by the body/json/pack options); the response is delivered to the callback or the
     *      `'response'` event. See the first request overload for the opts fields.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    post(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse.
     *      The body may be passed to `end(data)` or given by the body/json/pack options.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    post(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A DELETE request normally
     *      has no body, but a body may be given like with postSync; the opts fields are the same as
     *      postSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    delSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A DELETE request normally
     *      has no body, but a body may be given like with postSync; the opts fields are the same as
     *      postSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    delSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A DELETE request normally
     *      has no body, but a body may be given like with postSync; the opts fields are the same as
     *      postSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    delSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse
     *      when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    del(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end()` to send it. The opts fields are the same as
     *      post(url, opts); see the first request overload for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    del(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    del(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PUT replaces the target
     *      resource with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    putSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PUT replaces the target
     *      resource with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    putSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PUT replaces the target
     *      resource with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    putSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse
     *      when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    put(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end(data)` or `end()` to send it. The opts fields
     *      are the same as post(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    put(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    put(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PATCH applies a partial
     *      update with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    patchSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PATCH applies a partial
     *      update with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    patchSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PATCH applies a partial
     *      update with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    patchSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse
     *      when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    patch(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end(data)` or `end()` to send it. The opts fields
     *      are the same as post(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    patch(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    patch(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A HEAD response carries the
     *      status and headers of the equivalent GET but no body, so `resp.body` is empty; unlike GET,
     *      a HEAD response with a large Content-Length is not rejected by maxBodySize. The opts fields
     *      are the same as getSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    headSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A HEAD response carries the
     *      status and headers of the equivalent GET but no body, so `resp.body` is empty; unlike GET,
     *      a HEAD response with a large Content-Length is not rejected by maxBodySize. The opts fields
     *      are the same as getSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    headSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A HEAD response carries the
     *      status and headers of the equivalent GET but no body, so `resp.body` is empty; unlike GET,
     *      a HEAD response with a large Content-Length is not rejected by maxBodySize. The opts fields
     *      are the same as getSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    headSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The request is sent automatically without calling `end()`; the callback receives the
     *      HttpResponse when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    head(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method and returns an HttpRequest object
     *
     *      Like get(), the returned request is sent automatically without calling `end()`; only the
     *      status and headers of the response are received. See the get(url, opts) overload for the
     *      options.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    head(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The request is sent automatically without calling `end()`; the callback receives the
     *      HttpResponse when the headers arrive.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    head(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *
     *      The request is sent through this client, using its settings, cookie jar and proxy
     *      configuration. opts can override the request fields (`new Request(request, init)`
     *      semantics); the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // overrides the request method; the method of an HttpRequest source is kept when not given
     *          headers: {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          body: null, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          keepAlive: undefined, // overrides the keep-alive setting
     *          timeout: undefined, // request timeout in ms, uses the client default settings by default
     *          redirect: 'follow', // redirect mode: 'follow' (default) | 'error' | 'manual'
     *          signal: null, // AbortSignal object used to cancel the request
     *          streaming: false // whether to expose the response body as a stream instead of buffering it
     *      })
     *      ```
     *      Following the Fetch standard a GET or HEAD request must not carry a body (a TypeError is
     *      thrown) and `headers` replaces the headers of the request source instead of merging them.
     *      `redirect: 'error'` fails with a TypeError when the server redirects and `redirect: 'manual'`
     *      returns the redirect response as it is, with `redirected` false; otherwise redirects are
     *      followed according to `autoRedirect`. An aborted request fails with an AbortError (a
     *      TimeoutError for AbortSignal.timeout).
     *
     *      Example — fetch a URL through this client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ ok: true });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.fetch('http://127.0.0.1:' + port + '/api');
     *      console.log(resp.status, resp.ok, resp.json()); // 200 true { ok: true }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param request the request source
     *      @param opts the additional information, can override the corresponding fields in request; following the Fetch
     *      standard a GET or HEAD request must not carry a body, a string body is sent as `text/plain;charset=UTF-8`,
     *      and `headers` replaces the headers of the request source instead of merging them
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetch(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    fetch(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: Class_HttpResponse)=>any): void;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *
     *      The request is sent through this client, using its settings, cookie jar and proxy
     *      configuration. opts can override the request fields (`new Request(request, init)`
     *      semantics); the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // overrides the request method; the method of an HttpRequest source is kept when not given
     *          headers: {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          body: null, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          keepAlive: undefined, // overrides the keep-alive setting
     *          timeout: undefined, // request timeout in ms, uses the client default settings by default
     *          redirect: 'follow', // redirect mode: 'follow' (default) | 'error' | 'manual'
     *          signal: null, // AbortSignal object used to cancel the request
     *          streaming: false // whether to expose the response body as a stream instead of buffering it
     *      })
     *      ```
     *      Following the Fetch standard a GET or HEAD request must not carry a body (a TypeError is
     *      thrown) and `headers` replaces the headers of the request source instead of merging them.
     *      `redirect: 'error'` fails with a TypeError when the server redirects and `redirect: 'manual'`
     *      returns the redirect response as it is, with `redirected` false; otherwise redirects are
     *      followed according to `autoRedirect`. An aborted request fails with an AbortError (a
     *      TimeoutError for AbortSignal.timeout).
     *
     *      Example — fetch a URL through this client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ ok: true });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.fetch('http://127.0.0.1:' + port + '/api');
     *      console.log(resp.status, resp.ok, resp.json()); // 200 true { ok: true }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param request the request source
     *      @param opts the additional information, can override the corresponding fields in request; following the Fetch
     *      standard a GET or HEAD request must not carry a body, a string body is sent as `text/plain;charset=UTF-8`,
     *      and `headers` replaces the headers of the request source instead of merging them
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetchSync(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *
     *      The request is sent through this client, using its settings, cookie jar and proxy
     *      configuration. opts can override the request fields (`new Request(request, init)`
     *      semantics); the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // overrides the request method; the method of an HttpRequest source is kept when not given
     *          headers: {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          body: null, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          keepAlive: undefined, // overrides the keep-alive setting
     *          timeout: undefined, // request timeout in ms, uses the client default settings by default
     *          redirect: 'follow', // redirect mode: 'follow' (default) | 'error' | 'manual'
     *          signal: null, // AbortSignal object used to cancel the request
     *          streaming: false // whether to expose the response body as a stream instead of buffering it
     *      })
     *      ```
     *      Following the Fetch standard a GET or HEAD request must not carry a body (a TypeError is
     *      thrown) and `headers` replaces the headers of the request source instead of merging them.
     *      `redirect: 'error'` fails with a TypeError when the server redirects and `redirect: 'manual'`
     *      returns the redirect response as it is, with `redirected` false; otherwise redirects are
     *      followed according to `autoRedirect`. An aborted request fails with an AbortError (a
     *      TimeoutError for AbortSignal.timeout).
     *
     *      Example — fetch a URL through this client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ ok: true });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.fetch('http://127.0.0.1:' + port + '/api');
     *      console.log(resp.status, resp.ok, resp.json()); // 200 true { ok: true }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param request the request source
     *      @param opts the additional information, can override the corresponding fields in request; following the Fetch
     *      standard a GET or HEAD request must not carry a body, a string body is sent as `text/plain;charset=UTF-8`,
     *      and `headers` replaces the headers of the request source instead of merging them
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetchAsync(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/SecureContext.d.ts" />
/// <reference path="../interface/HttpCookie.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * The promise variant of the HttpClient class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_HttpClientPromise extends Class_EventEmitter {
    /**
     * @description HttpClient constructor, creates a new HttpClient object
     *
     *      The default options are keepAlive true, timeout 0, enableCookie/autoRedirect/enableEncoding/
     *      enableH2 true, maxHeadersCount 128, maxHeaderSize 8192, maxChunkSize 2, maxBodySize -1,
     *      poolTimeout 10000, maxFreeSockets 256, userAgent 'curl/8.14.1' and an empty proxyEnv.
     *
     */
    constructor();

    /**
     * @description HttpClient constructor, creates a new HttpClient object
     *
     *      options may be the options object used to create the secure context (the same object
     *      tls.createSecureContext accepts: `ca`, `cert`, `key`, `passphrase`, `ciphers`,
     *      `secureProtocol`, `rejectUnauthorized`, ...), or the SecureContext object itself.
     *
     *      In addition to the properties used to create a SecureContext, options also accepts the
     *      client properties listed below; each one has the same meaning and default as the instance
     *      property of the same name:
     *      - keepAlive: specifies whether to keep the connection alive (default true)
     *      - timeout: specifies the request timeout in milliseconds (default 0, no timeout)
     *      - enableCookie: specifies whether to enable the cookie feature (default true)
     *      - autoRedirect: specifies whether to enable the automatic redirect feature (default true)
     *      - enableEncoding: specifies whether to enable the automatic decompression feature (default true)
     *      - enableH2: specifies whether to enable HTTP/2 automatic upgrade (default true)
     *      - maxHeadersCount: specifies the maximum number of request headers (default 128)
     *      - maxHeaderSize: specifies the maximum request header size in bytes (default 8192)
     *      - maxChunkSize: specifies the maximum chunk size in MB (default 2)
     *      - maxBodySize: specifies the maximum body size in MB (default -1, no limit)
     *      - userAgent: specifies the browser identifier (default 'curl/8.14.1')
     *      - poolTimeout: specifies the keep-alive cached connection timeout in ms (default 10000)
     *      - maxFreeSockets: specifies the maximum number of idle connections (default 256)
     *      - proxyEnv: specifies the proxy configuration environment variables, including HTTP_PROXY, HTTPS_PROXY, NO_PROXY and their lowercase forms
     *
     *      Example — a client that trusts a private CA and identifies itself:
     *      ```JavaScript
     *      // fragment: options
     *      new http.Client({
     *          ca: fs.readFileSync('ca.pem'),
     *          cert: fs.readFileSync('client.pem'),
     *          key: fs.readFileSync('client-key.pem'),
     *          rejectUnauthorized: true,
     *          timeout: 5000,
     *          userAgent: 'my-service/1.0'
     *      })
     *      ```
     *      @param options the secure context or the options used to create one
     *
     */
    constructor(options: Class_SecureContext | Class_SecureContextPromise | FIBJS.GeneralObject);

    /**
     * @description Returns the HttpCookie object list of the http client
     *
     *      The jar of this client only: cookies collected from its responses are sent back on later
     *      requests to matching domains and paths. Two clients never share cookies; entries are updated
     *      in place when the same cookie is set again.
     *
     */
    readonly cookies: Class_HttpCookie[];

    /**
     * @description Queries and sets whether this client keeps connections alive
     *
     *      Default true: a finished connection is kept in the client pool and reused for later requests
     *      to the same host until `poolTimeout` expires. Set to false to open one connection per
     *      request. A per-request `keepAlive` option overrides this setting.
     *
     */
    keepAlive: boolean;

    /**
     * @description Queries and sets the timeout in milliseconds
     *
     *      Default 0, which means no timeout. The timeout covers a whole request; when it expires the
     *      request fails with error number 20021. A per-request `timeout` option overrides it, and
     *      `poolTimeout` separately controls idle pooled connections.
     *
     */
    timeout: number;

    /**
     * @description Cookie feature switch, enabled by default
     *
     *      When enabled, the Set-Cookie headers of responses are stored in `cookies` and matching
     *      cookies are sent with later requests. Set to false to ignore cookies completely.
     *
     */
    enableCookie: boolean;

    /**
     * @description Automatic redirect feature switch, enabled by default
     *
     *      When enabled, 301, 302, 303, 307 and 308 responses are followed automatically; 303 switches
     *      the request to GET and drops the body. There is no redirect count limit, but a URL visited
     *      twice raises a cyclic redirect error. When disabled, the redirect response itself is
     *      returned. Node.js never follows redirects automatically.
     *
     */
    autoRedirect: boolean;

    /**
     * @description Automatic decompression feature switch, enabled by default
     *
     *      When enabled, requests send `Accept-Encoding: gzip, deflate` and responses compressed with
     *      gzip or deflate are decompressed transparently, removing the Content-Encoding and
     *      Content-Length headers. When disabled, the raw compressed bytes are returned.
     *
     */
    enableEncoding: boolean;

    /**
     * @description HTTP/2 automatic upgrade switch, enabled by default
     *
     *      When enabled, an HTTPS request negotiates the protocol through ALPN and uses HTTP/2 when
     *      the server supports it; the switch has no effect on plain HTTP requests. HTTP/2 sessions
     *      are cached process-wide and shared by origin, proxy, SNI and TLS identity; `destroy()`
     *      clears the cache. Set to false to use HTTP/1.1 only.
     *
     */
    enableH2: boolean;

    /**
     * @description Queries and sets the maximum number of request headers, default 128
     *
     *      Applies to the messages parsed and generated by this client; a message can override it
     *      through its own `maxHeadersCount` property. Node.js defaults to 1000.
     *
     */
    maxHeadersCount: number;

    /**
     * @description Queries and sets the maximum request header size in bytes, default 8192
     *
     *      A response whose headers exceed the limit is rejected. Node.js defaults to 16384 bytes.
     *
     */
    maxHeaderSize: number;

    /**
     * @description Queries and sets the maximum chunk size in MB, default 2
     *
     *      Limits one chunk of a chunked request or response body; a chunk larger than the limit is
     *      rejected. Not a Node.js option.
     *
     */
    maxChunkSize: number;

    /**
     * @description Queries and sets the maximum body size in MB, default -1, no size limit
     *
     *      A response whose body exceeds the limit fails with error number 20024; 0 rejects every
     *      body. A HEAD response is exempt because it has no body. Not a Node.js option.
     *
     */
    maxBodySize: number;

    /**
     * @description Queries and sets the browser identifier in http requests
     *
     *      Default 'curl/8.14.1'. Sent as the User-Agent header when the request does not set one;
     *      assign an empty string to omit the header. Node.js sends no User-Agent by default.
     *
     */
    userAgent: string;

    /**
     * @description Queries and sets the keep-alive cached connection timeout, default 10000 ms
     *
     *      An idle pooled connection older than this is closed when the client looks for a free
     *      connection or stores one. Setting it to 0 disables connection reuse.
     *
     */
    poolTimeout: number;

    /**
     * @description Queries and sets the proxy configuration environment variables, supports HTTP_PROXY, HTTPS_PROXY, NO_PROXY and their lowercase forms
     *
     *      Default is an empty object, which means no proxy. Assigning an object routes HTTP/HTTPS
     *      requests through the given proxy (`http_proxy`/`HTTP_PROXY` and `https_proxy`/`HTTPS_PROXY`),
     *      with `no_proxy`/`NO_PROXY` listing hosts that must connect directly. Connections to
     *      localhost, 127.0.0.1 and ::1 always bypass the proxy. Equivalent to the proxyEnv constructor
     *      option; see the module `setGlobalProxyFromEnv` for the matching env parser.
     *
     */
    proxyEnv: FIBJS.GeneralObject;

    /**
     * @description Queries and sets the maximum number of connections per host, default unlimited
     *
     *      Accepted and stored for Node.js compatibility, but fibjs does not enforce it: concurrent
     *      requests are not queued when the limit is reached. Node.js queues the excess requests.
     *
     */
    maxSockets: number;

    /**
     * @description Queries and sets the maximum total number of connections, default unlimited
     *
     *      Accepted and stored for Node.js compatibility, but fibjs does not enforce it; only the
     *      number of idle pooled connections (`maxFreeSockets`) is limited.
     *
     */
    maxTotalSockets: number;

    /**
     * @description Queries and sets the maximum number of idle connections, default 256
     *
     *      This client keeps one idle list for all hosts and drops the oldest entries beyond this
     *      number; Node.js applies maxFreeSockets per host instead. Ignored when `keepAlive` is false.
     *
     */
    maxFreeSockets: number;

    /**
     * @description Queries and sets the default port used by getName(), default 80
     *
     *      Only used when building the connection pool key in getName(); it does not change the port
     *      of requests, which always comes from their URL.
     *
     */
    defaultPort: number;

    /**
     * @description Queries and sets the default protocol used by getName(), default "http:"
     *
     *      Only used when building the connection pool key in getName(); it does not change the
     *      protocol of requests, which always comes from their URL.
     *
     */
    protocol: string;

    /**
     * @description Returns the map of idle connections keyed by host:port
     *
     *      Node.js compatibility accessor: fibjs always returns an empty object because the internal
     *      pool is not exposed. Use `destroy()` to release the real pooled connections.
     *
     */
    readonly freeSockets: FIBJS.GeneralObject;

    /**
     * @description Returns the map of connections in use keyed by host:port
     *
     *      Node.js compatibility accessor: fibjs always returns an empty object because the internal
     *      pool is not exposed.
     *
     */
    readonly sockets: FIBJS.GeneralObject;

    /**
     * @description Returns the total number of connections in use across all hosts
     *
     *      Node.js compatibility accessor: fibjs always returns 0 because the internal pool is not
     *      exposed.
     *
     */
    readonly totalSocketCount: number;

    /**
     * @description Returns a unique key for the given request options, used for the connection pool
     *
     *      Reads `host` (default empty), `port` (default `defaultPort`) and `localAddress` from options
     *      and returns `protocol//host:port`, with `:localAddress` appended when one is given; the
     *      default result is therefore `http://:80`. This differs from Node.js, which returns
     *      `host:port:localAddress` without the protocol.
     *
     *      Example — the pool key of a host:
     *      ```JavaScript
     *      const http = require('http');
     *      const client = new http.Client({ protocol: 'https:', defaultPort: 443 });
     *      console.log(client.getName({ host: 'example.com', port: 8443 })); // https://example.com:8443
     *      ```
     *      @param options request options
     *      @return returns the connection pool key string
     *
     */
    getName(options?: FIBJS.GeneralObject): string;

    /**
     * @description Destroys all connections currently in use
     *
     *      Clears the idle connection pool of this client and destroys every cached HTTP/2 session;
     *      the HTTP/2 cache is process-wide, so this also ends sessions being used by other clients.
     *      Call it when the client is no longer needed to release the sockets immediately.
     *
     */
    destroy(): void;

    /**
     * @description Sends an HttpRequest over an existing stream and returns it with the response
     *
     *      This low-level form writes `req` to `conn` — any connected Stream such as a net.Socket or a
     *      TLSSocket — instead of creating a connection from a URL; it blocks until the response is
     *      received and returns the same HttpRequest object whose `response` property holds the reply.
     *      The request is sent with this client's settings, cookie jar and proxy configuration.
     *
     *      The option-based overloads below are the usual entry points:
     *      - `request(opts)`, `request(url, opts)` and `request(method, url, opts)` return an HttpRequest
     *        without sending it; `end()` sends it and the response arrives through the callback or the
     *        `'response'` event;
     *      - the forms taking a callback register it before returning the request.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // request method, used by the opts-only form
     *          protocol: 'http', // URL override fields: protocol/host/hostname/port/pathname/path/query/auth
     *          hostname: '', port: 80, pathname: '/', query: {},
     *          headers: {}, // Headers object or plain object, added to the generated headers
     *          body: null, // SeekableStream | Buffer | String | Object | FormData | URLSearchParams | Blob
     *          json: null, // encoded as JSON, Content-Type: application/json
     *          pack: null, // encoded as msgpack, Content-Type: application/msgpack
     *          keepAlive: undefined, // overrides the client keepAlive for this request
     *          timeout: undefined, // request timeout in ms, overrides the client timeout
     *          signal: null, // AbortSignal used to cancel the request
     *          agent: null // HttpClient that sends this request instead of this one
     *      })
     *      ```
     *      body, json and pack are mutually exclusive; `query` replaces the query string of the URL
     *      instead of merging with it. A string body is sent as application/x-www-form-urlencoded, a
     *      Buffer as application/octet-stream, a plain object or FormData as multipart/form-data with a
     *      generated boundary, and URLSearchParams as application/x-www-form-urlencoded.
     *
     *      Example — an event-style request through this client:
     *      ```JavaScript
     *      const http = require('http');
     *      const coroutine = require('coroutine');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.write('received: ' + (req.body ? req.body.readAll().toString() : ''));
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const done = new coroutine.Event();
     *      const req = client.request('POST', 'http://127.0.0.1:' + port + '/echo', (resp) => {
     *          console.log(resp.text()); // received: hello
     *          done.set();
     *      });
     *      req.end('hello'); // request() does not send before end()
     *      done.wait();
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param conn the stream object to process the request
     *      @param req the HttpRequest object to send
     *      @return returns req, whose response property receives the server response
     *
     */
    request(conn: Class_Stream | Class_StreamPromise, req: Class_HttpRequest | Class_HttpRequestPromise): Class_HttpRequest;

    /**
     * @description Requests the url specified by opts and returns the result
     *
     *      The request is sent through this client and blocks the current fiber until the response is
     *      received; the returned HttpResponse has its body ready to read. The request uses this
     *      client's defaults, cookie jar and proxy configuration, and all URL fields can be given in
     *      opts instead of a url argument.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // request method, used by the opts-only form
     *          protocol: 'http', // URL override fields: protocol/host/hostname/port/pathname/path/query/auth
     *          hostname: '', port: 80, pathname: '/', query: {},
     *          headers: {}, // Headers object or plain object, added to the generated headers
     *          body: null, // SeekableStream | Buffer | String | Object | FormData | URLSearchParams | Blob
     *          json: null, // encoded as JSON, Content-Type: application/json
     *          pack: null, // encoded as msgpack, Content-Type: application/msgpack
     *          keepAlive: undefined, // overrides the client keepAlive for this request
     *          timeout: undefined, // request timeout in ms, overrides the client timeout
     *          signal: null, // AbortSignal used to cancel the request
     *          agent: null // HttpClient that sends this request instead of this one
     *      })
     *      ```
     *      body, json and pack are mutually exclusive; `query` replaces the query string of the URL
     *      instead of merging with it. Without body/json/pack the request carries no body.
     *
     *      Example — a sync request through an independent client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.requestSync('http://127.0.0.1:' + port + '/status');
     *      console.log(resp.json()); // { path: '/status' }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSync(opts: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the url specified by opts and returns the result
     *
     *      The request is sent through this client and blocks the current fiber until the response is
     *      received; the returned HttpResponse has its body ready to read. The request uses this
     *      client's defaults, cookie jar and proxy configuration, and all URL fields can be given in
     *      opts instead of a url argument.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // request method, used by the opts-only form
     *          protocol: 'http', // URL override fields: protocol/host/hostname/port/pathname/path/query/auth
     *          hostname: '', port: 80, pathname: '/', query: {},
     *          headers: {}, // Headers object or plain object, added to the generated headers
     *          body: null, // SeekableStream | Buffer | String | Object | FormData | URLSearchParams | Blob
     *          json: null, // encoded as JSON, Content-Type: application/json
     *          pack: null, // encoded as msgpack, Content-Type: application/msgpack
     *          keepAlive: undefined, // overrides the client keepAlive for this request
     *          timeout: undefined, // request timeout in ms, overrides the client timeout
     *          signal: null, // AbortSignal used to cancel the request
     *          agent: null // HttpClient that sends this request instead of this one
     *      })
     *      ```
     *      body, json and pack are mutually exclusive; `query` replaces the query string of the URL
     *      instead of merging with it. Without body/json/pack the request carries no body.
     *
     *      Example — a sync request through an independent client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.requestSync('http://127.0.0.1:' + port + '/status');
     *      console.log(resp.json()); // { path: '/status' }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncSync(opts: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the url specified by opts and returns the result
     *
     *      The request is sent through this client and blocks the current fiber until the response is
     *      received; the returned HttpResponse has its body ready to read. The request uses this
     *      client's defaults, cookie jar and proxy configuration, and all URL fields can be given in
     *      opts instead of a url argument.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // request method, used by the opts-only form
     *          protocol: 'http', // URL override fields: protocol/host/hostname/port/pathname/path/query/auth
     *          hostname: '', port: 80, pathname: '/', query: {},
     *          headers: {}, // Headers object or plain object, added to the generated headers
     *          body: null, // SeekableStream | Buffer | String | Object | FormData | URLSearchParams | Blob
     *          json: null, // encoded as JSON, Content-Type: application/json
     *          pack: null, // encoded as msgpack, Content-Type: application/msgpack
     *          keepAlive: undefined, // overrides the client keepAlive for this request
     *          timeout: undefined, // request timeout in ms, overrides the client timeout
     *          signal: null, // AbortSignal used to cancel the request
     *          agent: null // HttpClient that sends this request instead of this one
     *      })
     *      ```
     *      body, json and pack are mutually exclusive; `query` replaces the query string of the URL
     *      instead of merging with it. Without body/json/pack the request carries no body.
     *
     *      Example — a sync request through an independent client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.requestSync('http://127.0.0.1:' + port + '/status');
     *      console.log(resp.json()); // { path: '/status' }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncAsync(opts: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body; the opts fields documented on requestSync(opts) apply here as well, with `url`
     *      providing protocol, host, port and path.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body; the opts fields documented on requestSync(opts) apply here as well, with `url`
     *      providing protocol, host, port and path.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body; the opts fields documented on requestSync(opts) apply here as well, with `url`
     *      providing protocol, host, port and path.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url and returns the result
     *
     *      Blocks the current fiber and returns the HttpResponse directly. method selects the request
     *      method (default GET) and opts carries the fields documented on requestSync(opts).
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSync(method: string, url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url and returns the result
     *
     *      Blocks the current fiber and returns the HttpResponse directly. method selects the request
     *      method (default GET) and opts carries the fields documented on requestSync(opts).
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncSync(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url and returns the result
     *
     *      Blocks the current fiber and returns the HttpResponse directly. method selects the request
     *      method (default GET) and opts carries the fields documented on requestSync(opts).
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    requestSyncAsync(method: string, url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the url specified by opts and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end()` to send it. The response arrives through the
     *      callback given here or registered later, or through the `'response'` event; it is also
     *      stored in the `response` property. All opts fields are documented on request(Stream,
     *      HttpRequest), the first request overload; unlike `get`, this function does not send
     *      automatically.
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    request(opts: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the url specified by opts, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The callback is called with the HttpResponse when the response arrives. The returned request
     *      must still be sent with `end()`; the request uses this client's settings and cookie jar.
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The method defaults to GET. The returned request must still be sent with `end()`; the
     *      callback receives the HttpResponse when the response arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end()` to send it; the response is delivered to the
     *      callback (when given), to the `'response'` event and to the `response` property. This form
     *      is the async counterpart of requestSync(url, opts) on this client; use `get(url, opts)` when
     *      the method is GET and the request should be sent automatically.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    request(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The method defaults to GET and the returned request must still be sent with `end()`; the
     *      callback is called with the HttpResponse. See the first request overload for the opts fields.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback is called with the
     *      HttpResponse when it arrives.
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(method: string, url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The general option form of the request family on this client; the returned request must
     *      still be sent with `end()`. See the first request overload for the opts fields.
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    request(method: string, url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback is called with the
     *      HttpResponse. See the first request overload for the opts fields.
     *      @param method the http request method: GET, POST, etc.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    request(method: string, url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body, so body/json/pack are not accepted; the other opts fields of requestSync(opts) apply.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    getSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body, so body/json/pack are not accepted; the other opts fields of requestSync(opts) apply.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    getSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the GET method and returns the result, equivalent to request("GET", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A GET request carries no
     *      body, so body/json/pack are not accepted; the other opts fields of requestSync(opts) apply.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    getSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The request is sent automatically without calling `end()`; the callback receives the
     *      HttpResponse when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    get(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method and returns an HttpRequest object
     *
     *      The returned request is sent automatically without calling `end()`; the response is
     *      delivered to the callback or the `'response'` event and stored in the `response` property.
     *      A GET request carries no body; the opts fields are the same as getSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    get(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the GET method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The request is sent automatically (no `end()` needed); the callback receives the HttpResponse
     *      when it arrives. See the get(url, opts) overload for the options.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    get(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. The request body is given
     *      by body, json or pack; a string body is sent as application/x-www-form-urlencoded, a plain
     *      object or FormData as multipart/form-data. See requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    postSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. The request body is given
     *      by body, json or pack; a string body is sent as application/x-www-form-urlencoded, a plain
     *      object or FormData as multipart/form-data. See requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    postSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the POST method and returns the result, equivalent to request("POST", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. The request body is given
     *      by body, json or pack; a string body is sent as application/x-www-form-urlencoded, a plain
     *      object or FormData as multipart/form-data. See requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    postSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse
     *      when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    post(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end(data)` or `end()` to send it (the body may also
     *      be given by the body/json/pack options); the response is delivered to the callback or the
     *      `'response'` event. See the first request overload for the opts fields.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    post(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the POST method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse.
     *      The body may be passed to `end(data)` or given by the body/json/pack options.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    post(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A DELETE request normally
     *      has no body, but a body may be given like with postSync; the opts fields are the same as
     *      postSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    delSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A DELETE request normally
     *      has no body, but a body may be given like with postSync; the opts fields are the same as
     *      postSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    delSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the DELETE method and returns the result, equivalent to request("DELETE", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A DELETE request normally
     *      has no body, but a body may be given like with postSync; the opts fields are the same as
     *      postSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    delSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse
     *      when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    del(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end()` to send it. The opts fields are the same as
     *      post(url, opts); see the first request overload for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    del(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the DELETE method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    del(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PUT replaces the target
     *      resource with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    putSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PUT replaces the target
     *      resource with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    putSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PUT method and returns the result, equivalent to request("PUT", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PUT replaces the target
     *      resource with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    putSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse
     *      when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    put(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end(data)` or `end()` to send it. The opts fields
     *      are the same as post(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    put(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PUT method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    put(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PATCH applies a partial
     *      update with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    patchSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PATCH applies a partial
     *      update with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    patchSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the PATCH method and returns the result, equivalent to request("PATCH", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. PATCH applies a partial
     *      update with the request body, which is given by the body/json/pack fields like with
     *      postSync; see requestSync(opts) for the field list.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    patchSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse
     *      when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    patch(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method and returns an HttpRequest object
     *
     *      The returned request is not sent: call `end(data)` or `end()` to send it. The opts fields
     *      are the same as post(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    patch(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the PATCH method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The returned request must still be sent with `end()`; the callback receives the HttpResponse.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    patch(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A HEAD response carries the
     *      status and headers of the equivalent GET but no body, so `resp.body` is empty; unlike GET,
     *      a HEAD response with a large Content-Length is not rejected by maxBodySize. The opts fields
     *      are the same as getSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    headSync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A HEAD response carries the
     *      status and headers of the equivalent GET but no body, so `resp.body` is empty; unlike GET,
     *      a HEAD response with a large Content-Length is not rejected by maxBodySize. The opts fields
     *      are the same as getSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    headSyncSync(url: string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Requests the specified url with the HEAD method and returns the result, equivalent to request("HEAD", ...)
     *
     *      Blocks the current fiber and returns the HttpResponse directly. A HEAD response carries the
     *      status and headers of the equivalent GET but no body, so `resp.body` is empty; unlike GET,
     *      a HEAD response with a large Content-Length is not rejected by maxBodySize. The opts fields
     *      are the same as getSync(url, opts).
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns the server response
     *
     */
    headSyncAsync(url: string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The request is sent automatically without calling `end()`; the callback receives the
     *      HttpResponse when it arrives.
     *      @param url the url to request; must be a complete url including the host
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    head(url: string, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method and returns an HttpRequest object
     *
     *      Like get(), the returned request is sent automatically without calling `end()`; only the
     *      status and headers of the response are received. See the get(url, opts) overload for the
     *      options.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @return returns an HttpRequest object (listen to the 'response' event to receive the response)
     *
     */
    head(url: string, opts?: FIBJS.GeneralObject): Class_HttpRequest;

    /**
     * @description Requests the specified url with the HEAD method, registers a callback to receive the response, and returns an HttpRequest object
     *
     *      The request is sent automatically without calling `end()`; the callback receives the
     *      HttpResponse when the headers arrive.
     *      @param url the url to request; must be a complete url including the host
     *      @param opts the additional information
     *      @param callback response callback function, receives HttpResponse as a parameter
     *      @return returns an HttpRequest object
     *
     */
    head(url: string, opts: FIBJS.GeneralObject, callback: (resp: Class_HttpResponse | Class_HttpResponsePromise)=>void): Class_HttpRequest;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *
     *      The request is sent through this client, using its settings, cookie jar and proxy
     *      configuration. opts can override the request fields (`new Request(request, init)`
     *      semantics); the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // overrides the request method; the method of an HttpRequest source is kept when not given
     *          headers: {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          body: null, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          keepAlive: undefined, // overrides the keep-alive setting
     *          timeout: undefined, // request timeout in ms, uses the client default settings by default
     *          redirect: 'follow', // redirect mode: 'follow' (default) | 'error' | 'manual'
     *          signal: null, // AbortSignal object used to cancel the request
     *          streaming: false // whether to expose the response body as a stream instead of buffering it
     *      })
     *      ```
     *      Following the Fetch standard a GET or HEAD request must not carry a body (a TypeError is
     *      thrown) and `headers` replaces the headers of the request source instead of merging them.
     *      `redirect: 'error'` fails with a TypeError when the server redirects and `redirect: 'manual'`
     *      returns the redirect response as it is, with `redirected` false; otherwise redirects are
     *      followed according to `autoRedirect`. An aborted request fails with an AbortError (a
     *      TimeoutError for AbortSignal.timeout).
     *
     *      Example — fetch a URL through this client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ ok: true });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.fetch('http://127.0.0.1:' + port + '/api');
     *      console.log(resp.status, resp.ok, resp.json()); // 200 true { ok: true }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param request the request source
     *      @param opts the additional information, can override the corresponding fields in request; following the Fetch
     *      standard a GET or HEAD request must not carry a body, a string body is sent as `text/plain;charset=UTF-8`,
     *      and `headers` replaces the headers of the request source instead of merging them
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetch(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *
     *      The request is sent through this client, using its settings, cookie jar and proxy
     *      configuration. opts can override the request fields (`new Request(request, init)`
     *      semantics); the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // overrides the request method; the method of an HttpRequest source is kept when not given
     *          headers: {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          body: null, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          keepAlive: undefined, // overrides the keep-alive setting
     *          timeout: undefined, // request timeout in ms, uses the client default settings by default
     *          redirect: 'follow', // redirect mode: 'follow' (default) | 'error' | 'manual'
     *          signal: null, // AbortSignal object used to cancel the request
     *          streaming: false // whether to expose the response body as a stream instead of buffering it
     *      })
     *      ```
     *      Following the Fetch standard a GET or HEAD request must not carry a body (a TypeError is
     *      thrown) and `headers` replaces the headers of the request source instead of merging them.
     *      `redirect: 'error'` fails with a TypeError when the server redirects and `redirect: 'manual'`
     *      returns the redirect response as it is, with `redirected` false; otherwise redirects are
     *      followed according to `autoRedirect`. An aborted request fails with an AbortError (a
     *      TimeoutError for AbortSignal.timeout).
     *
     *      Example — fetch a URL through this client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ ok: true });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.fetch('http://127.0.0.1:' + port + '/api');
     *      console.log(resp.status, resp.ok, resp.json()); // 200 true { ok: true }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param request the request source
     *      @param opts the additional information, can override the corresponding fields in request; following the Fetch
     *      standard a GET or HEAD request must not carry a body, a string body is sent as `text/plain;charset=UTF-8`,
     *      and `headers` replaces the headers of the request source instead of merging them
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetchSync(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Sends a request using the Web Fetch standard with an HttpRequest object as the request source and returns an HttpResponse object
     *
     *      The request is sent through this client, using its settings, cookie jar and proxy
     *      configuration. opts can override the request fields (`new Request(request, init)`
     *      semantics); the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          method: 'GET', // overrides the request method; the method of an HttpRequest source is kept when not given
     *          headers: {}, // when present it replaces the headers of the request source, like `new Request(request, init)`
     *          body: null, // overrides the request body; a string body is sent as text/plain;charset=UTF-8
     *          keepAlive: undefined, // overrides the keep-alive setting
     *          timeout: undefined, // request timeout in ms, uses the client default settings by default
     *          redirect: 'follow', // redirect mode: 'follow' (default) | 'error' | 'manual'
     *          signal: null, // AbortSignal object used to cancel the request
     *          streaming: false // whether to expose the response body as a stream instead of buffering it
     *      })
     *      ```
     *      Following the Fetch standard a GET or HEAD request must not carry a body (a TypeError is
     *      thrown) and `headers` replaces the headers of the request source instead of merging them.
     *      `redirect: 'error'` fails with a TypeError when the server redirects and `redirect: 'manual'`
     *      returns the redirect response as it is, with `redirected` false; otherwise redirects are
     *      followed according to `autoRedirect`. An aborted request fails with an AbortError (a
     *      TimeoutError for AbortSignal.timeout).
     *
     *      Example — fetch a URL through this client:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ ok: true });
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const client = new http.Client();
     *      const resp = client.fetch('http://127.0.0.1:' + port + '/api');
     *      console.log(resp.status, resp.ok, resp.json()); // 200 true { ok: true }
     *
     *      client.destroy();
     *      server.stop();
     *      ```
     *      @param request the request source
     *      @param opts the additional information, can override the corresponding fields in request; following the Fetch
     *      standard a GET or HEAD request must not carry a body, a string body is sent as `text/plain;charset=UTF-8`,
     *      and `headers` replaces the headers of the request source instead of merging them
     *      @return returns the server response, containing properties such as status, headers, body, ok, redirected, url and type
     *
     */
    fetchAsync(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

}


declare namespace Class_HttpClient {
    const promises: FIBJS.GeneralObject;
}
