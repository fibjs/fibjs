/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../module/fs_constants.d.ts" />
/// <reference path="../interface/Stat.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/// <reference path="../interface/BufferedStream.d.ts" />
/// <reference path="../interface/FSWatcher.d.ts" />
/// <reference path="../interface/StatsWatcher.d.ts" />
/**
 * @description 文件系统处理模块
 * 
 *  使用方法：
 *  ```JavaScript
 *  var fs = require('fs');
 *  ```
 * 
 *  一些注意点:
 * 
 *  - 运行 `fs.watch(filename)` 会返回一个继承自 EventEmitter 的 watcher, 它支持 'change', 'changeonly', 'renameonly' 三个事件
 *  - `fs.watchFile(target)` 和 `fs.unwatchFile(target)` 依然可以成对使用
 *  - `fs.watchFile(target)` 会返回一个继承自 EventEmitter 的 StatsWatcher 对象, 调用 `fs.unwatchFile(target)` 等价于调用 `StatsWatcher.close()`.
 *  - 因为 uv 在 Linux 上的实现, `fs.watch` 的 `recursive` 选项仅在 win32/darwin 被稳定支持. 你依然可以尝试在 Linux 中尝试使用 `fs.watch('/path/to', { recursive: true }, handler)`, 但可能会发现 `handler` 被回调的时机与你预期的有差异
 *  
 */
declare module 'fs' {
    /**
     * @description seek 方式常量，移动到绝对位置 
     */
    export const SEEK_SET: 0;

    /**
     * @description seek 方式常量，移动到当前位置的相对位置 
     */
    export const SEEK_CUR: 1;

    /**
     * @description seek 方式常量，移动到文件结尾的相对位置 
     */
    export const SEEK_END: 2;

    /**
     * ! fs模块的常量对象 
     */
    const constants: typeof import ('fs_constants');

    /**
     * @description 查询指定的文件或目录是否存在
     *      @param path 指定要查询的路径
     *      @return 返回 True 表示文件或目录存在
     *      
     */
    function exists(path: string): boolean;

    function exists(path: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 查询用户对指定的文件的权限
     *      @param path 指定要查询的路径
     *      @param mode 指定查询的权限,默认为文件是否存在
     *      
     */
    function access(path: string, mode?: number): void;

    function access(path: string, mode?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 创建硬链接文件, windows 下不支持此方法
     *      @param oldPath 源文件
     *      @param newPath 将要被创建的文件
     *      
     */
    function link(oldPath: string, newPath: string): void;

    function link(oldPath: string, newPath: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 删除指定的文件
     *      @param path 指定要删除的路径
     *      
     */
    function unlink(path: string): void;

    function unlink(path: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 创建一个目录
     *      @param path 指定要创建的目录名
     *      @param mode 指定文件权限，Windows 忽略此参数，默认值: 0777
     *      
     */
    function mkdir(path: string, mode?: number): void;

    function mkdir(path: string, mode?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 创建一个目录
     * 
     *      创建参数可以包含以下值：
     *      ```JavaScript
     *      {
     *          recursive: false, // 指定是否父目录不存在是是否自动创建，默认值: false
     *          mode: 0777 // 指定文件权限，Windows 忽略此参数，默认值: 0777
     *      }
     *      ```
     *      @param path 指定要创建的目录名
     *      @param opt 指定创建参数
     *      
     */
    function mkdir(path: string, opt: FIBJS.GeneralObject): void;

    function mkdir(path: string, opt: FIBJS.GeneralObject, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 删除一个目录
     *      @param path 指定要删除的目录名
     *      
     */
    function rmdir(path: string): void;

    function rmdir(path: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 重新命名一个文件
     *      @param from 指定更名的文件
     *      @param to 指定要修改的新文件名
     *      
     */
    function rename(from: string, to: string): void;

    function rename(from: string, to: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 将 src 拷贝到 dest。 默认情况下，如果 dest 已经存在，则覆盖它。
     * 
     *      mode 是一个可选的整数，指定拷贝操作的行为。 可以创建由两个或更多个值按位或组成的掩码（比如 fs.constants.COPYFILE_EXCL | fs.constants.COPYFILE_FICLONE）。
     *      - fs.constants.COPYFILE_EXCL - 如果 dest 已存在，则拷贝操作将失败。
     *      - fs.constants.COPYFILE_FICLONE - 拷贝操作将尝试创建写时拷贝（copy-on-write）链接。如果平台不支持写时拷贝，则使用后备的拷贝机制。
     *      - fs.constants.COPYFILE_FICLONE_FORCE - 拷贝操作将尝试创建写时拷贝链接。如果平台不支持写时拷贝，则拷贝操作将失败。
     * 
     *      @param from 指定要拷贝的源文件名
     *      @param to 指定要拷贝的目标文件名
     *      @param mode 指定拷贝操作的修饰符，缺省为 0
     *      
     */
    function copyFile(from: string, to: string, mode?: number): void;

    function copyFile(from: string, to: string, mode?: number, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 设置指定文件的访问权限，Windows 不支持此方法
     *      @param path 指定操作的文件
     *      @param mode 指定设定的访问权限
     *      
     */
    function chmod(path: string, mode: number): void;

    function chmod(path: string, mode: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 设置指定文件的访问权限，若文件是软连接则不改变指向文件的权限，只在macOS、BSD 系列平台上可用
     *      @param path 指定操作的文件
     *      @param mode 指定设定的访问权限
     *      
     */
    function lchmod(path: string, mode: number): void;

    function lchmod(path: string, mode: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 设置指定文件的拥有者，Windows 不支持此方法
     *      @param path 指定设置的文件
     *      @param uid 文件拥有者用户id
     *      @param gid 文件拥有者组id
     *      
     */
    function chown(path: string, uid: number, gid: number): void;

    function chown(path: string, uid: number, gid: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 设置指定文件的拥有者，如果指定的文件是软连接则不会改变其指向文件的拥有者，Windows 不支持此方法
     *      @param path 指定设置的文件
     *      @param uid 文件拥有者用户id
     *      @param gid 文件拥有者组id
     *      
     */
    function lchown(path: string, uid: number, gid: number): void;

    function lchown(path: string, uid: number, gid: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 查询指定文件的基础信息
     *      @param path 指定查询的文件
     *      @return 返回文件的基础信息
     *      
     */
    function stat(path: string): Class_Stat;

    function stat(path: string, callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description 查询指定文件的基础信息, 和stat不同的是, 当path是一个软连接的时候，返回的将是这个软连接的信息而不是指向的文件的信息
     *      @param path 指定查询的文件
     *      @return 返回文件的基础信息
     *      
     */
    function lstat(path: string): Class_Stat;

    function lstat(path: string, callback: (err: Error | undefined | null, retVal: Class_Stat)=>any): void;

    /**
     * @description 读取指定的软连接文件, windows 下不支持此方法
     *      @param path 指定读取的软连接文件
     *      @return 返回软连接指向的文件名
     *      
     */
    function readlink(path: string): string;

    function readlink(path: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 返回指定路径的绝对路径，如果指定路径中包含相对路径也会被展开
     *      @param path 指定读取的路径
     *      @return 返回处理后的绝对路径
     *      
     */
    function realpath(path: string): string;

    function realpath(path: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 创建软连接文件
     *      @param target 目标文件，可以是文件、目录、或不存在的路径
     *      @param linkpath 将被创建的软连接文件
     *      @param type 创建的软连接类型, 可选类型为'file', 'dir', 'junction', 默认为'file', 该参数只在windows上有效，当为'junction'的时候将要创建的目标路径linkpath必须为绝对路径, 而target则会被自动转化为绝对路径。
     *      
     */
    function symlink(target: string, linkpath: string, type?: string): void;

    function symlink(target: string, linkpath: string, type?: string, callback?: (err: Error | undefined | null)=>any): void;

    /**
     * @description 修改文件尺寸,如果指定的长度大于源文件大小则用'\0'填充，否则多于的文件内容将丢失
     *      @param path 指定被修改文件的路径
     *      @param len 指定修改后文件的大小
     *      
     */
    function truncate(path: string, len: number): void;

    function truncate(path: string, len: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 根据文件描述符，读取文件内容
     *      @param fd 文件描述符
     *      @param buffer 读取结果写入的 Buffer 对象
     *      @param offset Buffer 写入偏移量， 默认为 0
     *      @param length 文件读取字节数，默认为 0
     *      @param position 文件读取位置，默认为当前文件位置
     *      @return 实际读取的字节数
     *      
     */
    function read(fd: number, buffer: Class_Buffer, offset?: number, length?: number, position?: number): number;

    function read(fd: number, buffer: Class_Buffer, offset?: number, length?: number, position?: number, callback?: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 根据文件描述符，改变文件模式。只在 POSIX 系统有效。
     *      @param fd 文件描述符
     *      @param mode 文件的模式
     *      
     */
    function fchmod(fd: number, mode: number): void;

    function fchmod(fd: number, mode: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 根据文件描述符，改变所有者。只在 POSIX 系统有效。
     *      @param fd 文件描述符
     *      @param uid 用户id
     *      @param gid 组id
     *      
     */
    function fchown(fd: number, uid: number, gid: number): void;

    function fchown(fd: number, uid: number, gid: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 根据文件描述符，同步数据到磁盘
     *      @param fd 文件描述符
     *      
     */
    function fdatasync(fd: number): void;

    function fdatasync(fd: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 根据文件描述符，同步数据到磁盘
     *      @param fd 文件描述符
     *      
     */
    function fsync(fd: number): void;

    function fsync(fd: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 读取指定目录的文件信息
     *      @param path 指定查询的目录
     *      @return 返回目录的文件信息数组
     *      
     */
    function readdir(path: string): any[];

    function readdir(path: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description 打开文件，用于读取，写入，或者同时读写
     * 
     *      参数 flags 支持的方式如下：
     *      - 'r' 只读方式，文件不存在则抛出错误。
     *      - 'r+' 读写方式，文件不存在则抛出错误。
     *      - 'w' 只写方式，文件不存在则自动创建，存在则将被清空。
     *      - 'w+' 读写方式，文件不存在则自动创建。
     *      - 'a' 只写添加方式，文件不存在则自动创建。
     *      - 'a+' 读写添加方式，文件不存在则自动创建。
     *      @param fname 指定文件名
     *      @param flags 指定文件打开方式，缺省为 "r"，只读方式
     *      @return 返回打开的文件对象
     *      
     */
    function openFile(fname: string, flags?: string): Class_SeekableStream;

    function openFile(fname: string, flags?: string, callback?: (err: Error | undefined | null, retVal: Class_SeekableStream)=>any): void;

    /**
     * @description 打开文件描述符
     * 
     *      参数 flags 支持的方式如下：
     *      - 'r' 只读方式，文件不存在则抛出错误。
     *      - 'r+' 读写方式，文件不存在则抛出错误。
     *      - 'w' 只写方式，文件不存在则自动创建，存在则将被清空。
     *      - 'w+' 读写方式，文件不存在则自动创建。
     *      - 'a' 只写添加方式，文件不存在则自动创建。
     *      - 'a+' 读写添加方式，文件不存在则自动创建。
     *      @param fname 指定文件名
     *      @param flags 指定文件打开方式，缺省为 "r"，只读方式
     *      @param mode 当创建文件的时候，指定文件的模式，默认 0666
     *      @return 返回打开的文件描述符
     *      
     */
    function open(fname: string, flags?: string, mode?: number): number;

    function open(fname: string, flags?: string, mode?: number, callback?: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description 关闭文件描述符
     *      @param fd 文件描述符
     *      
     */
    function close(fd: number): void;

    function close(fd: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 打开文本文件，用于读取，写入，或者同时读写
     * 
     *      参数 flags 支持的方式如下：
     *      - 'r' 只读方式，文件不存在则抛出错误。
     *      - 'r+' 读写方式，文件不存在则抛出错误。
     *      - 'w' 只写方式，文件不存在则自动创建，存在则将被清空。
     *      - 'w+' 读写方式，文件不存在则自动创建。
     *      - 'a' 只写添加方式，文件不存在则自动创建。
     *      - 'a+' 读写添加方式，文件不存在则自动创建。
     *      @param fname 指定文件名
     *      @param flags 指定文件打开方式，缺省为 "r"，只读方式
     *      @return 返回打开的文件对象
     *      
     */
    function openTextStream(fname: string, flags?: string): Class_BufferedStream;

    function openTextStream(fname: string, flags?: string, callback?: (err: Error | undefined | null, retVal: Class_BufferedStream)=>any): void;

    /**
     * @description 打开文本文件，并读取内容
     *      @param fname 指定文件名
     *      @return 返回文件文本内容
     *      
     */
    function readTextFile(fname: string): string;

    function readTextFile(fname: string, callback: (err: Error | undefined | null, retVal: string)=>any): void;

    /**
     * @description 打开二进制文件，并读取内容
     *      @param fname 指定文件名
     *      @param encoding 指定解码方式，缺省不解码
     *      @return 返回文件文本内容
     *      
     */
    function readFile(fname: string, encoding?: string): any;

    function readFile(fname: string, encoding?: string, callback?: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description 打开文件，以数组方式读取一组文本行，行结尾标识基于 EOL 属性的设置，缺省时，posix:"\n"；windows:"\r\n"
     *      @param fname 指定文件名
     *      @param maxlines 指定此次读取的最大行数，缺省读取全部文本行
     *      @return 返回读取的文本行数组，若无数据可读，或者连接中断，空数组
     *      
     */
    function readLines(fname: string, maxlines?: number): any[];

    /**
     * @description 创建文本文件，并写入内容
     *      @param fname 指定文件名
     *      @param txt 指定要写入的字符串
     *      
     */
    function writeTextFile(fname: string, txt: string): void;

    function writeTextFile(fname: string, txt: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 创建二进制文件，并写入内容
     *      @param fname 指定文件名
     *      @param data 指定要写入的二进制数据
     *      
     */
    function writeFile(fname: string, data: Class_Buffer): void;

    function writeFile(fname: string, data: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 创建二进制文件，并写入内容
     *      @param fname 指定文件名
     *      @param data 指定要写入的二进制数据
     *      
     */
    function appendFile(fname: string, data: Class_Buffer): void;

    function appendFile(fname: string, data: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 设置 zip 虚拟文件映射
     *      @param fname 指定映射路径
     *      @param data 指定映射的 zip 文件数据
     *      
     */
    function setZipFS(fname: string, data: Class_Buffer): void;

    /**
     * @description 清除 zip 虚拟文件映射
     *      @param fname 指定映射路径，缺省清除全部缓存
     *      
     */
    function clearZipFS(fname?: string): void;

    /**
     * @description 观察一个文件, 返回对应的 watcher 对象
     *      @param fname 指定要观察的文件对象
     *      @return FSWatcher 对象
     *      
     */
    function watch(fname: string): Class_FSWatcher;

    /**
     * @description 观察一个文件, 返回对应的 watcher 对象
     *      @param fname 指定要观察的文件对象
     *      @param callback `(evtType: 'change' | 'rename', filename: string) => any` 当文件对象发生变化时的处理回调
     *      @return FSWatcher 对象
     *      
     */
    function watch(fname: string, callback: (...args: any[])=>any): Class_FSWatcher;

    /**
     * @description 观察一个文件, 返回对应的 watcher 对象
     *      
     *      options 支持的选项如下：
     *      ```JavaScript
     *      {
     *          "persistent": true, // {boolean} default: true 是否只要目标文件还在被观察, 进程就不退出
     *          "recursive": false, // {boolean} default: false 对于 fname 为文件夹的情况, 是否递归地观察其下所有的子目录
     *          "encoding": "utf8", // {enum} default: 'utf8' 指定解析传入的 fname 的字符编码
     *      }
     *      ```
     *      @param fname 指定要观察的文件对象
     *      @param options 观察选项
     *      @return FSWatcher 对象
     *      
     */
    function watch(fname: string, options: FIBJS.GeneralObject): Class_FSWatcher;

    /**
     * @description 观察一个文件, 返回对应的 watcher 对象
     * 
     *      options 支持的选项如下：
     *      ```JavaScript
     *      {
     *          "persistent": true, // {boolean} default: true 是否只要目标文件还在被观察, 进程就不退出
     *          "recursive": false, // {boolean} default: false 对于 fname 为文件夹的情况, 是否递归地观察其下所有的子目录
     *          "encoding": "utf8", // {enum} default: 'utf8' 指定解析传入的 fname 的字符编码
     *      }
     *      ```
     *      @param fname 指定要观察的文件对象
     *      @param options 观察选项
     *      @param callback `(evtType: 'change' | 'rename', filename: string) => any` 当文件对象发生变化时的处理回调
     *      @return FSWatcher 对象
     *      
     */
    function watch(fname: string, options: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_FSWatcher;

    /**
     * @description 观察一个文件, 返回对应的 StatsWatcher 对象
     *      @param fname 指定要观察的文件对象
     *      @param callback `(curStats: Stats, prevStats: Stats) => any` 当文件对象的 stats 发生变化时的处理回调
     *      @return StatsWatcher 对象
     *      
     */
    function watchFile(fname: string, callback: (...args: any[])=>any): Class_StatsWatcher;

    /**
     * @description 观察一个文件, 返回对应的 StatsWatcher 对象
     *     
     *      options 支持的选项如下：
     *      ```JavaScript
     *      {
     *          "persistent": true, // {boolean} default: true 是否只要目标文件还在被观察, 进程就不退出
     *          "recursive": false, // {boolean} default: false 对于 fname 为文件夹的情况, 是否递归地观察其下所有的子目录
     *          "encoding": "utf8", // {enum} default: 'utf8' 指定解析传入的 fname 的字符编码
     *      }
     *      ```
     *      @param fname 指定要观察的文件对象
     *      @param options 观察选项
     *      @param callback `(curStats: Stats, prevStats: Stats) => any` 当文件对象的 stats 发生变化时的处理回调
     *      @return StatsWatcher 对象
     *      
     */
    function watchFile(fname: string, options: FIBJS.GeneralObject, callback: (...args: any[])=>any): Class_StatsWatcher;

    /**
     * @description 从观察 fname 的 StatsWatcher 中移除所有观察事件的回调
     *      @param fname 指定要观察的文件对象
     * 
     *      @return StatsWatcher 对象
     *      
     */
    function unwatchFile(fname: string): void;

    /**
     * @description 从观察 fname 的 StatsWatcher 的观察事件回调中移除 `callback` 回调
     *     即便 callback 不再 StatsWatcher 的观察事件回调中也不会报错
     * 
     *      @param fname 指定要观察的文件对象
     *      @param callback 要移除的回调
     *      @return StatsWatcher 对象
     *      
     */
    function unwatchFile(fname: string, callback: (...args: any[])=>any): void;

}

