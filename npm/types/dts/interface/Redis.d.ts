/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/RedisHash.d.ts" />
/// <reference path="../interface/RedisList.d.ts" />
/// <reference path="../interface/RedisSet.d.ts" />
/// <reference path="../interface/RedisSortedSet.d.ts" />
/**
 * @description Redis database client object
 *
 *  Used to create and manage a Redis database. To create one:
 *  ```JavaScript
 *  var db = require("db");
 *  var test = new db.openRedis("redis-server");
 *  ```
 *
 */
declare class Class_Redis extends Class_object {
    /**
     * @description Basic redis command method
     *      @param cmd the command to send
     *      @param args the parameters to send
     *      @return returns the result returned by the server
     */
    command(cmd: string, ...args: any[]): any;

    /**
     * @description Associates the string value with key; if key already holds another value, SET overwrites the old value regardless of type
     *      @param key the key to associate
     *      @param value the data to associate
     *      @param ttl the time to live for key in milliseconds; if ttl is 0, no time to live is set
     */
    set(key: Class_Buffer, value: Class_Buffer, ttl?: number): void;

    /**
     * @description Sets the key to value only when the key does not exist. If the given key already exists, SETNX does nothing.
     *      @param key the key to associate
     *      @param value the data to associate
     *      @param ttl the time to live for key in milliseconds; if ttl is 0, no time to live is set
     */
    setNX(key: Class_Buffer, value: Class_Buffer, ttl?: number): void;

    /**
     * @description Sets the key to value only when the key already exists.
     *      @param key the key to associate
     *      @param value the data to associate
     *      @param ttl the time to live for key in milliseconds; if ttl is 0, no time to live is set
     */
    setXX(key: Class_Buffer, value: Class_Buffer, ttl?: number): void;

    /**
     * @description Sets one or more key-value pairs at the same time. If a given key already exists, MSET overwrites the old value with the new value
     *      @param kvs the key/value object to set
     */
    mset(kvs: FIBJS.GeneralObject): void;

    /**
     * @description Sets one or more key-value pairs at the same time. If a given key already exists, MSET overwrites the old value with the new value
     *      @param kvs the key/value list to set
     */
    mset(...kvs: any[]): void;

    /**
     * @description Sets one or more key-value pairs at the same time only when all the given keys do not exist
     *      @param kvs the key/value object to set
     */
    msetNX(kvs: FIBJS.GeneralObject): void;

    /**
     * @description Sets one or more key-value pairs at the same time only when all the given keys do not exist
     *      @param kvs the key/value list to set
     */
    msetNX(...kvs: any[]): void;

    /**
     * @description If key already exists and holds a string, the append command appends value to the end of the original value of key. If key does not exist, append simply sets the given key to value
     *      @param key the key to append to
     *      @param value the data to append
     *      @return the length of the string in key after appending value
     */
    append(key: Class_Buffer, value: Class_Buffer): number;

    /**
     * @description Overwrites the string value stored at key with the value parameter, starting from the offset
     *      @param key the key to modify
     *      @param offset the byte offset to modify
     *      @param value the data to overwrite
     *      @return the length of the string after the modification
     */
    setRange(key: Class_Buffer, offset: number, value: Class_Buffer): number;

    /**
     * @description Returns the substring of the string value stored at key; the range is determined by the start and end offsets (including start and end)
     *      @param key the key to query
     *      @param start the start byte offset of the query
     *      @param end the end byte offset of the query
     *      @return the extracted substring
     */
    getRange(key: Class_Buffer, start: number, end: number): Class_Buffer;

    /**
     * @description Returns the length of the string value stored at key. An error is returned when key does not hold a string value
     *      @param key the key to count
     *      @return the length of the string value, or 0 when key does not exist
     */
    strlen(key: Class_Buffer): number;

    /**
     * @description Counts the number of bits set to 1 in the given string
     *      @param key the key to count
     *      @param start the start byte to count; negative values are supported: -1 means the last byte, -2 means the second to last byte, and so on
     *      @param end the end byte to count; negative values are supported: -1 means the last byte, -2 means the second to last byte, and so on
     *      @return the number of bits set to 1
     */
    bitcount(key: Class_Buffer, start?: number, end?: number): number;

    /**
     * @description Returns the string value associated with key; if key does not exist, the special value Null is returned
     *      @param key the key to associate
     *      @return returns Null when key does not exist, otherwise returns the value of key
     */
    get(key: Class_Buffer): Class_Buffer;

    /**
     * @description Returns the values of all the given keys (one or more). If one of the given keys does not exist, the special value nil is returned for that key.
     *      @param keys the array of keys to query
     *      @return a list containing the values of all the given keys
     */
    mget(keys: any[]): any[];

    /**
     * @description Returns the values of all the given keys (one or more). If one of the given keys does not exist, the special value nil is returned for that key.
     *      @param keys the list of keys to query
     *      @return a list containing the values of all the given keys
     */
    mget(...keys: any[]): any[];

    /**
     * @description Sets the given key to value and returns the old value of key
     *      @param key the key to query and modify
     *      @param value the value to set
     *      @return returns the old value of the given key
     */
    getset(key: Class_Buffer, value: Class_Buffer): Class_Buffer;

    /**
     * @description Subtracts the decrement from the value stored at key
     *      @param key the key to modify
     *      @param num the number to subtract
     *      @return the value of key after subtracting num
     */
    decr(key: Class_Buffer, num?: number): number;

    /**
     * @description Adds the increment to the value stored at key
     *      @param key the key to modify
     *      @param num the number to add
     *      @return the value of key after adding num
     */
    incr(key: Class_Buffer, num?: number): number;

    /**
     * @description Sets or clears the bit at the given offset in the string value stored at key
     *      @param key the key to modify
     *      @param offset the bit offset to modify
     *      @param value the value to set or clear, either 0 or 1
     *      @return the bit originally stored at the offset
     */
    setBit(key: Class_Buffer, offset: number, value: number): number;

    /**
     * @description Gets the bit at the given offset in the string value stored at key
     *      @param key the key to query
     *      @param offset the bit offset to query
     *      @return the bit at the given offset of the string value
     */
    getBit(key: Class_Buffer, offset: number): number;

    /**
     * @description Checks whether the given key exists
     *      @param key the key to associate
     *      @return returns True if key exists, otherwise returns False
     */
    exists(key: Class_Buffer): boolean;

    /**
     * @description Returns the type of the value stored at key
     *      @param key the key to query
     *      @return returns the type of the value stored at key; possible values are none (key does not exist), string, list, set, zset (sorted set) and hash
     */
    type(key: Class_Buffer): string;

    /**
     * @description Finds all keys matching the given pattern
     *      @param pattern the pattern to query
     *      @return the list of keys matching the given pattern
     */
    keys(pattern: string): any[];

    /**
     * @description Deletes one or more given keys; non-existing keys are ignored
     *      @param keys the array of keys to delete
     *      @return the number of keys deleted
     */
    del(keys: any[]): number;

    /**
     * @description Deletes one or more given keys; non-existing keys are ignored
     *      @param keys the list of keys to delete
     *      @return the number of keys deleted
     */
    del(...keys: any[]): number;

    /**
     * @description Sets a time to live for the given key; when the key expires it is automatically deleted
     *      @param key the key to set
     *      @param ttl the time to live for key in milliseconds
     *      @return returns True if key exists, otherwise returns False
     */
    expire(key: Class_Buffer, ttl: number): boolean;

    /**
     * @description Returns the remaining time to live of the given key
     *      @param key the key to query
     *      @return returns the remaining time to live of key in milliseconds; returns -2 when key does not exist, and -1 when key exists but has no time to live set
     */
    ttl(key: Class_Buffer): number;

    /**
     * @description Removes the time to live of the given key, converting this key from volatile (a key with a time to live) to persistent (a key without a time to live that never expires)
     *      @param key the key to set
     *      @return returns True if key exists, otherwise returns False
     */
    persist(key: Class_Buffer): boolean;

    /**
     * @description Renames key to newkey; an error is returned when key and newkey are the same or key does not exist
     *      @param key the key to rename
     *      @param newkey the destination key to rename to
     */
    rename(key: Class_Buffer, newkey: Class_Buffer): void;

    /**
     * @description Renames key to newkey only when newkey does not exist; an error is returned when key does not exist
     *      @param key the key to rename
     *      @param newkey the destination key to rename to
     *      @return returns True when the rename succeeds, and False if newkey already exists
     */
    renameNX(key: Class_Buffer, newkey: Class_Buffer): boolean;

    /**
     * @description Subscribes to the given channel; func is called automatically when a message arrives; func takes two parameters, channel and message; the same function is called back only once for the same channel
     *      @param channel the name of the channel to subscribe to
     *      @param func the callback function
     *
     */
    sub(channel: Class_Buffer, func: (...args: any[])=>any): void;

    /**
     * @description Subscribes to the given set of channels; the corresponding callback function is called automatically when a message arrives; the same function is called back only once for the same channel
     *      @param map the channel mapping; object property names are used as channel names and property values as callback functions
     *
     */
    sub(map: FIBJS.GeneralObject): void;

    /**
     * @description Unsubscribes all callbacks of the given channel
     *      @param channel the name of the channel to unsubscribe from
     *
     */
    unsub(channel: Class_Buffer): void;

    /**
     * @description Unsubscribes the given callback function of the given channel
     *      @param channel the name of the channel to unsubscribe from
     *      @param func the callback function to unsubscribe
     *
     */
    unsub(channel: Class_Buffer, func: (...args: any[])=>any): void;

    /**
     * @description Unsubscribes all callbacks of the given set of channels
     *      @param channels the array of channels to unsubscribe from
     *
     */
    unsub(channels: any[]): void;

    /**
     * @description Unsubscribes the given callback function of the given set of channels
     *      @param map the channel mapping; object property names are used as channel names and property values as callback functions
     *
     */
    unsub(map: FIBJS.GeneralObject): void;

    /**
     * @description Subscribes to a set of channels by pattern; func is called automatically when a message arrives; func takes three parameters, channel, message and pattern; the same function is called back only once for the same pattern
     *      @param pattern the channel pattern to subscribe to
     *      @param func the callback function
     *
     */
    psub(pattern: string, func: (...args: any[])=>any): void;

    /**
     * @description Subscribes to the given set of channel patterns; the corresponding func is called automatically when a message arrives; the same function is called back only once for the same channel
     *      @param map the channel mapping; object property names are used as channel patterns and property values as callback functions
     *
     */
    psub(map: FIBJS.GeneralObject): void;

    /**
     * @description Unsubscribes all callbacks of the given pattern
     *      @param pattern the channel pattern to unsubscribe from
     *
     */
    unpsub(pattern: string): void;

    /**
     * @description Unsubscribes the given callback function of the given pattern
     *      @param pattern the channel pattern to unsubscribe from
     *      @param func the callback function to unsubscribe
     *
     */
    unpsub(pattern: string, func: (...args: any[])=>any): void;

    /**
     * @description Unsubscribes all callbacks of the given set of patterns
     *      @param patterns the array of channel patterns to publish
     *
     */
    unpsub(patterns: any[]): void;

    /**
     * @description Unsubscribes the given callback function of the given set of patterns
     *      @param map the channel mapping; object property names are used as channel patterns and property values as callback functions
     *
     */
    unpsub(map: FIBJS.GeneralObject): void;

    /**
     * @description Queries and sets the error handling function; it is called back when sub encounters an error or the network is interrupted; after the callback occurs, all subs of this object are aborted
     *
     */
    on(event: "suberror", listener: ()=>void): this;

    /**
     * @description Sends the message to the given channel
     *      @param channel the channel to publish to
     *      @param message the message to publish
     *      @return the number of clients that received this message
     *
     */
    pub(channel: Class_Buffer, message: Class_Buffer): number;

    /**
     * @description Gets the Hash object of the given key; this object is a client bound to the given key and only calling its methods operates on the database
     *      @param key the key to get
     *      @return returns the Hash object bound to the given key
     */
    getHash(key: Class_Buffer): Class_RedisHash;

    /**
     * @description Gets the List object of the given key; this object is a client bound to the given key and only calling its methods operates on the database
     *      @param key the key to get
     *      @return returns the List object bound to the given key
     */
    getList(key: Class_Buffer): Class_RedisList;

    /**
     * @description Gets the Set object of the given key; this object is a client bound to the given key and only calling its methods operates on the database
     *      @param key the key to get
     *      @return returns the Set object bound to the given key
     */
    getSet(key: Class_Buffer): Class_RedisSet;

    /**
     * @description Gets the SortedSet object of the given key; this object is a client bound to the given key and only calling its methods operates on the database
     *      @param key the key to get
     *      @return returns the SortedSet object bound to the given key
     */
    getSortedSet(key: Class_Buffer): Class_RedisSortedSet;

    /**
     * @description Serializes the given key and returns the serialized value; the value can be deserialized back into a Redis key with the restore command
     *      @param key the key to serialize
     *      @return returns the serialized value, or null if key does not exist
     */
    dump(key: Class_Buffer): Class_Buffer;

    /**
     * @description Deserializes the given serialized value and associates it with the given key
     *      @param key the key to deserialize to
     *      @param data the data to deserialize
     *      @param ttl the time to live for key in milliseconds; if ttl is 0, no time to live is set
     */
    restore(key: Class_Buffer, data: Class_Buffer, ttl?: number): void;

    /**
     * @description Closes the current database connection or transaction
     */
    close(): void;

}

