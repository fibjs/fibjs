/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description A view of one Redis list key: element operations without repeating the key in every call
 *
 *  RedisList is the object returned by Redis#getList. It captures the key name once and
 *  exposes the list command family, where every member maps to one command: push is LPUSH,
 *  pop is LPOP, rpush is RPUSH, rpop is RPOP, set is LSET, get is LINDEX, insertBefore and
 *  insertAfter are LINSERT, remove is LREM, trim is LTRIM, len is LLEN and range is LRANGE.
 *  Obtaining the view sends nothing to the server: the binding is resolved when its members
 *  run.
 *
 *  Concepts:
 *
 *  - **Two ends**: push inserts at the head and rpush at the tail; pop removes from the head
 *    and rpop from the tail. A push with several values inserts them one by one at the head,
 *    so the last argument becomes the first element; an rpush keeps the argument order.
 *  - **Indexes**: get and set count from the head of the list, and a negative index counts
 *    from the tail (-1 is the last element). get returns null for an index outside the list,
 *    while set fails with the server error. range(start, stop) and trim(start, stop) take an
 *    inclusive range at both ends and also accept negative indexes.
 *  - **Removing elements**: remove(count, value) follows the server rule: a positive count
 *    removes from head to tail, a negative count from tail to head and 0 removes every match.
 *    insertBefore/insertAfter return the new length, or -1 when the pivot value is not in the
 *    list.
 *  - **View semantics**: the key is not created by obtaining the view, and a missing key is
 *    not an error - len returns 0, range returns an empty array, pop returns null. Writing
 *    members create the list when it is missing, and a key holding another type fails with
 *    the server error.
 *
 *  Obtained from:
 *  - `rdb.getList(key)` — the only factory, where rdb is the Redis object returned by
 *    db.openRedis. The key is captured at call time and may be a Buffer.
 *
 *  Example 1 — the head and tail ends of the list:
 *  ```JavaScript
 *  // requires: redis
 *  const db = require('db');
 *  const rdb = db.openRedis('redis://127.0.0.1:6379');
 *  const list = rdb.getList('queue');
 *
 *  list.push('a', 'b', 'c'); // LPUSH: the last argument becomes the head
 *  console.log(list.pop().toString()); // c
 *  list.rpush('d'); // RPUSH appends at the tail
 *  console.log(list.rpop().toString()); // d
 *  console.log(list.len()); // 2
 *
 *  const rest = list.range(0, -1);
 *  console.log(rest[0].toString(), rest[1].toString()); // b a
 *
 *  rdb.del('queue');
 *  rdb.close();
 *  ```
 *
 *  Example 2 — random access and editing:
 *  ```JavaScript
 *  // requires: redis
 *  const db = require('db');
 *  const rdb = db.openRedis('redis://127.0.0.1:6379');
 *  const list = rdb.getList('log');
 *
 *  list.rpush('start', 'middle', 'end');
 *  list.set(1, 'center');
 *  console.log(list.get(1).toString()); // center
 *  console.log(list.insertBefore('end', 'before-end')); // 4 - the new length
 *  console.log(list.remove(1, 'start')); // 1 - one element removed
 *  list.trim(0, 1);
 *
 *  const rest = list.range(0, -1);
 *  console.log(rest[0].toString(), rest[1].toString()); // center before-end
 *
 *  rdb.del('log');
 *  rdb.close();
 *  ```
 *
 */
declare class Class_RedisList extends Class_object {
    /**
     * @description Inserts values at the head of the list
     *
     *      LPUSH. The values are inserted one by one at the head, so the last element of the
     *      array becomes the first element of the list. A missing key is created; the new length
     *      is returned. Each value is converted through its JavaScript string form, so a number
     *      is rejected with error 20005 and a Buffer is decoded as UTF-8 text - insertBefore and
     *      set keep Buffers byte-for-byte.
     *
     *      Example — insert several elements at once:
     *      ```JavaScript
     *      // requires: redis
     *      const db = require('db');
     *      const rdb = db.openRedis('redis://127.0.0.1:6379');
     *      const list = rdb.getList('queue');
     *
     *      console.log(list.push('a', 'b', 'c')); // 3
     *      console.log(list.push(['d', 'e'])); // 5
     *      console.log(list.pop().toString()); // e - the last value pushed is at the head
     *
     *      rdb.del('queue');
     *      rdb.close();
     *      ```
     *
     *      @param values the array of values to insert
     *      @return the length of the list after the insert
     *
     */
    push(values: any[]): number;

    /**
     * @description Inserts values at the head of the list
     *
     *      LPUSH. This is the flat form of push(Array); the two are the same command and both
     *      follow the string conversion described there.
     *
     *      @param values the values to insert, as a flat argument list
     *      @return the length of the list after the insert
     *
     */
    push(...values: any[]): number;

    /**
     * @description Removes and returns the head element of the list
     *
     *      LPOP. A missing key returns null; the element is a Buffer.
     *
     *      @return the head element as a Buffer, or null when the list is empty
     *
     */
    pop(): Class_Buffer;

    /**
     * @description Appends values at the tail of the list
     *
     *      RPUSH. The values keep their argument order, so the last element of the array becomes
     *      the last element of the list. A missing key is created; the new length is returned,
     *      and the string conversion of push applies.
     *
     *      @param values the array of values to append
     *      @return the length of the list after the append
     *
     */
    rpush(values: any[]): number;

    /**
     * @description Appends values at the tail of the list
     *
     *      RPUSH. This is the flat form of rpush(Array); the two are the same command and both
     *      follow the string conversion described there.
     *
     *      @param values the values to append, as a flat argument list
     *      @return the length of the list after the append
     *
     */
    rpush(...values: any[]): number;

    /**
     * @description Removes and returns the tail element of the list
     *
     *      RPOP. A missing key returns null; the element is a Buffer.
     *
     *      @return the tail element as a Buffer, or null when the list is empty
     *
     */
    rpop(): Class_Buffer;

    /**
     * @description Replaces the element at the given index
     *
     *      LSET. A negative index counts from the tail (-1 is the last element). An index outside
     *      the list or a missing key fails with the server error; value is sent byte-for-byte as
     *      a Buffer. The member reports no result.
     *
     *      @param index the index to write
     *      @param value the value to store
     *
     */
    set(index: number, value: Class_Buffer | string): void;

    /**
     * @description Returns the element at the given index
     *
     *      LINDEX. A negative index counts from the tail (-1 is the last element), and an index
     *      outside the list or a missing key returns null. The element is a Buffer.
     *
     *      @param index the index to read
     *      @return the element as a Buffer, or null when the index is outside the list
     *
     */
    get(index: number): Class_Buffer;

    /**
     * @description Inserts value just before the first element equal to pivot
     *
     *      LINSERT ... BEFORE. When several elements equal pivot, the first one from the head is
     *      used. pivot and value are sent byte-for-byte as Buffers. The new length is returned, or
     *      -1 when pivot is not in the list.
     *
     *      @param pivot the existing value to insert before
     *      @param value the value to insert
     *      @return the length of the list after the insert, -1 when pivot was not found
     *
     */
    insertBefore(pivot: Class_Buffer | string, value: Class_Buffer | string): number;

    /**
     * @description Inserts value just after the first element equal to pivot
     *
     *      LINSERT ... AFTER. When several elements equal pivot, the first one from the head is
     *      used. pivot and value are sent byte-for-byte as Buffers. The new length is returned, or
     *      -1 when pivot is not in the list.
     *
     *      @param pivot the existing value to insert after
     *      @param value the value to insert
     *      @return the length of the list after the insert, -1 when pivot was not found
     *
     */
    insertAfter(pivot: Class_Buffer | string, value: Class_Buffer | string): number;

    /**
     * @description Removes elements equal to value
     *
     *      LREM. count selects which matches are removed: a positive count removes up to that
     *      many elements from the head towards the tail, a negative count from the tail towards
     *      the head and 0 removes every match. The number of removed elements is returned; a
     *      missing key returns 0. value is sent byte-for-byte as a Buffer.
     *
     *      @param count the number of matches to remove, from the head when positive and from the
     *      tail when negative; 0 removes all of them
     *      @param value the value to remove
     *      @return the number of elements that were removed
     *
     */
    remove(count: number, value: Class_Buffer | string): number;

    /**
     * @description Keeps only the elements inside the given range
     *
     *      LTRIM. Both offsets are inclusive; a negative offset counts from the tail and offsets
     *      outside the list are clamped. A range that selects nothing removes every element, and
     *      the member reports no result.
     *
     *      @param start the first index to keep; -1 is the last element
     *      @param stop the last index to keep; -1 is the last element
     *
     */
    trim(start: number, stop: number): void;

    /**
     * @description Returns the number of elements in the list
     *
     *      LLEN. A missing key returns 0.
     *
     *      @return the number of elements
     *
     */
    len(): number;

    /**
     * @description Returns the elements inside the given range
     *
     *      LRANGE. Both offsets are inclusive and a negative offset counts from the tail (-1 is
     *      the last element), so range(0, -1) returns the whole list. An empty list, a missing key
     *      and a range that selects nothing all return an empty array; the elements are Buffers.
     *
     *      Example — random access with negative indexes:
     *      ```JavaScript
     *      // requires: redis
     *      const db = require('db');
     *      const rdb = db.openRedis('redis://127.0.0.1:6379');
     *      const list = rdb.getList('queue');
     *
     *      list.rpush('a', 'b', 'c');
     *      const all = list.range(0, -1);
     *      console.log(all.length); // 3
     *      console.log(all[0].toString(), all[2].toString()); // a c
     *      console.log(list.range(-2, -1).length); // 2 - the last two elements
     *
     *      rdb.del('queue');
     *      rdb.close();
     *      ```
     *
     *      @param start the first index of the range; -1 is the last element
     *      @param stop the last index of the range; -1 is the last element
     *      @return an array of Buffer elements
     *
     */
    range(start: number, stop: number): any[];

}

