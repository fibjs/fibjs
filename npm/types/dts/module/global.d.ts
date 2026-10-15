/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/URLSearchParams.d.ts" />
/// <reference path="../interface/UrlObject.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/// <reference path="../interface/File.d.ts" />
/// <reference path="../interface/Headers.d.ts" />
/// <reference path="../interface/FormData.d.ts" />
/// <reference path="../interface/HttpRequest.d.ts" />
/// <reference path="../interface/HttpResponse.d.ts" />
/// <reference path="../interface/TextDecoder.d.ts" />
/// <reference path="../interface/TextEncoder.d.ts" />
/// <reference path="../interface/AbortController.d.ts" />
/// <reference path="../interface/AbortSignal.d.ts" />
/// <reference path="../interface/DOMEvent.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/MessageEvent.d.ts" />
/// <reference path="../interface/MessagePort.d.ts" />
/// <reference path="../interface/MessageChannel.d.ts" />
/// <reference path="../interface/Worker.d.ts" />
/// <reference path="../interface/CryptoKey.d.ts" />
/// <reference path="../interface/DOMParser.d.ts" />
/// <reference path="../interface/CSSStyleDeclaration.d.ts" />
/// <reference path="../interface/DOMStringMap.d.ts" />
/// <reference path="../interface/XMLSerializer.d.ts" />
/// <reference path="../interface/XmlDocument.d.ts" />
/// <reference path="../interface/WebSocket.d.ts" />
/// <reference path="../module/console.d.ts" />
/// <reference path="../module/process.d.ts" />
/// <reference path="../module/performance.d.ts" />
/// <reference path="../interface/PerformanceObserver.d.ts" />
/// <reference path="../module/webcrypto.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/**
 * @description The global object, the base object that every script and module can access directly
 *
 *  The global object is available under two names: `globalThis`, the standard
 *  JavaScript name, and `global`, a read-only alias;
 *  both refer to the same object (`global === globalThis` is true).
 *
 *  Main capabilities:
 *
 *  - **Web standard classes**: `Buffer`, `URL`, `URLSearchParams`, `Blob`, `File`,
 *    `Headers`, `FormData`, `Request`, `Response`, `TextDecoder`, `TextEncoder`,
 *    `AbortController`, `AbortSignal`, `Event`, `EventTarget`, `MessageEvent`,
 *    `MessagePort`, `MessageChannel`, `Worker`, `WebSocket`, `CryptoKey`,
 *    `DOMParser`, `CSSStyleDeclaration`, `DOMStringMap`, `XMLSerializer` and
 *    `XMLDocument`;
 *  - **Core modules**: `console`, `process`, `performance`, `PerformanceObserver`
 *    and `crypto`;
 *  - **Module loading**: `require` loads modules and `run` runs scripts;
 *  - **Timers**: `setTimeout`, `clearTimeout`, `setInterval`, `clearInterval`,
 *    `setHrInterval`, `clearHrInterval`, `setImmediate` and `clearImmediate`;
 *  - **Helpers**: `btoa`, `atob`, `structuredClone`, `fetch` and `queueMicrotask`.
 *
 *  Concepts:
 *
 *  - **One global scope per sandbox**: a script runs in a sandbox whose global
 *    object forwards property reads and writes to the sandbox. `globalThis.x = 1`
 *    makes `x` visible as an implicit global in every script of the sandbox, while
 *    top-level `var` and `function` declarations stay in the module scope and do
 *    not become globals.
 *  - **Timers and process lifetime**: each pending timer keeps the process alive
 *    until it fires or is cleared, so a repeating timer that is never cleared
 *    prevents the process from exiting. Scheduling is fiber-based: the callback
 *    runs on its own fiber and the clear functions may be called from the callback
 *    itself or from any other fiber. `Timer#unref` drops the liveness hold without
 *    cancelling the timer. See the coroutine module for the fiber model.
 *  - **Task ordering**: after the current task, `process.nextTick` callbacks run
 *    first, then the V8 micro-tasks (promise jobs and `queueMicrotask`), then
 *    `setImmediate` callbacks, then timers.
 *  - **Error values**: `structuredClone` throws a `DOMException` (`DataCloneError`);
 *    `fetch` aborts with an `AbortError` and times out with a `TimeoutError`, both
 *    plain `Error` subclasses. `btoa` and `atob` throw plain `Error` objects.
 *  - **Global class notes**: `Request` and `Response` are the `HttpRequest` and
 *    `HttpResponse` classes; `EventTarget` is the
 *    `EventEmitter` class, so `addEventListener`/`removeEventListener` are aliases
 *    of `on`/`off` and `dispatchEvent` is not provided; `Worker` takes a script
 *    path; `setHrInterval` and `run` are extensions of the runtime.
 *
 *  Import:
 *  ```JavaScript
 *  // no import is needed, both names are already in scope
 *  console.log(globalThis === global); // true
 *  ```
 *
 *  Example 1 — timers with cleanup:
 *  ```JavaScript
 *  console.log('start');
 *
 *  // cancel a pending one-time timer
 *  const timeout = setTimeout((name) => console.log('late', name), 30, 'timer');
 *  setTimeout(() => {
 *      clearTimeout(timeout);
 *      console.log('cancelled');
 *  }, 5);
 *
 *  // a repeating timer must be cleared, or the process never exits
 *  let ticks = 0;
 *  const interval = setInterval(() => {
 *      ticks++;
 *      console.log('tick', ticks);
 *      if (ticks === 3) {
 *          clearInterval(interval);
 *      }
 *  }, 10);
 *  ```
 *
 *  Example 2 — encoding and structured clone helpers:
 *  ```JavaScript
 *  // btoa/atob convert the value to its string form and use the Latin1 range
 *  const encoded = btoa('fibjs');
 *  console.log(encoded, atob(encoded)); // ZmlianM fibjs
 *
 *  // structuredClone deep-copies values, including cycles, Map, Set and Date
 *  const original = { name: 'global', tags: new Map([['kind', 'runtime']]), when: new Date(0) };
 *  original.self = original;
 *  const copy = structuredClone(original);
 *  console.log(copy !== original, copy.self === copy, copy.tags.get('kind'), copy.when.getTime());
 *
 *  // the transfer list moves an ArrayBuffer instead of copying it
 *  const buffer = new ArrayBuffer(8);
 *  const moved = structuredClone({ buffer }, { transfer: [buffer] });
 *  console.log(buffer.byteLength, moved.buffer.byteLength); // 0 8
 *  ```
 *
 *  Example 3 — load modules and scripts:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-global-'));
 *
 *  fs.writeFile(path.join(dir, 'math.js'), 'module.exports = { add: (a, b) => a + b };\n');
 *  console.log(require(path.join(dir, 'math.js')).add(2, 3)); // 5
 *
 *  fs.writeFile(path.join(dir, 'data.json'), '{"port": 8080}');
 *  console.log(require(path.join(dir, 'data.json')).port); // 8080
 *
 *  fs.writeFile(path.join(dir, 'boot.js'), 'console.log("boot", __filename !== undefined);\n');
 *  run(path.join(dir, 'boot.js')); // boot true
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 4 — fetch from a local server and abort a request:
 *  ```JavaScript
 *  const http = require('http');
 *
 *  const server = new http.Server(0, (req) => {
 *      req.response.json({ path: req.address });
 *  });
 *  server.start();
 *  const base = 'http://127.0.0.1:' + server.address().port;
 *
 *  (async () => {
 *      const res = await fetch(base + '/hello');
 *      console.log(res.status, res.ok, (await res.json()).path); // 200 true /hello
 *
 *      const controller = new AbortController();
 *      controller.abort();
 *      try {
 *          await fetch(base, { signal: controller.signal });
 *      } catch (err) {
 *          console.log(err.name); // AbortError
 *      }
 *
 *      server.stop();
 *  })();
 *  ```
 *
 *  Example 5 — events and micro-task scheduling:
 *  ```JavaScript
 *  // EventTarget is the EventEmitter class, so listeners are plain callbacks
 *  const target = new EventTarget();
 *  target.addEventListener('ready', (value) => console.log('ready', value));
 *  target.emit('ready', 42);
 *
 *  // AbortSignal delivers a standard event object to its listeners
 *  const controller = new AbortController();
 *  controller.signal.addEventListener('abort', (ev) => {
 *      console.log(ev.type, ev.target === controller.signal, controller.signal.reason);
 *  });
 *  controller.abort('stop');
 *
 *  // micro-tasks run after the current task, before immediates and timers
 *  console.log('sync');
 *  queueMicrotask(() => console.log('microtask'));
 *  Promise.resolve().then(() => console.log('promise'));
 *  console.log('end');
 *  ```
 *
 *  Notes:
 *
 *  - The sandbox bootstrap also defines the `DOMException`, `AbortError` and
 *    `TimeoutError` error classes, and installs `fetchAsync` as an alias of
 *    `fetch` plus a synchronous `fetchSync`; none of them is a member declared in
 *    this definition.
 *  - `global` is read-only (assigning to it throws), while `globalThis` is an
 *    ordinary writable property.
 *
 */
declare module 'global' {
    /**
     * @description The binary data buffer class, see Buffer
     *
     *      The same class as `require('buffer').Buffer`;
     *      it is installed when the sandbox is created, so binary data handling is
     *      available without requiring the buffer module. Most io APIs accept and
     *      return Buffer objects.
     *
     */
    const Buffer: typeof Class_Buffer;

    type Buffer = Class_Buffer;

    /**
     * @description The URL query parameter collection class, see URLSearchParams
     *
     *      The same class as the url module's URLSearchParams and aligned with the
     *      WHATWG standard. Values are
     *      percent-encoded when the collection is serialized.
     *
     */
    const URLSearchParams: typeof Class_URLSearchParams;

    type URLSearchParams = Class_URLSearchParams;

    /**
     * @description The URL parser class, see UrlObject
     *
     *      `new URL(input, base)` parses an absolute or relative URL according to the
     *      WHATWG URL standard; the global is the url module's UrlObject, which
     *      implements the standard URL API.
     *
     */
    const URL: typeof Class_UrlObject;

    type URL = Class_UrlObject;

    /**
     * @description The immutable binary data block class of the Web File API, see Blob
     *
     *      `new Blob(parts, { type })` builds a blob from strings, buffers and other
     *      blobs; Blob objects are
     *      accepted as fetch bodies and form values.
     *
     */
    const Blob: typeof Class_Blob;

    type Blob = Class_Blob;

    /**
     * @description The in-memory file class, a Blob with a name and modification time, see File
     *
     *      `new File(parts, name, { type, lastModified })` builds one; unlike a file
     *      system handle it represents data in memory and never touches the disk.
     *
     */
    const File: typeof Class_File;

    type File = Class_File;

    /**
     * @description The HTTP header collection class, see Headers
     *
     *      It derives from HttpCollection and is shared by the fetch API and
     *      the http module.
     *
     */
    const Headers: typeof Class_Headers;

    type Headers = Class_Headers;

    /**
     * @description The multipart form data container used as a fetch body, see FormData
     *
     *      `new FormData()` creates an empty form and `append` adds fields and files.
     *      When a FormData object is used as a request body, the multipart boundary
     *      and the content type are generated automatically.
     *
     */
    const FormData: typeof Class_FormData;

    type FormData = Class_FormData;

    /**
     * @description The HTTP request class used as the fetch request source, see HttpRequest
     *
     *      `new Request(url, options)` creates a request that can be passed to fetch.
     *      This is the http module's HttpRequest class rather than the WHATWG Request
     *      class, so the object exposes the request API (`method`,
     *      `headers`, `body`, `response` and so on).
     *
     */
    const Request: typeof Class_HttpRequest;

    type Request = Class_HttpRequest;

    /**
     * @description The HTTP response class returned by fetch, see HttpResponse
     *
     *      fetch resolves with an HttpResponse and `new Response(body, options)`
     *      creates one for tests or synthetic replies. It is the HttpResponse class,
     *      so beyond the common members (`status`, `ok`, `headers`, `text()`,
     *      `json()`) the surface follows the http module.
     *
     */
    const Response: typeof Class_HttpResponse;

    type Response = Class_HttpResponse;

    /**
     * @description The text decoder class, see TextDecoder
     *
     *      `new TextDecoder(codec, options)` decodes Buffer or ArrayBuffer bytes into a
     *      string; the codec defaults to utf8.
     *
     */
    const TextDecoder: typeof Class_TextDecoder;

    type TextDecoder = Class_TextDecoder;

    /**
     * @description The text encoder class, see TextEncoder
     *
     *      `new TextEncoder(codec, options)` encodes a string into UTF-8 bytes and
     *      returns them as a Buffer; the codec defaults to utf8.
     *
     */
    const TextEncoder: typeof Class_TextEncoder;

    type TextEncoder = Class_TextEncoder;

    /**
     * @description The controller that aborts asynchronous Web requests, see AbortController
     *
     *      `new AbortController()` creates a controller with a fresh AbortSignal.
     *      `abort(reason)` fires the signal's `abort` event synchronously and rejects
     *      any fetch using that signal; when no reason is given it is the string
     *      `"AbortError"`.
     *
     */
    const AbortController: typeof Class_AbortController;

    type AbortController = Class_AbortController;

    /**
     * @description The signal that communicates cancellation to asynchronous APIs, see AbortSignal
     *
     *      Pass `signal` to fetch to cancel a request in flight. Static helpers create
     *      derived signals: `AbortSignal.abort(reason)`, `AbortSignal.timeout(ms)` and
     *      `AbortSignal.any(signals)`; a signal created by `timeout` makes fetch reject
     *      with a `TimeoutError`.
     *
     */
    const AbortSignal: typeof Class_AbortSignal;

    type AbortSignal = Class_AbortSignal;

    /**
     * @description The W3C DOM event class, see DOMEvent
     *
     *      `new Event(type, { bubbles, cancelable })` creates an event and `type` is
     *      required; DOMEvent exposes the standard members such as `type`, `bubbles`,
     *      `target`, `defaultPrevented` and the prevent/stop methods.
     *
     */
    const Event: typeof Class_DOMEvent;

    type Event = Class_DOMEvent;

    /**
     * @description The event target class, implemented by EventEmitter, see EventEmitter
     *
     *      This global is the events module's EventEmitter rather than the WHATWG
     *      EventTarget class: `addEventListener`/`removeEventListener` are aliases of
     *      `on`/`off` and take a plain listener, `dispatchEvent` is not provided, and
     *      events are listened to and dispatched with `on`/`once`/`emit`.
     *
     */
    const EventTarget: typeof Class_EventEmitter;

    type EventTarget = Class_EventEmitter;

    /**
     * @description The event object carrying a message delivered through MessagePort, see MessageEvent
     *
     *      `new MessageEvent(type, { data })` is accepted; only the `data` payload is
     *      exposed.
     *
     */
    const MessageEvent: typeof Class_MessageEvent;

    type MessageEvent = Class_MessageEvent;

    /**
     * @description One end of a message channel, see MessagePort
     *
     *      Obtained from `new MessageChannel()` or from a worker's parent port.
     *      Messages are delivered asynchronously; when listening with
     *      addEventListener instead of onmessage, call `start()` to begin receiving.
     *      Call `close()` when the port is no longer needed.
     *
     */
    const MessagePort: typeof Class_MessagePort;

    type MessagePort = Class_MessagePort;

    /**
     * @description A pair of connected MessagePort objects, see MessageChannel
     *
     *      `new MessageChannel()` returns `port1` and `port2`; a message posted to one
     *      port is delivered to the other with structured-clone semantics, and an
     *      optional transfer list moves ArrayBuffers instead of copying them.
     *
     */
    const MessageChannel: typeof Class_MessageChannel;

    type MessageChannel = Class_MessageChannel;

    /**
     * @description The child thread class, see Worker
     *
     *      `new Worker(path, opts)` starts a worker from a script path with the same
     *      semantics as the worker_threads module's Worker; the global is installed
     *      for convenience.
     *
     *      Example — run a worker script and terminate it:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-global-'));
     *      fs.writeFile(path.join(dir, 'worker.js'),
     *          'const { parentPort } = require("worker_threads");\n' +
     *          'parentPort.on("message", (msg) => parentPort.postMessage(msg + " from worker"));\n');
     *
     *      const worker = new Worker(path.join(dir, 'worker.js'));
     *      worker.on('message', async (msg) => {
     *          console.log(msg); // hello from worker
     *          await worker.terminate();
     *          fs.rmSync(dir, { recursive: true, force: true });
     *      });
     *      worker.postMessage('hello');
     *      ```
     *
     */
    const Worker: typeof Class_Worker;

    type Worker = Class_Worker;

    /**
     * @description The Web Crypto key class, see CryptoKey
     *
     *      Keys are created by `crypto.subtle.generateKey`/`importKey`; the class
     *      cannot be constructed directly.
     *
     */
    const CryptoKey: typeof Class_CryptoKey;

    type CryptoKey = Class_CryptoKey;

    /**
     * @description The DOM parser class, see DOMParser
     *
     *      `new DOMParser().parseFromString(source, mimeType)` parses HTML or XML into
     *      an XmlDocument.
     *
     */
    const DOMParser: typeof Class_DOMParser;

    type DOMParser = Class_DOMParser;

    /**
     * @description The inline CSS declaration block of an element, see CSSStyleDeclaration
     *
     *      Not a constructor in fibjs: instances are obtained from the `style`
     *      property of an element of a parsed document.
     *
     */
    const CSSStyleDeclaration: typeof Class_CSSStyleDeclaration;

    type CSSStyleDeclaration = Class_CSSStyleDeclaration;

    /**
     * @description The map of the data-* attributes of an element, see DOMStringMap
     *
     *      Not a constructor in fibjs: instances are obtained from the `dataset`
     *      property of an element; keys are camelCase and map to data-* attributes.
     *
     */
    const DOMStringMap: typeof Class_DOMStringMap;

    type DOMStringMap = Class_DOMStringMap;

    /**
     * @description The serializer that turns DOM nodes into XML strings, see XMLSerializer
     *
     *      `new XMLSerializer().serializeToString(node)` serializes a document or
     *      element.
     *
     */
    const XMLSerializer: typeof Class_XMLSerializer;

    type XMLSerializer = Class_XMLSerializer;

    /**
     * @description The XML document class, see XmlDocument
     *
     *      `new XMLDocument(type)` creates an empty document that can be loaded with
     *      `load(source)`. It is the same class as the xml module's XmlDocument.
     *
     */
    const XMLDocument: typeof Class_XmlDocument;

    type XMLDocument = Class_XmlDocument;

    /**
     * @description The WebSocket client and server class, see WebSocket
     *
     *      `new WebSocket(url, protocols, origin)` connects as a client, while
     *      `WebSocket.upgrade(options, handler)` accepts server connections, so both
     *      the client and the server side are available.
     *
     */
    const WebSocket: typeof Class_WebSocket;

    type WebSocket = Class_WebSocket;

    /**
     * @description The console output object, see console
     *
     *      The same object as `require('console')`; it
     *      provides log/info/warn/error and the other console methods.
     *
     */
    const console: typeof import ('console');

    /**
     * @description The process object, see process
     *
     *      The same object as `require('process')`, exposing argv, env, platform,
     *      exit and the other process members.
     *
     */
    const process: typeof import ('process');

    /**
     * @description The performance measurement object, see performance
     *
     *      The same object as `require('perf_hooks').performance`, providing `now()`,
     *      `mark()`, `measure()` and the other measurements.
     *
     */
    const performance: typeof import ('performance');

    /**
     * @description The observer that receives performance entries, see PerformanceObserver
     *
     *      `new PerformanceObserver(callback)` plus `observe({ entryTypes })`
     *      subscribes to performance records.
     *
     */
    const PerformanceObserver: typeof Class_PerformanceObserver;

    type PerformanceObserver = Class_PerformanceObserver;

    /**
     * @description The Web Crypto object, see the crypto module
     *
     *      This is not the hashing module: the global is the Web Crypto API object
     *      (`crypto.subtle`, `crypto.getRandomValues`, `crypto.randomUUID` and the
     *      CryptoKey class), equivalent to `require('crypto').webcrypto`.
     *
     */
    const crypto: typeof import ('webcrypto');

    /**
     * @description The global object itself, a read-only alias of globalThis, see globalThis
     *
     *      `global === globalThis` is true. The property is an accessor without a
     *      setter, so assigning to `global` throws a TypeError; use `globalThis` in
     *      new code.
     *
     */
    const global: FIBJS.GeneralObject;

    /**
     * @description The global object itself, exposed under the standard name
     *
     *      In fibjs it is an ordinary writable data property, so `globalThis = value`
     *      replaces the binding. Property reads and writes through it
     *      reach the sandbox global object, so `globalThis.x = 1` publishes `x` to
     *      every script of the sandbox.
     *
     */
    const globalThis: FIBJS.GeneralObject;

    /**
     * @description Runs a script file in the main sandbox
     *
     *      The file is executed synchronously in the same sandbox and global scope as
     *      the caller, so a script that sets `globalThis.x` makes `x` visible after
     *      the call. Relative paths are resolved against the current working
     *      directory; a file that cannot be opened throws. The return value is
     *      undefined. The member is an extension of the runtime.
     *
     *      Example — run a script from a temporary directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-global-'));
     *      fs.writeFile(path.join(dir, 'boot.js'),
     *          'globalThis.booted = "yes";\nconsole.log("boot script", __filename !== undefined);\n');
     *
     *      run(path.join(dir, 'boot.js')); // boot script true
     *      console.log(globalThis.booted); // yes
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      @param fname the path of the script to run
     *
     */
    function run(fname: string): void;

    /**
     * @description Loads a module and returns its exports object, see the module module
     *
     *      `require` loads internal modules and file modules. Internal modules are
     *      initialized when the sandbox is created and are referenced by id, for
     *      example `require("net")`; the `node:` prefix is also
     *      accepted, so `require("node:fs")` is the same as `require("fs")`.
     *
     *      File modules are referenced by a path starting with ./ or ../, or by an
     *      absolute path; the .js, .jsc and .json extensions are supported, and a .js
     *      file written with ESM syntax is retried as an ES module. When the path is a
     *      directory, a package.json `exports` entry takes precedence over `main`, and
     *      when neither is usable, index.js, index.jsc or index.json under the path is
     *      tried. A path that is not internal and does not start with ./ or ../ is
     *      searched in the node_modules directories walking up from the requiring
     *      module.
     *
     *      The function object also exposes `require.resolve(id)`, `require.cache` and
     *      `require.main`; `require.extensions` is not provided. In fibjs `require` is available both as a global
     *      and in the module scope, and calling it from an ES module throws.
     *
     *      The basic flow is as follows:
     *
     *      ```dot
     *         digraph{
     *             node [fontname = "Helvetica,sans-Serif", fontsize = 10];
     *             edge [fontname = "Helvetica,sans-Serif", fontsize = 10];
     *
     *             start [label="start"];
     *             resolve [label="path.resolve" shape="rect"];
     *             search [label="recursive lookup\nnode_modules\nfrom the current path" shape="rect"];
     *             load [label="load" shape="rect"];
     *             end [label="end" shape="doublecircle"];
     *
     *             is_native [label="is internal module?" shape="diamond"];
     *             is_mod [label="is module?" shape="diamond"];
     *             is_abs [label="is absolute?" shape="diamond"];
     *             has_file [label="module exists?" shape="diamond"];
     *             has_ext [label="module.js exists?" shape="diamond"];
     *             has_package [label="/package.json\nexists?" shape="diamond"];
     *             has_main [label="main exists?" shape="diamond"];
     *             has_index [label="index.js exists?" shape="diamond"];
     *
     *             start -> is_native;
     *             is_native -> end [label="Yes"];
     *             is_native -> is_mod [label="No"];
     *             is_mod -> search [label="Yes"];
     *             search -> has_file;
     *             is_mod -> is_abs [label="No"];
     *             is_abs -> has_file [label="Yes"];
     *             is_abs -> resolve [label="No"];
     *             resolve -> has_file;
     *             has_file -> load [label="Yes"];
     *             has_file -> has_ext [label="No"];
     *             has_ext -> load [label="Yes"];
     *             has_ext -> has_package [label="No"];
     *             has_package -> has_main [label="Yes"];
     *             has_package -> has_index [label="No"];
     *             has_main -> load [label="Yes"];
     *             has_main -> has_index [label="No"];
     *             has_index -> load [label="Yes"];
     *             has_index -> end [label="No"];
     *             load -> end;
     *         }
     *      ```
     *
     *      Example — load a module file and a JSON file:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-global-'));
     *      fs.writeFile(path.join(dir, 'config.json'), '{"name": "fibjs"}');
     *      fs.writeFile(path.join(dir, 'greet.js'), 'module.exports = (name) => "hello " + name;\n');
     *
     *      console.log(require(path.join(dir, 'config.json')).name); // fibjs
     *      console.log(require(path.join(dir, 'greet.js'))('world')); // hello world
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     *      @param id the name or path of the module to load
     *      @return the exported object of the loaded module
     *
     */
    function require(id: string): any;

    /**
     * @description Calls a function after the given delay, like the same-named timers module function
     *
     *      The delay defaults to 1 ms; values below 1 or above 2^31-1 are clamped to
     *      1 ms. Extra arguments
     *      are passed to the callback. The returned Timer keeps the process alive
     *      until it fires or is cleared; call `clearTimeout(timer)`, or `timer.unref()`
     *      to let the process exit without cancelling it. Timers run after
     *      `setImmediate` callbacks and after promise jobs.
     *
     *      Example — pass extra arguments to the callback:
     *      ```JavaScript
     *      setTimeout((name, count) => {
     *          console.log(name, count); // job 3
     *      }, 20, 'job', 3);
     *      ```
     *
     *      @param callback the callback function
     *      @param timeout the delay in milliseconds, 1 by default; values outside 1..2^31-1 become 1ms.
     *      @param args extra arguments passed to the callback, optional.
     *      @return the timer object
     *
     */
    function setTimeout(callback: (...args: any[])=>void, timeout?: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the given timer
     *
     *      Accepts any Timer object returned by setTimeout, setInterval, setImmediate
     *      or setHrInterval, so the four clear functions are interchangeable; clearing
     *      a value that is not a timer is a no-op. A cleared timer releases its hold on
     *      the process lifetime.
     *
     *      @param t the timer to clear
     *
     */
    function clearTimeout(t: any): void;

    /**
     * @description Calls a function after every given delay, like the same-named timers module function
     *
     *      The delay is in milliseconds and is clamped like setTimeout; each call
     *      passes the extra arguments to the callback. A repeating timer is never
     *      released automatically: clear it with `clearInterval(timer)` or the process
     *      will not exit, and `timer.unref()` releases the liveness hold while the
     *      timer keeps running.
     *
     *      Example — a counter that stops itself after three ticks:
     *      ```JavaScript
     *      let ticks = 0;
     *      const timer = setInterval(() => {
     *          ticks++;
     *          console.log('tick', ticks);
     *          if (ticks === 3) {
     *              clearInterval(timer);
     *          }
     *      }, 10);
     *      ```
     *
     *      @param callback the callback function
     *      @param timeout the interval in milliseconds; values below 1 or above 2^31-1 are treated as 1ms.
     *      @param args extra arguments passed to the callback, optional.
     *      @return the timer object
     *
     */
    function setInterval(callback: (...args: any[])=>void, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the given timer
     *
     *      The same operation as clearTimeout; it accepts any Timer object, so a
     *      repeating timer created by setInterval can also be cleared through
     *      clearTimeout and vice versa.
     *
     *      @param t the timer to clear
     *
     */
    function clearInterval(t: any): void;

    /**
     * @description Calls a function repeatedly with a high-precision timer that interrupts JavaScript
     *
     *      An extension of the runtime. Unlike setInterval, the timer
     *      fires by interrupting the isolate, so the callback can run while a busy loop
     *      is executing; keep the callback short and do not call async APIs or modify
     *      state that other modules may read, otherwise unpredictable results may
     *      occur. The compiler also assumes that a loop variable such as `cnt` does not
     *      change during a busy loop, so `while (cnt < 10);` never ends even though the
     *      callback changes `cnt`. Clear the timer with clearHrInterval when it is no
     *      longer needed, otherwise the process never exits.
     *
     *      Example — count three ticks and clear the timer:
     *      ```JavaScript
     *      let count = 0;
     *      const timer = setHrInterval(() => {
     *          count++;
     *          console.log(count);
     *          if (count >= 3) {
     *              clearHrInterval(timer);
     *          }
     *      }, 100);
     *      ```
     *
     *      @param callback the callback function
     *      @param timeout the interval in milliseconds; values below 1 or above 2^31-1 are treated as 1ms.
     *      @param args extra arguments passed to the callback, optional.
     *      @return the timer object
     *
     */
    function setHrInterval(callback: (...args: any[])=>void, timeout: number, ...args: any[]): Class_Timer;

    /**
     * @description Clears the given timer
     *
     *      Like the other clear functions it accepts any Timer object; use it for
     *      timers created by setHrInterval, which otherwise keep interrupting the
     *      script and prevent the process from exiting.
     *
     *      @param t the timer to clear
     *
     */
    function clearHrInterval(t: any): void;

    /**
     * @description Calls the callback as soon as the current task completes, before timers fire
     *
     *      Extra arguments are passed to the callback and the returned Timer can be
     *      cleared with clearImmediate. Immediates run after promise jobs and
     *      `queueMicrotask`, but before setTimeout/setInterval callbacks (whose
     *      smallest delay is 1 ms).
     *
     *      @param callback the callback function
     *      @param args extra arguments passed to the callback, optional.
     *      @return the timer object
     *
     */
    function setImmediate(callback: (...args: any[])=>void, ...args: any[]): Class_Timer;

    /**
     * @description Clears the given timer
     *
     *      The same operation as the other clear functions; it accepts any Timer
     *      object and clears an immediate created by setImmediate.
     *
     *      @param t the timer to clear
     *
     */
    function clearImmediate(t: any): void;

    /**
     * @description Encodes a value into a base64 string using the Latin1 range
     *
     *      The value is converted to its string form first, as the DOM does:
     *      `btoa(123)` encodes "123" and `btoa(null)` encodes "null". Characters above
     *      U+00FF throw an `Error`; use `Buffer.from(text).toString('base64')` for
     *      general strings.
     *
     *      Example — encode a value and decode it back:
     *      ```JavaScript
     *      const encoded = btoa('user:pass');
     *      console.log(encoded, atob(encoded)); // dXNlcjpwYXNz user:pass
     *      ```
     *
     *      @param data the value to encode
     *      @return the encoded string
     *
     */
    function btoa(data: any): string;

    /**
     * @description Decodes a base64 string into a Latin1 string
     *
     *      The value is converted to its string form first, as the DOM does:
     *      `atob(123)` decodes "123". Whitespace is ignored and invalid characters
     *      throw an `Error`; the result contains one character per decoded
     *      byte.
     *
     *      Example — decode a base64 string:
     *      ```JavaScript
     *      console.log(atob('aGVsbG8=')); // hello
     *      ```
     *
     *      @param data the value to decode
     *      @return the decoded binary data
     *
     */
    function atob(data: any): string;

    /**
     * @description Creates a deep copy of a value with the structured clone algorithm
     *
     *      Circular references, Map, Set, Date, RegExp, typed arrays, ArrayBuffer,
     *      SharedArrayBuffer and Error objects are supported. Functions and other
     *      values that cannot be cloned throw a `DOMException` (`DataCloneError`,
     *      code 25), matching the Web standard.
     *
     *      The `transfer` option lists ArrayBuffers to move instead of copy; a
     *      transferred buffer is detached and its `byteLength` becomes 0. Only
     *      Only ArrayBuffer entries are accepted, any other entry throws a
     *      TypeError.
     *
     *      options supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "transfer": [] // ArrayBuffers to move to the clone; default is an empty array
     *      })
     *      ```
     *
     *      Example — clone a cyclic object and transfer a buffer:
     *      ```JavaScript
     *      const original = { name: 'global' };
     *      original.self = original;
     *      const copy = structuredClone(original);
     *      console.log(copy !== original, copy.self === copy); // true true
     *
     *      const buffer = new ArrayBuffer(8);
     *      const moved = structuredClone({ buffer }, { transfer: [buffer] });
     *      console.log(buffer.byteLength, moved.buffer.byteLength); // 0 8
     *      ```
     *
     *      @param value the value to clone
     *      @param options optional options object containing the transfer array
     *      @return the cloned value
     *
     */
    function structuredClone(value: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description Sends a Web Fetch request given a Request object or a URL string
     *
     *      `request` may be an HttpRequest object or the target URL; opts overrides the
     *      fields of the request source (`new Request(request, init)` semantics) and
     *      supports method, headers, body, keepAlive, timeout, redirect, signal and
     *      streaming, documented in the http module. Following the Fetch standard a
     *      GET or HEAD request must not carry a body, a string body is sent as
     *      text/plain;charset=UTF-8, and `headers` replaces the headers of the request
     *      source instead of merging them.
     *
     *      The returned promise resolves with an HttpResponse (not the WHATWG Response
     *      class). It rejects with an `AbortError` (code ABORT_ERR) when an
     *      AbortSignal passed in opts is aborted, including during the request, and
     *      with a `TimeoutError` when the signal comes from `AbortSignal.timeout`.
     *      There is no global dispatcher; use the http module for
     *      proxies, agents and other client tuning.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "method": "GET",      // request method
     *          "headers": {},        // replaces the headers of the request source
     *          "body": {},           // SeekableStream | Buffer | String | FormData
     *          "timeout": 0,         // request timeout in milliseconds, 0 uses the client default
     *          "redirect": "follow", // "follow" | "error" | "manual"
     *          "signal": null,       // AbortSignal used to cancel the request
     *          "streaming": false    // return the body in streaming mode
     *      })
     *      ```
     *
     *      Example — fetch JSON from a local server and abort a request:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const base = 'http://127.0.0.1:' + server.address().port;
     *
     *      (async () => {
     *          const res = await fetch(base + '/hello');
     *          console.log(res.status, (await res.json()).path); // 200 /hello
     *
     *          const controller = new AbortController();
     *          controller.abort();
     *          try {
     *              await fetch(base, { signal: controller.signal });
     *          } catch (err) {
     *              console.log(err.name); // AbortError
     *          }
     *
     *          server.stop();
     *      })();
     *      ```
     *
     *      @param request the request source
     *      @param opts request options (may override the fields of request)
     *      @return the server response object
     *
     */
    function fetch(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Sends a Web Fetch request given a Request object or a URL string
     *
     *      `request` may be an HttpRequest object or the target URL; opts overrides the
     *      fields of the request source (`new Request(request, init)` semantics) and
     *      supports method, headers, body, keepAlive, timeout, redirect, signal and
     *      streaming, documented in the http module. Following the Fetch standard a
     *      GET or HEAD request must not carry a body, a string body is sent as
     *      text/plain;charset=UTF-8, and `headers` replaces the headers of the request
     *      source instead of merging them.
     *
     *      The returned promise resolves with an HttpResponse (not the WHATWG Response
     *      class). It rejects with an `AbortError` (code ABORT_ERR) when an
     *      AbortSignal passed in opts is aborted, including during the request, and
     *      with a `TimeoutError` when the signal comes from `AbortSignal.timeout`.
     *      There is no global dispatcher; use the http module for
     *      proxies, agents and other client tuning.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "method": "GET",      // request method
     *          "headers": {},        // replaces the headers of the request source
     *          "body": {},           // SeekableStream | Buffer | String | FormData
     *          "timeout": 0,         // request timeout in milliseconds, 0 uses the client default
     *          "redirect": "follow", // "follow" | "error" | "manual"
     *          "signal": null,       // AbortSignal used to cancel the request
     *          "streaming": false    // return the body in streaming mode
     *      })
     *      ```
     *
     *      Example — fetch JSON from a local server and abort a request:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const base = 'http://127.0.0.1:' + server.address().port;
     *
     *      (async () => {
     *          const res = await fetch(base + '/hello');
     *          console.log(res.status, (await res.json()).path); // 200 /hello
     *
     *          const controller = new AbortController();
     *          controller.abort();
     *          try {
     *              await fetch(base, { signal: controller.signal });
     *          } catch (err) {
     *              console.log(err.name); // AbortError
     *          }
     *
     *          server.stop();
     *      })();
     *      ```
     *
     *      @param request the request source
     *      @param opts request options (may override the fields of request)
     *      @return the server response object
     *
     */
    function fetchSync(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Class_HttpResponse;

    /**
     * @description Sends a Web Fetch request given a Request object or a URL string
     *
     *      `request` may be an HttpRequest object or the target URL; opts overrides the
     *      fields of the request source (`new Request(request, init)` semantics) and
     *      supports method, headers, body, keepAlive, timeout, redirect, signal and
     *      streaming, documented in the http module. Following the Fetch standard a
     *      GET or HEAD request must not carry a body, a string body is sent as
     *      text/plain;charset=UTF-8, and `headers` replaces the headers of the request
     *      source instead of merging them.
     *
     *      The returned promise resolves with an HttpResponse (not the WHATWG Response
     *      class). It rejects with an `AbortError` (code ABORT_ERR) when an
     *      AbortSignal passed in opts is aborted, including during the request, and
     *      with a `TimeoutError` when the signal comes from `AbortSignal.timeout`.
     *      There is no global dispatcher; use the http module for
     *      proxies, agents and other client tuning.
     *
     *      opts supports the following fields:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "method": "GET",      // request method
     *          "headers": {},        // replaces the headers of the request source
     *          "body": {},           // SeekableStream | Buffer | String | FormData
     *          "timeout": 0,         // request timeout in milliseconds, 0 uses the client default
     *          "redirect": "follow", // "follow" | "error" | "manual"
     *          "signal": null,       // AbortSignal used to cancel the request
     *          "streaming": false    // return the body in streaming mode
     *      })
     *      ```
     *
     *      Example — fetch JSON from a local server and abort a request:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const server = new http.Server(0, (req) => {
     *          req.response.json({ path: req.address });
     *      });
     *      server.start();
     *      const base = 'http://127.0.0.1:' + server.address().port;
     *
     *      (async () => {
     *          const res = await fetch(base + '/hello');
     *          console.log(res.status, (await res.json()).path); // 200 /hello
     *
     *          const controller = new AbortController();
     *          controller.abort();
     *          try {
     *              await fetch(base, { signal: controller.signal });
     *          } catch (err) {
     *              console.log(err.name); // AbortError
     *          }
     *
     *          server.stop();
     *      })();
     *      ```
     *
     *      @param request the request source
     *      @param opts request options (may override the fields of request)
     *      @return the server response object
     *
     */
    function fetchAsync(request: Class_HttpRequest | Class_HttpRequestPromise | string, opts?: FIBJS.GeneralObject): Promise<Class_HttpResponsePromise>;

    /**
     * @description Queues a function as a micro-task
     *
     *      The callback runs after the current task completes and before the next task
     *      starts, in the same V8 micro-task queue as promise jobs and in FIFO order
     *      with them; `process.nextTick` callbacks run earlier and `setImmediate`
     *      callbacks later. A non-function argument throws a TypeError, and an
     *      exception thrown by the callback is reported as an uncaught exception.
     *
     *      Example — observe the micro-task order:
     *      ```JavaScript
     *      console.log('sync');
     *      queueMicrotask(() => console.log('microtask'));
     *      Promise.resolve().then(() => console.log('promise'));
     *      console.log('end');
     *      // prints: sync, end, microtask, promise
     *      ```
     *
     *      @param callback the function to queue as a micro-task
     *
     */
    function queueMicrotask(callback: ()=>void): void;

}

