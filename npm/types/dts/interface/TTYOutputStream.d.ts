/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description tty write stream object, used to handle tty output
 *
 *  There is no way to create this class separately; globally there is only the `process.stdout` instance
 *
 *  ```JavaScript
 *  // clear line
 *  process.stdout.clearLine(1)
 *  ```
 *
 */
declare class Class_TTYOutputStream extends Class_Stream {
    /**
     * @description Creates a new TTYOutputStream object; the fd parameter specifies the underlying file descriptor or file object
     *      @param fd the underlying file descriptor or file handle object, which must be a tty device
     *      @param opts options object passed to the Stream constructor
     *
     */
    constructor(fd: number | Class_FileHandle | Class_FileHandlePromise, opts?: FIBJS.GeneralObject);

    /**
     * @description Always true
     *
     */
    readonly isTTY: boolean;

    /**
     * @description Returns the number of columns of the terminal corresponding to this TTYOutputStream
     *
     */
    readonly columns: number;

    /**
     * @description Returns the number of rows of the terminal corresponding to this TTYOutputStream
     *
     */
    readonly rows: number;

    /**
     * @description Clears the line according to the direction indicated by dir
     *
     *      Directions of dir:
     *      - -1: clear from the cursor to the beginning of the line
     *      - 0: clear the entire line
     *      - 1: clear from the cursor to the end of the line
     *
     *      @param dir clearing direction
     *
     */
    clearLine(dir?: number): void;

    /**
     * @description Clears the characters from the cursor to the end of the screen
     *
     */
    clearScreenDown(): void;

    /**
     * @description Moves the cursor to the specified position
     *      @param x the column number
     *      @param y the row number, default -1, meaning the row number is not changed
     *
     */
    cursorTo(x: number, y?: number): void;

    cursorTo(x: number, y?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Moves the cursor to the specified position
     *      @param x the column number
     *      @param y the row number, default -1, meaning the row number is not changed
     *
     */
    cursorToSync(x: number, y?: number): void;

    /**
     * @description Moves the cursor to the specified position
     *      @param x the column number
     *      @param y the row number, default -1, meaning the row number is not changed
     *
     */
    cursorToAsync(x: number, y?: number): Promise<void>;

    /**
     * @description Moves the cursor by the specified offset
     *      @param dx the column offset
     *      @param dy the row offset
     *
     */
    moveCursor(dx: number, dy: number): void;

    moveCursor(dx: number, dy: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Moves the cursor by the specified offset
     *      @param dx the column offset
     *      @param dy the row offset
     *
     */
    moveCursorSync(dx: number, dy: number): void;

    /**
     * @description Moves the cursor by the specified offset
     *      @param dx the column offset
     *      @param dy the row offset
     *
     */
    moveCursorAsync(dx: number, dy: number): Promise<void>;

    /**
     * @description Returns the size of the terminal corresponding to this TTYOutputStream
     *      @return returns the array [numColumns, numRows], where numColumns and numRows are the number of columns and rows in the corresponding terminal
     *
     */
    getWindowSize(): number[];

    /**
     * @description Emitted when the terminal size changes
     */
    on(event: "resize", listener: ()=>void): this;

    once(event: "resize", listener: ()=>void): this;

    off(event: "resize", listener: ()=>void): this;

    addListener(event: "resize", listener: ()=>void): this;

    removeListener(event: "resize", listener: ()=>void): this;

    addEventListener(event: "resize", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "resize", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "resize", listener: ()=>void): this;

    prependOnceListener(event: "resize", listener: ()=>void): this;

    /**
     * @description Emitted when the terminal size changes
     */
    onresize: (()=>void) | null;

    on(event: "data", listener: (data: Class_Buffer)=>void): this;

    on(event: "close", listener: ()=>void): this;

    on(event: "error", listener: (code: number)=>void): this;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(event: "data", listener: (data: Class_Buffer)=>void): this;

    once(event: "close", listener: ()=>void): this;

    once(event: "error", listener: (code: number)=>void): this;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(event: "data", listener: (data: Class_Buffer)=>void): this;

    off(event: "close", listener: ()=>void): this;

    off(event: "error", listener: (code: number)=>void): this;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    addListener(event: "error", listener: (code: number)=>void): this;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    removeListener(event: "error", listener: (code: number)=>void): this;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependListener(event: "error", listener: (code: number)=>void): this;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "error", listener: (code: number)=>void): this;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/**
 * The promise variant of the TTYOutputStream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TTYOutputStreamPromise extends Class_StreamPromise {
    /**
     * @description Creates a new TTYOutputStream object; the fd parameter specifies the underlying file descriptor or file object
     *      @param fd the underlying file descriptor or file handle object, which must be a tty device
     *      @param opts options object passed to the Stream constructor
     *
     */
    constructor(fd: number | Class_FileHandle | Class_FileHandlePromise, opts?: FIBJS.GeneralObject);

    /**
     * @description Always true
     *
     */
    readonly isTTY: boolean;

    /**
     * @description Returns the number of columns of the terminal corresponding to this TTYOutputStream
     *
     */
    readonly columns: number;

    /**
     * @description Returns the number of rows of the terminal corresponding to this TTYOutputStream
     *
     */
    readonly rows: number;

    /**
     * @description Clears the line according to the direction indicated by dir
     *
     *      Directions of dir:
     *      - -1: clear from the cursor to the beginning of the line
     *      - 0: clear the entire line
     *      - 1: clear from the cursor to the end of the line
     *
     *      @param dir clearing direction
     *
     */
    clearLine(dir?: number): void;

    /**
     * @description Clears the characters from the cursor to the end of the screen
     *
     */
    clearScreenDown(): void;

    /**
     * @description Moves the cursor to the specified position
     *      @param x the column number
     *      @param y the row number, default -1, meaning the row number is not changed
     *
     */
    cursorTo(x: number, y?: number): Promise<void>;

    /**
     * @description Moves the cursor to the specified position
     *      @param x the column number
     *      @param y the row number, default -1, meaning the row number is not changed
     *
     */
    cursorToSync(x: number, y?: number): void;

    /**
     * @description Moves the cursor to the specified position
     *      @param x the column number
     *      @param y the row number, default -1, meaning the row number is not changed
     *
     */
    cursorToAsync(x: number, y?: number): Promise<void>;

    /**
     * @description Moves the cursor by the specified offset
     *      @param dx the column offset
     *      @param dy the row offset
     *
     */
    moveCursor(dx: number, dy: number): Promise<void>;

    /**
     * @description Moves the cursor by the specified offset
     *      @param dx the column offset
     *      @param dy the row offset
     *
     */
    moveCursorSync(dx: number, dy: number): void;

    /**
     * @description Moves the cursor by the specified offset
     *      @param dx the column offset
     *      @param dy the row offset
     *
     */
    moveCursorAsync(dx: number, dy: number): Promise<void>;

    /**
     * @description Returns the size of the terminal corresponding to this TTYOutputStream
     *      @return returns the array [numColumns, numRows], where numColumns and numRows are the number of columns and rows in the corresponding terminal
     *
     */
    getWindowSize(): number[];

    /**
     * @description Emitted when the terminal size changes
     */
    onresize: (()=>void) | null;

}


declare namespace Class_TTYOutputStream {
    const promises: FIBJS.GeneralObject;
}
