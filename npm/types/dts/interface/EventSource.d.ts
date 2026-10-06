/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * @description A client for the Server-Sent Events protocol, the fibjs EventSource implementation
 *
 *  An EventSource keeps one HTTP response open and reads a `text/event-stream`
 *  body pushed by the server. Unlike WebSocket the channel is one-way: the
 *  server keeps sending events and the client only reads them. Use it for live
 *  feeds that travel over plain HTTP, such as notifications, logs or token
 *  streams.
 *
 *  This class is exported by the sse module as `sse.EventSource`, it is not a
 *  global variable; the same module provides `sse.upgrade` for the server side.
 *
 *  Concepts:
 *  - Stream format: the server sends UTF-8 text and one event is a group of
 *    lines terminated by an empty line. `data:` lines are joined with "\n",
 *    `event:` names the event type, `id:` carries the event id, `retry:`
 *    suggests a reconnection delay, and lines starting with ":" are comments
 *    and are ignored.
 *  - Dispatching: a group without an `event:` field is delivered to `onmessage`
 *    listeners; a group with `event: <name>` is delivered to listeners of
 *    <name> registered with addEventListener. The event object carries `data`,
 *    `id` when a non-empty id was set, and `retry` ("" when absent).
 *  - readyState: CONNECTING (0) while the request is being made, OPEN (1) once
 *    a response with Content-Type "text/event-stream" has been accepted, and
 *    CLOSED (2) after the stream ended or an HTTP error occurred. The server
 *    side of `sse.upgrade` starts in SENDER (3). The constants live in the sse
 *    module, so they are read as `sse.OPEN` and so on.
 *  - Reconnection: this implementation does not reconnect and never sends the
 *    Last-Event-ID request header; `retry` is parsed and reported but not
 *    acted upon. A finished stream raises `close`, a fatal response or a
 *    network failure raises `error`, and the application decides whether to
 *    create a new EventSource. MDN EventSource reconnects on its own, so
 *    porting code must add that logic explicitly.
 *  - Errors: a non-200 response reports the status as `code` and
 *    "Invalid status: ..." as `reason`; a response with a different
 *    Content-Type reports no code and "Invalid Content-Type: ..." as `reason`;
 *    a connection failure also reports no code and "Connection error" as
 *    `reason`. After an HTTP error readyState is CLOSED, but after a connection
 *    failure it stays CONNECTING because nothing is retried.
 *  - Server side: `sse.upgrade(accept)` upgrades an HTTP request and hands the
 *    callback an EventSource in SENDER state whose `send(data, opts)` writes
 *    `event:`, `id:`, `retry:` and `data:` fields; `close()` terminates the
 *    chunked response so that client-side readers see the end of the stream.
 *  - withCredentials is read only and always false in fibjs, and `response`
 *    exposes the underlying HttpResponse as a fibjs extension.
 *
 *  Obtained from:
 *  - `new (require('sse').EventSource)(url, options)` — client connection;
 *  - `sse.upgrade(accept)` — server handler; the accept callback receives the
 *    connected EventSource in SENDER state.
 *
 *  Example 1 — receive one event and observe the end of the stream:
 *  ```JavaScript
 *  const http = require('http');
 *  const sse = require('sse');
 *
 *  const server = new http.Server(0, {
 *      '/events': sse.upgrade((sender) => {
 *          sender.send('first event', { id: '1' });
 *          sender.close(); // ends the stream
 *      })
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const es = new sse.EventSource('http://127.0.0.1:' + port + '/events');
 *  es.onmessage = (ev) => console.log(ev.data, ev.id); // first event 1
 *  es.onclose = () => {
 *      console.log(es.readyState === sse.CLOSED); // true
 *      server.stop();
 *  };
 *  es.onerror = (ev) => console.log('error', ev.reason);
 *  ```
 *
 *  Example 2 — a named event with id and retry fields and multi-line data:
 *  ```JavaScript
 *  const http = require('http');
 *  const sse = require('sse');
 *
 *  const server = new http.Server(0, {
 *      '/feed': sse.upgrade((sender) => {
 *          sender.send('line one\nline two', {
 *              event: 'tick',
 *              id: '42',
 *              retry: 3000
 *          });
 *          sender.close();
 *      })
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const es = new sse.EventSource('http://127.0.0.1:' + port + '/feed');
 *  es.addEventListener('tick', (ev) => {
 *      console.log(ev.data); // line one\nline two
 *      console.log(ev.id, ev.retry); // 42 3000
 *  });
 *  es.onclose = () => server.stop();
 *  es.onerror = (ev) => console.log(ev.reason);
 *  ```
 *
 *  Example 3 — a rejected response reported through the error event:
 *  ```JavaScript
 *  const http = require('http');
 *  const sse = require('sse');
 *
 *  const server = new http.Server(0, (req) => {
 *      req.response.status = 404;
 *      req.response.write('missing');
 *  });
 *  server.start();
 *  const port = server.socket.localPort;
 *
 *  const es = new sse.EventSource('http://127.0.0.1:' + port + '/');
 *  es.onmessage = (ev) => console.log('never fires');
 *  es.onerror = (ev) => {
 *      console.log(ev.code, ev.reason); // 404 Invalid status: File Not Found
 *      console.log(es.readyState === sse.CLOSED); // true
 *      server.stop();
 *  };
 *  ```
 *
 */
declare class Class_EventSource extends Class_EventEmitter {
    /**
     * @description Creates a client and starts reading a text/event-stream response
     *
     *      The constructor returns with readyState CONNECTING and the request runs
     *      in the background; `open` fires after a response with Content-Type
     *      "text/event-stream" is received. `error` fires on an HTTP error, a wrong
     *      content type or a connection failure, and `close` fires when the server
     *      ends the stream.
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "method": "GET", // request method, inferred as POST when body/json/pack is used
     *          "headers": {}, // extra request headers
     *          "body": null, // request body for methods that carry one
     *          "json": null, // JSON body, implies POST and sets Content-Type
     *          "pack": null, // msgpack body, implies POST and sets Content-Type
     *          "query": {}, // query string parameters
     *          "keepAlive": true, // the connection is kept alive until close()
     *          "timeout": 0, // request timeout in milliseconds
     *          "httpClient": null // HttpClient used for the request, the global one by default
     *      })
     *      ```
     *      The URL and the remaining options are resolved the same way `http.request`
     *      resolves them.
     *
     *      @param url the server address
     *      @param options request options, {} by default
     *
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description Closes the connection and stops reading events
     *
     *      After close() the readyState is CLOSED and no further events are read;
     *      the HTTP connection to the server is released. On a server-side sender,
     *      close() terminates the chunked stream so that the client sees the end of
     *      the response and raises its `close` event. Closing an already closed
     *      object is harmless, and close() never raises `error`.
     *
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the connection and stops reading events
     *
     *      After close() the readyState is CLOSED and no further events are read;
     *      the HTTP connection to the server is released. On a server-side sender,
     *      close() terminates the chunked stream so that the client sees the end of
     *      the response and raises its `close` event. Closing an already closed
     *      object is harmless, and close() never raises `error`.
     *
     */
    closeSync(): void;

    /**
     * @description Closes the connection and stops reading events
     *
     *      After close() the readyState is CLOSED and no further events are read;
     *      the HTTP connection to the server is released. On a server-side sender,
     *      close() terminates the chunked stream so that the client sees the end of
     *      the response and raises its `close` event. Closing an already closed
     *      object is harmless, and close() never raises `error`.
     *
     */
    closeAsync(): Promise<void>;

    /**
     * @description Sends one event to the client; available on a server-side sender only
     *
     *      The object must be in SENDER state, which is how `sse.upgrade` hands it
     *      to the accept callback; calling send() on a client EventSource throws.
     *
     *      options contains additional options for the event, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "event": "message", // event name, "message" by default
     *          "id": "", // event id; the id line is written only when provided
     *          "retry": 0 // suggested reconnect delay in ms; written only when provided
     *      })
     *      ```
     *      The payload is written as UTF-8: every line of data becomes its own
     *      `data:` line and the event is terminated by an empty line, so a
     *      multi-line string is delivered as one event whose data contains
     *      newlines. retry must not be negative. The returned value is the number
     *      of bytes written to the connection.
     *
     *      Example — sending several named events on one connection:
     *      ```JavaScript
     *      const http = require('http');
     *      const sse = require('sse');
     *
     *      const server = new http.Server(0, {
     *          '/progress': sse.upgrade((sender) => {
     *              sender.send('start', { event: 'status', id: 's1' });
     *              sender.send('half', { event: 'progress' }); // no id, no retry
     *              sender.send('end', { event: 'status', id: 's2' });
     *              sender.close();
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const es = new sse.EventSource('http://127.0.0.1:' + port + '/progress');
     *      // status events print "start s1" and "end s2"
     *      es.addEventListener('status', (ev) => console.log('status', ev.data, ev.id));
     *      es.addEventListener('progress', (ev) => console.log('progress', ev.data)); // half
     *      es.onclose = () => server.stop();
     *      es.onerror = (ev) => console.log(ev.reason);
     *      ```
     *
     *      @param data the event data, sent as one or more data lines
     *      @param options event options, {} by default
     *      @return the number of bytes sent
     *
     */
    send(data: string, options?: FIBJS.GeneralObject): number;

    send(data: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Sends one event to the client; available on a server-side sender only
     *
     *      The object must be in SENDER state, which is how `sse.upgrade` hands it
     *      to the accept callback; calling send() on a client EventSource throws.
     *
     *      options contains additional options for the event, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "event": "message", // event name, "message" by default
     *          "id": "", // event id; the id line is written only when provided
     *          "retry": 0 // suggested reconnect delay in ms; written only when provided
     *      })
     *      ```
     *      The payload is written as UTF-8: every line of data becomes its own
     *      `data:` line and the event is terminated by an empty line, so a
     *      multi-line string is delivered as one event whose data contains
     *      newlines. retry must not be negative. The returned value is the number
     *      of bytes written to the connection.
     *
     *      Example — sending several named events on one connection:
     *      ```JavaScript
     *      const http = require('http');
     *      const sse = require('sse');
     *
     *      const server = new http.Server(0, {
     *          '/progress': sse.upgrade((sender) => {
     *              sender.send('start', { event: 'status', id: 's1' });
     *              sender.send('half', { event: 'progress' }); // no id, no retry
     *              sender.send('end', { event: 'status', id: 's2' });
     *              sender.close();
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const es = new sse.EventSource('http://127.0.0.1:' + port + '/progress');
     *      // status events print "start s1" and "end s2"
     *      es.addEventListener('status', (ev) => console.log('status', ev.data, ev.id));
     *      es.addEventListener('progress', (ev) => console.log('progress', ev.data)); // half
     *      es.onclose = () => server.stop();
     *      es.onerror = (ev) => console.log(ev.reason);
     *      ```
     *
     *      @param data the event data, sent as one or more data lines
     *      @param options event options, {} by default
     *      @return the number of bytes sent
     *
     */
    sendSync(data: string, options?: FIBJS.GeneralObject): number;

    /**
     * @description Sends one event to the client; available on a server-side sender only
     *
     *      The object must be in SENDER state, which is how `sse.upgrade` hands it
     *      to the accept callback; calling send() on a client EventSource throws.
     *
     *      options contains additional options for the event, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "event": "message", // event name, "message" by default
     *          "id": "", // event id; the id line is written only when provided
     *          "retry": 0 // suggested reconnect delay in ms; written only when provided
     *      })
     *      ```
     *      The payload is written as UTF-8: every line of data becomes its own
     *      `data:` line and the event is terminated by an empty line, so a
     *      multi-line string is delivered as one event whose data contains
     *      newlines. retry must not be negative. The returned value is the number
     *      of bytes written to the connection.
     *
     *      Example — sending several named events on one connection:
     *      ```JavaScript
     *      const http = require('http');
     *      const sse = require('sse');
     *
     *      const server = new http.Server(0, {
     *          '/progress': sse.upgrade((sender) => {
     *              sender.send('start', { event: 'status', id: 's1' });
     *              sender.send('half', { event: 'progress' }); // no id, no retry
     *              sender.send('end', { event: 'status', id: 's2' });
     *              sender.close();
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const es = new sse.EventSource('http://127.0.0.1:' + port + '/progress');
     *      // status events print "start s1" and "end s2"
     *      es.addEventListener('status', (ev) => console.log('status', ev.data, ev.id));
     *      es.addEventListener('progress', (ev) => console.log('progress', ev.data)); // half
     *      es.onclose = () => server.stop();
     *      es.onerror = (ev) => console.log(ev.reason);
     *      ```
     *
     *      @param data the event data, sent as one or more data lines
     *      @param options event options, {} by default
     *      @return the number of bytes sent
     *
     */
    sendAsync(data: string, options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description Queries the connection state: CONNECTING, OPEN, CLOSED or SENDER
     *
     *      CONNECTING (0) while the request is being made, OPEN (1) after a
     *      text/event-stream response has been accepted, CLOSED (2) after the
     *      stream ended or an HTTP error occurred. A connection failure leaves the
     *      state at CONNECTING because this implementation does not reconnect. The
     *      constants come from the sse module (`sse.OPEN` and so on), not from the
     *      instance.
     *
     */
    readonly readyState: number;

    /**
     * @description Queries the URL the client connected to
     *
     *      The value is the resolved request URL, including the query string, and
     *      is available immediately after construction.
     *
     */
    readonly url: string;

    /**
     * @description Reports whether the request carries credentials; always false in fibjs
     *
     *      fibjs does not implement the withCredentials behaviour of the browser
     *      EventSource: the property exists for API compatibility, is read only and
     *      is always false, so cookies are handled by the HttpClient that performs
     *      the request.
     *
     */
    readonly withCredentials: boolean;

    /**
     * @description Queries the HttpResponse of the connection
     *
     *      The response is null before the response headers are received and is
     *      available from the `open` event on; it exposes the status, the headers
     *      and the body stream of the text/event-stream response.
     *
     */
    readonly response: Class_HttpResponse;

    /**
     * @description Queries and binds the open event, equivalent to on("open", func)
     *
     *      The listener receives the event object of the connection; the response
     *      is available as `es.response` and readyState is OPEN. The event itself
     *      carries no argument data.
     *
     *      @param ev the event object of the connection
     *
     */
    on(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "open", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the open event, equivalent to on("open", func)
     *
     *      The listener receives the event object of the connection; the response
     *      is available as `es.response` and readyState is OPEN. The event itself
     *      carries no argument data.
     *
     *      @param ev the event object of the connection
     *
     */
    onopen: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the error event, equivalent to on("error", func)
     *
     *      The listener receives the fibjs event object: `ev.reason` is
     *      "Invalid status: ..." for a non-200 response, "Invalid Content-Type: ..."
     *      for a different content type or "Connection error" for a network
     *      failure, and `ev.code` carries the HTTP status when one was rejected. A
     *      wrong content type or a connection failure has no code. After an HTTP
     *      error readyState is CLOSED; after a connection failure it stays
     *      CONNECTING.
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
     *      The listener receives the fibjs event object: `ev.reason` is
     *      "Invalid status: ..." for a non-200 response, "Invalid Content-Type: ..."
     *      for a different content type or "Connection error" for a network
     *      failure, and `ev.code` carries the HTTP status when one was rejected. A
     *      wrong content type or a connection failure has no code. After an HTTP
     *      error readyState is CLOSED; after a connection failure it stays
     *      CONNECTING.
     *
     *      @param ev the event object carrying the error code and reason
     *
     */
    onerror: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the message event, equivalent to on("message", func)
     *
     *      The listener receives the default event of the stream: `ev.data` is the
     *      payload with data lines joined by "\n", `ev.id` is present when the
     *      group carried a non-empty `id:` field and `ev.retry` is the parsed retry
     *      value or "". A group with an `event:` field does not fire this event;
     *      register the named type with addEventListener(name, func).
     *
     *      @param ev the event object carrying the message data, id and retry interval
     *
     */
    on(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the message event, equivalent to on("message", func)
     *
     *      The listener receives the default event of the stream: `ev.data` is the
     *      payload with data lines joined by "\n", `ev.id` is present when the
     *      group carried a non-empty `id:` field and `ev.retry` is the parsed retry
     *      value or "". A group with an `event:` field does not fire this event;
     *      register the named type with addEventListener(name, func).
     *
     *      @param ev the event object carrying the message data, id and retry interval
     *
     */
    onmessage: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the close event, equivalent to on("close", func)
     *
     *      The listener receives the fibjs event object; this event fires when the
     *      server ends the stream and readyState becomes CLOSED. Fatal errors are
     *      reported through the `error` event instead and do not raise `close`, and
     *      calling close() does not raise it either.
     *
     *      @param ev the event object of the closed connection
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
     *      The listener receives the fibjs event object; this event fires when the
     *      server ends the stream and readyState becomes CLOSED. Fatal errors are
     *      reported through the `error` event instead and do not raise `close`, and
     *      calling close() does not raise it either.
     *
     *      @param ev the event object of the closed connection
     *
     */
    onclose: ((ev: FIBJS.GeneralObject)=>void) | null;

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


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/**
 * The promise variant of the EventSource class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_EventSourcePromise extends Class_EventEmitter {
    /**
     * @description Creates a client and starts reading a text/event-stream response
     *
     *      The constructor returns with readyState CONNECTING and the request runs
     *      in the background; `open` fires after a response with Content-Type
     *      "text/event-stream" is received. `error` fires on an HTTP error, a wrong
     *      content type or a connection failure, and `close` fires when the server
     *      ends the stream.
     *
     *      options contains additional options for the request, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "method": "GET", // request method, inferred as POST when body/json/pack is used
     *          "headers": {}, // extra request headers
     *          "body": null, // request body for methods that carry one
     *          "json": null, // JSON body, implies POST and sets Content-Type
     *          "pack": null, // msgpack body, implies POST and sets Content-Type
     *          "query": {}, // query string parameters
     *          "keepAlive": true, // the connection is kept alive until close()
     *          "timeout": 0, // request timeout in milliseconds
     *          "httpClient": null // HttpClient used for the request, the global one by default
     *      })
     *      ```
     *      The URL and the remaining options are resolved the same way `http.request`
     *      resolves them.
     *
     *      @param url the server address
     *      @param options request options, {} by default
     *
     */
    constructor(url: string, options?: FIBJS.GeneralObject);

    /**
     * @description Closes the connection and stops reading events
     *
     *      After close() the readyState is CLOSED and no further events are read;
     *      the HTTP connection to the server is released. On a server-side sender,
     *      close() terminates the chunked stream so that the client sees the end of
     *      the response and raises its `close` event. Closing an already closed
     *      object is harmless, and close() never raises `error`.
     *
     */
    close(): Promise<void>;

    /**
     * @description Closes the connection and stops reading events
     *
     *      After close() the readyState is CLOSED and no further events are read;
     *      the HTTP connection to the server is released. On a server-side sender,
     *      close() terminates the chunked stream so that the client sees the end of
     *      the response and raises its `close` event. Closing an already closed
     *      object is harmless, and close() never raises `error`.
     *
     */
    closeSync(): void;

    /**
     * @description Closes the connection and stops reading events
     *
     *      After close() the readyState is CLOSED and no further events are read;
     *      the HTTP connection to the server is released. On a server-side sender,
     *      close() terminates the chunked stream so that the client sees the end of
     *      the response and raises its `close` event. Closing an already closed
     *      object is harmless, and close() never raises `error`.
     *
     */
    closeAsync(): Promise<void>;

    /**
     * @description Sends one event to the client; available on a server-side sender only
     *
     *      The object must be in SENDER state, which is how `sse.upgrade` hands it
     *      to the accept callback; calling send() on a client EventSource throws.
     *
     *      options contains additional options for the event, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "event": "message", // event name, "message" by default
     *          "id": "", // event id; the id line is written only when provided
     *          "retry": 0 // suggested reconnect delay in ms; written only when provided
     *      })
     *      ```
     *      The payload is written as UTF-8: every line of data becomes its own
     *      `data:` line and the event is terminated by an empty line, so a
     *      multi-line string is delivered as one event whose data contains
     *      newlines. retry must not be negative. The returned value is the number
     *      of bytes written to the connection.
     *
     *      Example — sending several named events on one connection:
     *      ```JavaScript
     *      const http = require('http');
     *      const sse = require('sse');
     *
     *      const server = new http.Server(0, {
     *          '/progress': sse.upgrade((sender) => {
     *              sender.send('start', { event: 'status', id: 's1' });
     *              sender.send('half', { event: 'progress' }); // no id, no retry
     *              sender.send('end', { event: 'status', id: 's2' });
     *              sender.close();
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const es = new sse.EventSource('http://127.0.0.1:' + port + '/progress');
     *      // status events print "start s1" and "end s2"
     *      es.addEventListener('status', (ev) => console.log('status', ev.data, ev.id));
     *      es.addEventListener('progress', (ev) => console.log('progress', ev.data)); // half
     *      es.onclose = () => server.stop();
     *      es.onerror = (ev) => console.log(ev.reason);
     *      ```
     *
     *      @param data the event data, sent as one or more data lines
     *      @param options event options, {} by default
     *      @return the number of bytes sent
     *
     */
    send(data: string, options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description Sends one event to the client; available on a server-side sender only
     *
     *      The object must be in SENDER state, which is how `sse.upgrade` hands it
     *      to the accept callback; calling send() on a client EventSource throws.
     *
     *      options contains additional options for the event, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "event": "message", // event name, "message" by default
     *          "id": "", // event id; the id line is written only when provided
     *          "retry": 0 // suggested reconnect delay in ms; written only when provided
     *      })
     *      ```
     *      The payload is written as UTF-8: every line of data becomes its own
     *      `data:` line and the event is terminated by an empty line, so a
     *      multi-line string is delivered as one event whose data contains
     *      newlines. retry must not be negative. The returned value is the number
     *      of bytes written to the connection.
     *
     *      Example — sending several named events on one connection:
     *      ```JavaScript
     *      const http = require('http');
     *      const sse = require('sse');
     *
     *      const server = new http.Server(0, {
     *          '/progress': sse.upgrade((sender) => {
     *              sender.send('start', { event: 'status', id: 's1' });
     *              sender.send('half', { event: 'progress' }); // no id, no retry
     *              sender.send('end', { event: 'status', id: 's2' });
     *              sender.close();
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const es = new sse.EventSource('http://127.0.0.1:' + port + '/progress');
     *      // status events print "start s1" and "end s2"
     *      es.addEventListener('status', (ev) => console.log('status', ev.data, ev.id));
     *      es.addEventListener('progress', (ev) => console.log('progress', ev.data)); // half
     *      es.onclose = () => server.stop();
     *      es.onerror = (ev) => console.log(ev.reason);
     *      ```
     *
     *      @param data the event data, sent as one or more data lines
     *      @param options event options, {} by default
     *      @return the number of bytes sent
     *
     */
    sendSync(data: string, options?: FIBJS.GeneralObject): number;

    /**
     * @description Sends one event to the client; available on a server-side sender only
     *
     *      The object must be in SENDER state, which is how `sse.upgrade` hands it
     *      to the accept callback; calling send() on a client EventSource throws.
     *
     *      options contains additional options for the event, the supported contents are as follows:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "event": "message", // event name, "message" by default
     *          "id": "", // event id; the id line is written only when provided
     *          "retry": 0 // suggested reconnect delay in ms; written only when provided
     *      })
     *      ```
     *      The payload is written as UTF-8: every line of data becomes its own
     *      `data:` line and the event is terminated by an empty line, so a
     *      multi-line string is delivered as one event whose data contains
     *      newlines. retry must not be negative. The returned value is the number
     *      of bytes written to the connection.
     *
     *      Example — sending several named events on one connection:
     *      ```JavaScript
     *      const http = require('http');
     *      const sse = require('sse');
     *
     *      const server = new http.Server(0, {
     *          '/progress': sse.upgrade((sender) => {
     *              sender.send('start', { event: 'status', id: 's1' });
     *              sender.send('half', { event: 'progress' }); // no id, no retry
     *              sender.send('end', { event: 'status', id: 's2' });
     *              sender.close();
     *          })
     *      });
     *      server.start();
     *      const port = server.socket.localPort;
     *
     *      const es = new sse.EventSource('http://127.0.0.1:' + port + '/progress');
     *      // status events print "start s1" and "end s2"
     *      es.addEventListener('status', (ev) => console.log('status', ev.data, ev.id));
     *      es.addEventListener('progress', (ev) => console.log('progress', ev.data)); // half
     *      es.onclose = () => server.stop();
     *      es.onerror = (ev) => console.log(ev.reason);
     *      ```
     *
     *      @param data the event data, sent as one or more data lines
     *      @param options event options, {} by default
     *      @return the number of bytes sent
     *
     */
    sendAsync(data: string, options?: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description Queries the connection state: CONNECTING, OPEN, CLOSED or SENDER
     *
     *      CONNECTING (0) while the request is being made, OPEN (1) after a
     *      text/event-stream response has been accepted, CLOSED (2) after the
     *      stream ended or an HTTP error occurred. A connection failure leaves the
     *      state at CONNECTING because this implementation does not reconnect. The
     *      constants come from the sse module (`sse.OPEN` and so on), not from the
     *      instance.
     *
     */
    readonly readyState: number;

    /**
     * @description Queries the URL the client connected to
     *
     *      The value is the resolved request URL, including the query string, and
     *      is available immediately after construction.
     *
     */
    readonly url: string;

    /**
     * @description Reports whether the request carries credentials; always false in fibjs
     *
     *      fibjs does not implement the withCredentials behaviour of the browser
     *      EventSource: the property exists for API compatibility, is read only and
     *      is always false, so cookies are handled by the HttpClient that performs
     *      the request.
     *
     */
    readonly withCredentials: boolean;

    /**
     * @description Queries the HttpResponse of the connection
     *
     *      The response is null before the response headers are received and is
     *      available from the `open` event on; it exposes the status, the headers
     *      and the body stream of the text/event-stream response.
     *
     */
    readonly response: Class_HttpResponsePromise;

    /**
     * @description Queries and binds the open event, equivalent to on("open", func)
     *
     *      The listener receives the event object of the connection; the response
     *      is available as `es.response` and readyState is OPEN. The event itself
     *      carries no argument data.
     *
     *      @param ev the event object of the connection
     *
     */
    onopen: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the error event, equivalent to on("error", func)
     *
     *      The listener receives the fibjs event object: `ev.reason` is
     *      "Invalid status: ..." for a non-200 response, "Invalid Content-Type: ..."
     *      for a different content type or "Connection error" for a network
     *      failure, and `ev.code` carries the HTTP status when one was rejected. A
     *      wrong content type or a connection failure has no code. After an HTTP
     *      error readyState is CLOSED; after a connection failure it stays
     *      CONNECTING.
     *
     *      @param ev the event object carrying the error code and reason
     *
     */
    onerror: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the message event, equivalent to on("message", func)
     *
     *      The listener receives the default event of the stream: `ev.data` is the
     *      payload with data lines joined by "\n", `ev.id` is present when the
     *      group carried a non-empty `id:` field and `ev.retry` is the parsed retry
     *      value or "". A group with an `event:` field does not fire this event;
     *      register the named type with addEventListener(name, func).
     *
     *      @param ev the event object carrying the message data, id and retry interval
     *
     */
    onmessage: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the close event, equivalent to on("close", func)
     *
     *      The listener receives the fibjs event object; this event fires when the
     *      server ends the stream and readyState becomes CLOSED. Fatal errors are
     *      reported through the `error` event instead and do not raise `close`, and
     *      calling close() does not raise it either.
     *
     *      @param ev the event object of the closed connection
     *
     */
    onclose: ((ev: FIBJS.GeneralObject)=>void) | null;

}


declare namespace Class_EventSource {
    const promises: FIBJS.GeneralObject;
}
