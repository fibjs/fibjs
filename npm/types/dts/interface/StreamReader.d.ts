/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description StreamReader object, a lightweight reader compatible with the WHATWG ReadableStreamDefaultReader interface
 *
 * StreamReader wraps the fibjs Stream with a read() method compatible with the Web Streams API.
 * It can be obtained via Stream.getReader().
 *
 * ```JavaScript
 * const response = await fetch('http://example.com');
 * const reader = response.body.getReader();
 * while (true) {
 *     const { done, value } = await reader.read();
 *     if (done) break;
 *     console.log(value);
 * }
 * ```
 *
 */
declare class Class_StreamReader extends Class_object {
    /**
     * @description Reads the next chunk of data from the stream
     *      @return returns an object containing the `done` (boolean) and `value` (Buffer data or undefined) properties
     *
     */
    read(): Promise<{
        done: boolean;
        value: Class_Buffer;
    }>;

    /**
     * @description Reads the next chunk of data from the stream
     *      @return returns an object containing the `done` (boolean) and `value` (Buffer data or undefined) properties
     *
     */
    readSync(): {
        done: boolean;
        value: Class_Buffer;
    };

    /**
     * @description Reads the next chunk of data from the stream
     *      @return returns an object containing the `done` (boolean) and `value` (Buffer data or undefined) properties
     *
     */
    readAsync(): Promise<{
        done: boolean;
        value: Class_Buffer;
    }>;

    /**
     * @description Releases the lock on the stream
     */
    releaseLock(): void;

    /**
     * @description Cancels the stream and releases the lock
     *      @param reason optional cancellation reason
     *      @return returns a Promise that resolves when cancellation completes
     *
     */
    cancel(reason?: string): Promise<void>;

    /**
     * @description Cancels the stream and releases the lock
     *      @param reason optional cancellation reason
     *      @return returns a Promise that resolves when cancellation completes
     *
     */
    cancelSync(reason?: string): void;

    /**
     * @description Cancels the stream and releases the lock
     *      @param reason optional cancellation reason
     *      @return returns a Promise that resolves when cancellation completes
     *
     */
    cancelAsync(reason?: string): Promise<void>;

    /**
     * @description A Promise that resolves when the stream closes
     */
    readonly closed: Promise;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the StreamReader class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_StreamReaderPromise extends Class_object {
    /**
     * @description Reads the next chunk of data from the stream
     *      @return returns an object containing the `done` (boolean) and `value` (Buffer data or undefined) properties
     *
     */
    read(): Promise<{
        done: boolean;
        value: Class_Buffer;
    }>;

    /**
     * @description Reads the next chunk of data from the stream
     *      @return returns an object containing the `done` (boolean) and `value` (Buffer data or undefined) properties
     *
     */
    readSync(): {
        done: boolean;
        value: Class_Buffer;
    };

    /**
     * @description Reads the next chunk of data from the stream
     *      @return returns an object containing the `done` (boolean) and `value` (Buffer data or undefined) properties
     *
     */
    readAsync(): Promise<{
        done: boolean;
        value: Class_Buffer;
    }>;

    /**
     * @description Releases the lock on the stream
     */
    releaseLock(): void;

    /**
     * @description Cancels the stream and releases the lock
     *      @param reason optional cancellation reason
     *      @return returns a Promise that resolves when cancellation completes
     *
     */
    cancel(reason?: string): Promise<void>;

    /**
     * @description Cancels the stream and releases the lock
     *      @param reason optional cancellation reason
     *      @return returns a Promise that resolves when cancellation completes
     *
     */
    cancelSync(reason?: string): void;

    /**
     * @description Cancels the stream and releases the lock
     *      @param reason optional cancellation reason
     *      @return returns a Promise that resolves when cancellation completes
     *
     */
    cancelAsync(reason?: string): Promise<void>;

    /**
     * @description A Promise that resolves when the stream closes
     */
    readonly closed: Promise;

}


declare namespace Class_StreamReader {
    const promises: FIBJS.GeneralObject;
}
