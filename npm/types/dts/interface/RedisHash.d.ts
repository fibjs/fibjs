/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Redis database client Hash object; this object is a client bound to the given key and only calling its methods operates on the database
 *
 *  Used to operate on a Redis Hash object. To create one:
 *  ```JavaScript
 *  var db = require("db");
 *  var rdb = new db.openRedis("redis-server");
 *  var hash = rdb.getHash("test");
 *  ```
 *
 */
declare class Class_RedisHash extends Class_object {
    /**
     * @description Sets the field in the hash table to value; if the field already exists in the hash table the old value is overwritten; strings are encoded as utf8
     *      @param field the field to modify, a string is encoded as utf8
     *      @param value the value to modify, a string is encoded as utf8
     *
     */
    set(field: Class_Buffer | string, value: Class_Buffer | string): void;

    /**
     * @description Sets the field in the hash table to value only when the field does not exist. If the field already exists, the operation has no effect; strings are encoded as utf8
     *      @param field the field to modify, a string is encoded as utf8
     *      @param value the value to modify, a string is encoded as utf8
     *
     */
    setNX(field: Class_Buffer | string, value: Class_Buffer | string): void;

    /**
     * @description Sets multiple field-value pairs in the hash table at the same time; this command overwrites existing fields in the hash table
     *      @param kvs the field/value object to set
     */
    mset(kvs: FIBJS.GeneralObject): void;

    /**
     * @description Sets multiple field-value pairs in the hash table at the same time; this command overwrites existing fields in the hash table
     *      @param kvs the field/value list to set
     */
    mset(...kvs: any[]): void;

    /**
     * @description Returns the value of the given field in the hash table; a string field is encoded as utf8
     *      @param field the field to query, a string is encoded as utf8
     *      @return the value of the given field; returns null when the given field does not exist or the given key does not exist
     *
     */
    get(field: Class_Buffer | string): Class_Buffer;

    /**
     * @description Returns the values of one or more given fields in the hash table
     *      @param fields the array of fields to query
     *      @return a list containing the values of all the given fields
     */
    mget(fields: any[]): any[];

    /**
     * @description Returns the values of one or more given fields in the hash table
     *      @param fields the list of fields to query
     *      @return a list containing the values of all the given fields
     */
    mget(...fields: any[]): any[];

    /**
     * @description Adds the increment to the value stored in the field; a string field is encoded as utf8
     *      @param field the field to modify, a string is encoded as utf8
     *      @param num the number to add
     *      @return the value of the field after adding num
     *
     */
    incr(field: Class_Buffer | string, num?: number): number;

    /**
     * @description Returns all fields and values in the hash table
     *      @return returns a list containing all fields in the hash table
     */
    getAll(): any[];

    /**
     * @description Returns all fields in the hash table
     *      @return in the returned value, each field name (field name) is immediately followed by the field value (value), so the length of the returned value is twice the size of the hash table
     */
    keys(): any[];

    /**
     * @description Returns the number of fields in the hash table
     *      @return returns the number of fields in the hash table
     */
    len(): number;

    /**
     * @description Checks whether the given field exists in the hash table; a string field is encoded as utf8
     *      @param field the field to query, a string is encoded as utf8
     *      @return returns true if the hash table contains the given field; returns false if the hash table does not contain the given field or the key does not exist
     *
     */
    exists(field: Class_Buffer | string): boolean;

    /**
     * @description Removes one or more given fields from the hash table; non-existing fields are ignored
     *      @param fields the array of fields to remove
     *      @return the number of fields removed
     */
    del(fields: any[]): number;

    /**
     * @description Removes one or more given fields from the hash table; non-existing fields are ignored
     *      @param fields the list of fields to remove
     *      @return the number of fields removed
     */
    del(...fields: any[]): number;

}

