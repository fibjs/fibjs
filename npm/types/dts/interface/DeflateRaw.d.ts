/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description DeflateRaw compression codec, compresses data with the deflate algorithm (raw format, without the zlib header)
 */
declare class Class_DeflateRaw extends Class_ZlibCodec {
    /**
     * @description DeflateRaw constructor
     *      @param opts compression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

