/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description The signal that communicates cancellation to asynchronous operations
 *
 *   An AbortSignal is the read-only half of the AbortController pair and the object handed
 *   to APIs that support cancellation. It carries a one-shot aborted state and an abort
 *   reason; when the state flips the `abort` event fires and every consumer reacts. The
 *   class is a global, Node.js exposes the same class globally, and it derives from
 *   EventEmitter, so listeners can be registered with `on`/`once`/`addEventListener`.
 *
 *   Signals cannot be constructed directly: `new AbortSignal()` throws a TypeError. Obtain
 *   one from a controller, which owns the write side, or from one of the static factory
 *   methods below, which produce signals with no controller.
 *
 *  Concepts:
 *
 *  - **One-shot cancellation**: a signal starts active and becomes aborted at most once;
 *    the state never resets, later abort attempts are ignored and handlers registered after
 *    the abort are never called. There is no instance method that aborts a signal: only the
 *    owner (or a static factory) can.
 *  - **Abort reasons**: `reason` is `undefined` while the signal is active and afterwards
 *    holds the value stored by the aborting side. The default is the string `"AbortError"`;
 *    `AbortSignal.timeout()` stores `"TimeoutError"`; other values are preserved as-is.
 *    Node.js stores a DOMException by default and `abort(null)` stores null. throwIfAborted
 *    throws an AbortError or TimeoutError for the two default reasons, but does not rethrow
 *    a custom value as-is.
 *  - **Listener registration**: AbortSignal is an EventEmitter (see EventEmitter). The
 *    `abort` event also supports the DOM-style `onabort` handler and `addEventListener` with
 *    the `once` option; all handlers run synchronously while the signal is being aborted,
 *    and an exception raised by one of them is rethrown after the others finished.
 *  - **Derived signals**: `AbortSignal.abort(reason)` is aborted at creation,
 *    `AbortSignal.timeout(ms)` aborts itself after a delay and `AbortSignal.any(signals)`
 *    follows the first of several signals to abort; none of them has a controller.
 *  - **Integration points**: the signal is passed to fetch (`{ signal }` in its options
 *    object), to the http module request options, to `child_process.exec`/`spawn` through
 *    `options.signal` (the child is killed on abort) and to
 *    `events.addAbortListener(signal, handler)`. A fetch cancelled by a controller rejects
 *    with an AbortError (code `ABORT_ERR`); one cancelled through `AbortSignal.timeout`
 *    rejects with a TimeoutError (code `TIMEOUT_ERR`). The pending timer of a timeout
 *    signal keeps the process alive until it fires (Node.js lets the process exit), so a
 *    long timeout can delay shutdown.
 *
 *  Obtained from:
 *  - `new AbortController().signal` — a signal owned by the controller;
 *  - `AbortSignal.abort(reason)` — an already aborted signal;
 *  - `AbortSignal.timeout(ms)` — a signal that aborts after the delay;
 *  - `AbortSignal.any(signals)` — a composite signal that follows the first input to abort.
 *
 *  Example 1 — inspect a signal before and after the abort:
 *  ```JavaScript
 *  const controller = new AbortController();
 *  const signal = controller.signal;
 *
 *  console.log(signal.aborted, signal.reason); // false undefined
 *  signal.throwIfAborted(); // no-op while active
 *  controller.abort();
 *  console.log(signal.aborted, signal.reason); // true AbortError
 *  try {
 *      signal.throwIfAborted();
 *  } catch (err) {
 *      console.log(err.name, err.code); // AbortError ABORT_ERR
 *  }
 *  ```
 *
 *  Example 2 — let a timeout abort a signal on its own:
 *  ```JavaScript
 *  const signal = AbortSignal.timeout(20);
 *
 *  signal.onabort = (ev) => {
 *      console.log(signal.aborted, ev.type, ev.reason); // true abort TimeoutError
 *      try {
 *          signal.throwIfAborted();
 *      } catch (err) {
 *          console.log(err.name, err.code); // TimeoutError TIMEOUT_ERR
 *      }
 *  };
 *  ```
 *
 *  Example 3 — cancel a slow fetch with a timeout signal:
 *  ```JavaScript
 *  const http = require('http');
 *  const coroutine = require('coroutine');
 *
 *  const server = new http.Server(0, (req) => {
 *      coroutine.sleep(100);
 *      req.response.json({ ok: true });
 *  });
 *  server.start();
 *  const base = 'http://127.0.0.1:' + server.address().port;
 *
 *  (async () => {
 *      try {
 *          await fetch(base, { signal: AbortSignal.timeout(20) });
 *      } catch (err) {
 *          console.log(err.name, err.code); // TimeoutError TIMEOUT_ERR
 *      }
 *      server.stop();
 *  })();
 *  ```
 *
 */
declare class Class_AbortSignal extends Class_EventEmitter {
    /**
     * @description Creates an AbortSignal that is already aborted
     *
     *      Each call returns a distinct signal in the aborted state with `reason` set to the
     *      given value, or to the string `"AbortError"` when the argument is omitted,
     *      `undefined` or `null`. Because the signal is aborted at creation, `abort` handlers
     *      registered afterwards are never called and throwIfAborted throws immediately.
     *
     *      Example — an already aborted signal ignores late listeners:
     *      ```JavaScript
     *      const signal = AbortSignal.abort('pre-cancelled');
     *      console.log(signal.aborted, signal.reason); // true pre-cancelled
     *
     *      let called = false;
     *      signal.addEventListener('abort', () => {
     *          called = true;
     *      });
     *      console.log('listener called:', called); // listener called: false
     *      ```
     *      @param reason the abort reason, `"AbortError"` when omitted
     *      @return returns an already aborted AbortSignal object
     *
     */
    static abort(reason?: string | any): Class_AbortSignal;

    /**
     * @description Creates an AbortSignal that automatically aborts after a timeout
     *
     *      The signal aborts with the string reason `"TimeoutError"` once ms milliseconds have
     *      elapsed, so a fetch cancelled by it rejects with a TimeoutError (code `TIMEOUT_ERR`).
     *      ms is coerced to a number, so numeric strings are accepted; values below 1 are clamped
     *      to 1 ms and values above the internal maximum to that maximum. NaN and a missing
     *      argument throw a TypeError (Node.js throws a RangeError for NaN and negative values).
     *      The timer cannot be cancelled, because the signal has no abort method, and the pending
     *      timer keeps the process alive until it fires (Node.js lets the process exit).
     *
     *      Example — register a disposable handler for a timeout:
     *      ```JavaScript
     *      const events = require('events');
     *
     *      const signal = AbortSignal.timeout(20);
     *      events.addAbortListener(signal, () => {
     *          console.log('timeout:', signal.reason); // timeout: TimeoutError
     *      });
     *      ```
     *      @param ms timeout in milliseconds
     *      @return returns an AbortSignal object that will abort after ms milliseconds
     *
     */
    static timeout(ms: number): Class_AbortSignal;

    /**
     * @description Creates an AbortSignal that aborts when any of the given signals aborts
     *
     *      The argument must be an Array of AbortSignal objects; a non-array value or an element
     *      of another type throws a TypeError (Node.js accepts any iterable). The returned signal
     *      stays active while every input is active; when an input aborts, the composite aborts
     *      with that input's reason, and inputs that abort later do not change it again. Only the
     *      order in time matters, not the order in the array. If an input is already aborted, the
     *      composite is returned already aborted with that signal's reason and handlers
     *      registered afterwards are never called. An empty array produces a signal that never
     *      aborts, and the composite has no controller, so it cannot be aborted manually.
     *
     *      Example — the first signal to abort decides the reason:
     *      ```JavaScript
     *      const first = new AbortController();
     *      const second = new AbortController();
     *      const combined = AbortSignal.any([first.signal, second.signal]);
     *
     *      combined.addEventListener('abort', () => {
     *          console.log('combined:', combined.reason);
     *      });
     *      second.abort('second won');
     *      first.abort('first too late');
     *      console.log(combined.aborted, combined.reason); // true second won
     *      ```
     *      @param signals an array of AbortSignal objects
     *      @return returns a composite AbortSignal object
     *
     */
    static any(signals: any[]): Class_AbortSignal;

    /**
     * @description Throws the abort error when the signal has been aborted, otherwise does nothing
     *
     *      A no-op while the signal is active. After the abort the thrown error depends on how the
     *      signal was aborted: the default reason and a timeout produce an AbortError (name
     *      `AbortError`, code `ABORT_ERR`, message "The operation was aborted.") or a TimeoutError
     *      (name `TimeoutError`, code `TIMEOUT_ERR`, message "The operation timed out."); a custom
     *      string reason is not preserved and also throws the generic AbortError, and any other
     *      reason value throws a generic Error (20024) instead of the value itself. Node.js throws
     *      the reason exactly as stored in every case, so read `AbortSignal.reason` when the
     *      original value matters.
     *
     *      Example — a no-op while active, an AbortError after the abort:
     *      ```JavaScript
     *      const controller = new AbortController();
     *      controller.signal.throwIfAborted(); // no-op while active
     *      console.log('active');
     *
     *      controller.abort();
     *      try {
     *          controller.signal.throwIfAborted();
     *      } catch (err) {
     *          console.log(err.name, err.code); // AbortError ABORT_ERR
     *      }
     *      ```
     *
     */
    throwIfAborted(): void;

    /**
     * @description Whether the signal has been aborted
     *
     *      False until the owning controller or a static factory aborts the signal, then true
     *      forever; assigning to the property is silently ignored. A signal created by
     *      `AbortSignal.abort()` starts with true. The state only flips once, so polling this
     *      flag is safe, but registering a listener is the usual way to react.
     *
     */
    readonly aborted: boolean;

    /**
     * @description The abort reason stored when the signal was aborted
     *
     *      `undefined` while the signal is active; afterwards the value passed to the aborting
     *      call, with the string `"AbortError"` as the default and `"TimeoutError"` for
     *      `AbortSignal.timeout()`. Strings, numbers, objects and Errors are all preserved as-is
     *      (only `undefined` and `null` select the default), assigning to the property is
     *      silently ignored, and the value never changes after the first abort. Node.js stores a
     *      DOMException by default and preserves `null` as a reason.
     *
     */
    readonly reason: any;

    /**
     * @description Emitted once when the signal is aborted, carrying the event object
     *
     *      Handlers registered through addEventListener/on/once or the `onabort` handler property
     *      are all invoked synchronously by the aborting call (AbortController.abort(), the
     *      timeout timer or an AbortSignal.any propagation); handlers registered after the abort
     *      are never called. The event object is a plain object with `type` `"abort"` and `target`
     *      set to the signal; it carries `reason` only when the reason is a non-empty string,
     *      because a value reason is exposed solely through `AbortSignal.reason`. It is not a
     *      DOMEvent instance (Node.js passes an Event object). A handler that throws is rethrown
     *      by the aborting call after the remaining handlers ran. Do not emit this event manually:
     *      emitting `"abort"` marks the signal aborted without storing a reason.
     *
     *      @param ev the abort event object, with type, target and, for string reasons, reason
     *
     */
    on(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "abort", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted once when the signal is aborted, carrying the event object
     *
     *      Handlers registered through addEventListener/on/once or the `onabort` handler property
     *      are all invoked synchronously by the aborting call (AbortController.abort(), the
     *      timeout timer or an AbortSignal.any propagation); handlers registered after the abort
     *      are never called. The event object is a plain object with `type` `"abort"` and `target`
     *      set to the signal; it carries `reason` only when the reason is a non-empty string,
     *      because a value reason is exposed solely through `AbortSignal.reason`. It is not a
     *      DOMEvent instance (Node.js passes an Event object). A handler that throws is rethrown
     *      by the aborting call after the remaining handlers ran. Do not emit this event manually:
     *      emitting `"abort"` marks the signal aborted without storing a reason.
     *
     *      @param ev the abort event object, with type, target and, for string reasons, reason
     *
     */
    onabort: ((ev: FIBJS.GeneralObject)=>void) | null;

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

