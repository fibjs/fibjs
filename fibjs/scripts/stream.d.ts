/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `stream` module (no IDL unit: the whole family comes from the
// pure-JS compatibility layer - fibjs/scripts/stream.js plus the vendored
// _stream_*.js files).
//
// At runtime `require('stream')` is the base class itself (`Stream`), with the
// family attached as static properties (`Stream.Readable`, `Stream.pipeline`,
// ...). The prototype chain is `Readable -> Stream -> EventEmitter`, where that
// `Stream` is the compatibility layer's JS base class - it is *not* the `io`
// module's Class_Stream, which is the native async-stream base - so the
// declarations below extend Class_EventEmitter and stay faithful to the
// runtime.
//
// The shape - `class Stream` + two merged `namespace Stream` blocks + a
// namespace member `export { Stream }` + `export = Stream` - is the one
// @types/node uses for `node:stream`, and it is the only shape that makes all
// of these work at once:
//
//   import stream = require('stream')   stream.Readable (value)
//   import { Readable } from 'stream'   Readable as a type
//   import stream from 'stream'         the default binding
//   new stream.Stream()                 the base class as a value
//   let s: stream.Stream                the base class as a type
//
// (The `export { Stream }` inside the namespace is what publishes the class
// both as a value and as a type; without it the class value is a property while
// the type is not exported - TS2694/TS2339.)
//
// Scope of this pass (plans/fibjs-upstream-gaps-remediation-2026-10-14.md §7
// P2), beyond the import-resolving surface of the first one:
//
//   - the subclassing contract: `_read`/`_write`/`_transform`/`_flush`/
//     `_final`/`_destroy`, and the option objects the constructors take
//     (`read(size)`, `write(chunk, encoding, callback)`, `transform`, `flush`,
//     `objectMode`, `highWaterMark`, `signal`, ...)
//   - the async iteration protocol: `for await (const chunk of readable)`,
//     `iterator({ destroyOnReturn })` and the operator helpers (`map`,
//     `filter`, `flatMap`, `drop`, `take`, `toArray`, `forEach`, `some`,
//     `every`, `find`, `reduce`). They are implemented in the compat layer
//     (fibjs/scripts/internal/streams/{async_iterator,operators}.js) as of
//     this pass, so these declarations describe the runtime, not a wish.
//   - `objectMode` carries the chunk type: the classes take a type parameter
//     that defaults to `any`, so Node-shaped code (`new stream.Readable()`)
//     keeps working unchanged while `new stream.Readable<string>()` types the
//     chunks.
//
// Not declared because the runtime does not have them (checked with
// temp/p2-surface-2026-10-15.cjs): `readableEnded`, `readableLength`,
// `readableAborted`, `errored`, `closed`, `writableEnded`, `writableFinished`,
// `writableLength`, `writableCorked`, `writableNeedDrain`, `compose`,
// `isReadable`/`isWritable`/`isDisturbed`/`isErrored`,
// `getDefaultHighWaterMark` and `toWeb`. Declaring them would hand back
// `undefined` at runtime.

declare module 'stream' {
    /**
     * @description The options every stream constructor accepts
     */
    interface StreamOptions<T extends Stream = Stream> {
        /** Whether destroy() emits 'close' (on by default). */
        emitClose?: boolean;

        /** The buffer size of one side, in chunks. */
        highWaterMark?: number;

        /** Whether the stream carries values rather than bytes. */
        objectMode?: boolean;

        /** Whether the stream destroys itself once both sides finished. */
        autoDestroy?: boolean;

        /** The AbortSignal that destroys the stream when aborted. */
        signal?: Class_AbortSignal;

        /** The teardown hook, called by destroy(). */
        destroy?: (this: T, error: Error | null, callback: (error?: Error | null) => void) => void;
    }

    /**
     * @description The options a Readable constructor accepts
     */
    interface ReadableOptions<T extends Stream.Readable = Stream.Readable> extends StreamOptions<T> {
        /** The encoding setEncoding() starts with. */
        encoding?: string;

        /** The implementation of _read(size). */
        read?: (this: T, size: number) => void;
    }

    /**
     * @description The options a Writable constructor accepts
     */
    interface WritableOptions<T extends Stream.Writable = Stream.Writable> extends StreamOptions<T> {
        /** Whether written strings are decoded to a Buffer first (on by default). */
        decodeStrings?: boolean;

        /** The encoding end() defaults to. */
        defaultEncoding?: string;

        /** The implementation of _write(chunk, encoding, callback). */
        write?: (this: T, chunk: any, encoding: string, callback: (error?: Error | null) => void) => void;

        /** The implementation of _writev(chunks, callback). */
        writev?: (this: T, chunks: { chunk: any; encoding: string }[], callback: (error?: Error | null) => void) => void;

        /** The implementation of _final(callback). */
        final?: (this: T, callback: (error?: Error | null) => void) => void;
    }

    /**
     * @description The options a Duplex constructor accepts
     *
     *      The readable and the writable side can be configured separately
     *      (`readableObjectMode`, `readableHighWaterMark`, ...), which is what
     *      the layer reads when one options object feeds both sides.
     */
    interface DuplexOptions<T extends Stream.Duplex = Stream.Duplex> extends ReadableOptions<T>, WritableOptions<T> {
        /** Whether the writable side stays open when the readable side ends. */
        allowHalfOpen?: boolean;

        /** Whether the readable side carries values rather than bytes. */
        readableObjectMode?: boolean;

        /** Whether the writable side carries values rather than bytes. */
        writableObjectMode?: boolean;

        /** The buffer size of the readable side. */
        readableHighWaterMark?: number;

        /** The buffer size of the writable side. */
        writableHighWaterMark?: number;
    }

    /** The callback a Transform implementation answers with. */
    type TransformCallback = (error?: Error | null, data?: any) => void;

    /**
     * @description The options a Transform constructor accepts
     */
    interface TransformOptions<T extends Stream.Transform = Stream.Transform> extends DuplexOptions<T> {
        /** The implementation of _transform(chunk, encoding, callback). */
        transform?: (this: T, chunk: any, encoding: string, callback: TransformCallback) => void;

        /** The implementation of _flush(callback). */
        flush?: (this: T, callback: TransformCallback) => void;
    }

    /** The second argument the operator callbacks receive. */
    interface OperatorOptions {
        /** The signal of the operator options, when one was given. */
        signal?: Class_AbortSignal;
    }

    /**
     * @description The options `readable.iterator()` accepts
     */
    interface ReadableIteratorOptions {
        /**
         * Whether `return()` (a `break`, `return` or `throw` out of the
         * `for await` loop) destroys the stream.
         *
         * @default true
         */
        destroyOnReturn?: boolean;
    }

    /**
     * @description The options the readable operators accept
     *
     *      `signal` aborts the operation (the pending promise rejects with an
     *      AbortError). Node's `concurrency` is not implemented - the operators
     *      run one callback at a time, which is Node's default.
     */
    interface ReadableOperatorOptions {
        signal?: Class_AbortSignal;
    }

    /**
     * @description The async iterator of a Readable
     *
     *      What `readable[Symbol.asyncIterator]()` and `readable.iterator()`
     *      return: an async iterable iterator carrying the stream it reads.
     */
    interface ReadableAsyncIterator<T = any> extends AsyncIterableIterator<T> {
        /** The stream the iterator reads from. */
        readonly stream: Stream.Readable<T>;

        /** Advances the iterator; `done` marks the end of the stream. */
        next(...args: [] | [undefined]): Promise<IteratorResult<T>>;

        /**
         * Ends the iteration early. With `destroyOnReturn` (the default) the
         * stream is destroyed, without it the stream is left alone.
         */
        return(value?: any): Promise<IteratorResult<T>>;
    }

    /**
     * @description The events a Readable emits
     *
     *      Used by the typed `on`/`once` overloads below, so that a listener
     *      on 'data' sees the chunk type the class was parameterised with.
     */
    type ReadableEventMap<T = any> = {
        'close': [];
        'data': [chunk: T];
        'end': [];
        'error': [err: Error];
        'pause': [];
        'readable': [];
        'resume': [];
    };

    /**
     * @description The events a Writable emits
     */
    type WritableEventMap<T = any> = {
        'close': [];
        'drain': [];
        'error': [err: Error];
        'finish': [];
        'pipe': [source: any];
        'unpipe': [source: any];
    };

    /**
     * @description The base class of the compatibility layer's stream family
     *
     *      Node's `stream.Stream`; the Readable/Writable/Duplex/Transform/
     *      PassThrough classes below derive from it and it derives from
     *      EventEmitter, so every listener method (`on`, `once`, `off`, ...) is
     *      available on a stream.
     */
    class Stream extends Class_EventEmitter {
        /** Sends the stream's data to the writable `destination`. */
        pipe<D>(destination: D, options?: { end?: boolean }): D;

        /** Removes a destination registered with pipe(). */
        unpipe(destination?: any): this;

        /** Whether the stream has been destroyed. */
        readonly destroyed: boolean;

        /**
         * Destroys the stream, optionally reporting an error.
         *
         *      The callback runs when the teardown finished; the stream then
         *      emits 'close' (and 'error' first, when one was given).
         */
        destroy(error?: Error | null, callback?: (error?: Error | null) => void): this;

        /**
         * Binds an AbortSignal to this stream (the method form of the module's
         * `addAbortSignal(signal, stream)`); aborting destroys the stream.
         */
        addAbortSignal(signal: Class_AbortSignal): this;
    }

    namespace Stream {
        export { Stream };

        // The module publishes its types through the namespace: with
        // `export = Stream` a named import (`import { ReadableOptions } from
        // 'stream'`) resolves against the exported entity's members, so the
        // shapes declared at module level above are re-exported here - the
        // same way `Stream` itself is.
        export {
            StreamOptions,
            ReadableOptions,
            WritableOptions,
            DuplexOptions,
            TransformOptions,
            TransformCallback,
            OperatorOptions,
            ReadableIteratorOptions,
            ReadableOperatorOptions,
            ReadableAsyncIterator,
            ReadableEventMap,
            WritableEventMap,
        };
    }

    namespace Stream {
        /** @description A stream data source (the Node Readable interface) */
        class Readable<T = any> extends Stream {
            constructor(options?: ReadableOptions<any>);

            /** Whether the stream can still be read from. */
            readable: boolean;

            /** The configured buffer size of the readable side. */
            readonly readableHighWaterMark: number;

            /**
             * Adds a chunk of data to the internal queue; `null` signals the end
             * of the stream (after the chunks already buffered were handed out).
             */
            push(chunk: T | null, encoding?: string): boolean;

            /** Puts a chunk back at the front of the internal queue. */
            unshift(chunk: T, encoding?: string): void;

            /**
             * Pulls data out of the internal queue; `null` means "nothing is
             * ready yet" - the stream emits 'readable' when it is.
             */
            read(size?: number): T | null;

            /** Switches the stream to an encoding (`'utf8'`, `'hex'`, ...). */
            setEncoding(encoding: string): this;

            /** Resumes emitting 'data' events. */
            resume(): this;

            /** Stops emitting 'data' events. */
            pause(): this;

            /** Whether the stream stopped emitting 'data' events. */
            isPaused(): boolean;

            /** Wraps an old-style stream, forwarding its events. */
            wrap(stream: any): this;

            /**
             * The implementation hook of a source: push() what was read, then
             * `push(null)` at the end. Subclasses - or the `read` option -
             * override it.
             */
            _read(size: number): void;

            /** The teardown hook; override it to release what the stream holds. */
            _destroy(error: Error | null, callback: (error?: Error | null) => void): void;

            /**
             * @description Iterates the chunks: `for await (const chunk of r)`
             *
             *      Leaving the loop early (break/return/throw) destroys the
             *      stream unless `iterator({ destroyOnReturn: false })` was
             *      used. An error rejects the pending iteration.
             */
            [Symbol.asyncIterator](): ReadableAsyncIterator<T>;

            /** The same iterator, with its options exposed. */
            iterator(options?: ReadableIteratorOptions): ReadableAsyncIterator<T>;

            /** The listeners on the readable events, with the chunk typed. */
            on<K extends keyof ReadableEventMap<T>>(event: K, listener: (...args: ReadableEventMap<T>[K]) => void): this;
            on(ev: any, func: (...args: any[]) => void): FIBJS.GeneralObject;
            on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

            /** The once form of the typed listeners. */
            once<K extends keyof ReadableEventMap<T>>(event: K, listener: (...args: ReadableEventMap<T>[K]) => void): this;
            once(ev: any, func: (...args: any[]) => void): FIBJS.GeneralObject;
            once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

            /** @description The lazily mapped stream of the chunks */
            map<U = any>(fn: (data: T, options?: OperatorOptions) => U | Promise<U>, options?: ReadableOperatorOptions): Readable<U>;

            /** @description The chunks the predicate accepts */
            filter(fn: (data: T, options?: OperatorOptions) => boolean | Promise<boolean>, options?: ReadableOperatorOptions): Readable<T>;

            /** @description The flattened stream of the iterables the callback returns */
            flatMap<U = any>(fn: (data: T, options?: OperatorOptions) => Iterable<U> | AsyncIterable<U>, options?: ReadableOperatorOptions): Readable<U>;

            /** @description Skips the first `n` chunks */
            drop(n: number, options?: ReadableOperatorOptions): Readable<T>;

            /** @description Stops after the first `n` chunks */
            take(n: number, options?: ReadableOperatorOptions): Readable<T>;

            /** @description Collects every chunk (the promise of the array) */
            toArray(options?: ReadableOperatorOptions): Promise<T[]>;

            /** @description Runs the callback for every chunk */
            forEach(fn: (data: T, options?: OperatorOptions) => void | Promise<void>, options?: ReadableOperatorOptions): Promise<void>;

            /** @description Whether the predicate accepts any chunk */
            some(fn: (data: T, options?: OperatorOptions) => boolean | Promise<boolean>, options?: ReadableOperatorOptions): Promise<boolean>;

            /** @description Whether the predicate accepts every chunk */
            every(fn: (data: T, options?: OperatorOptions) => boolean | Promise<boolean>, options?: ReadableOperatorOptions): Promise<boolean>;

            /** @description The first chunk the predicate accepts */
            find(fn: (data: T, options?: OperatorOptions) => boolean | Promise<boolean>, options?: ReadableOperatorOptions): Promise<T | undefined>;

            /**
             * @description Folds the chunks into one value
             *
             *      Without `initial` the first chunk is the initial value; an
             *      empty stream then rejects with ERR_MISSING_ARGS, the way
             *      Node's reduce does.
             */
            reduce(fn: (previous: any, data: T) => any, initial?: any): Promise<any>;

            /**
             * @description Creates a readable from an iterable or an async
             *      iterable (`Readable.from(async function* () { ... }())`)
             */
            static from<T = any>(iterable: Iterable<T> | AsyncIterable<T>, options?: ReadableOptions<any>): Readable<T>;

            /** @description Creates a readable from a Web ReadableStream */
            static fromWeb<T = any>(readableStream: any, options?: ReadableOptions<any>): Readable<T>;
        }

        /** @description A stream data destination (the Node Writable interface) */
        class Writable<T = any> extends Stream {
            constructor(options?: WritableOptions<any>);

            /** Whether the stream can still be written to. */
            writable: boolean;

            /** The configured buffer size of the writable side. */
            readonly writableHighWaterMark: number;

            /**
             * Writes a chunk; the callback runs when the chunk is flushed. It
             * returns false when the internal buffer is full - wait for 'drain'
             * before writing more.
             */
            write(chunk: T, encoding?: string | ((error?: Error | null) => void), callback?: (error?: Error | null) => void): boolean;

            /** Flushes the stream; the callback runs when 'finish' fires. */
            end(chunk?: T, encoding?: string | (() => void), callback?: () => void): this;

            /** Buffers every write until uncork(). */
            cork(): void;

            /** Flushes what cork() buffered. */
            uncork(): void;

            /** Sets the encoding end() defaults to. */
            setDefaultEncoding(encoding: string): this;

            /** The implementation hook of a destination. */
            _write(chunk: T, encoding: string, callback: (error?: Error | null) => void): void;

            /** The batch form of _write, used when one is installed. */
            _writev?(chunks: { chunk: T; encoding: string }[], callback: (error?: Error | null) => void): void;

            /** The hook called once before 'finish'. */
            _final(callback: (error?: Error | null) => void): void;

            /** The teardown hook; override it to release what the stream holds. */
            _destroy(error: Error | null, callback: (error?: Error | null) => void): void;

            /** The listeners on the writable events, with the chunk typed. */
            on<K extends keyof WritableEventMap<T>>(event: K, listener: (...args: WritableEventMap<T>[K]) => void): this;
            on(ev: any, func: (...args: any[]) => void): FIBJS.GeneralObject;
            on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

            /** The once form of the typed listeners. */
            once<K extends keyof WritableEventMap<T>>(event: K, listener: (...args: WritableEventMap<T>[K]) => void): this;
            once(ev: any, func: (...args: any[]) => void): FIBJS.GeneralObject;
            once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;
        }

        /** @description A stream that is both readable and writable */
        class Duplex<T = any> extends Readable<T> {
            constructor(options?: DuplexOptions<any>);

            /** Whether the writable side stays open when the readable side ends. */
            allowHalfOpen: boolean;

            /** Whether the writable side can still be written to. */
            writable: boolean;

            /** The configured buffer size of the writable side. */
            readonly writableHighWaterMark: number;

            /** The listeners of both sides, with the chunk typed. */
            on<K extends keyof (ReadableEventMap<T> & WritableEventMap<T>)>(event: K, listener: (...args: (ReadableEventMap<T> & WritableEventMap<T>)[K]) => void): this;
            on(ev: any, func: (...args: any[]) => void): FIBJS.GeneralObject;
            on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

            /** The once form of the typed listeners. */
            once<K extends keyof (ReadableEventMap<T> & WritableEventMap<T>)>(event: K, listener: (...args: (ReadableEventMap<T> & WritableEventMap<T>)[K]) => void): this;
            once(ev: any, func: (...args: any[]) => void): FIBJS.GeneralObject;
            once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

            write(chunk: T, encoding?: string | ((error?: Error | null) => void), callback?: (error?: Error | null) => void): boolean;
            end(chunk?: T, encoding?: string | (() => void), callback?: () => void): this;
            cork(): void;
            uncork(): void;
            setDefaultEncoding(encoding: string): this;
            _write(chunk: T, encoding: string, callback: (error?: Error | null) => void): void;
            _writev?(chunks: { chunk: T; encoding: string }[], callback: (error?: Error | null) => void): void;
            _final(callback: (error?: Error | null) => void): void;
        }

        /**
         * @description A duplex stream that transforms what is written to it
         *
         *      Subclasses implement `_transform(chunk, encoding, callback)` and
         *      optionally `_flush(callback)`; both can also be given to the
         *      constructor (the `transform`/`flush` options).
         */
        class Transform<T = any> extends Duplex<T> {
            constructor(options?: TransformOptions<any>);

            /** Pushes a transformed chunk onto the readable side. */
            push(chunk: T | null, encoding?: string): boolean;

            /** The implementation hook: transform the chunk, then answer. */
            _transform(chunk: T, encoding: string, callback: TransformCallback): void;

            /** The hook called once before the readable side ends. */
            _flush(callback: TransformCallback): void;
        }

        /** @description A Transform that passes the data through unchanged */
        class PassThrough<T = any> extends Transform<T> {
            constructor(options?: TransformOptions<any>);
        }

        /**
         * @description Pipes a chain of streams
         *
         *      With a trailing callback it reports completion through it and
         *      returns the first stream; without one it returns the Promise of
         *      `stream/promises`'s pipeline.
         */
        function pipeline(...streams: any[]): Promise<void> | any;

        /**
         * @description Waits until a stream is finished
         *
         *      With a trailing callback it reports through it and returns a
         *      cleanup function; without one it returns the Promise of
         *      `stream/promises`'s finished.
         */
        function finished(stream: any, opts?: FIBJS.GeneralObject | ((error?: Error | null) => void), callback?: (error?: Error | null) => void): Promise<void> | (() => void);

        /**
         * @description Binds an AbortSignal to a stream
         *
         *      Node's `stream.addAbortSignal(signal, stream)`: aborting the
         *      signal destroys the stream - immediately when it is already
         *      aborted. Returns the stream, so the call chains. Available as the
         *      method `stream.addAbortSignal(signal)` as well.
         */
        function addAbortSignal<T extends Readable | Writable>(signal: Class_AbortSignal, stream: T): T;

        /** The promise-flavoured helpers: the same object as `stream/promises`. */
        const promises: typeof import('stream/promises');

        /** Internal, kept for compatibility with code probing the Node internals. */
        const _isUint8Array: (obj: any) => boolean;
        const _uint8ArrayToBuffer: (chunk: Uint8Array) => Class_Buffer;
    }

    export = Stream;
}
