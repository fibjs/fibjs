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
     * @description FileHandle 构造函数，从文件描述符创建文件句柄
     *      @param fd 文件描述符数值
     *      
     */
    constructor(fd: number);

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
     * @description 查询当前文件的访问权限，Windows 不支持此方法
     *      @param mode 指定设定的访问权限
     *      
     */
    chmodSync(mode: number): void;

    /**
     * @description 查询当前文件的访问权限，Windows 不支持此方法
     *      @param mode 指定设定的访问权限
     *      
     */
    chmodAsync(mode: number): Promise<void>;

    /**
     * @description 查询当前文件的基础信息
     *      @return 返回文件的基础信息
     *      
     */
    stat(): Class_Stat;

    stat(callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description 查询当前文件的基础信息
     *      @return 返回文件的基础信息
     *      
     */
    statSync(): Class_Stat;

    /**
     * @description 查询当前文件的基础信息
     *      @return 返回文件的基础信息
     *      
     */
    statAsync(): Promise<Class_Stat>;

    /**
     * @description 根据文件描述符，读取文件内容
     *      @param buffer 读取结果写入的 Buffer 对象
     *      @param offset Buffer 写入偏移量， 默认为 0
     *      @param length 文件读取字节数，默认为 0
     *      @param position 文件读取位置，默认为当前文件位置
     *      @return 返回包含 bytesRead 和 buffer 属性的对象
     *      
     */
    read(buffer: Class_Buffer, offset?: number, length?: number, position?: number): [bytesRead: number, buffer: Buffer];

    read(buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback: (err: Error | undefined | null, retVal: [bytesRead: number, buffer: Buffer])=>any): void;

    /**
     * @description 根据文件描述符，读取文件内容
     *      @param buffer 读取结果写入的 Buffer 对象
     *      @param offset Buffer 写入偏移量， 默认为 0
     *      @param length 文件读取字节数，默认为 0
     *      @param position 文件读取位置，默认为当前文件位置
     *      @return 返回包含 bytesRead 和 buffer 属性的对象
     *      
     */
    readSync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): [bytesRead: number, buffer: Buffer];

    /**
     * @description 根据文件描述符，读取文件内容
     *      @param buffer 读取结果写入的 Buffer 对象
     *      @param offset Buffer 写入偏移量， 默认为 0
     *      @param length 文件读取字节数，默认为 0
     *      @param position 文件读取位置，默认为当前文件位置
     *      @return 返回包含 bytesRead 和 buffer 属性的对象
     *      
     */
    readAsync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<[bytesRead: number, buffer: Buffer]>;

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

    write(buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 根据文件描述符，向文件写入内容
     *      @param buffer 待写入的 Buffer 对象
     *      @param offset Buffer 数据读取偏移量， 默认为 0
     *      @param length 文件写入字节数，默认为 -1
     *      @param position 文件写入取位置，默认为当前文件位置
     *      @return 实际写入的字节数
     *      
     */
    writeSync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    /**
     * @description 根据文件描述符，向文件写入内容
     *      @param buffer 待写入的 Buffer 对象
     *      @param offset Buffer 数据读取偏移量， 默认为 0
     *      @param length 文件写入字节数，默认为 -1
     *      @param position 文件写入取位置，默认为当前文件位置
     *      @return 实际写入的字节数
     *      
     */
    writeAsync(buffer: Class_Buffer, offset?: number, length?: number, position?: number): Promise<number>;

    /**
     * @description 根据文件描述符，向文件写入内容
     *      @param string 待写入的字符串
     *      @param position 文件写入取位置，默认为当前文件位置
     *      @param encoding 指定解码方式，缺省解码 utf8
     *      @return 实际写入的字节数
     *      
     */
    write(string: string, position?: number, encoding?: string): number;

    write(string: string, position?: number, encoding?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 根据文件描述符，向文件写入内容
     *      @param string 待写入的字符串
     *      @param position 文件写入取位置，默认为当前文件位置
     *      @param encoding 指定解码方式，缺省解码 utf8
     *      @return 实际写入的字节数
     *      
     */
    writeSync(string: string, position?: number, encoding?: string): number;

    /**
     * @description 根据文件描述符，向文件写入内容
     *      @param string 待写入的字符串
     *      @param position 文件写入取位置，默认为当前文件位置
     *      @param encoding 指定解码方式，缺省解码 utf8
     *      @return 实际写入的字节数
     *      
     */
    writeAsync(string: string, position?: number, encoding?: string): Promise<number>;

    /**
     * @description 读取文件的全部内容
     *      @param encoding 指定解码方式，缺省不解码
     *      @return 返回文件内容
     *      
     */
    readFile(encoding?: string): any;

    readFile(encoding?: string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 读取文件的全部内容
     *      @param encoding 指定解码方式，缺省不解码
     *      @return 返回文件内容
     *      
     */
    readFileSync(encoding?: string): any;

    /**
     * @description 读取文件的全部内容
     *      @param encoding 指定解码方式，缺省不解码
     *      @return 返回文件内容
     *      
     */
    readFileAsync(encoding?: string): Promise<any>;

    /**
     * @description 读取文件的全部内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param options 指定读取选项
     *      @return 返回文件内容
     *      
     */
    readFile(options: FIBJS.GeneralObject): any;

    readFile(options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 读取文件的全部内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param options 指定读取选项
     *      @return 返回文件内容
     *      
     */
    readFileSync(options: FIBJS.GeneralObject): any;

    /**
     * @description 读取文件的全部内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param options 指定读取选项
     *      @return 返回文件内容
     *      
     */
    readFileAsync(options: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description 将数据写入文件，替换其内容
     *      @param data 待写入的数据
     *      @param opt 指定写入选项，将被忽略
     *      @return 实际写入的字节数
     *      
     */
    writeFile(data: Class_Buffer, opt?: string): number;

    writeFile(data: Class_Buffer, opt?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 将数据写入文件，替换其内容
     *      @param data 待写入的数据
     *      @param opt 指定写入选项，将被忽略
     *      @return 实际写入的字节数
     *      
     */
    writeFileSync(data: Class_Buffer, opt?: string): number;

    /**
     * @description 将数据写入文件，替换其内容
     *      @param data 待写入的数据
     *      @param opt 指定写入选项，将被忽略
     *      @return 实际写入的字节数
     *      
     */
    writeFileAsync(data: Class_Buffer, opt?: string): Promise<number>;

    /**
     * @description 将数据写入文件，替换其内容
     *      @param data 待写入的数据
     *      @param opt 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFile(data: string, opt?: string): number;

    writeFile(data: string, opt?: string, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 将数据写入文件，替换其内容
     *      @param data 待写入的数据
     *      @param opt 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFileSync(data: string, opt?: string): number;

    /**
     * @description 将数据写入文件，替换其内容
     *      @param data 待写入的数据
     *      @param opt 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFileAsync(data: string, opt?: string): Promise<number>;

    /**
     * @description 将数据写入文件，替换其内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param data 待写入的数据
     *      @param options 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFile(data: Class_Buffer, options: FIBJS.GeneralObject): number;

    writeFile(data: Class_Buffer, options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 将数据写入文件，替换其内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param data 待写入的数据
     *      @param options 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFileSync(data: Class_Buffer, options: FIBJS.GeneralObject): number;

    /**
     * @description 将数据写入文件，替换其内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param data 待写入的数据
     *      @param options 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFileAsync(data: Class_Buffer, options: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description 将数据写入文件，替换其内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param data 待写入的数据
     *      @param options 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFile(data: string, options: FIBJS.GeneralObject): number;

    writeFile(data: string, options: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 将数据写入文件，替换其内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param data 待写入的数据
     *      @param options 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFileSync(data: string, options: FIBJS.GeneralObject): number;

    /**
     * @description 将数据写入文件，替换其内容
     * 
     *      options 支持以下选项：
     *      ```JavaScript
     *      {
     *          "encoding": "utf8" // 指定编码，默认为 utf8。
     *      }
     *      ```
     *      @param data 待写入的数据
     *      @param options 指定写入选项
     *      @return 实际写入的字节数
     *      
     */
    writeFileAsync(data: string, options: FIBJS.GeneralObject): Promise<number>;

    /**
     * @description 关闭当前文件句柄 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 关闭当前文件句柄 
     */
    closeSync(): void;

    /**
     * @description 关闭当前文件句柄 
     */
    closeAsync(): Promise<void>;

}

