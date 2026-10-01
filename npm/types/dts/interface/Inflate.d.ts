/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Inflate decompression codec, decompresses data compressed with the deflate algorithm (zlib format)
 */
declare class Class_Inflate extends Class_ZlibCodec {
    /**
     * @description Inflate constructor
     *      @param opts decompression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

