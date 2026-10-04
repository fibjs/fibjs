/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * @description Message handler routing object
 *
 *  The routing object is the core object of http message handling. According to the routing settings, the server matches url and method and forwards the http message to the corresponding handler, so as to complete different transactions.
 *
 *  A simple routing can be specified directly as a JSON object, for example:
 *  ```JavaScript
 *  var http = require('http');
 *
 *  var svr = new http.Server(8080, {
 *      '/': r => r.response.write('home'),
 *      '/help': r => r.response.write('help')
 *  });
 *
 *  svr.start();
 *  ```
 *  If more complex routing customization is needed, you can create a Routing object yourself and implement the routing strategy as needed:
 *  ```JavaScript
 *  var http = require('http');
 *  var mq = require('mq');
 *
 *  var app = new mq.Routing();
 *
 *  app.get('/', r => r.response.write('home'));
 *  app.get('/help', r => r.response.write('help'));
 *
 *  app.post('/help', r => r.response.write('post a help.'));
 *
 *  app.get('/home/:user', (r, user) => r.response.write('hello ' + user));
 *
 *  app.get('/user/:id(\\d+)', (r, id) => r.response.write('get ' + id));
 *
 *  app.get('/actions', {
 *      '/run': r => r.response.write('running'),
 *      '/sleep': r => r.response.write('sleeping'),
 *      '(.*)': r => r.response.write('........')
 *  });
 *
 *  var svr = new http.Server(8080, app);
 *  svr.start();
 *  ```
 *  The routing object matches messages according to the configured rules and passes the message to the first handler that matches the rules. Routing rules added later are matched with higher priority. Creation method:
 *  ```JavaScript
 *  var routing = new mq.Routing({
 *    "^/func1(/.*)$": func1,
 *    "^/func2(/.*)$": func2
 *  });
 *  ```
 *  For items matched by a regular expression, the value property of the message is modified and sub-items are stored in the params property of the message. For example:
 *  ```JavaScript
 *  var routing = new mq.Routing({
 *    "^/func1(/([0-9]+)/([0-9]+)\.html)$": func1,
 *  });
 *  ```
 *  After matching the message "/func1/123/456.html", value == "/123/456.html", params == ["123", "456"];
 *
 *  If the match result has no sub-items, value is empty and params is empty. For example:
 *  ```JavaScript
 *  var routing = new mq.Routing({
 *    "^/func1/[0-9]+/[0-9]+\.html$": func1,
 *  });
 *  ```
 *  After matching the message "/func1/123/456.html", value == "", params == [];
 *
 *  If the match result has multiple first-level sub-items, value is empty and params contains the first-level sub-items. For example:
 *  ```JavaScript
 *  var routing = new mq.Routing({
 *    "^/func1/([0-9]+)/([0-9]+)\.html$": func1,
 *  });
 *  ```
 *  After matching the message "/func1/123/456.html", value == "", params == ["123", "456"];
 *
 *  If the match result has only one sub-item and no lower-level sub-items, both value and params are that sub-item. For example:
 *  ```JavaScript
 *  var routing = new mq.Routing({
 *    "^/func1/([0-9]+)/[0-9]+\.html$": func1,
 *  });
 *  ```
 *  After matching the message "/func1/123/456.html", value == "123", params == ["123"];
 *
 */
declare class Class_Routing extends Class_Handler {
    /**
     * @description Creates a message handler routing object
     *    @param map initialization routing parameters
     *
     */
    constructor(map?: FIBJS.GeneralObject);

    /**
     * @description Creates a message handler routing object
     *    @param method the http request method to accept, "*" accepts all methods
     *    @param map initialization routing parameters
     *
     */
    constructor(method: string, map: FIBJS.GeneralObject);

    /**
     * @description Adds rules from an existing routing object; the source routing is cleared after adding
     *    @param route an initialized routing object
     *    @return returns the routing object itself
     *
     */
    append(route: Class_Routing | Class_RoutingPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    append(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    append(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a routing rule
     *    @param method the http request method to accept; "*" accepts all methods, "host" matches virtual host names
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    append(method: string, pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules for http host names
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    host(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts http host names
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    host(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules that accept all http methods
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    all(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts all http methods
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    all(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of GET method routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    get(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http GET method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    get(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules that accept the http POST method
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    post(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http POST method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    post(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules that accept the http DELETE method
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    del(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http DELETE method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    del(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of PUT method routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    put(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http PUT method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    put(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of PATCH method routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    patch(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http PATCH method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    patch(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of FIND method routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    find(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http FIND method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    find(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Handler.d.ts" />
/**
 * The promise variant of the Routing class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_RoutingPromise extends Class_HandlerPromise {
    /**
     * @description Creates a message handler routing object
     *    @param map initialization routing parameters
     *
     */
    constructor(map?: FIBJS.GeneralObject);

    /**
     * @description Creates a message handler routing object
     *    @param method the http request method to accept, "*" accepts all methods
     *    @param map initialization routing parameters
     *
     */
    constructor(method: string, map: FIBJS.GeneralObject);

    /**
     * @description Adds rules from an existing routing object; the source routing is cleared after adding
     *    @param route an initialized routing object
     *    @return returns the routing object itself
     *
     */
    append(route: Class_Routing | Class_RoutingPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    append(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    append(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a routing rule
     *    @param method the http request method to accept; "*" accepts all methods, "host" matches virtual host names
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    append(method: string, pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules for http host names
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    host(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts http host names
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    host(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules that accept all http methods
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    all(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts all http methods
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    all(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of GET method routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    get(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http GET method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    get(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules that accept the http POST method
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    post(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http POST method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    post(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of routing rules that accept the http DELETE method
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    del(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http DELETE method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    del(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of PUT method routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    put(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http PUT method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    put(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of PATCH method routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    patch(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http PATCH method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    patch(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

    /**
     * @description Adds a group of FIND method routing rules
     *    @param map routing parameters
     *    @return returns the routing object itself
     *
     */
    find(map: FIBJS.GeneralObject): Class_Routing;

    /**
     * @description Adds a routing rule that accepts the http FIND method
     *    @param pattern message match pattern
     *    @param hdlr built-in message handler, handler function, chain processing array, or routing object; see mq.Handler
     *    @return returns the routing object itself
     *
     */
    find(pattern: string, hdlr: Class_Handler | Class_HandlerPromise): Class_Routing;

}


declare namespace Class_Routing {
    const promises: FIBJS.GeneralObject;
}
