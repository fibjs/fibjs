/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Symmetric encryption algorithm object
 */
declare class Class_Cipher extends Class_object {
    /**
     * @description Sets the authentication tag
     *      @param buffer the authentication tag data to use, or a string decoded with encoding
     *      @param encoding the encoding of a string authentication tag data, default "utf8"
     *      @return returns the current Cipher object
     *
     */
    setAuthTag(buffer: Class_Buffer | string, encoding?: string): Class_Cipher;

    /**
     * @description Queries the authentication tag
     *       @return returns the authentication tag data
     *
     */
    getAuthTag(): Class_Buffer;

    /**
     * @description Sets additional authenticated data
     *      @param buffer the additional authenticated data to use, or a string decoded with the encoding option
     *      @param options the additional authenticated data options to use
     *      @return returns the current Cipher object
     *
     */
    setAAD(buffer: Class_Buffer | string, options?: FIBJS.GeneralObject): Class_Cipher;

    /**
     * @description Sets automatic padding
     *      @param autoPadding specifies whether to pad automatically
     *      @return returns the current Cipher object
     *
     */
    setAutoPadding(autoPadding?: boolean): Class_Cipher;

    /**
     * @description Updates the data
     *       @param data the data to update, or a string decoded with inputEncoding
     *       @param inputEncoding the encoding of the input data, default "utf8"
     *       @param outputEncoding the encoding of the output data
     *       @return returns the updated data
     *
     */
    update(data: Class_Buffer | string, inputEncoding?: string, outputEncoding?: string): any;

    /**
     * @description Finalizes the data
     *       @param outputEncoding the encoding of the output data
     *       @return returns the updated data
     *
     */
    final(outputEncoding?: string): any;

}

