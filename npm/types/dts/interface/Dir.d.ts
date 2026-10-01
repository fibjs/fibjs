/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Iterator.d.ts" />
/// <reference path="../interface/DirEntry.d.ts" />
/**
 * @description Directory iterator object, created by fs.opendir, used to read directory entries one by one
 */
declare class Class_Dir extends Class_Iterator {
    /**
     * @description Dir constructor, creates a directory iterator object from a path
     *      @param path the directory to iterate
     *
     */
    constructor(path: string);

    /**
     * @description Queries the directory path of the current iteration
     */
    readonly path: string;

    /**
     * @description Reads the next directory entry, returns null when iteration ends
     */
    read(): Class_DirEntry;

    read(callback: (err: Error | undefined | null, retVal: Class_DirEntry)=>any): void;

    /**
     * @description Reads the next directory entry, returns null when iteration ends
     */
    readSync(): Class_DirEntry;

    /**
     * @description Reads the next directory entry, returns null when iteration ends
     */
    readAsync(): Promise<Class_DirEntry>;

    /**
     * @description Closes the directory iterator object and releases the iteration state; safe to call repeatedly
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the directory iterator object and releases the iteration state; safe to call repeatedly
     */
    closeSync(): void;

    /**
     * @description Closes the directory iterator object and releases the iteration state; safe to call repeatedly
     */
    closeAsync(): Promise<void>;

}

