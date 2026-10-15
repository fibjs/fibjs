// Hand-written `stream/web` module (no IDL unit: fibjs/scripts/stream/web.js
// re-exports fibjs/scripts/internal/webstream.js, which also installs these on
// globalThis).
//
// The classes are the WHATWG Streams Standard implementation; the declaration
// below keeps that shape (Node's `node:stream/web` exports exactly this set).
// It is hand-written rather than deferring to the DOM lib because the fibjs
// checker runs an es-only default lib where these globals do not exist.

declare module 'stream/web' {
    /**
     * @description The result of one reader read()
     *
     *      The discriminating union the standard defines: `done: false`
     *      carries the chunk, `done: true` closes the iteration (and Node puts
     *      no `value` there).
     */
    type ReadableStreamReadResult<T> = { done: false; value: T } | { done: true; value?: undefined };

    /**
     * @description The source callbacks a ReadableStream is built from
     *
     *      `start`/`pull`/`cancel` receive the controller, which is what
     *      enqueues chunks - typing it is what makes
     *      `new ReadableStream({ start(c) { c.enqueue('x'); c.close(); } })`
     *      check the chunk type.
     */
    interface UnderlyingSource<R = any> {
        /** The stream kind; 'bytes' installs the BYOB machinery. */
        type?: 'bytes' | undefined;

        /** How many bytes one BYOB request allocates at least. */
        autoAllocateChunkSize?: number;

        /** Runs once, when the stream is built. */
        start?: (controller: ReadableStreamDefaultController<R>) => void | Promise<void>;

        /** Runs when the internal queue needs more data. */
        pull?: (controller: ReadableStreamDefaultController<R>) => void | Promise<void>;

        /** Runs when the consumer cancels the stream. */
        cancel?: (reason?: any) => void | Promise<void>;
    }

    /**
     * @description The sink callbacks a WritableStream is built from
     */
    interface UnderlyingSink<W = any> {
        /** Runs once, when the stream is built. */
        start?: (controller: WritableStreamDefaultController) => void | Promise<void>;

        /** Consumes one chunk. */
        write?: (chunk: W, controller: WritableStreamDefaultController) => void | Promise<void>;

        /** Runs when the stream is closed. */
        close?: () => void | Promise<void>;

        /** Runs when the consumer aborts the stream. */
        abort?: (reason?: any) => void | Promise<void>;
    }

    /**
     * @description The callbacks a TransformStream is built from
     */
    interface Transformer<I = any, O = any> {
        /** Runs once, when the stream is built. */
        start?: (controller: TransformStreamDefaultController<O>) => void | Promise<void>;

        /** Turns one written chunk into zero or more readable chunks. */
        transform?: (chunk: I, controller: TransformStreamDefaultController<O>) => void | Promise<void>;

        /** Runs when the writable side closes, after the last transform. */
        flush?: (controller: TransformStreamDefaultController<O>) => void | Promise<void>;

        /** Runs when the readable side is cancelled. */
        cancel?: (reason?: any) => void | Promise<void>;
    }

    /** @description A source of streaming data (the WHATWG ReadableStream) */
    class ReadableStream<R = any> {
        constructor(underlyingSource?: UnderlyingSource<R>, strategy?: QueuingStrategy<R>);

        /** Whether the stream already has a reader. */
        readonly locked: boolean;

        /** Cancels the stream. */
        cancel(reason?: any): Promise<void>;

        /** Takes the BYOB (byte-oriented) reader. */
        getReader(options: { mode: 'byob' }): ReadableStreamBYOBReader;

        /** Takes the default reader. */
        getReader(options?: { mode?: undefined }): ReadableStreamDefaultReader<R>;

        /** Passes the stream through a transform pair. */
        pipeThrough<O = any>(transform: { readable: ReadableStream<O>; writable: WritableStream<R> }, options?: any): ReadableStream<O>;

        /** Pipes the stream into a writable stream. */
        pipeTo(destination: WritableStream<R>, options?: any): Promise<void>;

        /** Splits the stream into two independent copies. */
        tee(): [ReadableStream<R>, ReadableStream<R>];

        /** The async iterator over the chunks. */
        values(options?: any): AsyncIterableIterator<R>;

        [Symbol.asyncIterator](): AsyncIterableIterator<R>;
    }

    /** @description Reads the chunks of a ReadableStream */
    class ReadableStreamDefaultReader<R = any> {
        constructor(stream: ReadableStream<R>);

        /** Resolves when the stream closes. */
        readonly closed: Promise<void>;

        /** Takes the next chunk (`{ value, done }`). */
        read(): Promise<ReadableStreamReadResult<R>>;

        /** Releases the reader, so another one can take over. */
        releaseLock(): void;

        /** Cancels the stream. */
        cancel(reason?: any): Promise<void>;
    }

    /** @description Controls a ReadableStream from the inside */
    class ReadableStreamDefaultController<R = any> {
        /** How much more the stream wants, or null when it does not. */
        readonly desiredSize: number | null;

        /** Closes the stream. */
        close(): void;

        /** Enqueues a chunk. */
        enqueue(chunk?: R): void;

        /** Errors the stream. */
        error(reason?: any): void;
    }

    /** @description Reads a byte-oriented ReadableStream */
    class ReadableStreamBYOBReader<R = any> {
        constructor(stream: ReadableStream<R>);

        /** Resolves when the stream closes. */
        readonly closed: Promise<void>;

        /** Reads into the given view. */
        read(view: ArrayBufferView): Promise<ReadableStreamReadResult<ArrayBufferView>>;

        /** Releases the reader. */
        releaseLock(): void;

        /** Cancels the stream. */
        cancel(reason?: any): Promise<void>;
    }

    /** @description Controls a byte-oriented ReadableStream from the inside */
    class ReadableByteStreamController {
        /** The pending BYOB request, or null. */
        readonly byobRequest: ReadableStreamBYOBRequest | null;

        /** How much more the stream wants, or null when it does not. */
        readonly desiredSize: number | null;

        close(): void;
        enqueue(chunk: ArrayBufferView): void;
        error(reason?: any): void;
    }

    /** @description A pending read into a caller-supplied buffer */
    class ReadableStreamBYOBRequest {
        /** The view the caller supplied. */
        readonly view: ArrayBufferView | null;

        /** Answers the request with `bytesWritten`. */
        respond(bytesWritten: number): void;

        /** Answers the request with a different view. */
        respondWithNewView(view: ArrayBufferView): void;
    }

    /** @description A destination for streaming data (the WHATWG WritableStream) */
    class WritableStream<W = any> {
        constructor(underlyingSink?: UnderlyingSink<W>, strategy?: QueuingStrategy<W>);

        /** Whether the stream already has a writer. */
        readonly locked: boolean;

        /** Aborts the stream. */
        abort(reason?: any): Promise<void>;

        /** Closes the stream. */
        close(): Promise<void>;

        /** Takes a writer. */
        getWriter(): WritableStreamDefaultWriter<W>;
    }

    /** @description Writes into a WritableStream */
    class WritableStreamDefaultWriter<W = any> {
        constructor(stream: WritableStream<W>);

        /** Resolves when the stream closes. */
        readonly closed: Promise<void>;

        /** How much more the stream wants, or null when it does not. */
        readonly desiredSize: number | null;

        /** Resolves when the internal queue drains. */
        readonly ready: Promise<void>;

        abort(reason?: any): Promise<void>;
        close(): Promise<void>;
        releaseLock(): void;
        write(chunk?: W): Promise<void>;
    }

    /** @description Controls a WritableStream from the inside */
    class WritableStreamDefaultController {
        /** The AbortSignal given to the sink's start(). */
        readonly signal: Class_AbortSignal;

        /** Errors the stream. */
        error(reason?: any): void;
    }

    /** @description A pair of transform streams (the WHATWG TransformStream) */
    class TransformStream<I = any, O = any> {
        constructor(transformer?: Transformer<I, O>, writableStrategy?: QueuingStrategy<I>, readableStrategy?: QueuingStrategy<O>);

        /** The output side. */
        readonly readable: ReadableStream<O>;

        /** The input side. */
        readonly writable: WritableStream<I>;
    }

    /** @description Controls a TransformStream from the inside */
    class TransformStreamDefaultController<O = any> {
        /** How much more the readable side wants. */
        readonly desiredSize: number | null;

        enqueue(chunk?: O): void;
        error(reason?: any): void;
        terminate(): void;
    }

    /** @description The backpressure model of a stream */
    interface QueuingStrategy<T = any> {
        highWaterMark?: number;
        size?: (chunk: T) => number;
        [k: string]: any;
    }

    /** @description Counts the queued chunks as a backpressure measure */
    class CountQueuingStrategy {
        constructor(init: { highWaterMark: number });
        size(chunk: any): number;
    }

    /** @description Measures the queued chunks in bytes for backpressure */
    class ByteLengthQueuingStrategy {
        constructor(init: { highWaterMark: number });
        size(chunk: ArrayBufferView | ArrayBufferLike): number;
    }

    export {
        ReadableStream,
        ReadableStreamDefaultReader,
        ReadableStreamDefaultController,
        ReadableStreamBYOBReader,
        ReadableByteStreamController,
        ReadableStreamBYOBRequest,
        WritableStream,
        WritableStreamDefaultWriter,
        WritableStreamDefaultController,
        TransformStream,
        TransformStreamDefaultController,
        QueuingStrategy,
        CountQueuingStrategy,
        ByteLengthQueuingStrategy,
        ReadableStreamReadResult,
        UnderlyingSource,
        UnderlyingSink,
        Transformer,
    };
}
