/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description Memory stream object
 *
 *  The MemoryStream object creates a memory-based stream object. Creation method:
 *  ```JavaScript
 *  var ms = new io.MemoryStream();
 *  ```
 *
 */
declare class Class_MemoryStream extends Class_SeekableStream {
    /**
     * @description MemoryStream constructor
     */
    constructor();

    /**
     * @description Forces the last update time of the memory stream object
     *      @param d the time to set
     *
     */
    setTime(d: typeof Date): void;

    /**
     * @description Creates a read-only copy of the current memory stream
     *      @return returns a read-only memory stream object
     *
     */
    clone(): Class_MemoryStream;

    /**
     * @description Clears the memory file data and resets the pointer
     */
    clear(): void;

}

