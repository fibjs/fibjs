/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Stat.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description File handle object
 *
 *  ```JavaScript
 *  var fd = fs.open('test.txt');
 *  ```
 *
 */
declare class Class_FileHandle extends Class_object {
    /**
     * @description FileHandle constructor, creates a file handle from a file descriptor
     *      @param fd the file descriptor value
     *
     */
    constructor(fd: number);

    /**
     * @description Queries the current file descriptor
     */
    readonly fd: number;

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

    /**
     * @description Queries the basic information of the current file
     *      @return returns the basic information of the file
     *
     */
    stat(): Class_Stat;

    stat(callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description Queries the basic information of the current file
     *      @return returns the basic information of the file
     *
     */
    statSync(): Class_Stat;

    /**
     * @description Queries the basic information of the current file
     *      @return returns the basic information of the file
     *
     */
    statAsync(): Promise<Class_Stat>;

    /**
     * @description Reads file content by file descriptor
     *      @param buffer the Buffer object to write the read result into
     *      @param offset the Buffer write offset, default is 0
     *      @param length the number of bytes to read from the file, default is 0
     *      @param position the file read position, default is the current file position
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    read(buffer: Class_Buffer, offset?: number, length?: number, position?: number): {
        bytesRead: number;
        buffer: Class_Buffer;
    };

    read(buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback: (err: Error | undefined | null, retVal: {
        bytesRead: number;
        buffer: Class_Buffer;
    })=>any): void;

    /**
     * @description Reads file content by file descriptor
     *      @param buffer the Buffer object to write the read result into
     *      @param offset the Buffer write offset, default is 0
     *      @param length the number of bytes to read from the file, default is 0
     *      @param position the file read position, default is the current file position
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    readSync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): {
        bytesRead: number;
        buffer: Class_Buffer;
    };

    /**
     * @description Reads file content by file descriptor
     *      @param buffer the Buffer object to write the read result into
     *      @param offset the Buffer write offset, default is 0
     *      @param length the number of bytes to read from the file, default is 0
     *      @param position the file read position, default is the current file position
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    readAsync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<{
        bytesRead: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Reads file content by file descriptor
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      {
     *          "buffer": Buffer.alloc(16384), // the Buffer object to write the read result into; allocated automatically when not provided
     *          "offset": 0, // the Buffer write offset, default is 0
     *          "length": 0, // the number of bytes to read, default is buffer.length - offset
     *          "position": -1 // the file read position, default is the current file position
     *      }
     *      ```
     *      @param options the read options
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    read(options: FIBJS.GeneralObject): {
        bytesRead: number;
        buffer: Class_Buffer;
    };

    read(options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: {
        bytesRead: number;
        buffer: Class_Buffer;
    })=>any): void;

    /**
     * @description Reads file content by file descriptor
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      {
     *          "buffer": Buffer.alloc(16384), // the Buffer object to write the read result into; allocated automatically when not provided
     *          "offset": 0, // the Buffer write offset, default is 0
     *          "length": 0, // the number of bytes to read, default is buffer.length - offset
     *          "position": -1 // the file read position, default is the current file position
     *      }
     *      ```
     *      @param options the read options
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    readSync(options: FIBJS.GeneralObject): {
        bytesRead: number;
        buffer: Class_Buffer;
    };

    /**
     * @description Reads file content by file descriptor
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      {
     *          "buffer": Buffer.alloc(16384), // the Buffer object to write the read result into; allocated automatically when not provided
     *          "offset": 0, // the Buffer write offset, default is 0
     *          "length": 0, // the number of bytes to read, default is buffer.length - offset
     *          "position": -1 // the file read position, default is the current file position
     *      }
     *      ```
     *      @param options the read options
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    readAsync(options: FIBJS.GeneralObject): Promise<{
        bytesRead: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Writes content to the file by file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the Buffer data read offset, default is 0
     *      @param length the number of bytes to write to the file, default is -1
     *      @param position the file write position, default is the current file position
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    write(buffer: Class_Buffer, offset?: number, length?: number, position?: number): {
        bytesWritten: number;
        buffer: Class_Buffer;
    };

    write(buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback: (err: Error | undefined | null, retVal: {
        bytesWritten: number;
        buffer: Class_Buffer;
    })=>any): void;

    /**
     * @description Writes content to the file by file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the Buffer data read offset, default is 0
     *      @param length the number of bytes to write to the file, default is -1
     *      @param position the file write position, default is the current file position
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    writeSync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): {
        bytesWritten: number;
        buffer: Class_Buffer;
    };

    /**
     * @description Writes content to the file by file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the Buffer data read offset, default is 0
     *      @param length the number of bytes to write to the file, default is -1
     *      @param position the file write position, default is the current file position
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    writeAsync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<{
        bytesWritten: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Writes content to the file by file descriptor
     *      @param string the string to write
     *      @param position the file write position, default is the current file position
     *      @param encoding the decoding method, utf8 by default
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    write(string: string, position?: number, encoding?: string): {
        bytesWritten: number;
        buffer: Class_Buffer;
    };

    write(string: string, position?: number, encoding?: string, callback: (err: Error | undefined | null, retVal: {
        bytesWritten: number;
        buffer: Class_Buffer;
    })=>any): void;

    /**
     * @description Writes content to the file by file descriptor
     *      @param string the string to write
     *      @param position the file write position, default is the current file position
     *      @param encoding the decoding method, utf8 by default
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    writeSync(string: string, position?: number, encoding?: string): {
        bytesWritten: number;
        buffer: Class_Buffer;
    };

    /**
     * @description Writes content to the file by file descriptor
     *      @param string the string to write
     *      @param position the file write position, default is the current file position
     *      @param encoding the decoding method, utf8 by default
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    writeAsync(string: string, position?: number, encoding?: string): Promise<{
        bytesWritten: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Reads the entire content of the file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      An encoding string is empty by default, no decoding is performed and a Buffer object is returned; a descriptor read with an options object decodes as utf8 unless the encoding option says otherwise.
     *      options may be the decoding method string, or the read options object.
     *      @param options the decoding method or the read options
     *      @return returns the file content
     *
     */
    readFile(options?: FIBJS.GeneralObject | string): any;

    readFile(options?: FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Reads the entire content of the file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      An encoding string is empty by default, no decoding is performed and a Buffer object is returned; a descriptor read with an options object decodes as utf8 unless the encoding option says otherwise.
     *      options may be the decoding method string, or the read options object.
     *      @param options the decoding method or the read options
     *      @return returns the file content
     *
     */
    readFileSync(options?: FIBJS.GeneralObject | string): any;

    /**
     * @description Reads the entire content of the file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      An encoding string is empty by default, no decoding is performed and a Buffer object is returned; a descriptor read with an options object decodes as utf8 unless the encoding option says otherwise.
     *      options may be the decoding method string, or the read options object.
     *      @param options the decoding method or the read options
     *      @return returns the file content
     *
     */
    readFileAsync(options?: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Writes data to the file, replacing its content
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      opt is the encoding of string data, utf8 by default, and an options object carries the encoding instead; the encoding of a Buffer is validated but not used.
     *      @param data the data to write
     *      opt may be the encoding of string data, or the write options object.
     *      @param opt the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    writeFile(data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): number;

    writeFile(data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Writes data to the file, replacing its content
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      opt is the encoding of string data, utf8 by default, and an options object carries the encoding instead; the encoding of a Buffer is validated but not used.
     *      @param data the data to write
     *      opt may be the encoding of string data, or the write options object.
     *      @param opt the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    writeFileSync(data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): number;

    /**
     * @description Writes data to the file, replacing its content
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      opt is the encoding of string data, utf8 by default, and an options object carries the encoding instead; the encoding of a Buffer is validated but not used.
     *      @param data the data to write
     *      opt may be the encoding of string data, or the write options object.
     *      @param opt the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    writeFileAsync(data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Modifies the access and modification times of the file
     *
     *     Time parameters can be a Date object, a Unix timestamp (in seconds) or a date string, consistent with Node.js.
     *      @param atime the last access time of the file
     *      @param mtime the last modification time of the file
     *
     */
    utimes(atime: any, mtime: any): void;

    utimes(atime: any, mtime: any, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Modifies the access and modification times of the file
     *
     *     Time parameters can be a Date object, a Unix timestamp (in seconds) or a date string, consistent with Node.js.
     *      @param atime the last access time of the file
     *      @param mtime the last modification time of the file
     *
     */
    utimesSync(atime: any, mtime: any): void;

    /**
     * @description Modifies the access and modification times of the file
     *
     *     Time parameters can be a Date object, a Unix timestamp (in seconds) or a date string, consistent with Node.js.
     *      @param atime the last access time of the file
     *      @param mtime the last modification time of the file
     *
     */
    utimesAsync(atime: any, mtime: any): Promise<void>;

    /**
     * @description Modifies the owner of the file; not supported on Windows
     *      @param uid the file owner user id
     *      @param gid the file owner group id
     *
     */
    chown(uid: number, gid: number): void;

    chown(uid: number, gid: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Modifies the owner of the file; not supported on Windows
     *      @param uid the file owner user id
     *      @param gid the file owner group id
     *
     */
    chownSync(uid: number, gid: number): void;

    /**
     * @description Modifies the owner of the file; not supported on Windows
     *      @param uid the file owner user id
     *      @param gid the file owner group id
     *
     */
    chownAsync(uid: number, gid: number): Promise<void>;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes file data and metadata, ensuring written content is persisted.
     *
     */
    sync(): void;

    sync(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes file data and metadata, ensuring written content is persisted.
     *
     */
    syncSync(): void;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes file data and metadata, ensuring written content is persisted.
     *
     */
    syncAsync(): Promise<void>;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes only the file data portion, not the file metadata; less expensive than sync.
     *
     */
    datasync(): void;

    datasync(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes only the file data portion, not the file metadata; less expensive than sync.
     *
     */
    datasyncSync(): void;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes only the file data portion, not the file metadata; less expensive than sync.
     *
     */
    datasyncAsync(): Promise<void>;

    /**
     * @description Modifies the file size
     *      @param len the file size to set, default is 0
     *
     */
    truncate(len?: number): void;

    truncate(len?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Modifies the file size
     *      @param len the file size to set, default is 0
     *
     */
    truncateSync(len?: number): void;

    /**
     * @description Modifies the file size
     *      @param len the file size to set, default is 0
     *
     */
    truncateAsync(len?: number): Promise<void>;

    /**
     * @description Appends content
     *      @param data the data to write
     *      @return the number of bytes actually written
     *
     */
    appendFile(data: Class_Buffer | string): number;

    appendFile(data: Class_Buffer | string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Appends content
     *      @param data the data to write
     *      @return the number of bytes actually written
     *
     */
    appendFileSync(data: Class_Buffer | string): number;

    /**
     * @description Appends content
     *      @param data the data to write
     *      @return the number of bytes actually written
     *
     */
    appendFileAsync(data: Class_Buffer | string): Promise<number>;

    /**
     * @description Closes the current file handle
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the current file handle
     */
    closeSync(): void;

    /**
     * @description Closes the current file handle
     */
    closeAsync(): Promise<void>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Stat.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the FileHandle class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_FileHandlePromise extends Class_object {
    /**
     * @description FileHandle constructor, creates a file handle from a file descriptor
     *      @param fd the file descriptor value
     *
     */
    constructor(fd: number);

    /**
     * @description Queries the current file descriptor
     */
    readonly fd: number;

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

    /**
     * @description Queries the basic information of the current file
     *      @return returns the basic information of the file
     *
     */
    stat(): Promise<Class_Stat>;

    /**
     * @description Queries the basic information of the current file
     *      @return returns the basic information of the file
     *
     */
    statSync(): Class_Stat;

    /**
     * @description Queries the basic information of the current file
     *      @return returns the basic information of the file
     *
     */
    statAsync(): Promise<Class_Stat>;

    /**
     * @description Reads file content by file descriptor
     *      @param buffer the Buffer object to write the read result into
     *      @param offset the Buffer write offset, default is 0
     *      @param length the number of bytes to read from the file, default is 0
     *      @param position the file read position, default is the current file position
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    read(buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<{
        bytesRead: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Reads file content by file descriptor
     *      @param buffer the Buffer object to write the read result into
     *      @param offset the Buffer write offset, default is 0
     *      @param length the number of bytes to read from the file, default is 0
     *      @param position the file read position, default is the current file position
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    readSync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): {
        bytesRead: number;
        buffer: Class_Buffer;
    };

    /**
     * @description Reads file content by file descriptor
     *      @param buffer the Buffer object to write the read result into
     *      @param offset the Buffer write offset, default is 0
     *      @param length the number of bytes to read from the file, default is 0
     *      @param position the file read position, default is the current file position
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    readAsync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<{
        bytesRead: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Reads file content by file descriptor
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      {
     *          "buffer": Buffer.alloc(16384), // the Buffer object to write the read result into; allocated automatically when not provided
     *          "offset": 0, // the Buffer write offset, default is 0
     *          "length": 0, // the number of bytes to read, default is buffer.length - offset
     *          "position": -1 // the file read position, default is the current file position
     *      }
     *      ```
     *      @param options the read options
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    read(options: FIBJS.GeneralObject): Promise<{
        bytesRead: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Reads file content by file descriptor
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      {
     *          "buffer": Buffer.alloc(16384), // the Buffer object to write the read result into; allocated automatically when not provided
     *          "offset": 0, // the Buffer write offset, default is 0
     *          "length": 0, // the number of bytes to read, default is buffer.length - offset
     *          "position": -1 // the file read position, default is the current file position
     *      }
     *      ```
     *      @param options the read options
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    readSync(options: FIBJS.GeneralObject): {
        bytesRead: number;
        buffer: Class_Buffer;
    };

    /**
     * @description Reads file content by file descriptor
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      {
     *          "buffer": Buffer.alloc(16384), // the Buffer object to write the read result into; allocated automatically when not provided
     *          "offset": 0, // the Buffer write offset, default is 0
     *          "length": 0, // the number of bytes to read, default is buffer.length - offset
     *          "position": -1 // the file read position, default is the current file position
     *      }
     *      ```
     *      @param options the read options
     *      @return returns an object containing the bytesRead and buffer properties
     *
     */
    readAsync(options: FIBJS.GeneralObject): Promise<{
        bytesRead: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Writes content to the file by file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the Buffer data read offset, default is 0
     *      @param length the number of bytes to write to the file, default is -1
     *      @param position the file write position, default is the current file position
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    write(buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<{
        bytesWritten: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Writes content to the file by file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the Buffer data read offset, default is 0
     *      @param length the number of bytes to write to the file, default is -1
     *      @param position the file write position, default is the current file position
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    writeSync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): {
        bytesWritten: number;
        buffer: Class_Buffer;
    };

    /**
     * @description Writes content to the file by file descriptor
     *      @param buffer the Buffer object to write
     *      @param offset the Buffer data read offset, default is 0
     *      @param length the number of bytes to write to the file, default is -1
     *      @param position the file write position, default is the current file position
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    writeAsync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<{
        bytesWritten: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Writes content to the file by file descriptor
     *      @param string the string to write
     *      @param position the file write position, default is the current file position
     *      @param encoding the decoding method, utf8 by default
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    write(string: string, position?: number, encoding?: string): Promise<{
        bytesWritten: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Writes content to the file by file descriptor
     *      @param string the string to write
     *      @param position the file write position, default is the current file position
     *      @param encoding the decoding method, utf8 by default
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    writeSync(string: string, position?: number, encoding?: string): {
        bytesWritten: number;
        buffer: Class_Buffer;
    };

    /**
     * @description Writes content to the file by file descriptor
     *      @param string the string to write
     *      @param position the file write position, default is the current file position
     *      @param encoding the decoding method, utf8 by default
     *      @return returns an object containing the bytesWritten and buffer properties
     *
     */
    writeAsync(string: string, position?: number, encoding?: string): Promise<{
        bytesWritten: number;
        buffer: Class_Buffer;
    }>;

    /**
     * @description Reads the entire content of the file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      An encoding string is empty by default, no decoding is performed and a Buffer object is returned; a descriptor read with an options object decodes as utf8 unless the encoding option says otherwise.
     *      options may be the decoding method string, or the read options object.
     *      @param options the decoding method or the read options
     *      @return returns the file content
     *
     */
    readFile(options?: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Reads the entire content of the file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      An encoding string is empty by default, no decoding is performed and a Buffer object is returned; a descriptor read with an options object decodes as utf8 unless the encoding option says otherwise.
     *      options may be the decoding method string, or the read options object.
     *      @param options the decoding method or the read options
     *      @return returns the file content
     *
     */
    readFileSync(options?: FIBJS.GeneralObject | string): any;

    /**
     * @description Reads the entire content of the file
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      An encoding string is empty by default, no decoding is performed and a Buffer object is returned; a descriptor read with an options object decodes as utf8 unless the encoding option says otherwise.
     *      options may be the decoding method string, or the read options object.
     *      @param options the decoding method or the read options
     *      @return returns the file content
     *
     */
    readFileAsync(options?: FIBJS.GeneralObject | string): Promise<any>;

    /**
     * @description Writes data to the file, replacing its content
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      opt is the encoding of string data, utf8 by default, and an options object carries the encoding instead; the encoding of a Buffer is validated but not used.
     *      @param data the data to write
     *      opt may be the encoding of string data, or the write options object.
     *      @param opt the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    writeFile(data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Writes data to the file, replacing its content
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      opt is the encoding of string data, utf8 by default, and an options object carries the encoding instead; the encoding of a Buffer is validated but not used.
     *      @param data the data to write
     *      opt may be the encoding of string data, or the write options object.
     *      @param opt the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    writeFileSync(data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): number;

    /**
     * @description Writes data to the file, replacing its content
     *
     *      options supports the following options:
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // the encoding to use, default is utf8.
     *      }
     *      ```
     *      opt is the encoding of string data, utf8 by default, and an options object carries the encoding instead; the encoding of a Buffer is validated but not used.
     *      @param data the data to write
     *      opt may be the encoding of string data, or the write options object.
     *      @param opt the encoding or the write options
     *      @return the number of bytes actually written
     *
     */
    writeFileAsync(data: Class_Buffer | string, opt?: FIBJS.GeneralObject | string): Promise<number>;

    /**
     * @description Modifies the access and modification times of the file
     *
     *     Time parameters can be a Date object, a Unix timestamp (in seconds) or a date string, consistent with Node.js.
     *      @param atime the last access time of the file
     *      @param mtime the last modification time of the file
     *
     */
    utimes(atime: any, mtime: any): Promise<void>;

    /**
     * @description Modifies the access and modification times of the file
     *
     *     Time parameters can be a Date object, a Unix timestamp (in seconds) or a date string, consistent with Node.js.
     *      @param atime the last access time of the file
     *      @param mtime the last modification time of the file
     *
     */
    utimesSync(atime: any, mtime: any): void;

    /**
     * @description Modifies the access and modification times of the file
     *
     *     Time parameters can be a Date object, a Unix timestamp (in seconds) or a date string, consistent with Node.js.
     *      @param atime the last access time of the file
     *      @param mtime the last modification time of the file
     *
     */
    utimesAsync(atime: any, mtime: any): Promise<void>;

    /**
     * @description Modifies the owner of the file; not supported on Windows
     *      @param uid the file owner user id
     *      @param gid the file owner group id
     *
     */
    chown(uid: number, gid: number): Promise<void>;

    /**
     * @description Modifies the owner of the file; not supported on Windows
     *      @param uid the file owner user id
     *      @param gid the file owner group id
     *
     */
    chownSync(uid: number, gid: number): void;

    /**
     * @description Modifies the owner of the file; not supported on Windows
     *      @param uid the file owner user id
     *      @param gid the file owner group id
     *
     */
    chownAsync(uid: number, gid: number): Promise<void>;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes file data and metadata, ensuring written content is persisted.
     *
     */
    sync(): Promise<void>;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes file data and metadata, ensuring written content is persisted.
     *
     */
    syncSync(): void;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes file data and metadata, ensuring written content is persisted.
     *
     */
    syncAsync(): Promise<void>;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes only the file data portion, not the file metadata; less expensive than sync.
     *
     */
    datasync(): Promise<void>;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes only the file data portion, not the file metadata; less expensive than sync.
     *
     */
    datasyncSync(): void;

    /**
     * @description Synchronizes data to disk
     *
     *     Synchronizes only the file data portion, not the file metadata; less expensive than sync.
     *
     */
    datasyncAsync(): Promise<void>;

    /**
     * @description Modifies the file size
     *      @param len the file size to set, default is 0
     *
     */
    truncate(len?: number): Promise<void>;

    /**
     * @description Modifies the file size
     *      @param len the file size to set, default is 0
     *
     */
    truncateSync(len?: number): void;

    /**
     * @description Modifies the file size
     *      @param len the file size to set, default is 0
     *
     */
    truncateAsync(len?: number): Promise<void>;

    /**
     * @description Appends content
     *      @param data the data to write
     *      @return the number of bytes actually written
     *
     */
    appendFile(data: Class_Buffer | string): Promise<number>;

    /**
     * @description Appends content
     *      @param data the data to write
     *      @return the number of bytes actually written
     *
     */
    appendFileSync(data: Class_Buffer | string): number;

    /**
     * @description Appends content
     *      @param data the data to write
     *      @return the number of bytes actually written
     *
     */
    appendFileAsync(data: Class_Buffer | string): Promise<number>;

    /**
     * @description Closes the current file handle
     */
    close(): Promise<void>;

    /**
     * @description Closes the current file handle
     */
    closeSync(): void;

    /**
     * @description Closes the current file handle
     */
    closeAsync(): Promise<void>;

}


declare namespace Class_FileHandle {
    const promises: FIBJS.GeneralObject;
}
