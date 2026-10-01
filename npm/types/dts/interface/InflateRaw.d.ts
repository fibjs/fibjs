/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description InflateRaw decompression codec, decompresses data compressed with the deflate algorithm (raw format)
 */
declare class Class_InflateRaw extends Class_ZlibCodec {
    /**
     * @description InflateRaw constructor
     *      @param opts decompression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

