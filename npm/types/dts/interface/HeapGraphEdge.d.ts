/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/HeapGraphNode.d.ts" />
/**
 * @description HeapGraphEdge represents the association between two HeapGraphNode nodes, from the upstream node to the downstream node
 */
declare class Class_HeapGraphEdge extends Class_object {
    /**
     * @description Link type of the downstream node, possible values:
     *      - profiler.Edge_ContextVariable,  Variable in a function
     *      - profiler.Edge_Element,          Element in an array
     *      - profiler.Edge_Property,         Property of a named object
     *      - profiler.Edge_Internal,         Link that JS cannot enter
     *      - profiler.Edge_Hidden,           Points to a node whose space size must be computed in advance
     *      - profiler.Edge_Shortcut,         Points to a node whose space size cannot be computed in advance
     *      - profiler.Edge_Weak,             A weak reference (ignored by the GC)
     *
     */
    readonly type: number;

    /**
     * @description Link name
     */
    readonly name: string;

    /**
     * @description Link description
     */
    readonly description: string;

    /**
     * @description Gets the upstream HeapGraphNode node of the HeapGraphEdge
     *      @return returns the source HeapGraphNode node
     *
     */
    getFromNode(): Class_HeapGraphNode;

    /**
     * @description Gets the downstream HeapGraphNode node of the HeapGraphEdge
     *      @return returns the destination HeapGraphNode node
     *
     */
    getToNode(): Class_HeapGraphNode;

}

