/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `stream/promises` module (no IDL unit:
// fibjs/scripts/stream/promises.js).

declare module 'stream/promises' {
    /**
     * @description Waits until a stream is no longer readable, writable, or has
     *      errored
     *
     *      Resolves when the stream finishes ('finish'/'end'), rejects on
     *      'error' and on a premature 'close' of a destroyed stream. An
     *      `opts.signal` aborts the wait.
     */
    function finished(stream: any, opts?: { signal?: Class_AbortSignal }): Promise<void>;

    /**
     * @description Pipes a chain of streams to completion
     *
     *      At least two streams are required; an error anywhere in the chain
     *      destroys the rest and rejects. A trailing
     *      `{ signal: AbortSignal }` option aborts the pipeline.
     */
    function pipeline(...streams: any[]): Promise<void>;

    export { finished, pipeline };
}
