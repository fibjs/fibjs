/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/HeapGraphEdge.d.ts" />
/**
 * @description HeapGraphNode represents a node in the heap view
 */
declare class Class_HeapGraphNode extends Class_object {
    /**
     * @description Node type, possible values:
     *      - profiler.Node_Hidden,         Hidden node, can be filtered out when shown to the user
     *      - profiler.Node_Array,          Array
     *      - profiler.Node_String,         String
     *      - profiler.Node_Object,         JS object (other than strings and arrays)
     *      - profiler.Node_Code,           Compiled code
     *      - profiler.Node_Closure,        Function closure
     *      - profiler.Node_RegExp,         Regular expression
     *      - profiler.Node_HeapNumber,     Sorted number in the heap
     *      - profiler.Node_Native,         Native object (not on the v8 heap)
     *      - profiler.Node_Synthetic,      Synthetic object
     *      - profiler.Node_ConsString,     Concatenated string
     *      - profiler.Node_SlicedString,   Sliced string
     *      - profiler.Node_Symbol,         Symbol (ES6)
     *      - profiler.Node_SimdValue,      Sorted SIMD value in the heap (ES7)
     *
     */
    readonly type: number;

    /**
     * @description Node name
     */
    readonly name: string;

    /**
     * @description Node description
     */
    readonly description: string;

    /**
     * @description Node ID
     */
    readonly id: number;

    /**
     * @description Node size, in bytes
     */
    readonly shallowSize: number;

    /**
     * @description Child node list, composed of HeapGraphEdge type objects
     */
    readonly childs: Class_HeapGraphEdge[];

}

