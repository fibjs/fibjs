/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Unzip decompression codec, automatically detects the gzip or deflate format and decompresses it
 */
declare class Class_Unzip extends Class_ZlibCodec {
    /**
     * @description Unzip constructor
     *      @param opts decompression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

