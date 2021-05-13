/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description LevelDB 数据库对象
 * 
 *  用以创建和管理字典对象，创建方法：
 *  ```JavaScript
 *  var db = require("db");
 *  var test = new db.openLevelDB("test.db");
 *  ```
 *  
 */
declare class Class_LevelDB extends Class_object {
    /**
     * @description 检查数据库内是否存在指定键值的数据
     *      @param key 指定要检查的键值
     *      @return 返回键值是否存在
     *      
     */
    has(key: Class_Buffer): boolean;

    has(key: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 查询指定键值的值
     *      @param key 指定要查询的键值
     *      @return 返回键值所对应的值，若不存在，则返回 null
     *      
     */
    get(key: Class_Buffer): Class_Buffer;

    get(key: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description 查询一组指定键值的值
     *      @param keys 指定要查询的键值数组
     *      @return 返回包含键值得数组
     *      
     */
    mget(keys: any[]): any[];

    /**
     * @description 设定一个键值数据，键值不存在则插入新数据
     *      @param key 指定要设定的键值
     *      @param value 指定要设定的数据
     *      
     */
    set(key: Class_Buffer, value: Class_Buffer): void;

    set(key: Class_Buffer, value: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 设定一组键值数据，键值不存在则插入新数据
     *      @param map 指定要设定的键值数据字典
     *      
     */
    mset(map: FIBJS.GeneralObject): void;

    /**
     * @description 删除一组指定键值的值
     *      @param keys 指定要删除的键值数组
     *      
     */
    mremove(keys: any[]): void;

    /**
     * @description 删除指定键值的全部值
     *      @param key 指定要删除的键值
     *      
     */
    remove(key: Class_Buffer): void;

    remove(key: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description 枚举数据库中所有的键值对
     * 
     *      回调函数有两个参数，(value, key)
     * 
     *      ```JavaScript
     *      var db = require("db");
     *      var test = new db.openLevelDB("test.db");
     * 
     *      test.forEach(function(value, key){
     *         ...
     *      });
     *      ```
     *      @param func 枚举回调函数
     *      
     */
    forEach(func: (...args: any[])=>any): void;

    /**
     * @description 枚举数据库中键值在 from 和 to 之间的键值对
     * 
     *      回调函数有两个参数，(value, key)
     * 
     *      ```JavaScript
     *      var db = require("db");
     *      var test = new db.openLevelDB("test.db");
     * 
     *      test.between("aaa", "bbb", function(value, key){
     *         ...
     *      });
     *      ```
     *      @param from 枚举的最小键值，枚举时包含此键值
     *      @param to 枚举的最大键值，枚举时不包含此键值
     *      @param func 枚举回调函数
     *      
     */
    between(from: Class_Buffer, to: Class_Buffer, func: (...args: any[])=>any): void;

    /**
     * @description 在当前数据库上开启一个事务
     *      @return 返回一个开启的事务对象 
     */
    begin(): Class_LevelDB;

    /**
     * @description 提交当前事务 
     */
    commit(): void;

    /**
     * @description 关闭当前数据库连接或事务 
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

}

