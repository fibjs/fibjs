/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `readline` module (no IDL unit: fibjs/scripts/readline.js plus
// fibjs/scripts/internal/readline/).

declare module 'readline' {
    /**
     * @description The options createInterface() accepts
     *
     *      `input`/`output` are fibjs streams (`io` module objects); anything
     *      with `on`/`read`/`write` works, the same duck typing Node uses.
     */
    interface InterfaceOptions {
        input?: any;
        output?: any;
        completer?: (line: string) => [string[], string] | Promise<[string[], string]>;
        terminal?: boolean;
        history?: string[];
        historySize?: number;
        removeHistoryDuplicates?: boolean;
        prompt?: string;
        crlfDelay?: number;
        escapeCodeTimeout?: number;
        tabSize?: number;
        signal?: Class_AbortSignal;
        [k: string]: any;
    }

    /**
     * @description A line-oriented read/write interface
     *
     *      Extends EventEmitter, so `rl.on('line', ...)` and the other listener
     *      methods are available; the events Node defines are 'line', 'close',
     *      'pause', 'resume', 'SIGINT' and 'history'. The runtime builds it on
     *      Node's `InterfaceConstructor`, so the members below are the real
     *      surface.
     */
    class Interface extends Class_EventEmitter {
        constructor(input: any, output?: any, completer?: any, terminal?: boolean);
        constructor(options: InterfaceOptions);

        /** Writes `data` to the output, followed by a line break by default. */
        write(data: string | Uint8Array, key?: any): void;

        /** Writes the prompt to the output. */
        prompt(preserveCursor?: boolean): void;

        /** Asks the user a question, calling back with the answer. */
        question(query: string, callback: (answer: string) => void): void;

        /**
         * Asks the user a question, calling back with the answer.
         *
         *      Node's signal form (v17+): aborting the signal cancels the
         *      question. The runtime answers it, so it is typed here.
         */
        question(query: string, options: { signal?: Class_AbortSignal }, callback: (answer: string) => void): void;

        /** Sets the prompt string (the alias of the `prompt` property). */
        setPrompt(prompt: string): void;

        /** Continues the input stream. */
        resume(): this;

        /** Pauses the input stream. */
        pause(): this;

        /** Closes the interface and releases the streams. */
        close(): void;

        /** The terminal cursor position, `{ rows, cols }`. */
        getCursorPos(): Promise<{ rows: number; cols: number }>;

        /** The current line. */
        line: string;

        /** The input stream the interface reads from. */
        input: any;

        /** The output stream the interface writes to. */
        output: any;

        /** Whether the interface is in terminal (raw, line-editing) mode. */
        terminal: boolean;

        /** The completion callback, when one was given. */
        completer: ((line: string) => any) | undefined;

        /** The entered history lines. */
        history: string[];

        /** How many history lines are kept. */
        historySize: number;

        /** Whether a duplicate line replaces the earlier one in the history. */
        removeHistoryDuplicates: boolean;

        /** The input chunk delay that counts as one line break, in ms. */
        crlfDelay: number;

        /** Whether tab completion is enabled. */
        isCompletionEnabled: boolean;

        /** The number of spaces one tab stands for. */
        tabSize: number;

        /** The async iterator over the entered lines. */
        [Symbol.asyncIterator](): AsyncIterableIterator<string>;

        /**
         * @description The listeners of the interface events
         *
         *      'line' carries the entered line, 'history' the history array,
         *      'SIGINT'/'SIGTSTP' nothing; the mirror of the EventEmitter
         *      overloads keeps the permissive form for everything else.
         */
        on<K extends keyof InterfaceEventMap>(event: K, listener: (...args: InterfaceEventMap[K]) => void): this;
        on(ev: any, func: (...args: any[]) => void): FIBJS.GeneralObject;
        on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

        /** The once form of the typed listeners. */
        once<K extends keyof InterfaceEventMap>(event: K, listener: (...args: InterfaceEventMap[K]) => void): this;
        once(ev: any, func: (...args: any[]) => void): FIBJS.GeneralObject;
        once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;
    }

    /** The events an Interface emits. */
    type InterfaceEventMap = {
        /** One entered line, without the line break. */
        'line': [line: string];
        /** The interface was closed. */
        'close': [];
        /** The input stream ran out of data. */
        'pause': [];
        /** The input stream has more data. */
        'resume': [];
        /** The user pressed Ctrl+C. */
        'SIGINT': [];
        /** The user pressed Ctrl+Z. */
        'SIGTSTP': [];
        /** A line was added to the history. */
        'history': [history: string[]];
    };

    /** @description Creates a line-oriented read/write interface */
    function createInterface(input: any, output?: any, completer?: any, terminal?: boolean): Interface;
    function createInterface(options: InterfaceOptions): Interface;

    /** @description Moves the cursor to the given column/row of the stream */
    function cursorTo(stream: any, x: number, y?: number, callback?: () => void): boolean;

    /** @description Moves the cursor the given number of lines up/down */
    function moveCursor(stream: any, dx: number, dy: number, callback?: () => void): boolean;

    /** @description Clears the current line of the stream */
    function clearLine(stream: any, dir: -1 | 0 | 1, callback?: () => void): boolean;

    /** @description Clears the whole screen of the stream */
    function clearScreenDown(stream: any, callback?: () => void): boolean;

    /** @description Makes the stream emit 'keypress' events for the keys read */
    function emitKeypressEvents(stream: any, iface?: Interface): void;

    /** The promise-flavoured interface: the same object as `readline/promises`. */
    const promises: typeof import('readline/promises');

    export {
        Interface,
        InterfaceOptions,
        createInterface,
        cursorTo,
        moveCursor,
        clearLine,
        clearScreenDown,
        emitKeypressEvents,
        promises,
    };
}
