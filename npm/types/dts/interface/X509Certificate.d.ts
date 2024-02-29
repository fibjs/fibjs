/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/KeyObject.d.ts" />
/**
 * @description 封装 X509 证书并提供信息读取
 * 
 * ```JavaScript
 * const crypto = require('crypto');
 * const cert = new crypto.X509Certificate(Buffer.from('...'));
 * ```
 * 
 */
declare class Class_X509Certificate extends Class_object {
    /**
     * @description 从证书创建一个 X509Certificate 对象
     *     @param cert 证书的二进制数据
     *     @return 返回一个 X509Certificate 对象
     *     
     */
    constructor(cert: Class_Buffer);

    /**
     * @description 证书的主题 
     */
    readonly subject: string;

    /**
     * @description 证书的序列号 
     */
    readonly serialNumber: string;

    /**
     * @description 证书的认证的公钥 
     */
    readonly publicKey: Class_KeyObject;

    /**
     * @description 证书的主题备用名称 
     */
    readonly subjectAltName: string;

    /**
     * @description 证书的信息访问扩展，返回一个换行分隔的访问描述列表。每行开头为访问方法和访问位置的类型，后跟冒号和与访问位置关联的值 
     */
    readonly infoAccess: string;

    /**
     * @description 证书的颁发者 
     */
    readonly issuer: string;

    /**
     * @description 证书是否是 CA 证书 
     */
    readonly ca: boolean;

    /**
     * @description 证书的路径长度约束 
     */
    readonly pathlen: number;

    /**
     * @description 证书的密钥用法 
     */
    readonly keyUsage: any[];

    /**
     * @description 证书的 Netscape 类型 
     */
    readonly type: any[];

    /**
     * @description 证书的有效期开始时间 
     */
    readonly validFrom: string;

    /**
     * @description 证书的有效期结束时间 
     */
    readonly validTo: string;

    /**
     * @description 证书的原始二进制数据 
     */
    readonly raw: Class_Buffer;

    /**
     * @description 证书的 PEM 编码 
     */
    readonly pem: string;

    /**
     * @description 证书的 SHA-1 指纹 
     */
    readonly fingerprint: string;

    /**
     * @description 证书的 SHA-256 指纹 
     */
    readonly fingerprint256: string;

    /**
     * @description 证书的 SHA-512 指纹 
     */
    readonly fingerprint512: string;

    /**
     * @description 检查证书是否与给定的电子邮件地址匹配
     * 
     *     如果 options.subject 选项未定义或设置为 'default'，则仅当主题备用名称扩展不存在或不包含任何电子邮件地址时才考虑证书主题。
     * 
     *     如果 options.subject 选项设置为 'always' 并且主题备用名称扩展不存在或不包含匹配的电子邮件地址，则考虑证书主题。
     * 
     *     如果 options.subject 选项设置为 'never'，则永远不会考虑证书主题，即使证书不包含主题备用名称。
     * 
     *     @param email 电子邮件地址
     *     @param options 选项
     *     @return 如果证书匹配，则返回 email，如果不匹配，则返回 undefined
     *     
     */
    checkEmail(email: string, options?: FIBJS.GeneralObject): string;

    /**
     * @description 检查证书是否与给定的主机名匹配
     * 
     *     如果证书与给定的主机名匹配，则返回匹配的主题名称。返回的名称可能是完全匹配的（foo.example.com），也可能包含通配符（*.example.com ）。由于主机名比较不区分大小写，因此返回的使用者名称的大小写也可能与给定的 name 不同。
     * 
     *     options 支持一下属性：
     *      - subject: 'default'、'always' 或 'never'。默认值：'default'。
     *      - wildcards: 默认值 true。
     *      - partialWildcards: 默认值 true。
     *      - multiLabelWildcards: 默认值 false。
     *      - singleLabelSubdomains: 默认值 false。
     * 
     *     如果 options.subject 选项未定义或设置为 'default'，则仅当使用者备用名称扩展不存在或不包含任何 DNS 名称时才考虑证书使用者。
     * 
     *     如果 options.subject 选项设置为 'always' 并且使用者备用名称扩展不存在或不包含匹配的 DNS 名称，则将考虑证书使用者。
     * 
     *     如果 options.subject 选项设置为 'never'，则永远不会考虑证书主题，即使证书不包含主题备用名称。
     * 
     *      @param name 主机名
     *      @param options 选项
     *      @return 返回与 name 匹配的主题名称，如果没有主题名称与 name 匹配，则返回 undefined
     *     
     */
    checkHost(name: string, options?: FIBJS.GeneralObject): string;

    /**
     * @description 检查证书是否与给定的 IP 地址（IPv4 或 IPv6）匹配
     *     @param ip IP 地址
     *     @return 如果证书匹配，则返回 ip ，如果不匹配，则返回 undefined
     *     
     */
    checkIP(ip: string): string;

    /**
     * @description 检查此证书是否由给定的 issuer 颁发
     *     @param issuer 颁发者证书
     *     @return 如果证书由 issuer 颁发，则返回 true，否则返回 false
     *     
     */
    checkIssued(issuer: Class_X509Certificate): boolean;

    /**
     * @description 检查证书的公钥是否与给定的私钥签名匹配
     *     @param privateKey 私钥
     *     @return 如果匹配，则返回 true，否则返回 false
     *     
     */
    checkPrivateKey(privateKey: Class_KeyObject): boolean;

    /**
     * @description 验证此证书是否由给定的公钥签名。不对证书执行任何其他验证检查
     *     @param publicKey 公钥
     *     @return 如果验证成功，则返回 true，否则返回 false
     *     
     */
    verify(publicKey: Class_KeyObject): boolean;

}

