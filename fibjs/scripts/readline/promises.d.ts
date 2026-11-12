/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `readline/promises` module (no IDL unit:
// fibjs/scripts/readline/promises.js).

declare module 'readline/promises' {
    import readline = require('readline');

    /** The promise-flavoured options of the `readline/promises` interface. */
    type InterfaceOptions = readline.InterfaceOptions;

    /**
     * @description The promise-flavoured line interface
     *
     *      Same as the callback Interface, except that question() returns a
     *      Promise instead of taking a callback.
     *
     *      An `interface` extending `Omit<..., 'question'>` rather than a
     *      `class ... implements`: `implements` re-types nothing (the class
     *      body would have to redeclare every member, and TS2420 fires when it
     *      declares only some of them), and a plain
     *      `interface ... extends readline.Interface` is TS2430 because the
     *      promise-shaped question is not assignable to the callback-shaped one.
     *      Omitting `question` from the base and re-adding the promise form in
     *      the body is the only spelling that inherits the rest of the surface
     *      while changing one member's signature.
     *
     *      The runtime exports a constructor, so the same name is also
     *      declared as a `const` with a construct signature: the interface
     *      carries the type, the const carries the value, and
     *      `new (require('readline/promises').Interface)(options)` plus
     *      `createInterface(...) instanceof Interface` both keep working.
     */
    interface Interface extends Omit<readline.Interface, 'question'> {
        /** Asks the user a question and resolves with the answer. */
        question(query: string, options?: { signal?: Class_AbortSignal }): Promise<string>;
    }

    const Interface: {
        new (input: any, output?: any, completer?: any, terminal?: boolean): Interface;
        new (options: InterfaceOptions): Interface;
    };

    /** @description Creates a promise-flavoured line interface */
    function createInterface(input: any, output?: any, completer?: any, terminal?: boolean): Interface;
    function createInterface(options: InterfaceOptions): Interface;

    /**
     * @description A batch cursor writer bound to one writable stream
     *
     *      Not the line interface: this is Node's low-level `Readline` class
     *      (`require('readline/promises').Readline`), which queues cursor moves
     *      and writes them with commit(). It needs the stream up front, so
     *      `new Readline(stream)`.
     */
    class Readline {
        constructor(stream: any, options?: { autoCommit?: boolean });

        /** Moves the cursor to the x/y coordinate of the stream. */
        cursorTo(x: number, y?: number): this;

        /** Moves the cursor relative to its current position. */
        moveCursor(dx: number, dy: number): this;

        /** Clears the current line of the stream. */
        clearLine(dir?: -1 | 0 | 1): this;

        /** Clears the whole screen of the stream. */
        clearScreenDown(): this;

        /** Writes the queued cursor moves to the stream; resolves when flushed. */
        commit(): Promise<void>;

        /** Discards the queued cursor moves. */
        rollback(): this;
    }

    export { Interface, InterfaceOptions, createInterface, Readline };
}
