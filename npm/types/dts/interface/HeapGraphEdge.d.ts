/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/HeapGraphNode.d.ts" />
/**
 * @description HeapGraphEdge表示两个HeapGraphNode节点间的关联，从上游节点到下游节点
 */
declare class Class_HeapGraphEdge extends Class_object {
    /**
     * @description 下游节点的链接方式，可能的值：
     *      - profiler.Edge_ContextVariable,  函数中的变量
     *      - profiler.Edge_Element,          数组中的元素
     *      - profiler.Edge_Property,         有名对象的属性
     *      - profiler.Edge_Internal,         JS无法进入的链接
     *      - profiler.Edge_Hidden,           指向需要事先计算出空间大小的节点
     *      - profiler.Edge_Shortcut,         指向无法事先计算出空间大小的节点
     *      - profiler.Edge_Weak,             一个弱引用（被GC忽视）
     *     
     */
    readonly type: number;

    /**
     * @description 链接名称 
     */
    readonly name: string;

    /**
     * @description 链接的描述 
     */
    readonly description: string;

    /**
     * @description 获取HeapGraphEdge的上游HeapGraphNode节点
     *      @return 返回源HeapGraphNode节点
     *     
     */
    getFromNode(): Class_HeapGraphNode;

    /**
     * @description 获取HeapGraphEdge的下游HeapGraphNode节点
     *      @return 返回目的HeapGraphNode节点
     *     
     */
    getToNode(): Class_HeapGraphNode;

}

