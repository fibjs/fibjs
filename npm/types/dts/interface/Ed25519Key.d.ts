/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/ECKey.d.ts" />
/**
 * @description 椭圆曲线加密算法对象 
 */
declare class Class_Ed25519Key extends Class_ECKey {
    /**
     * @description 从当前对象转换 X25519 公私钥对，仅支持 Ed25519
     *      @return 返回对应的 X25519 公钥的对象
     *     
     */
    toX25519(): Class_ECKey;

    toX25519(callback: (err: Error | undefined | null, retVal: Class_ECKey)=>any): void;

}

