/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/PKey.d.ts" />
/**
 * @description 信息摘要对象 
 */
declare class Class_Digest extends Class_object {
    /**
     * @description 更新二进制摘要信息
     *      @param data 二进制数据块
     *      @return 返回信息摘要对象本身
     *      
     */
    update(data: Class_Buffer): Class_Digest;

    /**
     * @description 计算并返回摘要
     *      @param codec 指定编码格式，允许值为："buffer", "hex", "base64", "utf8", 或者 iconv 模块支持的字符集
     *      @return 返回指定编码的摘要表示
     *      
     */
    digest(codec?: string): any;

    /**
     * @description 签名并返回摘要
     *      @param key 签名的私钥
     *      @return 返回签名后的数据
     *      
     */
    sign(key: Class_PKey): Class_Buffer;

    sign(key: Class_PKey, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 验证签名是否一致
     *      @param key 验证签名的公钥
     *      @param sign 指定要验证的签名
     *      @return 返回验证后的结果
     *      
     */
    verify(key: Class_PKey, sign: Class_Buffer): boolean;

    verify(key: Class_PKey, sign: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 查询当前信息摘要算法的摘要字节数 
     */
    readonly size: number;

}

