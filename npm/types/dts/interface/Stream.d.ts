/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/StreamReader.d.ts" />
/**
 * @description Stream operation object, used for binary data stream read/write
 *
 * Stream is a base object that defines the standard interface for stream processing and cannot be created independently. Concrete stream objects such as FileStream, MemoryStream and Socket all inherit from Stream.
 *
 * Stream objects provide the following capabilities:
 *
 *  - **Reading**: `read`, `readBuffer` read data of the specified size, `readAll` reads all remaining data; `setEncoding` sets the encoding so that `read` returns strings;
 *  - **Writing**: `write`, `writeBuffer` write data, `copyTo` copies data to the destination stream;
 *  - **Events**: `data`, `close`, `error` events (inherited from EventEmitter);
 *  - **Lifecycle**: `flush` flushes data, `end` ends writing, `close` closes the stream, `destroy` destroys the stream;
 *  - **Process control**: `ref`/`unref` control whether the stream object prevents the fibjs process from exiting;
 *  - **Compatible interfaces**: `resume`, `pause`, `pipe`, `unpipe`, `getReader` (WHATWG ReadableStreamDefaultReader compatible).
 *
 * Read methods return null when there is no data to read or the connection is interrupted.
 */
declare class Class_Stream extends Class_EventEmitter {
    /**
     * @description Queries the file descriptor value of the Stream, implemented by subclasses
     */
    readonly fd: number;

    /**
     * @description Queries whether the stream is writable
     */
    readonly writable: boolean;

    /**
     * @description Queries whether the stream is readable
     */
    readonly readable: boolean;

    /**
     * @description Queries the readable state object of the stream
     */
    readonly _readableState: FIBJS.GeneralObject;

    /**
     * @description Queries the writable state object of the stream
     */
    readonly _writableState: FIBJS.GeneralObject;

    /**
     * @description Reads data of the specified size from the stream
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the data read from the stream. Returns a string if an encoding is set, otherwise returns a Buffer. If there is no data to read, or the connection is interrupted, returns null
     *
     */
    read(bytes?: number): any;

    read(bytes?: number, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Reads data of the specified size from the stream
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the data read from the stream. Returns a string if an encoding is set, otherwise returns a Buffer. If there is no data to read, or the connection is interrupted, returns null
     *
     */
    readSync(bytes?: number): any;

    /**
     * @description Reads data of the specified size from the stream
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the data read from the stream. Returns a string if an encoding is set, otherwise returns a Buffer. If there is no data to read, or the connection is interrupted, returns null
     *
     */
    readAsync(bytes?: number): Promise<any>;

    /**
     * @description Reads data of the specified size from the stream, returned as a Buffer
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the Buffer data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readBuffer(bytes?: number): Class_Buffer;

    readBuffer(bytes?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Reads data of the specified size from the stream, returned as a Buffer
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the Buffer data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readBufferSync(bytes?: number): Class_Buffer;

    /**
     * @description Reads data of the specified size from the stream, returned as a Buffer
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the Buffer data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readBufferAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description Reads all remaining data from the stream
     *      @return returns the data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readAll(): Class_Buffer;

    readAll(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Reads all remaining data from the stream
     *      @return returns the data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readAllSync(): Class_Buffer;

    /**
     * @description Reads all remaining data from the stream
     *      @return returns the data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readAllAsync(): Promise<Class_Buffer>;

    /**
     * @description Sets the encoding of the stream. After setting, read() returns strings instead of Buffer objects
     *      @param encoding the encoding to use, such as 'utf8', 'ascii', 'hex', etc. Pass null to restore Buffer mode
     *      @return returns the current stream object
     *
     */
    setEncoding(encoding: string): Class_Stream;

    /**
     * @description Writes the given binary data to the stream
     *      @param data the Buffer data to write
     *
     */
    writeBuffer(data: Class_Buffer): void;

    writeBuffer(data: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Writes the given binary data to the stream
     *      @param data the Buffer data to write
     *
     */
    writeBufferSync(data: Class_Buffer): void;

    /**
     * @description Writes the given binary data to the stream
     *      @param data the Buffer data to write
     *
     */
    writeBufferAsync(data: Class_Buffer): Promise<void>;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    write(data: Class_Buffer): boolean;

    write(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeSync(data: Class_Buffer): boolean;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeAsync(data: Class_Buffer): Promise<boolean>;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    write(data: Class_Buffer, encoding: string): boolean;

    write(data: Class_Buffer, encoding: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeSync(data: Class_Buffer, encoding: string): boolean;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeAsync(data: Class_Buffer, encoding: string): Promise<boolean>;

    /**
     * @description Writes the given string to the stream
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    write(data: string, encoding?: string): boolean;

    write(data: string, encoding?: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Writes the given string to the stream
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeSync(data: string, encoding?: string): boolean;

    /**
     * @description Writes the given string to the stream
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeAsync(data: string, encoding?: string): Promise<boolean>;

    /**
     * @description Switches the stream to flowing read mode. In fibjs, switching to flowing read mode is irreversible and cannot be switched back to non-flowing read mode.
     *      @return returns the current stream object
     *
     */
    resume(): Class_Stream;

    /**
     * @description Pauses the automatic read mode of the stream. This method is for compatibility only; it currently has no effect
     *      @return returns the current stream object
     *
     */
    pause(): Class_Stream;

    /**
     * @description Pipes stream data to the destination stream. Data is transferred from the source stream to the destination stream in an event-driven way, with backpressure control
     *      @param destination the destination stream object
     *      @param options pipe options, optional
     *      @return returns the destination stream object, supporting chained calls
     *
     */
    pipe(destination: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description Removes all pipe destinations, or only the specified destination. This method is for compatibility only; it currently has no effect
     *      @param destination the specific writable destination to unpipe
     *
     */
    unpipe(destination?: Class_Stream | Class_StreamPromise): void;

    /**
     * @description Ends the stream operation, optionally writing the final data
     *      @return returns an asynchronous object
     *
     */
    end(): number;

    end(callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Ends the stream operation, optionally writing the final data
     *      @return returns an asynchronous object
     *
     */
    endSync(): number;

    /**
     * @description Ends the stream operation, optionally writing the final data
     *      @return returns an asynchronous object
     *
     */
    endAsync(): Promise<number>;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @return returns an asynchronous object
     *
     */
    end(data: Class_Buffer): number;

    end(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @return returns an asynchronous object
     *
     */
    endSync(data: Class_Buffer): number;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @return returns an asynchronous object
     *
     */
    endAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return returns an asynchronous object
     *
     */
    end(data: Class_Buffer, encoding: string): number;

    end(data: Class_Buffer, encoding: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return returns an asynchronous object
     *
     */
    endSync(data: Class_Buffer, encoding: string): number;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return returns an asynchronous object
     *
     */
    endAsync(data: Class_Buffer, encoding: string): Promise<number>;

    /**
     * @description Writes the given string to the stream and ends the stream operation
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns an asynchronous object
     *
     */
    end(data: string, encoding?: string): number;

    end(data: string, encoding?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes the given string to the stream and ends the stream operation
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns an asynchronous object
     *
     */
    endSync(data: string, encoding?: string): number;

    /**
     * @description Writes the given string to the stream and ends the stream operation
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns an asynchronous object
     *
     */
    endAsync(data: string, encoding?: string): Promise<number>;

    /**
     * @description Writes the file buffer content to the physical device
     */
    flush(): void;

    flush(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Writes the file buffer content to the physical device
     */
    flushSync(): void;

    /**
     * @description Writes the file buffer content to the physical device
     */
    flushAsync(): Promise<void>;

    /**
     * @description Closes the current stream object
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the current stream object
     */
    closeSync(): void;

    /**
     * @description Closes the current stream object
     */
    closeAsync(): Promise<void>;

    /**
     * @description Copies stream data to the destination stream
     *      @param stm the destination stream object
     *      @param bytes the number of bytes to copy
     *      @return returns the number of bytes copied
     *
     */
    copyTo(stm: Class_Stream | Class_StreamPromise, bytes?: number): number;

    copyTo(stm: Class_Stream | Class_StreamPromise, bytes?: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Copies stream data to the destination stream
     *      @param stm the destination stream object
     *      @param bytes the number of bytes to copy
     *      @return returns the number of bytes copied
     *
     */
    copyToSync(stm: Class_Stream | Class_StreamPromise, bytes?: number): number;

    /**
     * @description Copies stream data to the destination stream
     *      @param stm the destination stream object
     *      @param bytes the number of bytes to copy
     *      @return returns the number of bytes copied
     *
     */
    copyToAsync(stm: Class_Stream | Class_StreamPromise, bytes?: number): Promise<number>;

    /**
     * @description Queries and binds the stream data event, equivalent to on("data", func);
     *      @param data the data read
     *
     */
    on(event: "data", listener: (data: Class_Buffer)=>void): this;

    once(event: "data", listener: (data: Class_Buffer)=>void): this;

    off(event: "data", listener: (data: Class_Buffer)=>void): this;

    addListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    removeListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    addEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependOnceListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    /**
     * @description Queries and binds the stream data event, equivalent to on("data", func);
     *      @param data the data read
     *
     */
    ondata: ((data: Class_Buffer)=>void) | null;

    /**
     * @description Queries and binds the stream close event, equivalent to on("close", func);
     */
    on(event: "close", listener: ()=>void): this;

    once(event: "close", listener: ()=>void): this;

    off(event: "close", listener: ()=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    /**
     * @description Queries and binds the stream close event, equivalent to on("close", func);
     */
    onclose: (()=>void) | null;

    /**
     * @description Queries and binds the stream error event, equivalent to on("error", func);
     *      @param code the error code
     *
     */
    on(event: "error", listener: (code: number)=>void): this;

    once(event: "error", listener: (code: number)=>void): this;

    off(event: "error", listener: (code: number)=>void): this;

    addListener(event: "error", listener: (code: number)=>void): this;

    removeListener(event: "error", listener: (code: number)=>void): this;

    addEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "error", listener: (code: number)=>void): this;

    prependOnceListener(event: "error", listener: (code: number)=>void): this;

    /**
     * @description Queries and binds the stream error event, equivalent to on("error", func);
     *      @param code the error code
     *
     */
    onerror: ((code: number)=>void) | null;

    /**
     * @description Gets a reader for the stream, compatible with the WHATWG ReadableStreamDefaultReader interface
     *      @return returns a StreamReader object
     *
     */
    getReader(): Class_StreamReader;

    /**
     * @description Keeps the fibjs process alive, preventing it from exiting while the object is bound
     *      @return returns the current object
     *
     */
    ref(): Class_Stream;

    /**
     * @description Allows the fibjs process to exit; allows the fibjs process to exit while the object is bound
     *      @return returns the current object
     *
     */
    unref(): Class_Stream;

    /**
     * @description Destroys the stream. Optionally emits the 'error' event and emits the 'close' event.
     *      After calling, the stream can no longer be used.
     *      @param err optional error object, emitted as the 'error' event
     *      @return returns the current object
     *
     */
    destroy(err?: any): Class_Stream;

    destroy(err?: any, callback: (err: Error | undefined | null, retVal: Class_Stream)=>any): void;

    /**
     * @description Destroys the stream. Optionally emits the 'error' event and emits the 'close' event.
     *      After calling, the stream can no longer be used.
     *      @param err optional error object, emitted as the 'error' event
     *      @return returns the current object
     *
     */
    destroySync(err?: any): Class_Stream;

    /**
     * @description Destroys the stream. Optionally emits the 'error' event and emits the 'close' event.
     *      After calling, the stream can no longer be used.
     *      @param err optional error object, emitted as the 'error' event
     *      @return returns the current object
     *
     */
    destroyAsync(err?: any): Promise<Class_StreamPromise>;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/StreamReader.d.ts" />
/**
 * The promise variant of the Stream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_StreamPromise extends Class_EventEmitter {
    /**
     * @description Queries the file descriptor value of the Stream, implemented by subclasses
     */
    readonly fd: number;

    /**
     * @description Queries whether the stream is writable
     */
    readonly writable: boolean;

    /**
     * @description Queries whether the stream is readable
     */
    readonly readable: boolean;

    /**
     * @description Queries the readable state object of the stream
     */
    readonly _readableState: FIBJS.GeneralObject;

    /**
     * @description Queries the writable state object of the stream
     */
    readonly _writableState: FIBJS.GeneralObject;

    /**
     * @description Reads data of the specified size from the stream
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the data read from the stream. Returns a string if an encoding is set, otherwise returns a Buffer. If there is no data to read, or the connection is interrupted, returns null
     *
     */
    read(bytes?: number): Promise<any>;

    /**
     * @description Reads data of the specified size from the stream
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the data read from the stream. Returns a string if an encoding is set, otherwise returns a Buffer. If there is no data to read, or the connection is interrupted, returns null
     *
     */
    readSync(bytes?: number): any;

    /**
     * @description Reads data of the specified size from the stream
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the data read from the stream. Returns a string if an encoding is set, otherwise returns a Buffer. If there is no data to read, or the connection is interrupted, returns null
     *
     */
    readAsync(bytes?: number): Promise<any>;

    /**
     * @description Reads data of the specified size from the stream, returned as a Buffer
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the Buffer data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readBuffer(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description Reads data of the specified size from the stream, returned as a Buffer
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the Buffer data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readBufferSync(bytes?: number): Class_Buffer;

    /**
     * @description Reads data of the specified size from the stream, returned as a Buffer
     *      @param bytes the amount of data to read; by default a random-sized chunk is read, whose size depends on the device
     *      @return returns the Buffer data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readBufferAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description Reads all remaining data from the stream
     *      @return returns the data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readAll(): Promise<Class_Buffer>;

    /**
     * @description Reads all remaining data from the stream
     *      @return returns the data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readAllSync(): Class_Buffer;

    /**
     * @description Reads all remaining data from the stream
     *      @return returns the data read from the stream; if there is no data to read, or the connection is interrupted, returns null
     *
     */
    readAllAsync(): Promise<Class_Buffer>;

    /**
     * @description Sets the encoding of the stream. After setting, read() returns strings instead of Buffer objects
     *      @param encoding the encoding to use, such as 'utf8', 'ascii', 'hex', etc. Pass null to restore Buffer mode
     *      @return returns the current stream object
     *
     */
    setEncoding(encoding: string): Class_Stream;

    /**
     * @description Writes the given binary data to the stream
     *      @param data the Buffer data to write
     *
     */
    writeBuffer(data: Class_Buffer): Promise<void>;

    /**
     * @description Writes the given binary data to the stream
     *      @param data the Buffer data to write
     *
     */
    writeBufferSync(data: Class_Buffer): void;

    /**
     * @description Writes the given binary data to the stream
     *      @param data the Buffer data to write
     *
     */
    writeBufferAsync(data: Class_Buffer): Promise<void>;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    write(data: Class_Buffer): Promise<boolean>;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeSync(data: Class_Buffer): boolean;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeAsync(data: Class_Buffer): Promise<boolean>;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    write(data: Class_Buffer, encoding: string): Promise<boolean>;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeSync(data: Class_Buffer, encoding: string): boolean;

    /**
     * @description Writes the given data to the stream
     *      @param data the data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeAsync(data: Class_Buffer, encoding: string): Promise<boolean>;

    /**
     * @description Writes the given string to the stream
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    write(data: string, encoding?: string): Promise<boolean>;

    /**
     * @description Writes the given string to the stream
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeSync(data: string, encoding?: string): boolean;

    /**
     * @description Writes the given string to the stream
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return true if the stream wants the calling code to wait for the 'drain' event before writing more data; otherwise false
     *
     */
    writeAsync(data: string, encoding?: string): Promise<boolean>;

    /**
     * @description Switches the stream to flowing read mode. In fibjs, switching to flowing read mode is irreversible and cannot be switched back to non-flowing read mode.
     *      @return returns the current stream object
     *
     */
    resume(): Class_Stream;

    /**
     * @description Pauses the automatic read mode of the stream. This method is for compatibility only; it currently has no effect
     *      @return returns the current stream object
     *
     */
    pause(): Class_Stream;

    /**
     * @description Pipes stream data to the destination stream. Data is transferred from the source stream to the destination stream in an event-driven way, with backpressure control
     *      @param destination the destination stream object
     *      @param options pipe options, optional
     *      @return returns the destination stream object, supporting chained calls
     *
     */
    pipe(destination: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description Removes all pipe destinations, or only the specified destination. This method is for compatibility only; it currently has no effect
     *      @param destination the specific writable destination to unpipe
     *
     */
    unpipe(destination?: Class_Stream | Class_StreamPromise): void;

    /**
     * @description Ends the stream operation, optionally writing the final data
     *      @return returns an asynchronous object
     *
     */
    end(): Promise<number>;

    /**
     * @description Ends the stream operation, optionally writing the final data
     *      @return returns an asynchronous object
     *
     */
    endSync(): number;

    /**
     * @description Ends the stream operation, optionally writing the final data
     *      @return returns an asynchronous object
     *
     */
    endAsync(): Promise<number>;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @return returns an asynchronous object
     *
     */
    end(data: Class_Buffer): Promise<number>;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @return returns an asynchronous object
     *
     */
    endSync(data: Class_Buffer): number;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @return returns an asynchronous object
     *
     */
    endAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return returns an asynchronous object
     *
     */
    end(data: Class_Buffer, encoding: string): Promise<number>;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return returns an asynchronous object
     *
     */
    endSync(data: Class_Buffer, encoding: string): number;

    /**
     * @description Writes the given file buffer to the stream and ends the stream operation
     *      @param data the file buffer data to write
     *      @param encoding the encoding; this parameter is ignored because data is of type Buffer
     *      @return returns an asynchronous object
     *
     */
    endAsync(data: Class_Buffer, encoding: string): Promise<number>;

    /**
     * @description Writes the given string to the stream and ends the stream operation
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns an asynchronous object
     *
     */
    end(data: string, encoding?: string): Promise<number>;

    /**
     * @description Writes the given string to the stream and ends the stream operation
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns an asynchronous object
     *
     */
    endSync(data: string, encoding?: string): number;

    /**
     * @description Writes the given string to the stream and ends the stream operation
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns an asynchronous object
     *
     */
    endAsync(data: string, encoding?: string): Promise<number>;

    /**
     * @description Writes the file buffer content to the physical device
     */
    flush(): Promise<void>;

    /**
     * @description Writes the file buffer content to the physical device
     */
    flushSync(): void;

    /**
     * @description Writes the file buffer content to the physical device
     */
    flushAsync(): Promise<void>;

    /**
     * @description Closes the current stream object
     */
    close(): Promise<void>;

    /**
     * @description Closes the current stream object
     */
    closeSync(): void;

    /**
     * @description Closes the current stream object
     */
    closeAsync(): Promise<void>;

    /**
     * @description Copies stream data to the destination stream
     *      @param stm the destination stream object
     *      @param bytes the number of bytes to copy
     *      @return returns the number of bytes copied
     *
     */
    copyTo(stm: Class_Stream | Class_StreamPromise, bytes?: number): Promise<number>;

    /**
     * @description Copies stream data to the destination stream
     *      @param stm the destination stream object
     *      @param bytes the number of bytes to copy
     *      @return returns the number of bytes copied
     *
     */
    copyToSync(stm: Class_Stream | Class_StreamPromise, bytes?: number): number;

    /**
     * @description Copies stream data to the destination stream
     *      @param stm the destination stream object
     *      @param bytes the number of bytes to copy
     *      @return returns the number of bytes copied
     *
     */
    copyToAsync(stm: Class_Stream | Class_StreamPromise, bytes?: number): Promise<number>;

    /**
     * @description Queries and binds the stream data event, equivalent to on("data", func);
     *      @param data the data read
     *
     */
    ondata: ((data: Class_Buffer)=>void) | null;

    /**
     * @description Queries and binds the stream close event, equivalent to on("close", func);
     */
    onclose: (()=>void) | null;

    /**
     * @description Queries and binds the stream error event, equivalent to on("error", func);
     *      @param code the error code
     *
     */
    onerror: ((code: number)=>void) | null;

    /**
     * @description Gets a reader for the stream, compatible with the WHATWG ReadableStreamDefaultReader interface
     *      @return returns a StreamReader object
     *
     */
    getReader(): Class_StreamReader;

    /**
     * @description Keeps the fibjs process alive, preventing it from exiting while the object is bound
     *      @return returns the current object
     *
     */
    ref(): Class_Stream;

    /**
     * @description Allows the fibjs process to exit; allows the fibjs process to exit while the object is bound
     *      @return returns the current object
     *
     */
    unref(): Class_Stream;

    /**
     * @description Destroys the stream. Optionally emits the 'error' event and emits the 'close' event.
     *      After calling, the stream can no longer be used.
     *      @param err optional error object, emitted as the 'error' event
     *      @return returns the current object
     *
     */
    destroy(err?: any): Promise<Class_StreamPromise>;

    /**
     * @description Destroys the stream. Optionally emits the 'error' event and emits the 'close' event.
     *      After calling, the stream can no longer be used.
     *      @param err optional error object, emitted as the 'error' event
     *      @return returns the current object
     *
     */
    destroySync(err?: any): Class_Stream;

    /**
     * @description Destroys the stream. Optionally emits the 'error' event and emits the 'close' event.
     *      After calling, the stream can no longer be used.
     *      @param err optional error object, emitted as the 'error' event
     *      @return returns the current object
     *
     */
    destroyAsync(err?: any): Promise<Class_StreamPromise>;

}


declare namespace Class_Stream {
    const promises: FIBJS.GeneralObject;
}
