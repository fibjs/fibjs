/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description 文件的基础信息对象
 * 
 *   Stat 对象通过 fs.stat, File.stat, fs.readdir 查询，不可独立创建
 *  
 */
declare class Class_Stat extends Class_object {
    /**
     * @description 文件名称 
     */
    readonly name: string;

    /**
     * @description 包含该文件的设备 ID 
     */
    readonly dev: number;

    /**
     * @description 文件中的 Inode 数量 
     */
    readonly ino: number;

    /**
     * @description 文件权限，Windows 不支持此属性 
     */
    readonly mode: number;

    /**
     * @description 与此文件相关联的硬链接数量 
     */
    readonly nlink: number;

    /**
     * @description 文件拥有者的id 
     */
    readonly uid: number;

    /**
     * @description 文件所属的组id 
     */
    readonly gid: number;

    /**
     * @description 对于特殊类型的文件, 包含该文件的设备 ID 
     */
    readonly rdev: number;

    /**
     * @description 文件尺寸 
     */
    readonly size: number;

    /**
     * @description 在 I/O 操作中文件系统区块大小 
     */
    readonly blksize: number;

    /**
     * @description 分配给该文件的区块数量 
     */
    readonly blocks: number;

    /**
     * @description 文件最后修改时间 
     */
    readonly mtime: typeof Date;

    /**
     * @description 文件最后修改时间(ms) 
     */
    readonly mtimeMs: number;

    /**
     * @description 文件最后访问时间 
     */
    readonly atime: typeof Date;

    /**
     * @description 文件最后访问时间(ms) 
     */
    readonly atimeMs: number;

    /**
     * @description 文件创建时间 
     */
    readonly ctime: typeof Date;

    /**
     * @description 文件创建时间(ms) 
     */
    readonly ctimeMs: number;

    /**
     * @description 文件产生时间 
     */
    readonly birthtime: typeof Date;

    /**
     * @description 文件产生时间(ms) 
     */
    readonly birthtimeMs: number;

    /**
     * @description 查询文件是否有写入权限
     *      @return 为 true 则有写入权限
     *      
     */
    isWritable(): boolean;

    /**
     * @description 查询文件是否有读权限
     *      @return 为 true 则有读权限
     *      
     */
    isReadable(): boolean;

    /**
     * @description 查询文件是否有执行权限
     *      @return 为 true 则有执行权限
     *      
     */
    isExecutable(): boolean;

    /**
     * @description 查询文件是否隐藏
     *      @return 为 true 则隐藏
     *      
     */
    isHidden(): boolean;

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
     * @description 查询文件是否是内存文件
     *      @return 为 true 则是内存文件
     *      
     */
    isMemory(): boolean;

    /**
     * @description 查询文件是否是 Socket
     *      @return 为 true 则是 Socket
     *      
     */
    isSocket(): boolean;

}

