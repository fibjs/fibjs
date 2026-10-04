/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Message.d.ts" />
/// <reference path="../interface/HttpHandler.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Chain.d.ts" />
/// <reference path="../interface/Routing.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description message queue module
 */
declare module 'mq' {
    /**
     * @description creates a message object, see Message
     */
    const Message: typeof Class_Message;

    /**
     * @description creates an http protocol handler object, see HttpHandler
     */
    const HttpHandler: typeof Class_HttpHandler;

    /**
     * @description creates a message handler object; passing a built-in handler returns it directly
     *
     *      hdlr accepts a built-in message handler, a handling function, a chain handling array, or a routing object:
     *      - Function: a javascript function, which will be used for processing
     *      - Handler: a built-in handler, which will be used for processing
     *      - Chain handling array, equivalent to returning new mq.Chain(hdlr), see Chain
     *      - Routing object, equivalent to returning new mq.Routing(hdlr), see Routing
     *
     *      The message handling function syntax is as follows:
     *      ```JavaScript
     *      function func(v){
     *      }
     *      ```
     *      The parameter v is the message being processed, and the returned result can be one of four kinds:
     *      - Function: a javascript function, which will be used for the next stage of processing
     *      - Handler: a built-in handler, which will be used for the next stage of processing
     *      - Chain handling array, equivalent to new mq.Chain(v), see Chain
     *      - Routing object, equivalent to new mq.Routing(v), see Routing
     *
     *      No return value or any other returned result ends the message processing.
     *      @param hdlr built-in message handler, handling function, chain handling array, routing object
     *      @return returns a handler wrapping the handling function
     *
     */
    const Handler: typeof Class_Handler;

    /**
     * @description creates a message handler chain processing object, see Chain
     */
    const Chain: typeof Class_Chain;

    /**
     * @description creates a message handler routing object, see Routing
     */
    const Routing: typeof Class_Routing;

    /**
     * @description creates an empty handler object; this handler does nothing and returns directly
     *      @return returns the empty handling function
     *
     */
    function nullHandler(): Class_Handler;

    /**
     * @description processes a message or object with the given handler
     *
     *      Unlike the invoke method of a handler, this method will repeatedly call the returned handler of each handler until a handler returns null.
     *      @param hdlr specifies the handler to use
     *      @param v specifies the message or object to process
     *
     */
    function invoke(hdlr: Class_Handler | Class_HandlerPromise, v: Class_object): void;

    function invoke(hdlr: Class_Handler | Class_HandlerPromise, v: Class_object, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description processes a message or object with the given handler
     *
     *      Unlike the invoke method of a handler, this method will repeatedly call the returned handler of each handler until a handler returns null.
     *      @param hdlr specifies the handler to use
     *      @param v specifies the message or object to process
     *
     */
    function invokeSync(hdlr: Class_Handler | Class_HandlerPromise, v: Class_object): void;

    /**
     * @description processes a message or object with the given handler
     *
     *      Unlike the invoke method of a handler, this method will repeatedly call the returned handler of each handler until a handler returns null.
     *      @param hdlr specifies the handler to use
     *      @param v specifies the message or object to process
     *
     */
    function invokeAsync(hdlr: Class_Handler | Class_HandlerPromise, v: Class_object): Promise<void>;

}

