/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description ZipFile 对象是 zip 格式文件压缩解压模块中的重要对象，提供了对 zip 文件的读写访问
 * 
 * ZipFile 对象继承自Stream 对象，因此可以通过 Stream 对象一样的方式进行操作。
 * 
 * 常用的静态函数有：
 * 
 * - zip.open：打开一个 zip 文件或 stream
 * - zip.isZipFile：判断文件是否是 zip 文件
 * - fs.setZipFS：设置 zip 文件虚拟文件系统
 * - fs.clearZipFS：清除 zip 文件虚拟文件系统
 * 
 * ZipFile 对象的常用实例函数及方法有：
 * 
 * - NArray ZipFile.namelist()：获取文件名列表
 * - NObject ZipFile.getinfo(String member)：获取文件信息
 * - Buffer ZipFile.read(String member, String password = "")：读取指定文件
 * - NArray ZipFile.readAll(String password = "")：读取所有文件
 * - void ZipFile.extract(String member, String path, String password = "")：解压文件到指定路径中
 * - void ZipFile.extract(String member, SeekableStream strm, String password = "")：解压文件到流中
 * - void ZipFile.extractAll(String path, String password = "")：解压所有文件到指定路径
 * - void ZipFile.write(String filename, String inZipName, String password = "")：写入指定文件到压缩文件
 * - void ZipFile.write(Buffer data, String inZipName, String password = "")：写入指定文件到压缩文件
 * - void ZipFile.write(SeekableStream strm, String inZipName, String password = "")：写入指定文件到压缩文件
 * - void ZipFile.close()：关闭打开的zip文件
 * 
 * 代码实例如下：
 * 
 * ```JavaScript
 * var zip = require('zip');
 * var path = require('path');
 * var fs = require('fs');
 * 
 * var zipfile = zip.open(path.join(__dirname, 'unzip_test.zip' ), 'w');
 * 
 * // 写入一个文件
 * var buf = new Buffer('test data');
 * zipfile.write(buf, 'test.txt');
 * 
 * // 读取一个文件内容
 * buf = zipfile.read("unzip_test.js");
 * console.log(buf);
 * 
 * zipfile.close();
 * ``` 
 */
declare class Class_ZipFile extends Class_object {
    /**
     * @description 获取文件名列表
     * 	 @return 返回包含文件名的列表对象
     * 	 
     */
    namelist(): any[];

    namelist(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 获取文件信息列表 
     * 	 文件信息包含字段有：filename, date, compress_type, compress_size, file_size, password, data
     * 	 @return 返回包含文件信息的列表对象
     * 	 
     */
    infolist(): any[];

    infolist(callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 获取文件信息
     * 	 文件信息包含字段有：filename, date, compress_type, compress_size, file_size, password, data
     * 	 @param member 指定要获取信息的文件名
     * 	 @return 返回文件信息对象
     * 	
     */
    getinfo(member: string): FIBJS.GeneralObject;

    getinfo(member: string, callback: (err: Error | undefined | null, retVal: FIBJS.GeneralObject)=>any): void;

    /**
     * @description 返回从压缩文件读取的数据
     * 	 @param member 指定要读取的文件名
     * 	 @param password 解压密码, 默认没有密码
     * 	 @return 返回文件的所有数据
     * 	 
     */
    read(member: string, password?: string): Class_Buffer;

    read(member: string, password?: string, callback?: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 解压所有文件
     * 	 @param password 解压密码, 默认没有密码
     * 	 @return 包含所有文件数据及信息的列表
     *      
     */
    readAll(password?: string): any[];

    readAll(password?: string, callback?: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 解压指定文件
     * 	 @param member 指定要解压的文件名
     * 	 @param path 指定要解压到的路径
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    extract(member: string, path: string, password?: string): void;

    extract(member: string, path: string, password?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 解压指定文件到流
     * 	 @param member 指定要解压的文件名
     * 	 @param strm 指定要解压到的流
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    extract(member: string, strm: Class_SeekableStream, password?: string): void;

    extract(member: string, strm: Class_SeekableStream, password?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 解压所有文件到指定路径
     * 	 @param path 指定要解压到的路径
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    extractAll(path: string, password?: string): void;

    extractAll(path: string, password?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 写入指定文件到压缩文件
     * 	 @param filename 指定要写入的文件
     * 	 @param inZipName 压缩在zip文件内的文件名
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    write(filename: string, inZipName: string, password?: string): void;

    write(filename: string, inZipName: string, password?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 写入指定文件到压缩文件
     * 	 @param data 指定要写入的文件数据
     * 	 @param inZipName 压缩在zip文件内的文件名
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    write(data: Class_Buffer, inZipName: string, password?: string): void;

    write(data: Class_Buffer, inZipName: string, password?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 写入指定文件到压缩文件
     * 	 @param strm 指定要写入文件数据流
     * 	 @param inZipName 压缩在zip文件内的文件名
     * 	 @param password 解压密码, 默认没有密码
     * 	 
     */
    write(strm: Class_SeekableStream, inZipName: string, password?: string): void;

    write(strm: Class_SeekableStream, inZipName: string, password?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 关闭打开的zip文件 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

}

