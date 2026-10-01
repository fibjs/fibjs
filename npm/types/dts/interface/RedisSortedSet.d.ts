/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Redis database client SortedSet object; this object is a client bound to the given key and only calling its methods operates on the database
 *
 *  Used to operate on a Redis SortedSet object. To create one:
 *  ```JavaScript
 *  var db = require("db");
 *  var rdb = new db.openRedis("redis-server");
 *  var set = rdb.getSortedSet("test");
 *  ```
 *
 */
declare class Class_RedisSortedSet extends Class_object {
    /**
     * @description Adds one or more member elements and their scores to the sorted set
     *      @param sms the member/score object to add
     *      @return the number of new members successfully added, excluding updated existing members
     */
    add(sms: FIBJS.GeneralObject): number;

    /**
     * @description Adds one or more member elements and their scores to the sorted set
     *      @param sms the member/score list to add
     *      @return the number of new members successfully added, excluding updated existing members
     */
    add(...sms: any[]): number;

    /**
     * @description Returns the score of the member in the sorted set
     *      @param member the member to query
     *      @return the score of member as a string
     */
    score(member: Class_Buffer): Class_Buffer;

    /**
     * @description Adds the increment num to the score of the member in the sorted set
     *      @param member the member to modify
     *      @param num the number to add
     *      @return the new score of member as a string
     */
    incr(member: Class_Buffer, num?: number): Class_Buffer;

    /**
     * @description Removes one or more member elements from the sorted set
     *      @param members the array of elements to remove
     *      @return the number of elements successfully removed, excluding ignored elements
     */
    remove(members: any[]): number;

    /**
     * @description Removes one or more member elements from the sorted set
     *      @param members the list of elements to remove
     *      @return the number of elements successfully removed, excluding ignored elements
     */
    remove(...members: any[]): number;

    /**
     * @description Returns the number of elements in the sorted set
     *      @return returns the length of the sorted set
     */
    len(): number;

    /**
     * @description Returns the number of members in the sorted set whose score is between min and max (including scores equal to min or max by default)
     *      @param min the minimum score to count
     *      @param max the maximum score to count
     *      @return the number of members whose score is between min and max
     */
    count(min: number, max: number): number;

    /**
     * @description Returns the members in the given range of the sorted set, ordered by increasing score (from smallest to largest)
     *      @param start the start index to query; 0 means the first element and -1 means the last element
     *      @param stop the stop index to query; 0 means the first element and -1 means the last element
     *      @param withScores whether to include scores in the result
     *      @return the list of sorted set members in the given range, with scores (optional)
     */
    range(start: number, stop: number, withScores?: boolean): any[];

    /**
     * @description Returns the members in the given range of the sorted set, ordered by decreasing score (from largest to smallest)
     *      @param start the start index to query; 0 means the first element and -1 means the last element
     *      @param stop the stop index to query; 0 means the first element and -1 means the last element
     *      @param withScores whether to include scores in the result
     *      @return the list of sorted set members in the given range, with scores (optional)
     */
    rangeRev(start: number, stop: number, withScores?: boolean): any[];

    /**
     * @description Returns the rank of member in the sorted set. Members are ordered by increasing score (from smallest to largest)
     *      @param member the member to query
     *      @return the rank of member if member is a member of the sorted set key; returns nil if member is not a member of the sorted set key
     */
    rank(member: Class_Buffer): number;

    /**
     * @description Returns the rank of member in the sorted set. Members are ordered by decreasing score (from largest to smallest)
     *      @param member the member to query
     *      @return the rank of member if member is a member of the sorted set key; returns nil if member is not a member of the sorted set key
     */
    rankRev(member: Class_Buffer): number;

}

