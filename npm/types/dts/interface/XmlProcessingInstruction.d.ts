/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description XmlProcessingInstruction 对象表示 xml 处理指令
 *  
 */
declare class Class_XmlProcessingInstruction extends Class_XmlNode {
    /**
     * @description 返回此处理指令的目标
     * 	
     */
    readonly target: string;

    /**
     * @description 设置或返回此处理指令的内容
     * 	
     */
    data: string;

}

