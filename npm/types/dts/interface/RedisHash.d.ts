/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description A view of one Redis hash key: field and value operations without repeating the key
 *
 *  RedisHash is the object returned by Redis#getHash. It captures the key name once and
 *  exposes the hash command family, where every member maps to one H* command: set is HSET,
 *  setNX is HSETNX, mset is HMSET, get is HGET, mget is HMGET, incr is HINCRBY, getAll is
 *  HGETALL, keys is HKEYS, len is HLEN, exists is HEXISTS and del is HDEL. Obtaining the view
 *  sends nothing to the server: the binding is resolved when its members run.
 *
 *  Concepts:
 *
 *  - **A view, not a copy**: the object stores the key only. A hash created after the view
 *    was obtained is visible through it, and a missing key is not an error - the members
 *    report the empty result (get returns null, len returns 0, keys returns an empty array)
 *    until the key exists again.
 *  - **Field and value types**: fields and values are declared Buffer|String and a Buffer is
 *    sent byte-for-byte, so non-UTF-8 data round-trips through set/get. The variadic
 *    mset/mget/del forms instead convert each argument through its JavaScript string form:
 *    a number is rejected with error 20005 and a Buffer is decoded as UTF-8 text.
 *  - **Creating and counting**: set, setNX and mset create the key when it is missing, get
 *    returns null for a missing field, and incr starts from 0 for a missing field. len counts
 *    the fields and exists tests one of them.
 *  - **Type conflicts**: a member called on a key that holds another type fails with the
 *    server error (number 20024).
 *
 *  Obtained from:
 *  - `rdb.getHash(key)` — the only factory, where rdb is the Redis object returned by
 *    db.openRedis. The key is captured at call time and may be a Buffer.
 *
 *  Example 1 — store, read and count fields:
 *  ```JavaScript
 *  // requires: redis
 *  const db = require('db');
 *  const rdb = db.openRedis('redis://127.0.0.1:6379');
 *  const hash = rdb.getHash('user:1');
 *
 *  hash.set('name', 'alice');
 *  hash.set('age', '30');
 *  console.log(hash.get('name').toString()); // alice
 *  console.log(hash.len()); // 2
 *  console.log(hash.exists('age')); // true
 *  console.log(hash.exists('mail')); // false
 *  console.log(hash.get('mail')); // null
 *
 *  rdb.del('user:1');
 *  rdb.close();
 *  ```
 *
 *  Example 2 — several fields at once, then an increment:
 *  ```JavaScript
 *  // requires: redis
 *  const db = require('db');
 *  const rdb = db.openRedis('redis://127.0.0.1:6379');
 *  const hash = rdb.getHash('scores');
 *
 *  hash.mset({ math: '90', art: '80' });
 *  hash.mset('physics', '70', 'chemistry', '60');
 *  console.log(hash.len()); // 4
 *  console.log(hash.keys().length); // 4 - HKEYS returns the field names unpaired
 *  console.log(hash.incr('math', 5)); // 95
 *
 *  const values = hash.mget('math', 'missing');
 *  console.log(values[0].toString(), values[1]); // 95 null
 *
 *  rdb.del('scores');
 *  rdb.close();
 *  ```
 *
 */
declare class Class_RedisHash extends Class_object {
    /**
     * @description Stores value in field, replacing the previous value
     *
     *      HSET. The key is created when it is missing and the previous value of the field is
     *      discarded. field and value are sent byte-for-byte as Buffers and as UTF-8 text as
     *      strings. The member reports no result.
     *
     *      @param field the field to write
     *      @param value the value to store
     *
     */
    set(field: Class_Buffer | string, value: Class_Buffer | string): void;

    /**
     * @description Stores value in field only when the field is missing
     *
     *      HSETNX. The command does nothing when the field already exists, and the member reports
     *      no result, so read the field back to know whether the write happened.
     *
     *      @param field the field to write
     *      @param value the value to store
     *
     */
    setNX(field: Class_Buffer | string, value: Class_Buffer | string): void;

    /**
     * @description Stores several field/value pairs at once, replacing the fields
     *
     *      HMSET. The property names of kvs are the fields and the property values are the
     *      values, in property order; the command is atomic. Each value is converted through its
     *      JavaScript string form, so a number is rejected with error 20005 and a Buffer is
     *      decoded as UTF-8 text. The member reports no result.
     *
     *      @param kvs the field/value pairs to write, as property names and values
     *
     */
    mset(kvs: FIBJS.GeneralObject): void;

    /**
     * @description Stores several field/value pairs at once from a flat argument list
     *
     *      HMSET. The arguments alternate field and value: mset('a', '1', 'b', '2') is the same
     *      command as mset({ a: '1', b: '2' }); an odd argument count reaches the server, which
     *      rejects the command. Values follow the string conversion of the object form. The
     *      member reports no result.
     *
     *      Example — two fields, then read them back:
     *      ```JavaScript
     *      // requires: redis
     *      const db = require('db');
     *      const rdb = db.openRedis('redis://127.0.0.1:6379');
     *      const hash = rdb.getHash('user:1');
     *
     *      hash.mset('name', 'alice', 'mail', 'alice@example.com');
     *      const values = hash.mget('name', 'mail');
     *      console.log(values[0].toString(), values[1].toString()); // alice alice@example.com
     *
     *      rdb.del('user:1');
     *      rdb.close();
     *      ```
     *
     *      @param kvs the flat field/value list to write
     *
     */
    mset(...kvs: any[]): void;

    /**
     * @description Returns the value stored in field
     *
     *      HGET. A missing field or a missing key returns null; the value is a Buffer.
     *
     *      @param field the field to read
     *      @return the value as a Buffer, or null when the field or the key does not exist
     *
     */
    get(field: Class_Buffer | string): Class_Buffer;

    /**
     * @description Returns the values of the given fields, one element per field
     *
     *      HMGET. A missing field yields null in its position, so the result has the same length
     *      as the field list. The fields go through the JavaScript string conversion of a
     *      variadic argument: a number is rejected with error 20005 and a Buffer is decoded as
     *      UTF-8 text. The values are Buffers.
     *
     *      @param fields the array of fields to read
     *      @return an array with one Buffer or null per field, in the given order
     *
     */
    mget(fields: any[]): any[];

    /**
     * @description Returns the values of the given fields, one element per field
     *
     *      HMGET. This is the flat form of mget(Array); the two are the same command and both
     *      follow the string conversion described there.
     *
     *      @param fields the fields to read, as a flat argument list
     *      @return an array with one Buffer or null per field, in the given order
     *
     */
    mget(...fields: any[]): any[];

    /**
     * @description Adds num to the integer stored in field
     *
     *      HINCRBY. The value is a signed 64-bit integer; a missing field starts at 0 and a
     *      missing key is created, so incr('views') on a new hash returns 1. A value that is not
     *      an integer string or an overflow fails with the server error.
     *
     *      Example — a counter field in an existing hash:
     *      ```JavaScript
     *      // requires: redis
     *      const db = require('db');
     *      const rdb = db.openRedis('redis://127.0.0.1:6379');
     *      const hash = rdb.getHash('metrics');
     *
     *      console.log(hash.incr('views')); // 1 - a missing field starts at 0
     *      console.log(hash.incr('views', 9)); // 10
     *
     *      rdb.del('metrics');
     *      rdb.close();
     *      ```
     *
     *      @param field the field to modify
     *      @param num the amount to add
     *      @return the value of the field after the addition
     *
     */
    incr(field: Class_Buffer | string, num?: number): number;

    /**
     * @description Returns every field and value of the hash
     *
     *      HGETALL. The result is one flat array that alternates field and value:
     *      [field1, value1, field2, value2, ...], with both as Buffers. The order is chosen by
     *      the server and is not the insertion order; a missing key returns an empty array.
     *
     *      Example — the flat pair layout:
     *      ```JavaScript
     *      // requires: redis
     *      const db = require('db');
     *      const rdb = db.openRedis('redis://127.0.0.1:6379');
     *      const hash = rdb.getHash('user:1');
     *
     *      hash.mset({ name: 'alice', age: '30' });
     *      const pairs = hash.getAll();
     *      console.log(pairs.length); // 4 - two fields, two values
     *      console.log(pairs[0].toString(), pairs[1].toString()); // name alice
     *
     *      rdb.del('user:1');
     *      rdb.close();
     *      ```
     *
     *      @return a flat array of alternating field and value Buffers
     *
     */
    getAll(): any[];

    /**
     * @description Returns every field name of the hash
     *
     *      HKEYS. Unlike getAll, the field names are returned alone: the result has one element
     *      per field, with no values interleaved. The order is chosen by the server and a missing
     *      key returns an empty array.
     *
     *      @return an array of the field names as Buffers
     *
     */
    keys(): any[];

    /**
     * @description Returns the number of fields in the hash
     *
     *      HLEN. A missing key returns 0.
     *
     *      @return the number of fields
     *
     */
    len(): number;

    /**
     * @description Checks whether the hash contains the given field
     *
     *      HEXISTS. A missing key returns false.
     *
     *      @param field the field to test
     *      @return true when the field exists
     *
     */
    exists(field: Class_Buffer | string): boolean;

    /**
     * @description Removes the given fields from the hash
     *
     *      HDEL. Missing fields are ignored and the number of removed fields is returned. The
     *      fields go through the JavaScript string conversion of a variadic argument: a number is
     *      rejected with error 20005 and a Buffer is decoded as UTF-8 text.
     *
     *      @param fields the array of fields to remove
     *      @return the number of fields that were removed
     *
     */
    del(fields: any[]): number;

    /**
     * @description Removes the given fields from the hash
     *
     *      HDEL. This is the flat form of del(Array); the two are the same command and both
     *      follow the string conversion described there.
     *
     *      @param fields the fields to remove, as a flat argument list
     *      @return the number of fields that were removed
     *
     */
    del(...fields: any[]): number;

}

