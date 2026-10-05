/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/**
 * @description Basic message object
 *
 *  The Message object is compatible with all mq modules and can be used to build a custom message processing system. It is created as follows:
 *  ```JavaScript
 *  var mq = require("mq");
 *  var m = new mq.Message();
 *  ```
 *
 */
declare class Class_Message extends Class_EventEmitter {
    /**
     * @description Message type 1, representing a text type
     */
    static readonly TEXT: 1;

    /**
     * @description Message type 2, representing a binary type
     */
    static readonly BINARY: 2;

    /**
     * @description Message object constructor
     */
    constructor();

    /**
     * @description Whether the current message has been sent
     */
    readonly sent: boolean;

    /**
     * @description The basic content of the message
     */
    value: string;

    /**
     * @description The basic parameters of the message
     */
    readonly params: any[];

    /**
     * @description Message type
     */
    type: number;

    /**
     * @description The stream object containing the data part of the message
     */
    body: Class_Stream;

    /**
     * @description Queries whether the body of the message has been consumed
     */
    readonly bodyUsed: boolean;

    /**
     * @description Reads the specified amount of data from the stream; this method is an alias of the corresponding body method
     *      @param bytes the amount of data to read; the default is to read a data block of random size, and the size of the data read depends on the device
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    read(bytes?: number): Class_Buffer;

    read(bytes?: number, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Reads the specified amount of data from the stream; this method is an alias of the corresponding body method
     *      @param bytes the amount of data to read; the default is to read a data block of random size, and the size of the data read depends on the device
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readSync(bytes?: number): Class_Buffer;

    /**
     * @description Reads the specified amount of data from the stream; this method is an alias of the corresponding body method
     *      @param bytes the amount of data to read; the default is to read a data block of random size, and the size of the data read depends on the device
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description Reads all remaining data from the stream; this method is an alias of the corresponding body method
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readAll(): Class_Buffer;

    readAll(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Reads all remaining data from the stream; this method is an alias of the corresponding body method
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readAllSync(): Class_Buffer;

    /**
     * @description Reads all remaining data from the stream; this method is an alias of the corresponding body method
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readAllAsync(): Promise<Class_Buffer>;

    /**
     * @description Sets the encoding of the message body; this method is an alias of the corresponding body method
     *
     *      After setting, the `data` event and `read()` return strings instead of Buffer objects,
     *      consistent with the behavior of Node's IncomingMessage.setEncoding
     *      @param encoding the encoding to use, such as 'utf8', 'ascii', 'hex', etc. Pass null to restore Buffer mode
     *      @return returns the current message object
     *
     */
    setEncoding(encoding: string): Class_Message;

    /**
     * @description Writes the given data; this method is an alias of the corresponding body method; a string data is encoded as utf8
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    write(data: Class_Buffer | string): number;

    write(data: Class_Buffer | string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes the given data; this method is an alias of the corresponding body method; a string data is encoded as utf8
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    writeSync(data: Class_Buffer | string): number;

    /**
     * @description Writes the given data; this method is an alias of the corresponding body method; a string data is encoded as utf8
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    writeAsync(data: Class_Buffer | string): Promise<number>;

    /**
     * @description Writes the given text data
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    text(data: string): string;

    text(data: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Writes the given text data
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    textSync(data: string): string;

    /**
     * @description Writes the given text data
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    textAsync(data: string): Promise<string>;

    /**
     * @description Parses the data in the message as text encoding
     *      @return returns the parsing result
     *
     */
    text(): string;

    text(callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description Parses the data in the message as text encoding
     *      @return returns the parsing result
     *
     */
    textSync(): string;

    /**
     * @description Parses the data in the message as text encoding
     *      @return returns the parsing result
     *
     */
    textAsync(): Promise<string>;

    /**
     * @description Returns the data part of the message in binary form
     *      @return returns an ArrayBuffer object containing the data part of the message
     *
     */
    arrayBuffer(): ArrayBuffer;

    arrayBuffer(callback: (err: Error | undefined | null, retVal: ArrayBuffer)=>any): void;

    /**
     * @description Returns the data part of the message in binary form
     *      @return returns an ArrayBuffer object containing the data part of the message
     *
     */
    arrayBufferSync(): ArrayBuffer;

    /**
     * @description Returns the data part of the message in binary form
     *      @return returns an ArrayBuffer object containing the data part of the message
     *
     */
    arrayBufferAsync(): Promise<ArrayBuffer>;

    /**
     * @description Returns the data part of the message as a Blob
     *      @param type the MIME type of the Blob, default is an empty string
     *      @return returns a Blob object containing the data part of the message
     *
     */
    blob(type?: string): Class_Blob;

    blob(type?: string, callback: (err: Error | undefined | null, retVal: Class_Blob)=>any): void;

    /**
     * @description Returns the data part of the message as a Blob
     *      @param type the MIME type of the Blob, default is an empty string
     *      @return returns a Blob object containing the data part of the message
     *
     */
    blobSync(type?: string): Class_Blob;

    /**
     * @description Returns the data part of the message as a Blob
     *      @param type the MIME type of the Blob, default is an empty string
     *      @return returns a Blob object containing the data part of the message
     *
     */
    blobAsync(type?: string): Promise<Class_BlobPromise>;

    /**
     * @description Returns the data part of the message as a Buffer
     *      @return returns a Buffer containing the data part of the message, or an empty Buffer if there is no data
     *
     */
    bytes(): Class_Buffer;

    bytes(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Returns the data part of the message as a Buffer
     *      @return returns a Buffer containing the data part of the message, or an empty Buffer if there is no data
     *
     */
    bytesSync(): Class_Buffer;

    /**
     * @description Returns the data part of the message as a Buffer
     *      @return returns a Buffer containing the data part of the message, or an empty Buffer if there is no data
     *
     */
    bytesAsync(): Promise<Class_Buffer>;

    /**
     * @description Writes the given data with JSON encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    json(data: any): any;

    json(data: any, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Writes the given data with JSON encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    jsonSync(data: any): any;

    /**
     * @description Writes the given data with JSON encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    jsonAsync(data: any): Promise<any>;

    /**
     * @description Parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    json(): any;

    json(callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    jsonSync(): any;

    /**
     * @description Parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    jsonAsync(): Promise<any>;

    /**
     * @description Writes the given data with msgpack encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    pack(data: any): any;

    pack(data: any, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Writes the given data with msgpack encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    packSync(data: any): any;

    /**
     * @description Writes the given data with msgpack encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    packAsync(data: any): Promise<any>;

    /**
     * @description Parses the data in the message as msgpack
     *      @return returns the parsing result
     *
     */
    pack(): any;

    pack(callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Parses the data in the message as msgpack
     *      @return returns the parsing result
     *
     */
    packSync(): any;

    /**
     * @description Parses the data in the message as msgpack
     *      @return returns the parsing result
     *
     */
    packAsync(): Promise<any>;

    /**
     * @description The length of the data part of the message
     */
    readonly length: number;

    /**
     * @description Sets the end of current message processing; the Chain handler no longer continues with subsequent transactions
     *      @return returns 0 on success
     *
     */
    end(): number;

    end(callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Sets the end of current message processing; the Chain handler no longer continues with subsequent transactions
     *      @return returns 0 on success
     *
     */
    endSync(): number;

    /**
     * @description Sets the end of current message processing; the Chain handler no longer continues with subsequent transactions
     *      @return returns 0 on success
     *
     */
    endAsync(): Promise<number>;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @return returns 0 on success
     *
     */
    end(data: Class_Buffer): number;

    end(data: Class_Buffer, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @return returns 0 on success
     *
     */
    endSync(data: Class_Buffer): number;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @return returns 0 on success
     *
     */
    endAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @param encoding the encoding to use; since data is a Buffer, this parameter is ignored
     *      @return returns 0 on success
     *
     */
    end(data: Class_Buffer, encoding: string): number;

    end(data: Class_Buffer, encoding: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @param encoding the encoding to use; since data is a Buffer, this parameter is ignored
     *      @return returns 0 on success
     *
     */
    endSync(data: Class_Buffer, encoding: string): number;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @param encoding the encoding to use; since data is a Buffer, this parameter is ignored
     *      @return returns 0 on success
     *
     */
    endAsync(data: Class_Buffer, encoding: string): Promise<number>;

    /**
     * @description Writes the given string data and sets the end of current message processing
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns 0 on success
     *
     */
    end(data: string, encoding?: string): number;

    end(data: string, encoding?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes the given string data and sets the end of current message processing
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns 0 on success
     *
     */
    endSync(data: string, encoding?: string): number;

    /**
     * @description Writes the given string data and sets the end of current message processing
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns 0 on success
     *
     */
    endAsync(data: string, encoding?: string): Promise<number>;

    /**
     * @description Queries whether the current message has ended
     *      @return returns true if ended
     *
     */
    isEnded(): boolean;

    /**
     * @description Clears the content of the message
     */
    clear(): void;

    /**
     * @description Sends a formatted message to the given stream object
     *      @param stm the stream object that receives the formatted message
     *      @param options the sending options
     *
     */
    sendTo(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): void;

    sendTo(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sends a formatted message to the given stream object
     *      @param stm the stream object that receives the formatted message
     *      @param options the sending options
     *
     */
    sendToSync(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): void;

    /**
     * @description Sends a formatted message to the given stream object
     *      @param stm the stream object that receives the formatted message
     *      @param options the sending options
     *
     */
    sendToAsync(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Reads a formatted message from the given cached stream object and parses and fills the object
     *      @param stm the stream object from which the formatted message is read
     *      @param options the reading options
     *
     */
    readFrom(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): void;

    readFrom(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Reads a formatted message from the given cached stream object and parses and fills the object
     *      @param stm the stream object from which the formatted message is read
     *      @param options the reading options
     *
     */
    readFromSync(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): void;

    /**
     * @description Reads a formatted message from the given cached stream object and parses and fills the object
     *      @param stm the stream object from which the formatted message is read
     *      @param options the reading options
     *
     */
    readFromAsync(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Queries the stream object used when the message was read from
     */
    readonly stream: Class_Stream;

    /**
     * @description Queries and sets the last error of message processing
     */
    lastError: string;

    /**
     * @description Copies the current message object
     *      @return returns the copied message object
     *
     */
    clone(): Class_Message;

    /**
     * @description Switches the message body stream to flowing read mode
     *      @return returns the message object
     *
     */
    resume(): Class_Message;

    /**
     * @description Pauses the automatic reading mode of the message body stream. This method is for compatibility only and has no actual effect when called
     *      @return returns the message object
     *
     */
    pause(): Class_Message;

    /**
     * @description Pipes the message body stream data to a destination stream
     *      @param destination the destination stream object
     *      @param options pipe options, optional
     *      @return returns the destination stream object
     *
     */
    pipe(destination: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description Removes all pipe destinations of the message body stream. This method is for compatibility only and has no actual effect when called
     *      @param destination a specific writable destination to unpipe
     *
     */
    unpipe(destination?: Class_Stream | Class_StreamPromise): void;

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
     *      @param code error code
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
     *      @param code error code
     *
     */
    onerror: ((code: number)=>void) | null;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/**
 * The promise variant of the Message class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_MessagePromise extends Class_EventEmitter {
    /**
     * @description Message type 1, representing a text type
     */
    static readonly TEXT: 1;

    /**
     * @description Message type 2, representing a binary type
     */
    static readonly BINARY: 2;

    /**
     * @description Message object constructor
     */
    constructor();

    /**
     * @description Whether the current message has been sent
     */
    readonly sent: boolean;

    /**
     * @description The basic content of the message
     */
    value: string;

    /**
     * @description The basic parameters of the message
     */
    readonly params: any[];

    /**
     * @description Message type
     */
    type: number;

    /**
     * @description The stream object containing the data part of the message
     */
    body: Class_StreamPromise;

    /**
     * @description Queries whether the body of the message has been consumed
     */
    readonly bodyUsed: boolean;

    /**
     * @description Reads the specified amount of data from the stream; this method is an alias of the corresponding body method
     *      @param bytes the amount of data to read; the default is to read a data block of random size, and the size of the data read depends on the device
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    read(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description Reads the specified amount of data from the stream; this method is an alias of the corresponding body method
     *      @param bytes the amount of data to read; the default is to read a data block of random size, and the size of the data read depends on the device
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readSync(bytes?: number): Class_Buffer;

    /**
     * @description Reads the specified amount of data from the stream; this method is an alias of the corresponding body method
     *      @param bytes the amount of data to read; the default is to read a data block of random size, and the size of the data read depends on the device
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readAsync(bytes?: number): Promise<Class_Buffer>;

    /**
     * @description Reads all remaining data from the stream; this method is an alias of the corresponding body method
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readAll(): Promise<Class_Buffer>;

    /**
     * @description Reads all remaining data from the stream; this method is an alias of the corresponding body method
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readAllSync(): Class_Buffer;

    /**
     * @description Reads all remaining data from the stream; this method is an alias of the corresponding body method
     *      @return returns the data read from the stream, or null if no data is available or the connection is interrupted
     *
     */
    readAllAsync(): Promise<Class_Buffer>;

    /**
     * @description Sets the encoding of the message body; this method is an alias of the corresponding body method
     *
     *      After setting, the `data` event and `read()` return strings instead of Buffer objects,
     *      consistent with the behavior of Node's IncomingMessage.setEncoding
     *      @param encoding the encoding to use, such as 'utf8', 'ascii', 'hex', etc. Pass null to restore Buffer mode
     *      @return returns the current message object
     *
     */
    setEncoding(encoding: string): Class_Message;

    /**
     * @description Writes the given data; this method is an alias of the corresponding body method; a string data is encoded as utf8
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    write(data: Class_Buffer | string): Promise<number>;

    /**
     * @description Writes the given data; this method is an alias of the corresponding body method; a string data is encoded as utf8
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    writeSync(data: Class_Buffer | string): number;

    /**
     * @description Writes the given data; this method is an alias of the corresponding body method; a string data is encoded as utf8
     *      data may be a Buffer or a string; a string is encoded as utf8.
     *      @param data the data to write
     *      @return returns the number of bytes actually written
     *
     */
    writeAsync(data: Class_Buffer | string): Promise<number>;

    /**
     * @description Writes the given text data
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    text(data: string): Promise<string>;

    /**
     * @description Writes the given text data
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    textSync(data: string): string;

    /**
     * @description Writes the given text data
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    textAsync(data: string): Promise<string>;

    /**
     * @description Parses the data in the message as text encoding
     *      @return returns the parsing result
     *
     */
    text(): Promise<string>;

    /**
     * @description Parses the data in the message as text encoding
     *      @return returns the parsing result
     *
     */
    textSync(): string;

    /**
     * @description Parses the data in the message as text encoding
     *      @return returns the parsing result
     *
     */
    textAsync(): Promise<string>;

    /**
     * @description Returns the data part of the message in binary form
     *      @return returns an ArrayBuffer object containing the data part of the message
     *
     */
    arrayBuffer(): Promise<ArrayBuffer>;

    /**
     * @description Returns the data part of the message in binary form
     *      @return returns an ArrayBuffer object containing the data part of the message
     *
     */
    arrayBufferSync(): ArrayBuffer;

    /**
     * @description Returns the data part of the message in binary form
     *      @return returns an ArrayBuffer object containing the data part of the message
     *
     */
    arrayBufferAsync(): Promise<ArrayBuffer>;

    /**
     * @description Returns the data part of the message as a Blob
     *      @param type the MIME type of the Blob, default is an empty string
     *      @return returns a Blob object containing the data part of the message
     *
     */
    blob(type?: string): Promise<Class_BlobPromise>;

    /**
     * @description Returns the data part of the message as a Blob
     *      @param type the MIME type of the Blob, default is an empty string
     *      @return returns a Blob object containing the data part of the message
     *
     */
    blobSync(type?: string): Class_Blob;

    /**
     * @description Returns the data part of the message as a Blob
     *      @param type the MIME type of the Blob, default is an empty string
     *      @return returns a Blob object containing the data part of the message
     *
     */
    blobAsync(type?: string): Promise<Class_BlobPromise>;

    /**
     * @description Returns the data part of the message as a Buffer
     *      @return returns a Buffer containing the data part of the message, or an empty Buffer if there is no data
     *
     */
    bytes(): Promise<Class_Buffer>;

    /**
     * @description Returns the data part of the message as a Buffer
     *      @return returns a Buffer containing the data part of the message, or an empty Buffer if there is no data
     *
     */
    bytesSync(): Class_Buffer;

    /**
     * @description Returns the data part of the message as a Buffer
     *      @return returns a Buffer containing the data part of the message, or an empty Buffer if there is no data
     *
     */
    bytesAsync(): Promise<Class_Buffer>;

    /**
     * @description Writes the given data with JSON encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    json(data: any): Promise<any>;

    /**
     * @description Writes the given data with JSON encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    jsonSync(data: any): any;

    /**
     * @description Writes the given data with JSON encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    jsonAsync(data: any): Promise<any>;

    /**
     * @description Parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    json(): Promise<any>;

    /**
     * @description Parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    jsonSync(): any;

    /**
     * @description Parses the data in the message as JSON
     *      @return returns the parsing result
     *
     */
    jsonAsync(): Promise<any>;

    /**
     * @description Writes the given data with msgpack encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    pack(data: any): Promise<any>;

    /**
     * @description Writes the given data with msgpack encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    packSync(data: any): any;

    /**
     * @description Writes the given data with msgpack encoding
     *      @param data the data to write
     *      @return this method does not return data
     *
     */
    packAsync(data: any): Promise<any>;

    /**
     * @description Parses the data in the message as msgpack
     *      @return returns the parsing result
     *
     */
    pack(): Promise<any>;

    /**
     * @description Parses the data in the message as msgpack
     *      @return returns the parsing result
     *
     */
    packSync(): any;

    /**
     * @description Parses the data in the message as msgpack
     *      @return returns the parsing result
     *
     */
    packAsync(): Promise<any>;

    /**
     * @description The length of the data part of the message
     */
    readonly length: number;

    /**
     * @description Sets the end of current message processing; the Chain handler no longer continues with subsequent transactions
     *      @return returns 0 on success
     *
     */
    end(): Promise<number>;

    /**
     * @description Sets the end of current message processing; the Chain handler no longer continues with subsequent transactions
     *      @return returns 0 on success
     *
     */
    endSync(): number;

    /**
     * @description Sets the end of current message processing; the Chain handler no longer continues with subsequent transactions
     *      @return returns 0 on success
     *
     */
    endAsync(): Promise<number>;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @return returns 0 on success
     *
     */
    end(data: Class_Buffer): Promise<number>;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @return returns 0 on success
     *
     */
    endSync(data: Class_Buffer): number;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @return returns 0 on success
     *
     */
    endAsync(data: Class_Buffer): Promise<number>;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @param encoding the encoding to use; since data is a Buffer, this parameter is ignored
     *      @return returns 0 on success
     *
     */
    end(data: Class_Buffer, encoding: string): Promise<number>;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @param encoding the encoding to use; since data is a Buffer, this parameter is ignored
     *      @return returns 0 on success
     *
     */
    endSync(data: Class_Buffer, encoding: string): number;

    /**
     * @description Writes the given data and sets the end of current message processing
     *      @param data the data to write
     *      @param encoding the encoding to use; since data is a Buffer, this parameter is ignored
     *      @return returns 0 on success
     *
     */
    endAsync(data: Class_Buffer, encoding: string): Promise<number>;

    /**
     * @description Writes the given string data and sets the end of current message processing
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns 0 on success
     *
     */
    end(data: string, encoding?: string): Promise<number>;

    /**
     * @description Writes the given string data and sets the end of current message processing
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns 0 on success
     *
     */
    endSync(data: string, encoding?: string): number;

    /**
     * @description Writes the given string data and sets the end of current message processing
     *      @param data the string data to write
     *      @param encoding the encoding of the string, default is "utf8"
     *      @return returns 0 on success
     *
     */
    endAsync(data: string, encoding?: string): Promise<number>;

    /**
     * @description Queries whether the current message has ended
     *      @return returns true if ended
     *
     */
    isEnded(): boolean;

    /**
     * @description Clears the content of the message
     */
    clear(): void;

    /**
     * @description Sends a formatted message to the given stream object
     *      @param stm the stream object that receives the formatted message
     *      @param options the sending options
     *
     */
    sendTo(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Sends a formatted message to the given stream object
     *      @param stm the stream object that receives the formatted message
     *      @param options the sending options
     *
     */
    sendToSync(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): void;

    /**
     * @description Sends a formatted message to the given stream object
     *      @param stm the stream object that receives the formatted message
     *      @param options the sending options
     *
     */
    sendToAsync(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Reads a formatted message from the given cached stream object and parses and fills the object
     *      @param stm the stream object from which the formatted message is read
     *      @param options the reading options
     *
     */
    readFrom(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Reads a formatted message from the given cached stream object and parses and fills the object
     *      @param stm the stream object from which the formatted message is read
     *      @param options the reading options
     *
     */
    readFromSync(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): void;

    /**
     * @description Reads a formatted message from the given cached stream object and parses and fills the object
     *      @param stm the stream object from which the formatted message is read
     *      @param options the reading options
     *
     */
    readFromAsync(stm: Class_Stream | Class_StreamPromise, options?: FIBJS.GeneralObject): Promise<void>;

    /**
     * @description Queries the stream object used when the message was read from
     */
    readonly stream: Class_StreamPromise;

    /**
     * @description Queries and sets the last error of message processing
     */
    lastError: string;

    /**
     * @description Copies the current message object
     *      @return returns the copied message object
     *
     */
    clone(): Class_Message;

    /**
     * @description Switches the message body stream to flowing read mode
     *      @return returns the message object
     *
     */
    resume(): Class_Message;

    /**
     * @description Pauses the automatic reading mode of the message body stream. This method is for compatibility only and has no actual effect when called
     *      @return returns the message object
     *
     */
    pause(): Class_Message;

    /**
     * @description Pipes the message body stream data to a destination stream
     *      @param destination the destination stream object
     *      @param options pipe options, optional
     *      @return returns the destination stream object
     *
     */
    pipe(destination: any, options?: FIBJS.GeneralObject): any;

    /**
     * @description Removes all pipe destinations of the message body stream. This method is for compatibility only and has no actual effect when called
     *      @param destination a specific writable destination to unpipe
     *
     */
    unpipe(destination?: Class_Stream | Class_StreamPromise): void;

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
     *      @param code error code
     *
     */
    onerror: ((code: number)=>void) | null;

}


declare namespace Class_Message {
    const promises: FIBJS.GeneralObject;
}
