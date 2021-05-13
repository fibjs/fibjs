/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description mongodb 数据库数据游标对象 
 */
declare class Class_MongoCursor extends Class_object {
    /**
     * @description 指定返回结果时跳过的记录数
     *     @param num 记录数
     *     @return 返回游标对象本身便于链式调用
     *    
     */
    skip(num: number): Class_MongoCursor;

    skip(num: number, callback: (err: Error | undefined | null, retVal: Class_MongoCursor)=>any): void;

    /**
     * @description 指定返回结果的最大记录数
     *     @param size 记录数
     *     @return 返回游标对象本身便于链式调用
     *    
     */
    limit(size: number): Class_MongoCursor;

    limit(size: number, callback: (err: Error | undefined | null, retVal: Class_MongoCursor)=>any): void;

    /**
     * @description 设定返回结果的排序
     *     @param opts 指定排序条件
     *     @return 返回游标对象本身便于链式调用
     *    
     */
    sort(opts: FIBJS.GeneralObject): Class_MongoCursor;

    /**
     * @description 查询当前游标是否有下一条记录
     *     @return 有记录则返回 true
     *    
     */
    hasNext(): boolean;

    /**
     * @description 返回当前游标的下一条记录
     *     @return 记录对象，无记录则返回 null
     *    
     */
    next(): FIBJS.GeneralObject;

    /**
     * @description 查询游标的记录总数
     *     @param applySkipLimit 指定是否查询 skip 和 limit 后的记录数，缺省为 false，查询全部记录数
     *     @return 返回记录总数
     *    
     */
    count(applySkipLimit?: boolean): number;

    /**
     * @description 查询游标的记录总数，相当于 count(true)
     *     @return 返回记录总数
     *    
     */
    size(): number;

    /**
     * @description 遍历全部记录并回调处理函数
     *     @param func 指定处理函数
     *    
     */
    forEach(func: (...args: any[])=>any): void;

    /**
     * @description 遍历处理全部记录，并返回处理结果
     *     @param func 指定处理函数
     *     @return 返回处理结果数组
     *    
     */
    map(func: (...args: any[])=>any): any[];

    /**
     * @description 返回当前游标全部记录的数组
     *    @return 返回包含全部数据的 Javascript 数组
     *   
     */
    toArray(): any[];

    /**
     * @description 修改 mongodb 服务器缺省索引策略，使用指定的索引进行查询
     *     @param opts 指定强制使用的索引
     *     @return 返回游标对象本身便于链式调用
     *    
     */
    hint(opts: FIBJS.GeneralObject): Class_MongoCursor;

}

