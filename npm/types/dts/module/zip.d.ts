/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZipFile.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description zip 格式文件压缩解压模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  var zip = require('zip');
 *  ```
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
     * 	 @param codec 设置 zip 文件编码方式，缺省为 "utf-8"
     * 	 @return 返回zip文件对象
     * 	 
     */
    function open(path: string, mod?: string, codec?: string): Class_ZipFile;

    function open(path: string, mod?: string, codec?: string, callback?: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

    /**
     * @description 打开一个zip文件
     * 	 @param data zip文件数据
     * 	 @param mod 打开文件模式, "r"代表读取, "w"代表创建, "a"代表在zip文件后追加
     * 	 @param codec 设置 zip 文件编码方式，缺省为 "utf-8"
     * 	 @return 返回zip文件对象
     * 	 
     */
    function open(data: Class_Buffer, mod?: string, codec?: string): Class_ZipFile;

    function open(data: Class_Buffer, mod?: string, codec?: string, callback?: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

    /**
     * @description 打开一个zip文件
     * 	 @param strm zip文件流
     * 	 @param mod 打开文件模式, "r"代表读取, "w"代表创建, "a"代表在zip文件后追加
     * 	 @param codec 设置 zip 文件编码方式，缺省为 "utf-8"
     * 	 @return 返回zip文件对象
     * 	 
     */
    function open(strm: Class_SeekableStream, mod?: string, codec?: string): Class_ZipFile;

    function open(strm: Class_SeekableStream, mod?: string, codec?: string, callback?: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

}

