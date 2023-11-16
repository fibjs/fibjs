/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZipFile.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description zip 模块是一个用于文件压缩和解压缩的模块。它提供了压缩、解压缩、查找和枚举 zip 文件中的文件列表等操作
 * 
 * 通过 zip 模块，我们可以将多个文件打包成一个 zip 文件，也可以对 zip 文件进行解压缩以恢复原始文件。
 * 
 * 下面是一些示例：
 * 
 * 1.压缩文件：
 * 
 * ```JavaScript
 * var zip = require('zip');
 * var zipfile = zip.open('/path/to/dest.zip', 'w');
 * 
 * zipfile.write('/path/to/src1', 'src1');
 * zipfile.write('/path/to/src2', 'src2');
 * zipfile.close();
 * ```
 * 
 * 2.解压缩文件：
 * 
 * ```JavaScript
 * var zip = require('zip');
 * var zipfile = zip.open('/path/to/src.zip', 'r');
 * 
 * var filenames = zipfile.namelist();
 * for (var i = 0; i < filenames.length; ++i) {
 *   var filename = filenames[i];
 *   var data = zipfile.read(filename);
 *   console.log(filename + ': ' + data.length + ' bytes');
 * }
 * zipfile.close();
 * ```
 *  
 */
declare module 'zip' {
    /**
     * @description  判断文件是否是zip格式
     * 	 @param filename 文件名
     * 	 @return 返回true代表文件是zip文件
     * 	 
     */
    function isZipFile(filename: string): boolean;

    function isZipFile(filename: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 打开一个zip文件
     * 	 @param path 文件路径
     * 	 @param mod 打开文件模式, "r"代表读取, "w"代表创建, "a"代表在zip文件后追加
     * 	 @param codec 设置 zip 文件编码方式，缺省为 "utf8"
     * 	 @return 返回zip文件对象
     * 	 
     */
    function open(path: string, mod?: string, codec?: string): Class_ZipFile;

    function open(path: string, mod?: string, codec?: string, callback?: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

    /**
     * @description 打开一个zip文件
     * 	 @param data zip文件数据
     * 	 @param mod 打开文件模式, "r"代表读取, "w"代表创建, "a"代表在zip文件后追加
     * 	 @param codec 设置 zip 文件编码方式，缺省为 "utf8"
     * 	 @return 返回zip文件对象
     * 	 
     */
    function open(data: Class_Buffer, mod?: string, codec?: string): Class_ZipFile;

    function open(data: Class_Buffer, mod?: string, codec?: string, callback?: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

    /**
     * @description 打开一个zip文件
     * 	 @param strm zip文件流
     * 	 @param mod 打开文件模式, "r"代表读取, "w"代表创建, "a"代表在zip文件后追加
     * 	 @param codec 设置 zip 文件编码方式，缺省为 "utf8"
     * 	 @return 返回zip文件对象
     * 	 
     */
    function open(strm: Class_SeekableStream, mod?: string, codec?: string): Class_ZipFile;

    function open(strm: Class_SeekableStream, mod?: string, codec?: string, callback?: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

}

