/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Information representing a directory entry
 *
 *  The DirEntry object is queried through fs.glob, fs.readdir and cannot be created independently
 *
 */
declare class Class_DirEntry extends Class_object {
    /**
     * @description File name
     */
    readonly name: string;

    /**
     * @description Parent path of the file
     */
    readonly parentPath: string;

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
     * @description Queries whether the file is a Socket
     *      @return true if it is a Socket
     *
     */
    isSocket(): boolean;

}

