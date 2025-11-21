/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 表示目录项的信息
 * 
 *  Dirent 对象通过 fs.glob, fs.readdir 查询，不可独立创建
 *  
 */
declare class Class_Dirent extends Class_object {
    /**
     * @description 文件名称 
     */
    readonly name: string;

    /**
     * @description 文件的父路径 
     */
    readonly parentPath: string;

    /**
     * @description 查询 Stat 是否描述了一个 block device
     *      @return 为 true 表示描述了一个 block device
     *      
     */
    isBlockDevice(): boolean;

    /**
     * @description 查询 Stat 是否描述了一个 character device
     *      @return 为 true 表示描述了一个 character device
     *      
     */
    isCharacterDevice(): boolean;

    /**
     * @description 查询文件是否是目录
     *      @return 为 true 则是目录
     *      
     */
    isDirectory(): boolean;

    /**
     * @description 查询 Stat 是否描述了一个 FIFO 管道
     *      @return 为 true 表示描述了一个 FIFO 管道
     *      
     */
    isFIFO(): boolean;

    /**
     * @description 查询文件是否是文件
     *      @return 为 true 则是文件
     *      
     */
    isFile(): boolean;

    /**
     * @description 查询文件是否是符号链接
     *      @return 为 true 则是符号链接
     *      
     */
    isSymbolicLink(): boolean;

    /**
     * @description 查询文件是否是 Socket
     *      @return 为 true 则是 Socket
     *      
     */
    isSocket(): boolean;

}

