/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The CryptoKey class represents symmetric or asymmetric keys, each exposing different features
 */
declare class Class_CryptoKey extends Class_object {
    /**
     * @description The type of the key; for secret (symmetric) keys this property is 'secret', for public (asymmetric) keys it is 'public' or 'private'
     */
    readonly type: string;

    /**
     * @description The key's algorithm information
     */
    readonly algorithm: FIBJS.GeneralObject;

    /**
     * @description Whether the key can be exported
     */
    readonly extractable: boolean;

    /**
     * @description The usages of the key, which can be an array of the following values:
     *         - 'encrypt'
     *         - 'decrypt'
     *         - 'sign'
     *         - 'verify'
     *         - 'deriveKey'
     *         - 'deriveBits'
     *         - 'wrapKey'
     *         - 'unwrapKey'
     *
     */
    readonly usages: any[];

}

