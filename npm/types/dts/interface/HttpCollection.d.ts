/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description HttpCollection is a general-purpose container for handling headers, query, form and cookie data in http messages
 *
 * We use headers as an example to explain how to use HttpCollection.
 *
 * HttpCollection supports three forms of adding data:
 *
 * 1. Add a key-value entry; adding data does not modify the data of an existing key. `add`
 *
 * ```JavaScript
 * headers.add({
 *     'Content-Type': 'text/plain',
 *     'User-Agent': 'fibjs'
 * });
 * ```
 *
 * 2. Add a group of data for a key; adding data does not modify the data of an existing key. `add`
 *
 * ```JavaScript
 * headers.add('Set-Cookie', [
 *     'a=10',
 *     'b=20'
 * ]);
 * ```
 *
 * 3. Add a key-value entry; adding data does not modify the data of an existing key. `add`
 *
 * ```JavaScript
 * headers.add('Accept-Encoding', 'gzip');
 * ```
 *
 * Setting data in HttpCollection works the same way as adding, using the `set` method.
 *
 * We can use `has` to check whether data of the specified key exists in the container
 *
 * ```JavaScript
 * const contentTypeExists = headers.has('Content-Type');
 * ```
 *
 * Use `first` to get the first value corresponding to a key in the container:
 *
 * ```JavaScript
 * const contentType = headers.first('Content-Type');
 * ```
 *
 * Use `all` to query all values of the specified key, returning an array. If an empty string parameter is passed, all values are returned
 *
 * ```JavaScript
 * const cookieArray = headers.all('Set-Cookie');
 * const alls = headers.all();
 * ```
 *
 * Use the `delete` method to delete all data of the specified key; returning `true` indicates
 */
declare class Class_HttpCollection extends Class_object {
    /**
     * @description clears the container data
     */
    clear(): void;

    /**
     * @description checks whether data of the specified key exists in the container
     *      @param name specifies the key to check
     *      @return returns whether the key exists
     *
     */
    has(name: string): boolean;

    /**
     * @description queries the first value of the specified key
     *      @param name specifies the key to query
     *      @return returns the value corresponding to the key, or undefined if it does not exist
     *
     */
    first(name: string): any;

    /**
     * @description queries the first value of the specified key, same as first
     *      @param name specifies the key to query
     *      @return returns the value corresponding to the key, or undefined if it does not exist
     *
     */
    get(name: string): any;

    /**
     * @description queries all values of the specified key
     *      @param name specifies the key to query; passing an empty string returns the result of all keys
     *      @return returns an array of all values corresponding to the key, or null if the data does not exist
     *
     */
    all(name?: string): FIBJS.GeneralObject;

    /**
     * @description queries all values of the specified key
     *      @param name specifies the key to query
     *      @return returns an array of all values corresponding to the key, or null if the data does not exist
     *
     */
    getAll(name: string): any[];

    /**
     * @description appends a key-value entry; appending data does not modify the data of an existing key
     *      @param map specifies the key-value data dictionary to append
     *
     */
    append(map: FIBJS.GeneralObject): void;

    /**
     * @description appends data for a key; appending data does not modify the data of an existing key
     *
     *      An array appends every element in order, any other value appends a single entry
     *      @param name specifies the key to append
     *      @param value specifies the group of data to append, or the single value to append
     *
     */
    append(name: string, value: any[] | any): void;

    /**
     * @description appends a group of data; appending data does not modify the data of an existing key
     *      @param entries specifies the group of data to append, in the format [[<key>, <value>]]
     *
     */
    append(entries: any[]): void;

    /**
     * @description sets a key-value entry; setting data modifies the first value of the key and clears the remaining data with the same key
     *      @param map specifies the key-value data dictionary to set
     *
     */
    set(map: FIBJS.GeneralObject): void;

    /**
     * @description sets data for a key; setting data modifies the value of the key and clears the remaining data with the same key
     *
     *      An array sets every element in order, any other value sets a single entry
     *      @param name specifies the key to set
     *      @param value specifies the group of data to set, or the single value to set
     *
     */
    set(name: string, value: any[] | any): void;

    /**
     * @description deletes all values of the specified key
     *      @param name specifies the key to delete
     *
     */
    remove(name: string): void;

    /**
     * @description deletes all values of the specified key
     *      @param name specifies the key to delete
     *
     */
    delete(name: string): void;

    /**
     * @description sorts the contents of the container by key
     *
     */
    sort(): void;

    /**
     * @description iterates over the contents of the container
     *      @param callback specifies the function called during iteration, whose parameters are (value, key, object)
     *
     */
    forEach(callback: (value: any, key: string, obj: FIBJS.GeneralObject)=>void): void;

    /**
     * @description iterates over the contents of the container
     *      @param callback specifies the function called during iteration, whose parameters are (value, key, object)
     *      @param thisArg specifies the this object of the callback function
     *
     */
    forEach(callback: (value: any, key: string, obj: FIBJS.GeneralObject)=>void, thisArg: any): void;

    /**
     * @description queries the keys in the container
     *      @return returns an iterator containing all keys
     *
     */
    keys(): Iterator<any>;

    /**
     * @description queries the values in the container
     *      @return returns an iterator containing all values
     *
     */
    values(): Iterator<any>;

    /**
     * @description queries the keys and values in the container
     *      @return returns an iterator containing all keys and values
     *
     */
    entries(): Iterator<any>;

    /**
     * @description allows direct access to values by using keys as subscripts
     */
    [index: string]: any;

    "[Symbol.iterator]"(): Iterator<any>;

}

