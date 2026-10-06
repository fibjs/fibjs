/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/// <reference path="../interface/HttpHandler.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Chain.d.ts" />
/// <reference path="../interface/Routing.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The message queue module: the handler pipeline behind the network servers
 *
 *  mq is the message kernel of fibjs. A message (a Message, an http.Request or
 *  any other value) is processed by handlers — functions, Chains, Routings and
 *  the built-in handlers — and the module provides both the base types and the
 *  driver that runs them. The network servers accept the same handlers and use
 *  this kernel internally, so the concepts here apply to `http.Server`,
 *  `net.TcpServer`, websocket handlers and so on.
 *
 *  Main capabilities:
 *  - **Handler types**: `Handler` (the contract and its construction forms),
 *    `Chain` (run handlers in order), `Routing` (match a value and dispatch),
 *    `HttpHandler` (turn a stream into HTTP request/response handling) and
 *    `Message` (the generic message object);
 *  - **Driving the pipeline**: `invoke` runs a handler to completion,
 *    `nullHandler` returns an empty handler that ends the pipeline immediately.
 *
 *  Concepts:
 *  - **One stage or the whole pipeline**: a handler's invoke processes one stage
 *    and returns the next handler; mq.invoke is the loop that keeps invoking the
 *    returned handler until null comes back. Most code only needs mq.invoke.
 *  - **Handler forms**: wherever a handler is expected the same set of forms is
 *    accepted — a Handler object, an array of handlers (a Chain), a function, a
 *    routing map object (a Routing) and a path/address string (a file handler or
 *    an `http(s)://` repeater). The Handler constructor performs the conversion,
 *    so building the concrete class directly and passing the plain form are
 *    equivalent.
 *  - **Returning a handler**: a JavaScript handler function may return the next
 *    handler, a handling function, an array or a routing map; the runtime
 *    continues with it. Returning nothing ends the function's stage, and any
 *    other returned value is an error.
 *  - **Call forms**: like every fibjs asynchronous function, invoke works
 *    synchronously without a callback, asynchronously with a trailing callback
 *    and as a promise (`mq.invokeSync`, `mq.invokeAsync`, `mq.promises.invoke`).
 *
 *  Import:
 *  ```JavaScript
 *  const mq = require('mq');
 *  ```
 *
 *  Example 1 — a function handler returning the handler of the next stage:
 *  ```JavaScript
 *  const mq = require('mq');
 *
 *  const msg = new mq.Message();
 *  msg.value = 'start';
 *
 *  mq.invoke((v) => {
 *      console.log('stage 1: ' + v.value);
 *      return (v) => console.log('stage 2: ' + v.value);
 *  }, msg);
 *  ```
 *
 *  Example 2 — the accepted handler forms and a routing dispatch:
 *  ```JavaScript
 *  const mq = require('mq');
 *
 *  console.log(new mq.Handler(() => { }).isRouting()); // false
 *  console.log(new mq.Handler([() => { }]).isRouting()); // false
 *  console.log(new mq.Handler({ '/a': () => { } }).isRouting()); // true
 *
 *  const msg = new mq.Message();
 *  msg.value = '/a';
 *  mq.invoke(new mq.Handler({ '/a': (req) => console.log('routed ' + req.value) }), msg);
 *  ```
 *
 *  Example 3 — an empty handler ends the pipeline:
 *  ```JavaScript
 *  const mq = require('mq');
 *
 *  const empty = mq.nullHandler();
 *  console.log(empty.isRouting()); // false
 *
 *  mq.invoke(empty, new mq.Message());
 *  console.log('done');
 *  ```
 *
 *  Notes:
 *  - The module statics Message, HttpHandler, Chain and Routing expose the
 *    classes of the kernel; Handler additionally converts the value it is given
 *    into the class of the matching form.
 *  - The `invokeSync`/`invokeAsync`/`promises` variants are generated from the
 *    `invoke` declaration and are reached through the module object, not through
 *    separate members of this manual.
 *
 */
declare module 'mq' {
    /**
     * @description Creates a message object, see Message
     *
     *      The static exposes the Message class, so `new mq.Message()` builds the
     *      generic message used by routing and chains: the value carries the matched
     *      address, params carries the captured groups, and the body carries the
     *      payload.
     *
     *      Example — build a message and feed it to the pipeline:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const msg = new mq.Message();
     *      msg.value = '/hello';
     *      console.log(msg.value); // /hello
     *      console.log(msg.params.length); // 0
     *      ```
     *
     */
    const Message: typeof Class_Message;

    /**
     * @description Creates an http protocol handler object, see HttpHandler
     *
     *      The static exposes the HttpHandler class, the same object as
     *      `http.Handler`; it wraps a handler and runs the HTTP server side of a
     *      stream (request parsing, keep-alive, response sending and the response
     *      options).
     *
     *      Example — build one and read its default limits:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const hdlr = new mq.HttpHandler((req, res) => res.write('ok'));
     *      hdlr.maxBodySize = 8;
     *      console.log(hdlr.maxBodySize); // 8
     *      console.log(hdlr.isRouting()); // false
     *      ```
     *
     */
    const HttpHandler: typeof Class_HttpHandler;

    /**
     * @description Creates a message handler object from any accepted form
     *
     *      The constructor converts its argument and returns the concrete handler of
     *      the matching form:
     *      - Function: a JavaScript handler, called with the message;
     *      - array: a Chain, run in order;
     *      - routing map object: a Routing (see Routing);
     *      - path/address string: a file handler (a directory) or an
     *        HttpRepeater (an `http(s)://` address).
     *      The declared union also accepts a Handler object in parameter positions,
     *      but constructing from one is not a supported conversion; build the
     *      concrete class directly instead.
     *
     *      Example — convert a handling function and run it:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const hdlr = new mq.Handler((v) => console.log('called with ' + v.value));
     *
     *      const msg = new mq.Message();
     *      msg.value = 'data';
     *      mq.invoke(hdlr, msg);
     *      ```
     *
     */
    const Handler: typeof Class_Handler;

    /**
     * @description Creates a message handler chain processing object, see Chain
     *
     *      The static exposes the Chain class: `new mq.Chain([...])` links the
     *      handlers and runs them in order, each of them receiving the same message.
     *
     *      Example — append a handler and run the chain:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const chain = new mq.Chain([(v) => console.log('first')]);
     *      chain.append((v) => console.log('second'));
     *
     *      mq.invoke(chain, new mq.Message());
     *      ```
     *
     */
    const Chain: typeof Class_Chain;

    /**
     * @description Creates a message handler routing object, see Routing
     *
     *      The static exposes the Routing class: `new mq.Routing(map)` turns a map of
     *      patterns into a router, `new mq.Routing()` builds an empty one and the
     *      method helpers (get/post/...) add rules one by one.
     *
     *      Example — a map with one rule:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const msg = new mq.Message();
     *      msg.value = '/ping';
     *
     *      mq.invoke(new mq.Routing({ '/ping': (req) => console.log('pong') }), msg);
     *      ```
     *
     */
    const Routing: typeof Class_Routing;

    /**
     * @description Creates an empty handler object; this handler does nothing and returns directly
     *
     *      The returned handler has no routing behavior and no side effects: it is
     *      useful as a placeholder in a chain, as the fallback of a routing or as the
     *      value to return when a handler must explicitly stop the pipeline.
     *
     *      Example — an empty handler ends mq.invoke immediately:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const empty = mq.nullHandler();
     *      console.log(empty.isRouting()); // false
     *      ```
     *      @return returns the empty handling function
     *
     */
    function nullHandler(): Class_Handler;

    /**
     * @description Processes a message or object with the given handler
     *
     *      Unlike invoke of a handler, this method repeatedly calls the returned
     *      handler of each stage until a handler returns null, so a single call runs
     *      the whole pipeline. The handler may be given in any of the usual forms:
     *      - a built-in Handler object, used as it is;
     *      - an array of handlers, equivalent to `new mq.Chain(hdlr)`, see Chain;
     *      - a handling function `(v, ...params) => any`, called with the message;
     *      - a routing map object, whose values are handlers in these same forms,
     *        equivalent to `new mq.Routing(hdlr)`, see Routing;
     *      - a path/address string, converted through the Handler constructor.
     *
     *      The method is asynchronous: it blocks the current fiber when called
     *      without a callback, or completes through the trailing callback or the
     *      promise form.
     *
     *      Example — the trailing callback form:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const msg = new mq.Message();
     *      msg.value = 'callback';
     *
     *      mq.invoke((v) => console.log('invoked ' + v.value), msg, (err) => {
     *          console.log('callback: ' + (err ? err.message : 'ok'));
     *      });
     *      ```
     *      @param hdlr the handler to run
     *      @param v specifies the message or object to process
     *
     */
    function invoke(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((v: Class_object, ...params: any[])=>any) | FIBJS.GeneralObject | string, v: Class_object): void;

    function invoke(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((v: Class_object, ...params: any[])=>any) | FIBJS.GeneralObject | string, v: Class_object, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Processes a message or object with the given handler
     *
     *      Unlike invoke of a handler, this method repeatedly calls the returned
     *      handler of each stage until a handler returns null, so a single call runs
     *      the whole pipeline. The handler may be given in any of the usual forms:
     *      - a built-in Handler object, used as it is;
     *      - an array of handlers, equivalent to `new mq.Chain(hdlr)`, see Chain;
     *      - a handling function `(v, ...params) => any`, called with the message;
     *      - a routing map object, whose values are handlers in these same forms,
     *        equivalent to `new mq.Routing(hdlr)`, see Routing;
     *      - a path/address string, converted through the Handler constructor.
     *
     *      The method is asynchronous: it blocks the current fiber when called
     *      without a callback, or completes through the trailing callback or the
     *      promise form.
     *
     *      Example — the trailing callback form:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const msg = new mq.Message();
     *      msg.value = 'callback';
     *
     *      mq.invoke((v) => console.log('invoked ' + v.value), msg, (err) => {
     *          console.log('callback: ' + (err ? err.message : 'ok'));
     *      });
     *      ```
     *      @param hdlr the handler to run
     *      @param v specifies the message or object to process
     *
     */
    function invokeSync(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((v: Class_object, ...params: any[])=>any) | FIBJS.GeneralObject | string, v: Class_object): void;

    /**
     * @description Processes a message or object with the given handler
     *
     *      Unlike invoke of a handler, this method repeatedly calls the returned
     *      handler of each stage until a handler returns null, so a single call runs
     *      the whole pipeline. The handler may be given in any of the usual forms:
     *      - a built-in Handler object, used as it is;
     *      - an array of handlers, equivalent to `new mq.Chain(hdlr)`, see Chain;
     *      - a handling function `(v, ...params) => any`, called with the message;
     *      - a routing map object, whose values are handlers in these same forms,
     *        equivalent to `new mq.Routing(hdlr)`, see Routing;
     *      - a path/address string, converted through the Handler constructor.
     *
     *      The method is asynchronous: it blocks the current fiber when called
     *      without a callback, or completes through the trailing callback or the
     *      promise form.
     *
     *      Example — the trailing callback form:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const msg = new mq.Message();
     *      msg.value = 'callback';
     *
     *      mq.invoke((v) => console.log('invoked ' + v.value), msg, (err) => {
     *          console.log('callback: ' + (err ? err.message : 'ok'));
     *      });
     *      ```
     *      @param hdlr the handler to run
     *      @param v specifies the message or object to process
     *
     */
    function invokeAsync(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((v: Class_object, ...params: any[])=>any) | FIBJS.GeneralObject | string, v: Class_object): Promise<void>;

}

