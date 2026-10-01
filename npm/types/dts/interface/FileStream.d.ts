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

