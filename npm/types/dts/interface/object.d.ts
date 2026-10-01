/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description Basic object; all objects inherit from it
 */
declare class Class_object {
    /**
     * @description Returns the string representation of the object, which is generally "[Native Object]"; objects can reimplement this according to their own characteristics
     *      @return returns the string representation of the object
     *
     */
    toString(): string;

    /**
     * @description Returns the JSON representation of the object, which is generally the set of readable properties defined by the object
     *      @param key unused
     *      @return returns the JSON-serializable value
     *
     */
    toJSON(key?: string): any;

}

