/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/XmlNode.d.ts" />
/**
 * @description The XmlProcessingInstruction object represents an xml processing instruction
 *
 */
declare class Class_XmlProcessingInstruction extends Class_XmlNode {
    /**
     * @description Returns the target of this processing instruction
     *
     */
    readonly target: string;

    /**
     * @description Sets or returns the content of this processing instruction
     *
     */
    data: string;

}

