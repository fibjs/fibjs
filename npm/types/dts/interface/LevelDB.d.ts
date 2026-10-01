/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description LevelDB is a database operation object built into fibjs for creating and managing key-value dictionaries. With the LevelDB object, key-value data can be stored, queried, deleted and enumerated easily. It is based on Google's open source LevelDB implementation and offers high efficiency, reliability and scalability
 *
 * Creating a LevelDB object is very simple: call the db.openLevelDB method to create a database object with the given name. For example:
 *
 * ```JavaScript
 * var db = require("db");
 * var test = db.openLevelDB("test.db");
 * ```
 *
 * Here db is the fibjs database operation object, the openLevelDB method opens the leveldb database, test.db is the database name, and the returned test object is the object for operating on the database.
 *
 * The main operations supported by the LevelDB object include:
 *
 * - set(key, value): sets a key-value pair; inserts new data if the key does not exist.
 * - get(key): queries the value of the given key.
 * - has(key): checks whether the given key exists.
 * - remove(key): removes all values of the given key.
 * - forEach(func): enumerates all key-value pairs in the database.
 * - between(from, to, func): enumerates key-value pairs whose keys are between from and to.
 * - toJSON(key): returns the JSON representation of the object, generally the readable property set defined by the object.
 * - begin(): starts a transaction on the current database.
 * - commit(): commits the current transaction.
 * - close(): closes the current database connection or transaction.
 *
 * For example:
 *
 * ```JavaScript
 * var db = require("db");
 * var test = db.openLevelDB("test.db");
 *
 * test.set("test_key", "test_value");
 *
 * var value = test.get("test_key");
 * console.log("test_key:", value.toString());
 *
 * test.remove("test_key");
 *
 * console.log("has test_key:", test.has("test_key"));
 *
 * test.close();
 * ```
 *
 * The above covers the basic usage of the LevelDB object with examples, making key-value data easy and flexible to operate on. In practice it can be used for storage, caching, logging and other scenarios, improving data read/write efficiency, simplifying program logic and reducing development complexity.
 *
 */
declare class Class_LevelDB extends Class_object {
    /**
     * @description Checks whether data with the given key exists in the database
     *      @param key the key to check
     *      @return returns whether the key exists
     *
     */
    has(key: Class_Buffer): boolean;

    has(key: Class_Buffer, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Checks whether data with the given key exists in the database
     *      @param key the key to check
     *      @return returns whether the key exists
     *
     */
    hasSync(key: Class_Buffer): boolean;

    /**
     * @description Checks whether data with the given key exists in the database
     *      @param key the key to check
     *      @return returns whether the key exists
     *
     */
    hasAsync(key: Class_Buffer): Promise<boolean>;

    /**
     * @description Queries the value of the given key
     *      @param key the key to query
     *      @return returns the value of the key, or null if it does not exist
     *
     */
    get(key: Class_Buffer): Class_Buffer;

    get(key: Class_Buffer, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Queries the value of the given key
     *      @param key the key to query
     *      @return returns the value of the key, or null if it does not exist
     *
     */
    getSync(key: Class_Buffer): Class_Buffer;

    /**
     * @description Queries the value of the given key
     *      @param key the key to query
     *      @return returns the value of the key, or null if it does not exist
     *
     */
    getAsync(key: Class_Buffer): Promise<Class_Buffer>;

    /**
     * @description Queries the values of the given keys
     *      @param keys the array of keys to query
     *      @return returns an array containing the values of the keys
     *
     */
    mget(keys: any[]): any[];

    /**
     * @description Sets a key-value pair; inserts new data if the key does not exist
     *      @param key the key to set
     *      @param value the value to set
     *
     */
    set(key: Class_Buffer, value: Class_Buffer): void;

    set(key: Class_Buffer, value: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Sets a key-value pair; inserts new data if the key does not exist
     *      @param key the key to set
     *      @param value the value to set
     *
     */
    setSync(key: Class_Buffer, value: Class_Buffer): void;

    /**
     * @description Sets a key-value pair; inserts new data if the key does not exist
     *      @param key the key to set
     *      @param value the value to set
     *
     */
    setAsync(key: Class_Buffer, value: Class_Buffer): Promise<void>;

    /**
     * @description Sets a group of key-value pairs; inserts new data if the keys do not exist
     *      @param map the key-value dictionary to set
     *
     */
    mset(map: FIBJS.GeneralObject): void;

    /**
     * @description Removes the values of the given keys
     *      @param keys the array of keys to remove
     *
     */
    mremove(keys: any[]): void;

    /**
     * @description Removes all values of the given key
     *      @param key the key to remove
     *
     */
    remove(key: Class_Buffer): void;

    remove(key: Class_Buffer, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Removes all values of the given key
     *      @param key the key to remove
     *
     */
    removeSync(key: Class_Buffer): void;

    /**
     * @description Removes all values of the given key
     *      @param key the key to remove
     *
     */
    removeAsync(key: Class_Buffer): Promise<void>;

    /**
     *  @description Queries the smallest key
     *       @return returns the smallest key
     *
     */
    firstKey(): Class_Buffer;

    firstKey(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     *  @description Queries the smallest key
     *       @return returns the smallest key
     *
     */
    firstKeySync(): Class_Buffer;

    /**
     *  @description Queries the smallest key
     *       @return returns the smallest key
     *
     */
    firstKeyAsync(): Promise<Class_Buffer>;

    /**
     *  @description Queries the largest key
     *     @return returns the largest key
     */
    lastKey(): Class_Buffer;

    lastKey(callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     *  @description Queries the largest key
     *     @return returns the largest key
     */
    lastKeySync(): Class_Buffer;

    /**
     *  @description Queries the largest key
     *     @return returns the largest key
     */
    lastKeyAsync(): Promise<Class_Buffer>;

    /**
     * @description Enumerates all key-value pairs in the database
     *
     *      The callback function takes two parameters, (value, key)
     *
     *      ```JavaScript
     *      var db = require("db");
     *      var test = new db.openLevelDB("test.db");
     *
     *      test.forEach(function(value, key){
     *         ...
     *      });
     *      ```
     *      @param func the enumeration callback function
     *
     */
    forEach(func: (...args: any[])=>any): void;

    /**
     * @description Enumerates all key-value pairs in the database
     *
     *      The callback function takes two parameters, (value, key)
     *
     *      ```JavaScript
     *      var db = require("db");
     *      var test = new db.openLevelDB("test.db");
     *
     *      test.forEach("aaa", "bbb", function(value, key){
     *         ...
     *      });
     *      ```
     *      @param from the smallest key to enumerate; this key is included in the enumeration
     *      @param func the enumeration callback function
     *
     */
    forEach(from: Class_Buffer, func: (...args: any[])=>any): void;

    /**
     * @description Enumerates all key-value pairs in the database
     *
     *      The callback function takes two parameters, (value, key)
     *
     *      ```JavaScript
     *      var db = require("db");
     *      var test = new db.openLevelDB("test.db");
     *
     *      test.forEach("aaa", "bbb", function(value, key){
     *         ...
     *      });
     *      ```
     *      @param from the smallest key to enumerate; this key is included in the enumeration
     *      @param to the largest key to enumerate; this key is not included in the enumeration
     *      @param func the enumeration callback function
     *
     */
    forEach(from: Class_Buffer, to: Class_Buffer, func: (...args: any[])=>any): void;

    /**
     * @description Enumerates all key-value pairs in the database
     *
     *      The callback function takes two parameters, (value, key)
     *
     *      ```JavaScript
     *      var db = require("db");
     *      var test = new db.openLevelDB("test.db");
     *
     *      test.forEach(function(value, key){
     *         ...
     *      });
     *      ```
     *      @param opt the enumeration options, supporting skip, limit and reverse
     *      @param func the enumeration callback function
     *
     */
    forEach(opt: FIBJS.GeneralObject, func: (...args: any[])=>any): void;

    /**
     * @description Enumerates all key-value pairs in the database
     *
     *      The callback function takes two parameters, (value, key)
     *
     *      ```JavaScript
     *      var db = require("db");
     *      var test = new db.openLevelDB("test.db");
     *
     *      test.forEach("aaa", "bbb", function(value, key){
     *         ...
     *      });
     *      ```
     *      @param from the smallest key to enumerate; this key is included in the enumeration
     *      @param opt the enumeration options, supporting skip, limit and reverse
     *      @param func the enumeration callback function
     *
     */
    forEach(from: Class_Buffer, opt: FIBJS.GeneralObject, func: (...args: any[])=>any): void;

    /**
     * @description Enumerates all key-value pairs in the database
     *
     *      The callback function takes two parameters, (value, key)
     *
     *      ```JavaScript
     *      var db = require("db");
     *      var test = new db.openLevelDB("test.db");
     *
     *      test.forEach("aaa", "bbb", function(value, key){
     *         ...
     *      });
     *      ```
     *      @param from the smallest key to enumerate; this key is included in the enumeration
     *      @param to the largest key to enumerate; this key is not included in the enumeration
     *      @param opt the enumeration options, supporting skip, limit and reverse
     *      @param func the enumeration callback function
     *
     */
    forEach(from: Class_Buffer, to: Class_Buffer, opt: FIBJS.GeneralObject, func: (...args: any[])=>any): void;

    /**
     * @description Starts a transaction on the current database
     *      @return returns the started transaction object
     */
    begin(): Class_LevelDB;

    /**
     * @description Commits the current transaction
     */
    commit(): void;

    /**
     * @description Closes the current database connection or transaction
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the current database connection or transaction
     */
    closeSync(): void;

    /**
     * @description Closes the current database connection or transaction
     */
    closeAsync(): Promise<void>;

}

