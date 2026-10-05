/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Redis database client List object; this object is a client bound to the given key and only calling its methods operates on the database
 *
 *  Used to operate on a Redis List object. To create one:
 *  ```JavaScript
 *  var db = require("db");
 *  var rdb = new db.openRedis("redis-server");
 *  var list = rdb.getList("test");
 *  ```
 *
 */
declare class Class_RedisList extends Class_object {
    /**
     * @description Inserts one or more values at the head of the list
     *      @param values the values to insert
     *      @return the length of the list after insertion
     */
    push(values: any[]): number;

    /**
     * @description Inserts one or more values at the head of the list
     *      @param values the values to insert
     *      @return the length of the list after insertion
     */
    push(...values: any[]): number;

    /**
     * @description Removes and returns the head element of the list key
     *      @return the head element of the list, or null if the list is empty
     */
    pop(): Class_Buffer;

    /**
     * @description Inserts one or more values at the tail (rightmost end) of the list
     *      @param values the values to insert
     *      @return the length of the list after insertion
     */
    rpush(values: any[]): number;

    /**
     * @description Inserts one or more values at the tail (rightmost end) of the list
     *      @param values the values to insert
     *      @return the length of the list after insertion
     */
    rpush(...values: any[]): number;

    /**
     * @description Removes and returns the tail (rightmost) element of the list key
     *      @return the head element of the list, or null if the list is empty
     */
    rpop(): Class_Buffer;

    /**
     * @description Sets the element at the given index of the list to value
     *
     *      value may be a Buffer or a string; a string is encoded as utf8.
     *      @param index the index to modify
     *      @param value the value to modify
     *
     */
    set(index: number, value: Class_Buffer | string): void;

    /**
     * @description Returns the element at the given index of the list
     *      @param index the index to query
     *      @return the element at the given index of the list
     */
    get(index: number): Class_Buffer;

    /**
     * @description Inserts value into the list before the pivot value
     *
     *      pivot and value may each be a Buffer or a string; a string is encoded as utf8.
     *      @param pivot the value to search for on insertion
     *      value may be a Buffer or a string; a string is encoded as utf8.
     *      @param value the value to insert
     *      @return the length of the list after insertion
     *
     */
    insertBefore(pivot: Class_Buffer | string, value: Class_Buffer | string): number;

    /**
     * @description Inserts value into the list after the pivot value
     *
     *      pivot and value may each be a Buffer or a string; a string is encoded as utf8.
     *      @param pivot the value to search for on insertion
     *      value may be a Buffer or a string; a string is encoded as utf8.
     *      @param value the value to insert
     *      @return the length of the list after insertion
     *
     */
    insertAfter(pivot: Class_Buffer | string, value: Class_Buffer | string): number;

    /**
     * @description Removes elements equal to the value parameter from the list according to the count parameter
     *
     *      @param count the number of elements to remove
     *      value may be a Buffer or a string; a string is encoded as utf8.
     *      @param value the value to remove
     *      @return the number of elements removed
     *
     */
    remove(count: number, value: Class_Buffer | string): number;

    /**
     * @description Trims a list so that it keeps only the elements in the given range; elements outside the range are removed
     *      @param start the start index to trim; 0 means the first element and -1 means the last element
     *      @param stop the stop index to trim; 0 means the first element and -1 means the last element
     */
    trim(start: number, stop: number): void;

    /**
     * @description Returns the length of the list
     *      @return returns the length of the list
     */
    len(): number;

    /**
     * @description Returns the elements in the given range of the list; the range is specified by the start and stop offsets and includes the elements at start and stop
     *      @param start the start index to query; 0 means the first element and -1 means the last element
     *      @param stop the stop index to query; 0 means the first element and -1 means the last element
     *      @return an array containing the elements in the given range
     */
    range(start: number, stop: number): any[];

}

