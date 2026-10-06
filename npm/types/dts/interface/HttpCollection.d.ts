/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description HttpCollection is the ordered multi-map base class behind the HTTP collections of fibjs: Headers, URLSearchParams, FormData and the request cookie collection
 *
 * A collection stores entries in insertion order and allows the same name to appear more than
 * once; every operation decides whether it adds another entry or replaces the existing ones.
 * The concrete containers are:
 *
 * - `Headers` (also the global Headers) stores header fields; names are compared
 *   case-insensitively, stored in lower case, and an empty name is rejected;
 * - `URLSearchParams` (also the global URLSearchParams) stores query parameters with
 *   case-sensitive names in insertion order, allows an empty name and adds `size`,
 *   `toString`, value-aware `has`/`delete` and the standard `sort`;
 * - `FormData` stores form fields and files with case-sensitive names, allows an empty name
 *   and keeps `File`/`Blob` values;
 * - `querystring.parse` returns a plain HttpCollection with case-insensitive names, string
 *   values and sorted iteration.
 *
 * The base class cannot be constructed with `new`; obtain an instance through one of the
 * containers above.
 *
 * Concepts:
 *
 * - **Ordered multi-map**: append adds entries, set replaces every value of a name,
 *   first/get return one value and all/getAll return every value. The insertion order is
 *   preserved until `sort` is called; the iteration helpers of a sorting container call
 *   `sort` first, while URLSearchParams and FormData keep insertion order.
 * - **Case rules**: case sensitivity is decided by the concrete container (see the list
 *   above); Headers additionally lower-cases stored names.
 * - **Values**: containers with string-only semantics convert values to strings; when a name
 *   has several values, `operator[]` and `all()` return an array while `first`/`get` return
 *   the first value.
 * - **Iteration and serialization**: forEach, keys, values, entries and @iterator are
 *   available on every container; entries() is what `for ... of` uses, and the inherited
 *   toJSON() returns a plain object whose repeated names become arrays.
 *
 * Example 1 — an ordered multi-map through Headers:
 * ```JavaScript
 * const http = require('http');
 *
 * const headers = new http.Headers();
 * headers.append('Accept', 'text/html');
 * headers.append('accept', 'application/json');
 * headers.append('X-Trace', 'abc');
 *
 * console.log(headers.get('ACCEPT')); // text/html, application/json
 * console.log(headers.first('accept')); // text/html
 * console.log(headers.all('accept')); // [ 'text/html', 'application/json' ]
 *
 * headers.set('accept', 'image/png');
 * console.log(headers.all('accept')); // [ 'image/png' ]
 * ```
 *
 * Example 2 — insertion order and replacement through URLSearchParams:
 * ```JavaScript
 * const params = new URLSearchParams('b=2&a=1&a=3');
 * params.append('c', '4');
 *
 * console.log(params.get('a')); // 1
 * console.log(params.getAll('a')); // [ '1', '3' ]
 *
 * const pairs = [];
 * params.forEach((value, name) => pairs.push(name + '=' + value));
 * console.log(pairs.join('&')); // b=2&a=1&a=3&c=4
 *
 * params.set('a', '9');
 * console.log(params.toString()); // b=2&c=4&a=9, the name moves to the end
 * ```
 *
 * Example 3 — form fields and files through FormData:
 * ```JavaScript
 * const form = new FormData();
 * form.append('name', 'lion');
 * form.append('file', new Blob(['hello'], { type: 'text/plain' }), 'hello.txt');
 *
 * console.log(form.get('name')); // lion
 * console.log(form.all('file').length); // 1
 * console.log(form.all('file')[0].type); // text/plain
 * console.log(form.all('file')[0].name); // hello.txt
 * ```
 *
 */
declare class Class_HttpCollection extends Class_object {
    /**
     * @description Removes every entry from the container
     *
     *      The container becomes empty and the iteration helpers yield nothing afterwards. The
     *      method is available on every concrete container.
     *
     */
    clear(): void;

    /**
     * @description Checks whether a name is present
     *
     *      The comparison follows the case rules of the concrete container; Headers rejects an
     *      empty name with error 20004 while URLSearchParams and FormData accept it.
     *
     *      Example — check a header through its case-insensitive name:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const headers = new http.Headers({ 'Content-Type': 'text/plain' });
     *      console.log(headers.has('content-type')); // true
     *      ```
     *
     *      @param name name to check
     *      @return true when the name exists
     *
     */
    has(name: string): boolean;

    /**
     * @description Queries the first value of a name
     *
     *      Values are returned in insertion order, so the first appended value wins. The result
     *      is null when the name does not exist. Headers rejects an empty name with error 20004,
     *      while URLSearchParams and FormData accept it.
     *
     *      Example — read the first value of a repeated header:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const headers = new http.Headers();
     *      headers.append('X-Tag', 'a');
     *      headers.append('x-tag', 'b');
     *      console.log(headers.first('X-TAG')); // a
     *      console.log(headers.first('missing')); // null
     *      ```
     *
     *      @param name name to query
     *      @return the first value, or null when the name does not exist
     *
     */
    first(name: string): any;

    /**
     * @description Queries the first value of a name, an alias of first
     *
     *      Headers overrides this member with a different semantic: its get joins every value of
     *      the name with `, ` as required by the Fetch API, while first still returns only the
     *      first raw value. The other containers return the same value as first.
     *
     *      @param name name to query
     *      @return the value of the name, or null when it does not exist
     *
     */
    get(name: string): any;

    /**
     * @description Queries all values of a name, or the whole container as an object
     *
     *      Called with a non-empty name, the method returns an array with every value of that
     *      name in insertion order and an empty array when the name is missing. Called with an
     *      empty string or without an argument, it returns a plain object with every entry, where
     *      a name that has several values becomes an array. URLSearchParams and FormData, which
     *      accept an empty name, return the whole container in that case; use getAll('') to read
     *      the values of the empty name.
     *
     *      Example — collect every value of a name:
     *      ```JavaScript
     *      const params = new URLSearchParams('tag=a&tag=b&tag=c');
     *      console.log(params.all('tag')); // [ 'a', 'b', 'c' ]
     *      console.log(params.all('missing')); // []
     *      ```
     *
     *      @param name name to query; an empty string returns the whole container
     *      @return an array of values, or an object with every entry when the name is empty
     *
     */
    all(name?: string): FIBJS.GeneralObject;

    /**
     * @description Queries all values of a name as an array
     *
     *      Always returns an array, empty when the name is missing and including for the empty
     *      name, so it is the value-oriented counterpart of all. Values keep the insertion order
     *      of the container.
     *
     *      @param name name to query
     *      @return an array with every value of the name
     *
     */
    getAll(name: string): any[];

    /**
     * @description Appends every entry of an object
     *
     *      The own enumerable properties are visited in enumeration order; a property whose value
     *      is an array appends every element in order, any other value appends a single entry,
     *      and existing entries are not modified. Values are converted according to the container
     *      (Headers and URLSearchParams store strings).
     *
     *      Example — append a group of headers:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const headers = new http.Headers();
     *      headers.append({ 'Accept-Encoding': 'gzip', 'Set-Cookie': ['a=1', 'b=2'] });
     *      console.log(headers.all('set-cookie')); // [ 'a=1', 'b=2' ]
     *      ```
     *
     *      @param map object whose properties are appended
     *
     */
    append(map: FIBJS.GeneralObject): void;

    /**
     * @description Appends one value, or every element of an array, for a name
     *
     *      An array appends every element in order, any other value appends a single entry, and
     *      existing entries are not modified. An empty name is rejected by Headers with error
     *      20004 and accepted by URLSearchParams and FormData.
     *
     *      Example — append two values of one name:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const headers = new http.Headers();
     *      headers.append('X-Tag', ['a', 'b']);
     *      console.log(headers.all('x-tag')); // [ 'a', 'b' ]
     *      ```
     *
     *      @param name name to append
     *      @param value value, or array of values, to append
     *
     */
    append(name: string, value: any[] | any): void;

    /**
     * @description Appends an array of [name, value] entries
     *
     *      Every element of the argument must be an array of exactly two elements; a different
     *      shape fails with a bad variable type error (20003). Elements are appended in order and
     *      existing entries are not modified.
     *
     *      Example — append pairs through URLSearchParams:
     *      ```JavaScript
     *      const params = new URLSearchParams();
     *      params.append([['a', '1'], ['b', '2']]);
     *      console.log(params.toString()); // a=1&b=2
     *      ```
     *
     *      @param entries array of [name, value] pairs to append
     *
     */
    append(entries: any[]): void;

    /**
     * @description Sets every entry of an object, replacing the existing values of each name
     *
     *      The own enumerable properties are visited in enumeration order; a property whose value
     *      is an array sets every element in order, any other value sets a single entry. Every
     *      existing value of a name is removed before its new values are appended.
     *
     *      Example — replace a group of headers:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const headers = new http.Headers({ Accept: 'text/html', 'X-Tag': 'old' });
     *      headers.set({ Accept: 'application/json', 'X-Tag': ['a', 'b'] });
     *      console.log(headers.all('accept')); // [ 'application/json' ]
     *      console.log(headers.all('x-tag')); // [ 'a', 'b' ]
     *      ```
     *
     *      @param map object whose properties are set
     *
     */
    set(map: FIBJS.GeneralObject): void;

    /**
     * @description Sets one value, or every element of an array, for a name
     *
     *      Every existing value of the name is removed first, then an array appends every element
     *      in order or another value appends a single entry; a name that already existed therefore
     *      moves to the end of the insertion order. An empty name is rejected by Headers with
     *      error 20004 and accepted by URLSearchParams and FormData.
     *
     *      @param name name to set
     *      @param value value, or array of values, to set
     *
     */
    set(name: string, value: any[] | any): void;

    /**
     * @description Removes every value of a name
     *
     *      The name is looked up with the case rules of the container and removing a name that
     *      does not exist is not an error. delete is an alias: the two differ only in that
     *      `delete container[name]` can be used as an operator (its return value is not reliable,
     *      see the operator member).
     *
     *      @param name name to remove
     *
     */
    remove(name: string): void;

    /**
     * @description Removes every value of a name, an alias of remove
     *
     *      See remove. The `delete container[name]` operator removes the same entries, but its
     *      result is always true in the current implementation, so test the removal with has
     *      instead of relying on the return value.
     *
     *      @param name name to remove
     *
     */
    delete(name: string): void;

    /**
     * @description Sorts the entries by name in place
     *
     *      The sort is stable and compares names byte by byte, so upper-case letters sort before
     *      lower-case ones. Sorting changes the order seen by every later operation, including
     *      all() and toJSON(). The iteration helpers of Headers and of the collection returned by
     *      querystring.parse call sort automatically; URLSearchParams and FormData never sort
     *      implicitly.
     *
     */
    sort(): void;

    /**
     * @description Visits every entry in order
     *
     *      The callback receives (value, name, collection); returning from the callback does not
     *      stop the iteration. Containers that sort on iteration (Headers and the querystring
     *      collection) sort before the first callback, so the names are visited in sorted order.
     *
     *      Example — list every header:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const headers = new http.Headers({ 'B-Header': '1', 'A-Header': '2' });
     *      headers.forEach((value, name) => console.log(name, value));
     *      // a-header 2, then b-header 1 (names are lower-cased and sorted)
     *      ```
     *
     *      @param callback function called with (value, name, collection)
     *
     */
    forEach(callback: (value: any, key: string, obj: FIBJS.GeneralObject)=>void): void;

    /**
     * @description Visits every entry in order with an explicit this value
     *
     *      Identical to forEach except that the callback runs with thisArg as `this` and receives
     *      thisArg as its third argument instead of the collection.
     *
     *      @param callback function called with (value, name, thisArg)
     *      @param thisArg value used as `this` inside the callback
     *
     */
    forEach(callback: (value: any, key: string, obj: FIBJS.GeneralObject)=>void, thisArg: any): void;

    /**
     * @description Returns an iterator over the names
     *
     *      Repeated names appear once per entry; containers that sort on iteration sort first.
     *      The iterator implements the standard iterator protocol, so it can be used in a
     *      `for ... of` loop or queried with next().
     *
     *      Example — iterate names:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const headers = new http.Headers({ B: '1', A: '2' });
     *      for (const name of headers.keys())
     *          console.log(name); // a, then b (Headers sorts on iteration)
     *      ```
     *
     *      @return an iterator with every name
     *
     */
    keys(): Iterator<any>;

    /**
     * @description Returns an iterator over the values
     *
     *      Values are visited in the same order as the names of keys(), one value per entry.
     *
     *      @return an iterator with every value
     *
     */
    values(): Iterator<any>;

    /**
     * @description Returns an iterator over [name, value] pairs
     *
     *      This is also the iterator used by `for ... of` and by spread on the container; a name
     *      with several values appears once per value.
     *
     *      Example — turn query parameters into pairs:
     *      ```JavaScript
     *      const params = new URLSearchParams('a=1&a=2');
     *      console.log(JSON.stringify([...params])); // [["a","1"],["a","2"]]
     *      ```
     *
     *      @return an iterator with every [name, value] pair
     *
     */
    entries(): Iterator<any>;

    /**
     * @description Accesses values by name with the subscript syntax
     *
     *      Reading a name returns its first value, or an array with every value when the name
     *      repeats; reading a missing name returns undefined. Assigning to a name behaves like
     *      set(name, value), and `delete container[name]` behaves like remove; the delete
     *      operator always returns true in the current implementation, even when no entry was
     *      removed, so use has to test the result.
     *
     *      Example — read a repeated name:
     *      ```JavaScript
     *      const http = require('http');
     *
     *      const headers = new http.Headers();
     *      headers.append('X-Tag', 'a');
     *      headers.append('X-Tag', 'b');
     *      console.log(headers['X-Tag']); // [ 'a', 'b' ]
     *      console.log(headers['missing']); // undefined
     *      ```
     *
     */
    [index: string]: any;

    "[Symbol.iterator]"(): Iterator<any>;

}

