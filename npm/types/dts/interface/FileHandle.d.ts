/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Stat.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description 文件句柄对象
 * 
 *  ```JavaScript
 *  var fd = fs.open('test.txt');
 *  ```
 *  
 */
declare class Class_FileHandle extends Class_object {
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

    /**
     * @description 查询当前文件的基础信息
     *      @return 返回文件的基础信息
     *      
     */
    stat(): Class_Stat;

    stat(callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description 根据文件描述符，读取文件内容
     *      @param buffer 读取结果写入的 Buffer 对象
     *      @param offset Buffer 写入偏移量， 默认为 0
     *      @param length 文件读取字节数，默认为 0
     *      @param position 文件读取位置，默认为当前文件位置
     *      @return 实际读取的字节数
     *      
     */
    read(buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    read(buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback?: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 根据文件描述符，向文件写入内容
     *      @param buffer 待写入的 Buffer 对象
     *      @param offset Buffer 数据读取偏移量， 默认为 0
     *      @param length 文件写入字节数，默认为 -1
     *      @param position 文件写入取位置，默认为当前文件位置
     *      @return 实际写入的字节数
     *      
     */
    write(buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    write(buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback?: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 根据文件描述符，向文件写入内容
     *      @param string 待写入的字符串
     *      @param position 文件写入取位置，默认为当前文件位置
     *      @param encoding 指定解码方式，缺省解码 utf8
     *      @return 实际写入的字节数
     *      
     */
    write(string: string, position?: number, encoding?: string): number;

    write(string: string, position?: number, encoding?: string, callback?: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 关闭当前文件句柄 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

}

