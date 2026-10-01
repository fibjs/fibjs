/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/TTYInputStream.d.ts" />
/// <reference path="../interface/TTYOutputStream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/**
 * @description tty module
 *
 *  Usage:
 *  ```JavaScript
 *  const tty = require('tty');
 *  ```
 *
 */
declare module 'tty' {
    /**
     * @description TTY input stream object, see TTYInputStream
     */
    const ReadStream: typeof Class_TTYInputStream;

    /**
     * @description TTY output stream object, see TTYOutputStream
     */
    const WriteStream: typeof Class_TTYOutputStream;

    /**
     * @description Queries whether it is a command interactive window
     *     @param fd file descriptor
     *      @return returns true if the file descriptor is associated with a terminal window, otherwise returns false
     *
     */
    function isatty(fd: number): boolean;

    /**
     * @description Queries whether it is a command interactive window
     *     @param fd file handle object
     *      @return returns true if the file handle is associated with a terminal window, otherwise returns false
     *
     */
    function isatty(fd: Class_FileHandle): boolean;

}

