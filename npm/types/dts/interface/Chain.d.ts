/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Message handler chain object
 *
 *  The Chain object is a message handler chain object in fibjs, used to link a series of message handlers and process them in a chain in the specified order. A Chain object is created as follows:
 *
 * ```
 * var chain = new mq.Chain([
 *     func1, func2
 * ]);
 * ```
 *
 * In this creation method, func1 and func2 are both message handler function objects. The Chain object links these handlers together in order to form a handler chain. When processing each message, a handler can customize the processing of the message and then pass the message to the next handler. In this way, a message can be processed step by step to its final state.
 *
 * The invoke() function of the Chain object processes a message or object. When this function is called, the Chain object passes the message or object to each handler in turn and processes them in the order of the handlers until all handlers have finished. During this process, each handler can customize the processing of the message or object, and may also choose to pass the message or object to the next handler for processing.
 *
 * In practice, the Chain object can be applied to various scenarios; for example, in a web framework, request messages can be passed to each handler in turn for processing, and in a message queue, a batch of messages can be passed to each handler in turn for processing. The Chain object is very flexible to use and can be customized according to actual needs, with high extensibility and reusability.
 *
 */
declare class Class_Chain extends Class_Handler {
    /**
     * @description Constructs a message handler chain object
     *      @param hdlrs handler array; each element is converted like a single handler (a Handler object, an array of handlers, a handler function called with the same message the chain receives, a routing map object, or a path/address string)
     *
     */
    constructor(hdlrs: (Class_Handler | Class_HandlerPromise)[]);

    /**
     * @description Adds a handler array
     *      @param hdlrs handler array; each element is converted like a single handler (a Handler object, an array of handlers, a handler function called with the same message the chain receives, a routing map object, or a path/address string)
     *
     */
    append(hdlrs: (Class_Handler | Class_HandlerPromise)[]): void;

    /**
     * @description Adds a handler
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a nested Chain and invoked in order;
     *      - a handler function `(req, ...params) => any`, called with the same message the chain receives;
     *      - a routing map object, whose values are handlers in these same forms;
     *      - a path/address string, converted through the Handler constructor.
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
     *      @param hdlrs handler array; each element is converted like a single handler (a Handler object, an array of handlers, a handler function called with the same message the chain receives, a routing map object, or a path/address string)
     *
     */
    constructor(hdlrs: (Class_Handler | Class_HandlerPromise)[]);

    /**
     * @description Adds a handler array
     *      @param hdlrs handler array; each element is converted like a single handler (a Handler object, an array of handlers, a handler function called with the same message the chain receives, a routing map object, or a path/address string)
     *
     */
    append(hdlrs: (Class_Handler | Class_HandlerPromise)[]): void;

    /**
     * @description Adds a handler
     *
     *      hdlr may be given in any of these forms:
     *      - a Handler object, invoked as it is;
     *      - an array of handlers, wrapped in a nested Chain and invoked in order;
     *      - a handler function `(req, ...params) => any`, called with the same message the chain receives;
     *      - a routing map object, whose values are handlers in these same forms;
     *      - a path/address string, converted through the Handler constructor.
     *      @param hdlr the handler appended to the chain
     *
     */
    append(hdlr: Class_Handler | Class_HandlerPromise | (Class_Handler | Class_HandlerPromise)[] | ((req: Class_object, ...params: any[])=>any) | FIBJS.GeneralObject | string): void;

}


declare namespace Class_Chain {
    const promises: FIBJS.GeneralObject;
}
