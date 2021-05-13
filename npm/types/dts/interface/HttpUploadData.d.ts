/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description 包含 multipart 的一个条目数据 
 */
declare class Class_HttpUploadData extends Class_object {
    /**
     * @description 包含本条目数据的文件名 
     */
    readonly fileName: string;

    /**
     * @description 包含本条目数据的类型 
     */
    readonly contentType: string;

    /**
     * @description 包含本条目数据的传输编码类型 
     */
    readonly contentTransferEncoding: string;

    /**
     * @description 包含本条目数据部分的流对象 
     */
    readonly body: Class_SeekableStream;

}

