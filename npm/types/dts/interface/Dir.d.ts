/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Iterator.d.ts" />
/// <reference path="../interface/DirEntry.d.ts" />
/**
 * @description 目录迭代对象，由 fs.opendir 创建，用于逐个读取目录项
 */
declare class Class_Dir extends Class_Iterator {
    /**
     * @description Dir 构造函数，从路径创建目录迭代对象
     *      @param path 指定要迭代的目录
     *
     */
    constructor(path: string);

    /**
     * @description 查询当前迭代的目录路径
     */
    readonly path: string;

    /**
     * @description 读取下一个目录项，迭代结束时返回 null
     */
    read(): Class_DirEntry;

    read(callback: (err: Error | undefined | null, retVal: Class_DirEntry)=>any): void;

    /**
     * @description 读取下一个目录项，迭代结束时返回 null
     */
    readSync(): Class_DirEntry;

    /**
     * @description 读取下一个目录项，迭代结束时返回 null
     */
    readAsync(): Promise<Class_DirEntry>;

    /**
     * @description 关闭目录迭代对象，释放迭代状态，可安全重复调用
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 关闭目录迭代对象，释放迭代状态，可安全重复调用
     */
    closeSync(): void;

    /**
     * @description 关闭目录迭代对象，释放迭代状态，可安全重复调用
     */
    closeAsync(): Promise<void>;

}

