/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/HeapGraphNode.d.ts" />
/**
 * @description HeapSnapshots records the state of the JS heap at a certain moment
 */
declare class Class_HeapSnapshot extends Class_object {
    /**
     * @description Compares with the specified heap snapshot
     *      @param before the heap snapshot to compare with
     *      @return returns the heap snapshot comparison result
     *
     */
    diff(before: Class_HeapSnapshot): FIBJS.GeneralObject;

    /**
     * @description Gets a heap view node by ID
     *      @param id the node ID, of number type
     *      @return returns the obtained heap view node
     *
     */
    getNodeById(id: number): Class_HeapGraphNode;

    /**
     * @description Saves the HeapSnapshot under the specified name
     *      @param fname the snapshot name
     *
     */
    save(fname: string): void;

    save(fname: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Saves the HeapSnapshot under the specified name
     *      @param fname the snapshot name
     *
     */
    saveSync(fname: string): void;

    /**
     * @description Saves the HeapSnapshot under the specified name
     *      @param fname the snapshot name
     *
     */
    saveAsync(fname: string): Promise<void>;

    /**
     * @description Time information
     */
    readonly time: typeof Date;

    /**
     * @description Root node of the heap view
     */
    readonly root: Class_HeapGraphNode;

    /**
     * @description List composed of heap view nodes
     */
    readonly nodes: any[];

}

