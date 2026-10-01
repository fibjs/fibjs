/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description data of one multipart entry
 */
declare class Class_HttpUploadData extends Class_object {
    /**
     * @description the file name of the data of this entry
     */
    readonly fileName: string;

    /**
     * @description the type of the data of this entry
     */
    readonly contentType: string;

    /**
     * @description the transfer encoding type of the data of this entry
     */
    readonly contentTransferEncoding: string;

    /**
     * @description the stream object containing the data part of this entry
     */
    readonly body: Class_SeekableStream;

}

