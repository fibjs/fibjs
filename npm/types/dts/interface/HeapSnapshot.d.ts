/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/HeapGraphNode.d.ts" />
/**
 * @description HeapSnapshots记录JS堆在某个时刻的状态 
 */
declare class Class_HeapSnapshot extends Class_object {
    /**
     * @description 和指定的堆快照进行比较
     *      @param before 待比较的堆快照
     *      @return 返回堆快照的比较结果
     *      
     */
    diff(before: Class_HeapSnapshot): FIBJS.GeneralObject;

    /**
     * @description 根据ID获取堆视图节点
     *      @param id 数字类型的节点ID
     *      @return 返回获取到的堆视图节点
     *      
     */
    getNodeById(id: number): Class_HeapGraphNode;

    /**
     * @description 根据指定名称保存HeapSnapshot
     *      @param fname 快照名称
     *     
     */
    save(fname: string): void;

    save(fname: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 时间信息 
     */
    readonly time: typeof Date;

    /**
     * @description 堆视图的根节点 
     */
    readonly root: Class_HeapGraphNode;

    /**
     * @description 堆视图节点组成的列表 
     */
    readonly nodes: any[];

}

