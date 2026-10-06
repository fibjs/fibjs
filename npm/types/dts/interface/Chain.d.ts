/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description A message handler chain that runs a series of handlers in order
 *
 *  Chain links several handlers into one handler: the value is passed to each
 *  element in turn, each element may change it, and the chain ends when the queue
 *  is exhausted or the value is ended. It is the object behind the array form of
 *  every handler parameter — `new mq.Handler([a, b])`, the handler array of an
 *  http server and the array returned by a handling function all become a Chain —
 *  so the class is usually reached through that conversion rather than
 *  constructed by name.
 *
 *  Concepts:
 *  - **Order**: the elements run in the order they were given; append adds to the
 *    end. A handler returning nothing finishes its own stage and the chain moves
 *    to the next element, so a chain of plain functions behaves like a pipeline.
 *  - **Ending the chain**: calling end() on a Message or response.end() on an
 *    http.Request marks the value as finished; the chain stops before the next
 *    element, which is the way a middle handler short-circuits a request.
 *  - **Returning a handler**: a handling function may return another handler,
 *    function, array (a nested Chain) or routing map (a nested Routing); the
 *    returned handler runs before the next element of the queue.
 *  - **Fibers**: plain functions run in the fiber that invoked the chain, while a
 *    handler object (another Chain, a Routing, a repeater) runs in its own
 *    fiber, which matters when the elements share mutable state.
 *
 *  Obtained from:
 *  - `new mq.Chain([...])` — the constructor below;
 *  - `new mq.Handler([...])` — the array form of the Handler constructor returns
 *    a Chain (see Handler);
 *  - any array passed where a handler is expected (http.Server, net.TcpServer,
 *    Routing.append, mq.invoke) is converted through the same path.
 *
 *  Example 1 — handlers run in order and may change the value:
 *  ```JavaScript
 *  const mq = require('mq');
 *
 *  const order = [];
 *  const chain = new mq.Chain([
 *      (v) => {
 *          order.push('first');
 *      },
 *      (v) => {
 *          order.push('second');
 *      },
 *      (v) => {
 *          order.push('third');
 *      }
 *  ]);
 *
 *  mq.invoke(chain, new mq.Message());
 *  console.log(order.join(' -> ')); // first -> second -> third
 *  ```
 *
 *  Example 2 — end() stops the chain:
 *  ```JavaScript
 *  const mq = require('mq');
 *  const http = require('http');
 *
 *  const seen = [];
 *  const chain = new mq.Chain([
 *      (v) => {
 *          seen.push('before');
 *      },
 *      (v) => {
 *          v.end();
 *      },
 *      (v) => {
 *          seen.push('after');
 *      }
 *  ]);
 *
 *  mq.invoke(chain, new mq.Message());
 *  console.log(seen.join(',')); // before
 *
 *  // the response form ends an http pipeline the same way
 *  const seen2 = [];
 *  const chain2 = new mq.Chain([
 *      (req) => {
 *          seen2.push('request');
 *      },
 *      (req) => {
 *          req.response.end();
 *      },
 *      (req) => {
 *          seen2.push('unreachable');
 *      }
 *  ]);
 *
 *  mq.invoke(chain2, new http.Request());
 *  console.log(seen2.join(',')); // request
 *  ```
 *
 */
declare class Class_Chain extends Class_Handler {
    /**
     * @description Constructs a message handler chain object
     *
     *      Every element is converted with the same rules as a single handler: a
     *      Handler object is used as it is, an array becomes a nested Chain, a
     *      function becomes a JavaScript handler called with the message the chain
     *      receives, a routing map object becomes a Routing, and a path/address
     *      string becomes a file handler or an http(s) repeater. The elements run in
     *      order; see the class description for the chain semantics.
     *
     *      Example — a chain mixing the accepted forms:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const chain = new mq.Chain([
     *          (v) => {
     *              console.log('function');
     *          },
     *          new mq.Handler((v) => {
     *              console.log('handler');
     *          }),
     *          { '/': (v) => console.log('nested map') }
     *      ]);
     *
     *      const msg = new mq.Message();
     *      msg.value = '/';
     *      mq.invoke(chain, msg);
     *      ```
     *
     *      @param hdlrs the handlers to link, each converted like a single handler
     *
     */
    constructor(hdlrs: (Class_Handler | Class_HandlerPromise)[]);

    /**
     * @description Adds a handler array to the end of the chain
     *
     *      The elements are converted like the constructor argument and appended in
     *      order, so they run after the handlers already in the chain. The member
     *      returns nothing; the chain is modified in place.
     *
     *      Example — extend a chain and run it:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const chain = new mq.Chain([() => console.log('one')]);
     *      chain.append([() => console.log('two'), () => console.log('three')]);
     *      chain.append(() => console.log('four'));
     *
     *      mq.invoke(chain, new mq.Message());
     *      ```
     *
     *      @param hdlrs the handlers to append, each converted like a single handler
     *
     */
    append(hdlrs: (Class_Handler | Class_HandlerPromise)[]): void;

    /**
     * @description Adds a handler to the end of the chain
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a nested Chain and invoked in order;
     *      - a handler function `(req, ...params) => any`, called with the same
     *        message the chain receives;
     *      - a routing map object, whose values are handlers in these same forms;
     *      - a path/address string, converted through the Handler constructor.
     *
     *      The handler is appended after the existing elements and runs when its turn
     *      comes; the member returns nothing.
     *
     *      @param hdlr the handler appended to the chain
     *
     */
    append(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_object, ...params: any[])=>any) | FIBJS.GeneralObject | string): void;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * The promise variant of the Chain class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_ChainPromise extends Class_HandlerPromise {
    /**
     * @description Constructs a message handler chain object
     *
     *      Every element is converted with the same rules as a single handler: a
     *      Handler object is used as it is, an array becomes a nested Chain, a
     *      function becomes a JavaScript handler called with the message the chain
     *      receives, a routing map object becomes a Routing, and a path/address
     *      string becomes a file handler or an http(s) repeater. The elements run in
     *      order; see the class description for the chain semantics.
     *
     *      Example — a chain mixing the accepted forms:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const chain = new mq.Chain([
     *          (v) => {
     *              console.log('function');
     *          },
     *          new mq.Handler((v) => {
     *              console.log('handler');
     *          }),
     *          { '/': (v) => console.log('nested map') }
     *      ]);
     *
     *      const msg = new mq.Message();
     *      msg.value = '/';
     *      mq.invoke(chain, msg);
     *      ```
     *
     *      @param hdlrs the handlers to link, each converted like a single handler
     *
     */
    constructor(hdlrs: (Class_Handler | Class_HandlerPromise)[]);

    /**
     * @description Adds a handler array to the end of the chain
     *
     *      The elements are converted like the constructor argument and appended in
     *      order, so they run after the handlers already in the chain. The member
     *      returns nothing; the chain is modified in place.
     *
     *      Example — extend a chain and run it:
     *      ```JavaScript
     *      const mq = require('mq');
     *
     *      const chain = new mq.Chain([() => console.log('one')]);
     *      chain.append([() => console.log('two'), () => console.log('three')]);
     *      chain.append(() => console.log('four'));
     *
     *      mq.invoke(chain, new mq.Message());
     *      ```
     *
     *      @param hdlrs the handlers to append, each converted like a single handler
     *
     */
    append(hdlrs: (Class_Handler | Class_HandlerPromise)[]): void;

    /**
     * @description Adds a handler to the end of the chain
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a nested Chain and invoked in order;
     *      - a handler function `(req, ...params) => any`, called with the same
     *        message the chain receives;
     *      - a routing map object, whose values are handlers in these same forms;
     *      - a path/address string, converted through the Handler constructor.
     *
     *      The handler is appended after the existing elements and runs when its turn
     *      comes; the member returns nothing.
     *
     *      @param hdlr the handler appended to the chain
     *
     */
    append(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_object, ...params: any[])=>any) | FIBJS.GeneralObject | string): void;

}


declare namespace Class_Chain {
    const promises: FIBJS.GeneralObject;
}
