/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description File operation object, used for binary file read/write
 *
 *  The file operation object is used to operate on binary files; the fs module can be used to open and create files:
 *  ```JavaScript
 *  var f = fs.openFile('test.txt');
 *  ```
 *
 */
declare class Class_FileStream extends Class_SeekableStream {
    /**
     * @description Queries the current file name
     */
    readonly name: string;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *      @param mode the access permission to set
     *
     */
    chmod(mode: number): void;

    chmod(mode: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *      @param mode the access permission to set
     *
     */
    chmodSync(mode: number): void;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *      @param mode the access permission to set
     *
     */
    chmodAsync(mode: number): Promise<void>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * The promise variant of the FileStream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_FileStreamPromise extends Class_SeekableStreamPromise {
    /**
     * @description Queries the current file name
     */
    readonly name: string;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *      @param mode the access permission to set
     *
     */
    chmod(mode: number): Promise<void>;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *      @param mode the access permission to set
     *
     */
    chmodSync(mode: number): void;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *      @param mode the access permission to set
     *
     */
    chmodAsync(mode: number): Promise<void>;

}


declare namespace Class_FileStream {
    const promises: FIBJS.GeneralObject;
}
