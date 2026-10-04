/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/**
 * @description tty read stream object, used to read from and write to a tty
 *
 *  There is no way to create this class separately; globally there is only the `process.stdin` instance
 *
 *  ```JavaScript
 *  process.stdin.read(1)
 *  ```
 *
 */
declare class Class_TTYInputStream extends Class_Stream {
    /**
     * @description Creates a new TTYInputStream object; the fd parameter specifies the underlying file descriptor
     *      @param fd the underlying file descriptor, which must be a tty device
     *      @param opts options object passed to the Stream constructor
     *
     */
    constructor(fd: number, opts?: FIBJS.GeneralObject);

    /**
     * @description Creates a new TTYInputStream object; the fd parameter specifies the underlying file object
     *      @param fd the underlying file object, which must be a tty device
     *      @param opts options object passed to the Stream constructor
     *
     */
    constructor(fd: Class_FileHandle | Class_FileHandlePromise, opts?: FIBJS.GeneralObject);

    /**
     * @description Always true
     *
     */
    readonly isTTY: boolean;

    /**
     * @description Always true, indicating the stream is readable
     *
     */
    readonly readable: boolean;

    /**
     * @description Queries whether it is in raw mode; when true, it means the tty is configured to operate as a raw device
     *
     */
    readonly isRaw: boolean;

    /**
     * @description Sets whether the tty works in raw mode
     *      @param isRawMode true means it works in raw mode; otherwise it works in the default mode. `readStream.isRaw` is affected by this setting
     *      @return returns itself
     *
     */
    setRawMode(isRawMode: boolean): Class_TTYInputStream;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/**
 * The promise variant of the TTYInputStream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TTYInputStreamPromise extends Class_StreamPromise {
    /**
     * @description Creates a new TTYInputStream object; the fd parameter specifies the underlying file descriptor
     *      @param fd the underlying file descriptor, which must be a tty device
     *      @param opts options object passed to the Stream constructor
     *
     */
    constructor(fd: number, opts?: FIBJS.GeneralObject);

    /**
     * @description Creates a new TTYInputStream object; the fd parameter specifies the underlying file object
     *      @param fd the underlying file object, which must be a tty device
     *      @param opts options object passed to the Stream constructor
     *
     */
    constructor(fd: Class_FileHandle | Class_FileHandlePromise, opts?: FIBJS.GeneralObject);

    /**
     * @description Always true
     *
     */
    readonly isTTY: boolean;

    /**
     * @description Always true, indicating the stream is readable
     *
     */
    readonly readable: boolean;

    /**
     * @description Queries whether it is in raw mode; when true, it means the tty is configured to operate as a raw device
     *
     */
    readonly isRaw: boolean;

    /**
     * @description Sets whether the tty works in raw mode
     *      @param isRawMode true means it works in raw mode; otherwise it works in the default mode. `readStream.isRaw` is affected by this setting
     *      @return returns itself
     *
     */
    setRawMode(isRawMode: boolean): Class_TTYInputStream;

}


declare namespace Class_TTYInputStream {
    const promises: FIBJS.GeneralObject;
}
