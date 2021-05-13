/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description 文件操作对象，用于二进制文件读写
 * 
 *  文件操作对象用于对二进制文件进行操作，可使用 fs 模块打开和创建文件：
 *  ```JavaScript
 *  var f = fs.openFile('test.txt');
 *  ```
 *  
 */
declare class Class_File extends Class_SeekableStream {
    /**
     * @description 查询当前文件名 
     */
    readonly name: string;

    /**
     * @description 查询当前文件描述符 
     */
    readonly fd: number;

    /**
     * @description 查询当前文件的访问权限，Windows 不支持此方法
     *      @param mode 指定设定的访问权限
     *      
     */
    chmod(mode: number): void;

    chmod(mode: number, callback: (err: Error | undefined | null)=>any): void;

}

