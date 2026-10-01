/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ZlibCodec.d.ts" />
/**
 * @description Deflate compression codec, compresses data with the deflate algorithm (zlib format)
 */
declare class Class_Deflate extends Class_ZlibCodec {
    /**
     * @description Deflate constructor
     *      @param opts compression options
     *
     */
    constructor(opts?: FIBJS.GeneralObject);

}

