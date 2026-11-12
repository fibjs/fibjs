/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `events` module (no IDL unit: fibjs/src/base/events.cpp builds
// the module from the EventEmitter class directly).
//
// At runtime `require('events')` returns the EventEmitter class itself
// (`require('events') === require('events').EventEmitter`), with the Node
// statics attached to it: the class carries the instance API through its
// prototype and the static helpers (`events.once`, `events.addAbortListener`,
// ...) as own properties. The declaration mirrors that with `export =` on the
// class value, the shape @types/node gives `events`.

declare module 'events' {
    /**
     * @description The event emitter class
     *
     *      The runtime's EventEmitter (Class_EventEmitter): a class declaration
     *      here (rather than a `const` alias) is what makes it usable in the
     *      type position too - `import { EventEmitter } from 'node:events'`
     *      binds one name for both, the way the Node definitions publish it.
     *      The statics (`EventEmitter.once`, `EventEmitter.addAbortListener`,
     *      `defaultMaxListeners`, ...) come down from Class_EventEmitter.
     *
     *      At runtime the module object *is* the class
     *      (`require('events') === require('events').EventEmitter`), which is
     *      what `import events = require('events')` sees.
     */
    class EventEmitter extends Class_EventEmitter {
    }

    /** The listener-count default of newly constructed emitters. */
    let defaultMaxListeners: number;

    /**
     * @description Subscribes to a promise for the named event
     *
     *      Node's `events.once(emitter, name)`: resolves with the event
     *      arguments, rejects with an 'error' event.
     */
    function once(emitter: Class_EventEmitter, name: string, options?: FIBJS.GeneralObject): Promise<any[]>;

    /**
     * @description Binds an AbortSignal to a listener
     *
     *      Node's `events.addAbortListener(signal, listener)`: runs the
     *      listener when the signal aborts (at once when it is already
     *      aborted) and returns a handle whose `dispose()` removes it.
     */
    function addAbortListener(signal: Class_AbortSignal, listener: (event: FIBJS.GeneralObject) => void): { dispose(): void };

    export { EventEmitter, defaultMaxListeners, once, addAbortListener };
}
