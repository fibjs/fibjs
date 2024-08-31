/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description CryptoKey 类来表示对称或非对称密钥，每种密钥公开不同的功能 
 */
declare class Class_CryptoKey extends Class_object {
    /**
     * @description 密钥的类型，对于秘密（对称）密钥，此属性为 'secret'，对于公共（非对称）密钥，此属性为 'public' 或 'private' 
     */
    readonly type: string;

    /**
     * @description 密钥的算法信息 
     */
    readonly algorithm: FIBJS.GeneralObject;

    /**
     * @description 密钥是否可以导出到外部 
     */
    readonly extractable: boolean;

    /**
     * @description 密钥的用途，可以是以下值的数组：
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

