/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/WebSocketMessage.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description A WebSocket client and server endpoint, the fibjs implementation of the WebSocket API
 *
 *  A WebSocket connection starts as an HTTP/1.1 request carrying an Upgrade
 *  handshake and, after the server answers 101, becomes a full-duplex message
 *  channel between exactly two peers. fibjs exposes both ends of the protocol
 *  through this interface:
 *
 *  - a client is created by the `WebSocket` constructor with a `ws://` or
 *    `wss://` URL; the handshake runs asynchronously and the `open` event
 *    reports success;
 *  - a server object is produced by the `WebSocket.upgrade` handler, which
 *    converts matching HTTP upgrade requests into connected sockets.
 *
 *  Concepts:
 *  - Handshake: the client sends `Upgrade: websocket`, `Connection: Upgrade`,
 *    `Sec-WebSocket-Version: 13` and a random `Sec-WebSocket-Key`; the server
 *    answers 101 with the matching `Sec-WebSocket-Accept` header. A failed
 *    handshake raises the `error` event and then the `close` event.
 *  - Frame types: TEXT (1), BINARY (2), CLOSE (8), PING (9), PONG (10) and
 *    CONTINUE (0) fragments. Received fragments are re-assembled into one
 *    WebSocketMessage before the `message` event fires.
 *  - Text and binary data: `msg.data` is a String for TEXT messages and a
 *    Buffer for BINARY messages. `send` accepts a Buffer, a typed array, an
 *    ArrayBuffer or a Blob and sends a BINARY frame; every other value is
 *    sent as its string form in a TEXT frame, so `send(null)` sends the text
 *    "null" and `send(123)` sends "123".
 *  - Ping/pong keep-alive: a PING frame from the peer is answered with a PONG
 *    frame automatically and PONG frames are consumed silently; there is no
 *    manual ping API.
 *  - Closing: `close(code, reason)` sends a CLOSE frame; close codes are
 *    limited to 1000 or 3000-4999. When the peer drops the connection without
 *    a close handshake, the `close` event reports code 1006, "Abnormal
 *    Closure".
 *  - permessage-deflate: compression is negotiated only when the client
 *    enables the `perMessageDeflate` option and the server enables it in
 *    `WebSocket.upgrade`; on a compressed connection a compressed message
 *    reports `WebSocketMessage.compress` as true.
 *  - Sub-protocols: the client may offer a list of protocols and the server
 *    selects one of its own; the selected value is available as `protocol`.
 *    When the client offers protocols and the server does not select one, the
 *    client aborts the handshake.
 *  - Server-side sockets have empty `url` and `origin`; the handshake request
 *    received by the accept callback carries the original header values.
 *  - Node.js and the DOM expose only the client role, so the
 *    `WebSocket.upgrade` handler is a fibjs extension. The `message` event
 *    delivers the WebSocketMessage object itself rather than a DOM
 *    MessageEvent, and binary payloads are Buffers rather than ArrayBuffers.
 *
 *  Obtained from:
 *  - `new WebSocket(url, ...)` — client connection; `url` must use the `ws://`
 *    or `wss://` scheme;
 *  - `WebSocket.upgrade(opts, accept)` — protocol handler for an HttpServer
 *    route, a Routing table or a Chain; the accept callback receives the
 *    connected WebSocket.
 *
 *  Notes:
 *  - An `error` event with no registered listener is re-thrown as an unhandled
 *    error, so bind `onerror` whenever the handshake may fail.
 *
 *  Example 1 — an echo server and a client exchanging text and binary frames:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, {
 *      '/ws': WebSocket.upgrade((conn) => {
 *          conn.onmessage = (msg) => conn.send(msg.data); // echo the payload as it arrived
 *      })
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const sock = new WebSocket('ws://127.0.0.1:' + port + '/ws');
 *  let count = 0;
 *  sock.onopen = () => sock.send('hello');
 *  sock.onmessage = (msg) => {
 *      console.log(typeof msg.data, msg.data); // string hello on the first call
 *      if (count++ === 0)
 *          sock.send(Buffer.from([1, 2, 3]));
 *      else
 *          sock.close(1000, 'done');
 *  };
 *  sock.onclose = (ev) => {
 *      console.log(ev.code, ev.reason); // 1000 done
 *      server.stop();
 *  };
 *  ```
 *
 *  Example 2 — sub-protocol selection and permessage-deflate compression:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, {
 *      '/ws': WebSocket.upgrade({
 *          protocols: ['json', 'text'],
 *          perMessageDeflate: true
 *      }, (conn) => {
 *          console.log(conn.protocol); // json: selected from the client offer
 *          conn.onmessage = (msg) => {
 *              console.log(msg.compress, msg.data.length); // true 88
 *              conn.send(msg.data);
 *          };
 *      })
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const sock = new WebSocket('ws://127.0.0.1:' + port + '/ws', {
 *      protocols: ['json', 'text'],
 *      perMessageDeflate: true
 *  });
 *  sock.onopen = () => sock.send('deflate me '.repeat(8)); // 88 characters
 *  sock.onmessage = (msg) => {
 *      console.log(sock.protocol, msg.compress); // json true
 *      sock.close();
 *  };
 *  sock.onclose = () => server.stop();
 *  ```
 *
 *  Example 3 — a failed handshake reported through error and close:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, (req) => {
 *      req.response.write('plain HTTP, not a websocket endpoint');
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const sock = new WebSocket('ws://127.0.0.1:' + port + '/');
 *  sock.onopen = () => console.log('never fires');
 *  sock.onerror = (ev) => console.log('error', ev.code, ev.reason); // 1002 server error.
 *  sock.onclose = (ev) => {
 *      console.log('close', ev.code, ev.reason); // 1006 Abnormal Closure
 *      server.stop();
 *  };
 *  ```
 *
 */
declare class Class_WebSocket extends Class_EventEmitter {
    /**
     * @description Creates a client and starts the handshake with a list of sub-protocols
     *
     *      The connection is asynchronous: the constructor returns with readyState
     *      CONNECTING, the handshake runs in the background and `open` fires after
     *      the server accepted. A rejected upgrade, a missing Sec-WebSocket-Accept
     *      header, or a server that does not select one of the offered protocols
     *      raises `error` and then `close` instead.
     *
     *      The three constructor forms differ only in how the handshake is
     *      described: this one offers a protocol list, the single-protocol form
     *      offers one value and the options form collects everything in one
     *      object. When the server selects a protocol it is reported by the
     *      `protocol` property.
     *
     *      `origin` is sent as the `Origin` header and stored in the `origin`
     *      property; an empty string omits the header.
     *
     *      @param url the server address, using the `ws://` or `wss://` scheme
     *      @param protocols the list of sub-protocols offered to the server
     *      @param origin the origin to simulate during the handshake, "" by default
     *
     */
    constructor(url: string, protocols: string[], origin?: string);

    /**
     * @description Creates a client with all handshake options in one object
     *
     *      opts contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "protocol": "", // a single sub-protocol, "" when omitted
     *          "protocols": [], // a list of sub-protocols, takes precedence over protocol
     *          "origin": "", // the value of the Origin header, "" when omitted
     *          "perMessageDeflate": false, // request permessage-deflate compression
     *          "maxPayload": 67108864, // max accepted message size in bytes (64 MB)
     *          "httpClient": null, // the HttpClient used for the handshake, the global one by default
     *          "headers": {} // extra headers sent with the handshake request
     *      })
     *      ```
     *      Both protocol forms end up in the `Sec-WebSocket-Protocol` request
     *      header and the server may select one of them. `maxPayload` limits
     *      incoming messages; a larger message fails the connection with error
     *      code 1009 (see the WebSocketMessage maxSize property).
     *
     *      @param url the server address, using the `ws://` or `wss://` scheme
     *      @param opts connection options, {} by default
     *
     */
    constructor(url: string, opts: FIBJS.GeneralObject);

    /**
     * @description Creates a client and starts the handshake with a single sub-protocol
     *
     *      This form is equivalent to passing a one-element protocols array to the
     *      first constructor, and the selected protocol is reported by `protocol`.
     *      Unlike the options form, `perMessageDeflate` and extra headers cannot be
     *      set here.
     *
     *      @param url the server address, using the `ws://` or `wss://` scheme
     *      @param protocol the single sub-protocol offered to the server, "" by default
     *      @param origin the origin to simulate during the handshake, "" by default
     *
     */
    constructor(url: string, protocol?: string, origin?: string);

    /**
     * @description Frame type of a continuation frame; fragments carry the rest of a message
     */
    static readonly CONTINUE: 0;

    /**
     * @description Frame type of a text frame whose payload is a UTF-8 string
     */
    static readonly TEXT: 1;

    /**
     * @description Frame type of a binary frame whose payload is binary data
     */
    static readonly BINARY: 2;

    /**
     * @description Frame type of a close frame carrying the close code and reason
     */
    static readonly CLOSE: 8;

    /**
     * @description Frame type of a ping frame, answered with a pong frame automatically
     */
    static readonly PING: 9;

    /**
     * @description Frame type of a pong frame, consumed silently by the protocol layer
     */
    static readonly PONG: 10;

    /**
     * @description Connection state: the handshake is in progress (0)
     */
    static readonly CONNECTING: 0;

    /**
     * @description Connection state: the handshake completed and data can be exchanged (1)
     */
    static readonly OPEN: 1;

    /**
     * @description Connection state: a close frame is being exchanged (2)
     */
    static readonly CLOSING: 2;

    /**
     * @description Connection state: the connection is closed and no more data can be exchanged (3)
     */
    static readonly CLOSED: 3;

    /**
     * @description Queries the URL of the server the client connected to
     *
     *      For a client socket this is the URL passed to the constructor. A socket
     *      created by `WebSocket.upgrade` has no URL of its own and reports "";
     *      read the requested address from the HttpRequest received by the accept
     *      callback instead.
     *
     */
    readonly url: string;

    /**
     * @description Queries the sub-protocol negotiated during the handshake
     *
     *      The value is "" before the handshake completes and stays "" when no
     *      sub-protocol was selected. On a client it comes from the
     *      `Sec-WebSocket-Protocol` response header; on a server socket it is the
     *      protocol selected by `WebSocket.upgrade` from the client offer.
     *
     */
    readonly protocol: string;

    /**
     * @description Queries the origin used during the handshake
     *
     *      The value is the `origin` constructor argument or option, which is also
     *      sent as the `Origin` request header. Server-side sockets report "";
     *      read the `Origin` header from the handshake HttpRequest instead.
     *
     */
    readonly origin: string;

    /**
     * @description Queries the connection state: CONNECTING, OPEN, CLOSING or CLOSED
     *
     *      A client starts in CONNECTING, becomes OPEN when the handshake completes
     *      and moves to CLOSING while a close frame is exchanged. The state is
     *      CLOSED after a normal close, after a failed handshake and after an
     *      abnormal loss of the connection, once the `close` event has fired.
     *
     */
    readonly readyState: number;

    /**
     * @description Closes the connection by sending a CLOSE frame to the peer
     *
     *      The close code must be 1000 or a value between 3000 and 4999; any other
     *      value throws. The call returns immediately and the socket is released in
     *      the background, then the `close` event reports the code and the reason.
     *
     *      Calling close() when the socket is not OPEN is a no-op: in particular a
     *      close() during CONNECTING does not cancel the handshake, and calling
     *      close() a second time after it started is harmless.
     *
     *      @param code the close code: 1000 or 3000-4999, 1000 by default
     *      @param reason the close reason carried by the CLOSE frame, "" by default
     *
     */
    close(code?: number, reason?: string): void;

    /**
     * @description Sends data to the peer; binary values use a BINARY frame, other values a text frame
     *
     *      A Buffer, a typed array, an ArrayBuffer or a Blob is sent as a BINARY
     *      frame; every other value is converted to its string form and sent as a
     *      TEXT frame, so `send(123)` sends the text "123", `send(null)` sends
     *      "null" and `send(['a', 'b'])` sends "a,b". Frames are queued and sent
     *      in call order; there is no per-message completion callback and failures
     *      of the local state (for example when the socket is not OPEN) throw.
     *
     *      Sending while the socket is CONNECTING, CLOSING or CLOSED throws, the
     *      same restriction the DOM WebSocket has.
     *
     *      Example — the accepted data forms and the frame each one produces:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, {
     *          '/ws': WebSocket.upgrade((conn) => {
     *              conn.onmessage = (msg) => console.log(msg.type, String(msg.data));
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const sock = new WebSocket('ws://127.0.0.1:' + port + '/ws');
     *      sock.onopen = () => {
     *          sock.send('text'); // WebSocket.TEXT "text"
     *          sock.send(Buffer.from('binary')); // WebSocket.BINARY "binary"
     *          sock.send(new Uint8Array([65, 66])); // WebSocket.BINARY "AB"
     *          sock.send(null); // WebSocket.TEXT "null"
     *          sock.close();
     *      };
     *      sock.onclose = () => server.stop();
     *      ```
     *
     *      @param data the data to send
     *
     */
    send(data: any): void;

    /**
     * @description Queries and binds the open event, equivalent to on("open", func)
     *
     *      The listener receives no argument. On a client the handshake completed
     *      and `protocol` is final; on a server socket the connection is ready when
     *      the accept callback runs.
     *
     */
    on(event: "open", listener: ()=>void): this;

    once(event: "open", listener: ()=>void): this;

    off(event: "open", listener: ()=>void): this;

    addListener(event: "open", listener: ()=>void): this;

    removeListener(event: "open", listener: ()=>void): this;

    addEventListener(event: "open", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "open", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "open", listener: ()=>void): this;

    prependOnceListener(event: "open", listener: ()=>void): this;

    /**
     * @description Queries and binds the open event, equivalent to on("open", func)
     *
     *      The listener receives no argument. On a client the handshake completed
     *      and `protocol` is final; on a server socket the connection is ready when
     *      the accept callback runs.
     *
     */
    onopen: (()=>void) | null;

    /**
     * @description Queries and binds the message event, equivalent to on("message", func)
     *
     *      The listener receives the received WebSocketMessage itself, not a DOM
     *      MessageEvent: `msg.type` is the frame type and `msg.data` is a String for
     *      TEXT messages or a Buffer for BINARY messages. PING and PONG frames are
     *      handled by the protocol layer and never reach this event.
     *
     *      @param msg the received message
     *
     */
    on(event: "message", listener: (msg: Class_WebSocketMessage)=>void): this;

    once(event: "message", listener: (msg: Class_WebSocketMessage)=>void): this;

    off(event: "message", listener: (msg: Class_WebSocketMessage)=>void): this;

    addListener(event: "message", listener: (msg: Class_WebSocketMessage)=>void): this;

    removeListener(event: "message", listener: (msg: Class_WebSocketMessage)=>void): this;

    addEventListener(event: "message", listener: (msg: Class_WebSocketMessage)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: (msg: Class_WebSocketMessage)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: (msg: Class_WebSocketMessage)=>void): this;

    prependOnceListener(event: "message", listener: (msg: Class_WebSocketMessage)=>void): this;

    /**
     * @description Queries and binds the message event, equivalent to on("message", func)
     *
     *      The listener receives the received WebSocketMessage itself, not a DOM
     *      MessageEvent: `msg.type` is the frame type and `msg.data` is a String for
     *      TEXT messages or a Buffer for BINARY messages. PING and PONG frames are
     *      handled by the protocol layer and never reach this event.
     *
     *      @param msg the received message
     *
     */
    onmessage: ((msg: Class_WebSocketMessage)=>void) | null;

    /**
     * @description Queries and binds the close event, equivalent to on("close", func)
     *
     *      The listener receives the fibjs event object: `ev.code` is the close code
     *      reported by the peer (1000 or 3000-4999) or 1006 when the connection was
     *      lost without a closing handshake, and `ev.reason` is the close reason or
     *      "Abnormal Closure". readyState is CLOSED when the event fires.
     *
     *      @param ev the event object carrying the close code and reason
     *
     */
    on(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "close", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the close event, equivalent to on("close", func)
     *
     *      The listener receives the fibjs event object: `ev.code` is the close code
     *      reported by the peer (1000 or 3000-4999) or 1006 when the connection was
     *      lost without a closing handshake, and `ev.reason` is the close reason or
     *      "Abnormal Closure". readyState is CLOSED when the event fires.
     *
     *      @param ev the event object carrying the close code and reason
     *
     */
    onclose: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the error event, equivalent to on("error", func)
     *
     *      The listener receives the fibjs event object: `ev.code` is the protocol or
     *      transport error code (1001 going away, 1002 protocol error, 1007 invalid
     *      payload, 1009 message too big, for example) and `ev.reason` describes the
     *      failure when the protocol layer supplies one. Transport failures also
     *      carry fields such as `errno`, `syscall` and `hostname`. The event fires
     *      before `close`, and an error event with no listener registered is
     *      re-thrown as an unhandled error.
     *
     *      @param ev the event object carrying the error code and reason
     *
     */
    on(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "error", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the error event, equivalent to on("error", func)
     *
     *      The listener receives the fibjs event object: `ev.code` is the protocol or
     *      transport error code (1001 going away, 1002 protocol error, 1007 invalid
     *      payload, 1009 message too big, for example) and `ev.reason` describes the
     *      failure when the protocol layer supplies one. Transport failures also
     *      carry fields such as `errno`, `syscall` and `hostname`. The event fires
     *      before `close`, and an error event with no listener registered is
     *      re-thrown as an unhandled error.
     *
     *      @param ev the event object carrying the error code and reason
     *
     */
    onerror: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Keeps the fibjs process alive while this socket is bound
     *
     *      The socket holds the event loop open until it is closed; `unref` releases
     *      it again. Returns the socket itself, so calls can be chained.
     *
     *      @return the socket itself
     *
     */
    ref(): Class_WebSocket;

    /**
     * @description Allows the fibjs process to exit while this socket is bound
     *
     *      The socket no longer holds the event loop open; `ref` restores the
     *      default. Returns the socket itself, so calls can be chained.
     *
     *      @return the socket itself
     *
     */
    unref(): Class_WebSocket;

    /**
     * @description The WebSocketMessage class, reachable as `WebSocket.Message`
     *
     *      The class is not a global variable; use this property (or
     *      `new WebSocket.Message()`) to build protocol messages by hand, as the
     *      WebSocketMessage examples do.
     *
     */
    static Message: Class_WebSocketMessage;

    /**
     * @description Creates a WebSocket protocol handler with default options
     *
     *      The returned handler turns an HTTP upgrade request into a connected
     *      WebSocket and calls accept(conn, req) after the 101 response has been
     *      sent. A request without a valid WebSocket handshake is answered with an
     *      error status. When accept runs, `conn.protocol` is already final and
     *      `req` is the HttpRequest of the handshake, useful to read headers such as
     *      Origin or the requested address.
     *
     *      The handler is a routing handler: use it as the value of an HttpServer
     *      route, inside a Routing table or in a Chain.
     *
     *      @param accept called with the connected WebSocket and the handshake HttpRequest
     *      @return the protocol handler
     *
     */
    static upgrade(accept: (conn: Class_WebSocket, req: Class_HttpRequest | Class_HttpRequestPromise)=>void): Class_Handler;

    /**
     * @description Creates a WebSocket protocol handler with explicit options
     *
     *      opts contains additional options for the handshake, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "protocol": "", // a single sub-protocol accepted by the server
     *          "protocols": [], // a list of accepted sub-protocols, takes precedence over protocol
     *          "perMessageDeflate": false, // accept permessage-deflate compression
     *          "maxPayload": 67108864 // max accepted message size in bytes (64 MB)
     *      })
     *      ```
     *      The server selects the first protocol of the client offer that appears
     *      in its own list and echoes it in `Sec-WebSocket-Protocol`; when nothing
     *      matches, the handshake succeeds without a sub-protocol and a client that
     *      offered one aborts the connection. permessage-deflate is enabled only
     *      when the client requests it too. A message larger than maxPayload fails
     *      the connection: the local `error` event reports code 1009 and `close`
     *      reports 1006.
     *
     *      @param opts connection options, {} by default
     *      @param accept called with the connected WebSocket and the handshake HttpRequest
     *      @return the protocol handler
     *
     */
    static upgrade(opts: FIBJS.GeneralObject, accept: (conn: Class_WebSocket, req: Class_HttpRequest | Class_HttpRequestPromise)=>void): Class_Handler;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}

