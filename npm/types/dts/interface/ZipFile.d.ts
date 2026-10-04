/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description The ZipFile object is an important object of the zip file compression and decompression module; it provides read and write access to zip files
 *
 * The ZipFile object inherits from the Stream object, so it can be operated in the same way as a Stream object.
 *
 * Commonly used static functions are:
 *
 * - zip.open: opens a zip file or stream
 * - zip.isZipFile: determines whether a file is a zip file
 * - fs.setZipFS: sets the zip file virtual file system
 * - fs.clearZipFS: clears the zip file virtual file system
 *
 * Commonly used instance functions and methods of the ZipFile object are:
 *
 * - NArray ZipFile.namelist(): gets the file name list
 * - NObject ZipFile.getinfo(String member): gets file information
 * - Buffer ZipFile.read(String member, String password = ""): reads the specified file
 * - NArray ZipFile.readAll(String password = ""): reads all files
 * - void ZipFile.extract(String member, String path, String password = ""): extracts a file to the specified path
 * - void ZipFile.extract(String member, SeekableStream strm, String password = ""): extracts a file to a stream
 * - void ZipFile.extractAll(String path, String password = ""): extracts all files to the specified path
 * - void ZipFile.write(String filename, String inZipName, String password = ""): writes the specified file to the zip file
 * - void ZipFile.write(Buffer data, String inZipName, String password = ""): writes the specified file to the zip file
 * - void ZipFile.write(SeekableStream strm, String inZipName, String password = ""): writes the specified file to the zip file
 * - void ZipFile.close(): closes the opened zip file
 *
 * The code example is as follows:
 *
 * ```JavaScript
 * var zip = require('zip');
 * var path = require('path');
 * var fs = require('fs');
 *
 * var zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' ), 'w');
 *
 * // write a file
 * var buf = new Buffer('test data');
 * zipfile.write(buf, 'test.txt');
 *
 * // read a file
 * buf = zipfile.read("unzip_test.js");
 * console.log(buf);
 *
 * zipfile.close();
 * ```
 */
declare class Class_ZipFile extends Class_object {
    /**
     * @description Gets the file name list
     * 	 @return returns a list object containing the file names
     *
     */
    namelist(): any[];

    namelist(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Gets the file name list
     * 	 @return returns a list object containing the file names
     *
     */
    namelistSync(): any[];

    /**
     * @description Gets the file name list
     * 	 @return returns a list object containing the file names
     *
     */
    namelistAsync(): Promise<any[]>;

    /**
     * @description Gets the file information list
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @return returns a list object containing the file information
     *
     */
    infolist(): any[];

    infolist(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Gets the file information list
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @return returns a list object containing the file information
     *
     */
    infolistSync(): any[];

    /**
     * @description Gets the file information list
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @return returns a list object containing the file information
     *
     */
    infolistAsync(): Promise<any[]>;

    /**
     * @description Gets file information
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @param member the name of the file whose information is to be obtained
     * 	 @return returns the file information object
     *
     */
    getinfo(member: string): FIBJS.GeneralObject;

    getinfo(member: string, callback: (err: Error | undefined | null, retVal: FIBJS.GeneralObject)=>any): void;

    /**
     * @description Gets file information
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @param member the name of the file whose information is to be obtained
     * 	 @return returns the file information object
     *
     */
    getinfoSync(member: string): FIBJS.GeneralObject;

    /**
     * @description Gets file information
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @param member the name of the file whose information is to be obtained
     * 	 @return returns the file information object
     *
     */
    getinfoAsync(member: string): Promise<FIBJS.GeneralObject>;

    /**
     * @description Returns the data read from the zip file
     * 	 @param member the name of the file to read
     * 	 @param password decompression password, no password by default
     * 	 @return returns all the data of the file
     *
     */
    read(member: string, password?: string): Class_Buffer;

    read(member: string, password?: string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Returns the data read from the zip file
     * 	 @param member the name of the file to read
     * 	 @param password decompression password, no password by default
     * 	 @return returns all the data of the file
     *
     */
    readSync(member: string, password?: string): Class_Buffer;

    /**
     * @description Returns the data read from the zip file
     * 	 @param member the name of the file to read
     * 	 @param password decompression password, no password by default
     * 	 @return returns all the data of the file
     *
     */
    readAsync(member: string, password?: string): Promise<Class_Buffer>;

    /**
     * @description Decompresses all files
     * 	 @param password decompression password, no password by default
     * 	 @return a list containing the data and information of all files
     *
     */
    readAll(password?: string): any[];

    readAll(password?: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Decompresses all files
     * 	 @param password decompression password, no password by default
     * 	 @return a list containing the data and information of all files
     *
     */
    readAllSync(password?: string): any[];

    /**
     * @description Decompresses all files
     * 	 @param password decompression password, no password by default
     * 	 @return a list containing the data and information of all files
     *
     */
    readAllAsync(password?: string): Promise<any[]>;

    /**
     * @description Decompresses the specified file
     * 	 @param member the name of the file to decompress
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extract(member: string, path: string, password?: string): void;

    extract(member: string, path: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses the specified file
     * 	 @param member the name of the file to decompress
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractSync(member: string, path: string, password?: string): void;

    /**
     * @description Decompresses the specified file
     * 	 @param member the name of the file to decompress
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAsync(member: string, path: string, password?: string): Promise<void>;

    /**
     * @description Decompresses the specified file to a stream
     * 	 @param member the name of the file to decompress
     * 	 @param strm the stream to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extract(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): void;

    extract(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses the specified file to a stream
     * 	 @param member the name of the file to decompress
     * 	 @param strm the stream to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractSync(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): void;

    /**
     * @description Decompresses the specified file to a stream
     * 	 @param member the name of the file to decompress
     * 	 @param strm the stream to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAsync(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): Promise<void>;

    /**
     * @description Decompresses all files to the specified path
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAll(path: string, password?: string): void;

    extractAll(path: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses all files to the specified path
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAllSync(path: string, password?: string): void;

    /**
     * @description Decompresses all files to the specified path
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAllAsync(path: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param filename the file to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    write(filename: string, inZipName: string, password?: string): void;

    write(filename: string, inZipName: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param filename the file to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeSync(filename: string, inZipName: string, password?: string): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param filename the file to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeAsync(filename: string, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param data the file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    write(data: Class_Buffer, inZipName: string, password?: string): void;

    write(data: Class_Buffer, inZipName: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param data the file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeSync(data: Class_Buffer, inZipName: string, password?: string): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param data the file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeAsync(data: Class_Buffer, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param strm the stream of file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    write(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): void;

    write(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param strm the stream of file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeSync(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param strm the stream of file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeAsync(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Closes the opened zip file
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the opened zip file
     */
    closeSync(): void;

    /**
     * @description Closes the opened zip file
     */
    closeAsync(): Promise<void>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * The promise variant of the ZipFile class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_ZipFilePromise extends Class_object {
    /**
     * @description Gets the file name list
     * 	 @return returns a list object containing the file names
     *
     */
    namelist(): Promise<any[]>;

    /**
     * @description Gets the file name list
     * 	 @return returns a list object containing the file names
     *
     */
    namelistSync(): any[];

    /**
     * @description Gets the file name list
     * 	 @return returns a list object containing the file names
     *
     */
    namelistAsync(): Promise<any[]>;

    /**
     * @description Gets the file information list
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @return returns a list object containing the file information
     *
     */
    infolist(): Promise<any[]>;

    /**
     * @description Gets the file information list
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @return returns a list object containing the file information
     *
     */
    infolistSync(): any[];

    /**
     * @description Gets the file information list
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @return returns a list object containing the file information
     *
     */
    infolistAsync(): Promise<any[]>;

    /**
     * @description Gets file information
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @param member the name of the file whose information is to be obtained
     * 	 @return returns the file information object
     *
     */
    getinfo(member: string): Promise<FIBJS.GeneralObject>;

    /**
     * @description Gets file information
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @param member the name of the file whose information is to be obtained
     * 	 @return returns the file information object
     *
     */
    getinfoSync(member: string): FIBJS.GeneralObject;

    /**
     * @description Gets file information
     * 	 The file information contains the fields: filename, date, compress_type, compress_size, file_size, password, data
     * 	 @param member the name of the file whose information is to be obtained
     * 	 @return returns the file information object
     *
     */
    getinfoAsync(member: string): Promise<FIBJS.GeneralObject>;

    /**
     * @description Returns the data read from the zip file
     * 	 @param member the name of the file to read
     * 	 @param password decompression password, no password by default
     * 	 @return returns all the data of the file
     *
     */
    read(member: string, password?: string): Promise<Class_Buffer>;

    /**
     * @description Returns the data read from the zip file
     * 	 @param member the name of the file to read
     * 	 @param password decompression password, no password by default
     * 	 @return returns all the data of the file
     *
     */
    readSync(member: string, password?: string): Class_Buffer;

    /**
     * @description Returns the data read from the zip file
     * 	 @param member the name of the file to read
     * 	 @param password decompression password, no password by default
     * 	 @return returns all the data of the file
     *
     */
    readAsync(member: string, password?: string): Promise<Class_Buffer>;

    /**
     * @description Decompresses all files
     * 	 @param password decompression password, no password by default
     * 	 @return a list containing the data and information of all files
     *
     */
    readAll(password?: string): Promise<any[]>;

    /**
     * @description Decompresses all files
     * 	 @param password decompression password, no password by default
     * 	 @return a list containing the data and information of all files
     *
     */
    readAllSync(password?: string): any[];

    /**
     * @description Decompresses all files
     * 	 @param password decompression password, no password by default
     * 	 @return a list containing the data and information of all files
     *
     */
    readAllAsync(password?: string): Promise<any[]>;

    /**
     * @description Decompresses the specified file
     * 	 @param member the name of the file to decompress
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extract(member: string, path: string, password?: string): Promise<void>;

    /**
     * @description Decompresses the specified file
     * 	 @param member the name of the file to decompress
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractSync(member: string, path: string, password?: string): void;

    /**
     * @description Decompresses the specified file
     * 	 @param member the name of the file to decompress
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAsync(member: string, path: string, password?: string): Promise<void>;

    /**
     * @description Decompresses the specified file to a stream
     * 	 @param member the name of the file to decompress
     * 	 @param strm the stream to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extract(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): Promise<void>;

    /**
     * @description Decompresses the specified file to a stream
     * 	 @param member the name of the file to decompress
     * 	 @param strm the stream to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractSync(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): void;

    /**
     * @description Decompresses the specified file to a stream
     * 	 @param member the name of the file to decompress
     * 	 @param strm the stream to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAsync(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): Promise<void>;

    /**
     * @description Decompresses all files to the specified path
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAll(path: string, password?: string): Promise<void>;

    /**
     * @description Decompresses all files to the specified path
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAllSync(path: string, password?: string): void;

    /**
     * @description Decompresses all files to the specified path
     * 	 @param path the path to decompress to
     * 	 @param password decompression password, no password by default
     *
     */
    extractAllAsync(path: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param filename the file to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    write(filename: string, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param filename the file to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeSync(filename: string, inZipName: string, password?: string): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param filename the file to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeAsync(filename: string, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param data the file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    write(data: Class_Buffer, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param data the file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeSync(data: Class_Buffer, inZipName: string, password?: string): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param data the file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeAsync(data: Class_Buffer, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param strm the stream of file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    write(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param strm the stream of file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeSync(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): void;

    /**
     * @description Writes the specified file to the zip file
     * 	 @param strm the stream of file data to write
     * 	 @param inZipName the file name inside the zip file
     * 	 @param password decompression password, no password by default
     *
     */
    writeAsync(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Closes the opened zip file
     */
    close(): Promise<void>;

    /**
     * @description Closes the opened zip file
     */
    closeSync(): void;

    /**
     * @description Closes the opened zip file
     */
    closeAsync(): Promise<void>;

}


declare namespace Class_ZipFile {
    const promises: FIBJS.GeneralObject;
}
