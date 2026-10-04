/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Basic file information object
 *
 *   The Stat object is queried through fs.stat, FileStream.stat, fs.readdir and cannot be created independently
 *
 */
declare class Class_Stat extends Class_object {
    /**
     * @description File name
     */
    readonly name: string;

    /**
     * @description Device ID containing the file
     */
    readonly dev: number;

    /**
     * @description Number of Inodes in the file
     */
    readonly ino: number;

    /**
     * @description File permission; not supported on Windows
     */
    readonly mode: number;

    /**
     * @description Number of hard links associated with this file
     */
    readonly nlink: number;

    /**
     * @description Owner id of the file
     */
    readonly uid: number;

    /**
     * @description Group id of the file
     */
    readonly gid: number;

    /**
     * @description For special file types, the device ID containing the file
     */
    readonly rdev: number;

    /**
     * @description File size
     */
    readonly size: number;

    /**
     * @description File system block size for I/O operations
     */
    readonly blksize: number;

    /**
     * @description Number of blocks allocated to the file
     */
    readonly blocks: number;

    /**
     * @description Last modification time of the file
     */
    readonly mtime: Date;

    /**
     * @description Last modification time of the file (ms)
     */
    readonly mtimeMs: number;

    /**
     * @description Last modification time of the file (ns), valid only when bigint is true
     */
    readonly mtimeNs: number;

    /**
     * @description Last access time of the file
     */
    readonly atime: Date;

    /**
     * @description Last access time of the file (ms)
     */
    readonly atimeMs: number;

    /**
     * @description Last access time of the file (ns), valid only when bigint is true
     */
    readonly atimeNs: number;

    /**
     * @description File status change time
     */
    readonly ctime: Date;

    /**
     * @description File status change time (ms)
     */
    readonly ctimeMs: number;

    /**
     * @description File status change time (ns), valid only when bigint is true
     */
    readonly ctimeNs: number;

    /**
     * @description File creation time
     */
    readonly birthtime: Date;

    /**
     * @description File creation time (ms)
     */
    readonly birthtimeMs: number;

    /**
     * @description File creation time (ns), valid only when bigint is true
     */
    readonly birthtimeNs: number;

    /**
     * @description Queries whether the file is writable
     *      @return true if it is writable
     *
     */
    isWritable(): boolean;

    /**
     * @description Queries whether the file is readable
     *      @return true if it is readable
     *
     */
    isReadable(): boolean;

    /**
     * @description Queries whether the file is executable
     *      @return true if it is executable
     *
     */
    isExecutable(): boolean;

    /**
     * @description Queries whether the file is hidden
     *      @return true if it is hidden
     *
     */
    isHidden(): boolean;

    /**
     * @description Queries whether the Stat describes a block device
     *      @return true if it describes a block device
     *
     */
    isBlockDevice(): boolean;

    /**
     * @description Queries whether the Stat describes a character device
     *      @return true if it describes a character device
     *
     */
    isCharacterDevice(): boolean;

    /**
     * @description Queries whether the file is a directory
     *      @return true if it is a directory
     *
     */
    isDirectory(): boolean;

    /**
     * @description Queries whether the Stat describes a FIFO pipe
     *      @return true if it describes a FIFO pipe
     *
     */
    isFIFO(): boolean;

    /**
     * @description Queries whether the file is a file
     *      @return true if it is a file
     *
     */
    isFile(): boolean;

    /**
     * @description Queries whether the file is a symbolic link
     *      @return true if it is a symbolic link
     *
     */
    isSymbolicLink(): boolean;

    /**
     * @description Queries whether the file is a memory file
     *      @return true if it is a memory file
     *
     */
    isMemory(): boolean;

    /**
     * @description Queries whether the file is a Socket
     *      @return true if it is a Socket
     *
     */
    isSocket(): boolean;

}

