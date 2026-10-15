/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `timers/promises` module (no IDL unit:
// fibjs/scripts/timers/promises.js).

declare module 'timers/promises' {
    /** The options both helpers accept: aborting the signal rejects with AbortError. */
    interface TimerOptions {
        signal?: Class_AbortSignal;
        ref?: boolean;
    }

    /**
     * @description Resolves with `value` after `delay` milliseconds (default 1)
     *
     *      An already-aborted `options.signal` rejects immediately; aborting it
     *      later rejects the pending Promise with the signal's reason (an
     *      AbortError by default).
     */
    function setTimeout<T = void>(delay?: number, value?: T, options?: TimerOptions): Promise<T>;

    /**
     * @description Resolves with `value` on the next iteration of the event loop
     *
     *      Same abort semantics as setTimeout. fibjs also uses it to yield to
     *      the fiber scheduler, so `await setImmediate()` lets other fibers run.
     */
    function setImmediate<T = void>(value?: T, options?: TimerOptions): Promise<T>;

    export { setTimeout, setImmediate, TimerOptions };
}
