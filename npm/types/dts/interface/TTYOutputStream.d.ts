/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
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
     * @description Creates a new TTYOutputStream object; the fd parameter specifies the underlying file descriptor
     *      @param fd the underlying file descriptor, which must be a tty device
     *      @param opts options object passed to the Stream constructor
     *
     */
    constructor(fd: number, opts?: FIBJS.GeneralObject);

    /**
     * @description Creates a new TTYOutputStream object; the fd parameter specifies the underlying file object
     *      @param fd the underlying file object, which must be a tty device
     *      @param opts options object passed to the Stream constructor
     *
     */
    constructor(fd: Class_FileHandle, opts?: FIBJS.GeneralObject);

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
    getWindowSize(): any[];

    /**
     * @description Emitted when the terminal size changes
     */
    on(event: "resize", listener: ()=>void): this;

}

