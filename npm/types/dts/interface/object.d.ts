/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description 基础对象，所有对象均继承于此
 */
declare class Class_object {
    /**
     * @description 返回对象的字符串表示，一般返回 "[Native Object]"，对象可以根据自己的特性重新实现
     *      @return 返回对象的字符串表示
     *     
     */
    toString(): string;

    /**
     * @description 返回对象的 JSON 格式表示，一般返回对象定义的可读属性集合
     *      @param key 未使用
     *      @return 返回包含可 JSON 序列化的值
     *      
     */
    toJSON(key?: string): any;

}

