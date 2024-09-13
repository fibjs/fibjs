/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/X509Certificate.d.ts" />
/**
 * @description X509CertificateRequest 对象是用于创建 x509 证书请求的对象，属于 crypto 模块
 * 
 * 可以使用 crpto.createCertificateRequest 创建证书请求，例如：
 * 
 * ```JavaScript
 * var crypto = require('crypto');
 * 
 * var pk = crypto.createPrivateKey(rsa4096_pem);
 * var req = crypto.createCertificateRequest({
 *     key: pk,
 *     subject: {
 *         C: "CN",
 *         O: "baoz.cn",
 *         CN: "baoz.me"
 *     }
 * });
 * ```
 * 
 * 通过调用 X509CertificateRequest 的 issue 方法可以生成正式的证书对象，例如：
 * 
 * ```JavaScript
 * let crt = req.issue({
 *     key: issuer_pk,
 *     issuer: {
 *         C: "CN",
 *         O: "baoz.cn",
 *         CN: "baoz.me"
 *     }
 * }); 
 * ```
 * 
 * 需要注意的是，X509CertificateRequest 对象的作用是创建 x509 证书请求，而不是证书本身，要想获得有效的证书还需要对其进行签名。同时，签名证书所使用的公钥必须和证书请求中使用的公钥一致。
 *  
 */
declare class Class_X509CertificateRequest extends Class_object {
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

    /**
     * @description 根据请求内容签发正式证书
     * 
     *      options 内的参数会用于调用 crypto.createPrivateKey 创建私钥对象，此外还支持以下签名参数：
     *       - issuer: 签发者的主题信息，包含 key/value 对的 subject 信息
     *       - ca: 是否是 CA 证书，缺省为 false
     *       - pathlen: 证书链的最大长度，缺省为 -1
     *       - validFrom: 证书的生效时间，缺省为当前时间
     *       - validTo: 证书的失效时间，如果同时设定 days，则以 validTo 为准
     *       - days: 证书有效期，缺省为 100 天
     *       - hashAlgorithm: 签名的 hash 算法，缺省为 'sha256'
     *       - keyUsage: 证书的密钥用途，以数组形式提供，缺省为 []，支持的值有：
     *         - 'digitalSignature': 数字签名
     *         - 'nonRepudiation': 非否认
     *         - 'keyEncipherment': 密钥加密
     *         - 'dataEncipherment': 数据加密
     *         - 'keyAgreement': 密钥协商
     *         - 'keyCertSign': 证书签名
     *         - 'cRLSign': CRL 签名
     *         - 'encipherOnly': 加密
     *         - 'decipherOnly': 解密
     * 
     *      - type: 证书的 Nescape 类型，以数组形式提供，缺省为 []，支持的值有：
     *         - 'client': 客户端
     *         - 'server': 服务器
     *         - 'email': 电子邮件
     *         - 'objsign': 对象签名
     *         - 'reserved': 保留
     *         - 'sslCA': SSL CA
     *         - 'emailCA': 电子邮件 CA
     *         - 'objCA': 对象 CA
     * 
     *      @param options 证书签发选项
     *      @return 返回签发的证书对象
     *     
     */
    issue(options: FIBJS.GeneralObject): Class_X509Certificate;

}

