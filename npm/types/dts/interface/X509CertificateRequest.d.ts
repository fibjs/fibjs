/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
declare class Class_X509CertificateRequest extends Class_object {
    constructor(csr: Class_Buffer);

    /**
     * @description 证书请求的主题 
     */
    readonly subject: string;

    /**
     * @description 证书请求认证的公钥 
     */
    readonly publicKey: Class_KeyObject;

    /**
     * @description 证书请求的主题备用名称 
     */
    readonly subjectAltName: string;

    /**
     * @description 证书请求的信息访问扩展，返回一个换行分隔的访问描述列表。每行开头为访问方法和访问位置的类型，后跟冒号和与访问位置关联的值 
     */
    readonly infoAccess: string;

    /**
     * @description 证书请求的原始二进制数据 
     */
    readonly raw: Class_Buffer;

    /**
     * @description 证书请求的 PEM 编码 
     */
    readonly pem: string;

    /**
     * @description 检查证书请求的公钥是否与给定的私钥签名匹配
     *     @param privateKey 私钥
     *     @return 如果匹配，则返回 true，否则返回 false
     *     
     */
    checkPrivateKey(privateKey: Class_KeyObject): boolean;

    issue(options: FIBJS.GeneralObject): Class_X509Certificate;

}

