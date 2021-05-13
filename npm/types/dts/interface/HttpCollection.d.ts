/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description http 容器对象，用于 http header，cookie，query，form，等数据的存储与组织 
 */
declare class Class_HttpCollection extends Class_object {
    /**
     * @description 清除容器数据 
     */
    clear(): void;

    /**
     * @description 检查容器内是否存在指定键值的数据
     *      @param name 指定要检查的键值
     *      @return 返回键值是否存在
     *      
     */
    has(name: string): boolean;

    /**
     * @description 查询指定键值的第一个值
     *      @param name 指定要查询的键值
     *      @return 返回键值所对应的值，若不存在，则返回 undefined
     *      
     */
    first(name: string): any;

    /**
     * @description 查询指定键值的全部值
     *      @param name 指定要查询的键值，传递空字符串返回全部键值的结果
     *      @return 返回键值所对应全部值的数组，若数据不存在，则返回 null
     *      
     */
    all(name?: string): FIBJS.GeneralObject;

    /**
     * @description 添加一个键值数据，添加数据并不修改已存在的键值的数据
     *      @param map 指定要添加的键值数据字典
     *      
     */
    add(map: FIBJS.GeneralObject): void;

    /**
     * @description 添加一个键值的一组数据，添加数据并不修改已存在的键值的数据
     *      @param name 指定要添加的键值
     *      @param values 指定要添加的一组数据
     *      
     */
    add(name: string, values: any[]): void;

    /**
     * @description 添加一个键值数据，添加数据并不修改已存在的键值的数据
     *      @param name 指定要添加的键值
     *      @param value 指定要添加的数据
     *      
     */
    add(name: string, value: any): void;

    /**
     * @description 设定一个键值数据，设定数据将修改键值所对应的第一个数值，并清除相同键值的其余数据
     *      @param map 指定要设定的键值数据字典
     *      
     */
    set(map: FIBJS.GeneralObject): void;

    /**
     * @description 设定一个键值的一组数据，设定数据将修改键值所对应的数值，并清除相同键值的其余数据
     *      @param name 指定要设定的键值
     *      @param values 指定要设定的一组数据
     *      
     */
    set(name: string, values: any[]): void;

    /**
     * @description 设定一个键值数据，设定数据将修改键值所对应的第一个数值，并清除相同键值的其余数据
     *      @param name 指定要设定的键值
     *      @param value 指定要设定的数据
     *      
     */
    set(name: string, value: any): void;

    /**
     * @description 删除指定键值的全部值
     *      @param name 指定要删除的键值
     *      
     */
    remove(name: string): void;

    /**
     * @description 删除指定键值的全部值
     *      @param name 指定要删除的键值
     *      
     */
    delete(name: string): void;

    /**
     * @description 按照键值排序容器内的内容
     *      
     */
    sort(): void;

    /**
     * @description 查询容器内的键值
     *      @return 返回包含所有键值的数组
     *      
     */
    keys(): any[];

    /**
     * @description 查询容器内的数值
     *      @return 返回包含所有数值的数组
     *      
     */
    values(): any[];


}

