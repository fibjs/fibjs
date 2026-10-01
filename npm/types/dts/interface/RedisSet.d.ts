/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Redis database client Set object; this object is a client bound to the given key and only calling its methods operates on the database
 *
 *  Used to operate on a Redis Set object. To create one:
 *  ```JavaScript
 *  var db = require("db");
 *  var rdb = new db.openRedis("redis-server");
 *  var set = rdb.getSet("test");
 *  ```
 *
 */
declare class Class_RedisSet extends Class_object {
    /**
     * @description Adds one or more member elements to the set key; member elements already in the set are ignored
     *      @param members the array of elements to add
     *      @return the number of new elements added to the set, excluding ignored elements
     */
    add(members: any[]): number;

    /**
     * @description Sets multiple field-value pairs in the hash table at the same time; this command overwrites existing fields in the hash table
     *      @param members the list of elements to add
     *      @return the number of new elements added to the set, excluding ignored elements
     */
    add(...members: any[]): number;

    /**
     * @description Removes one or more member elements from the set
     *      @param members the array of elements to remove
     *      @return the number of elements successfully removed, excluding ignored elements
     */
    remove(members: any[]): number;

    /**
     * @description Removes one or more member elements from the set
     *      @param members the list of elements to remove
     *      @return the number of elements successfully removed, excluding ignored elements
     */
    remove(...members: any[]): number;

    /**
     * @description Returns the number of elements in the set
     *      @return returns the length of the set
     */
    len(): number;

    /**
     * @description Checks whether member is a member of the set
     *      @param member the member to check
     *      @return returns true if member is a member of the set
     */
    exists(member: Class_Buffer): boolean;

    /**
     * @description Returns all members of the set
     *      @return the list of all members of the set
     */
    members(): any[];

    /**
     * @description Removes and returns a random element from the set
     *      @return the removed random element. Returns null when the set is empty
     */
    pop(): Class_Buffer;

    /**
     * @description Gets one random element from the set
     *      @return returns an element; returns null if the set is empty
     */
    randMember(): any;

    /**
     * @description Gets several random elements from the set
     *      @param count the number of elements to return. A positive count returns an array containing count elements; a negative count returns an array whose elements may repeat multiple times and whose length is the absolute value of count
     *      @return returns a list; returns an empty list if the set is empty
     */
    randMember(count: number): any;

}

