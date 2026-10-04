/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The uuid module provides creation and manipulation of unique ids. It can be used to generate UUIDs (Universally Unique Identifiers) meeting various requirements
 *
 * The `uuid` module provides several static functions that can be used to configure and generate different kinds of UUIDs.
 * The following is an example of creating a uuid with md5:
 * ```JavaScript
 * const uuid = require('uuid');
 * const ns = uuid.DNS;
 * const name = 'example.com';
 * console.log(uuid.md5(ns, name));
 * ```
 * In the example above, the uuid module is first imported, then the namespace and name are specified, and a UUID meeting the requirements is generated with the md5 algorithm and printed to the console.
 * Similarly, we can also generate a uuid with the snowflake algorithm. The following is an example of creating a uuid with the snowflake algorithm:
 * ```JavaScript
 * const uuid = require('uuid');
 * const s = uuid.snowflake();
 * console.log(s);
 * ```
 * In the example above, the snowflake() method returns a Buffer object, which can be converted to a string and printed to the console to obtain the generated uuid.
 *
 */
declare module 'uuid' {
    /**
     * @description Specifies the name as a domain name when creating a uuid with md5 or sha1
     */
    export const DNS: 0;

    /**
     * @description Specifies the name as a url address when creating a uuid with md5 or sha1
     */
    export const URL: 1;

    /**
     * @description Specifies the name as an ISO OID when creating a uuid with md5 or sha1
     */
    export const OID: 2;

    /**
     * @description Specifies the name as an X.500 DN when creating a uuid with md5 or sha1
     */
    export const X509: 3;

    /**
     * @description An empty uuid
     */
    export const NIL: "00000000-0000-0000-0000-000000000000";

    /**
     * @description The maximum UUID string
     */
    export const MAX: "ffffffff-ffff-ffff-ffff-ffffffffffff";

    /**
     * @description The DNS namespace UUID for v3 and v5
     */
    export const DNS_NAMESPACE: "6ba7b810-9dad-11d1-80b4-00c04fd430c8";

    /**
     * @description The URL namespace UUID for v3 and v5
     */
    export const URL_NAMESPACE: "6ba7b811-9dad-11d1-80b4-00c04fd430c8";

    /**
     * @description Parses a uuid string
     *      @param uuid the uuid string to parse
     *      @return returns the parsed binary id
     *
     */
    function parse(uuid: string): Class_Buffer;

    /**
     * @description Converts a binary array to a uuid string
     *      @param arr the array or Buffer containing the uuid binary data; its length must be at least 16 bytes
     *      @param offset optional; the starting offset of the uuid data in the array, default 0
     *      @return returns the converted uuid string
     *
     */
    function stringify(arr: Class_Buffer, offset?: number): string;

    /**
     * @description Creates a uuid using a timestamp
     *      @param options optional parameter object; supports the following properties: node (Buffer, node ID), clockseq (Integer, clock sequence), msecs (Integer, millisecond timestamp), nsecs (Integer, nanosecond timestamp)
     *      @return returns a generated uuid string
     *
     */
    function v1(options?: FIBJS.GeneralObject): string;

    /**
     * @description Creates a uuid with an MD5 namespace (binary namespace format)
     *      @param name the name to use
     *      @param ns the binary representation of the namespace UUID; its length must be 16 bytes
     *      @return returns a generated uuid string
     *
     */
    function v3(name: string, ns: Class_Buffer): string;

    /**
     * @description Creates a uuid with an MD5 namespace (string format)
     *      @param name the name to use
     *      @param ns the namespace UUID string, or use a predefined namespace
     *      @return returns a generated uuid string
     *
     */
    function v3(name: string, ns: string): string;

    function v3(name: string, ns: Class_Buffer | string): string;

    /**
     * @description Creates a uuid using random numbers
     *      @param options optional parameter object; supports the following properties: random (Buffer, random numbers), rng (Function, random number generator)
     *      @return returns a generated uuid string
     *
     */
    function v4(options?: FIBJS.GeneralObject): string;

    /**
     * @description Creates a uuid with a SHA1 namespace (binary namespace format)
     *      @param name the name to use
     *      @param ns the binary representation of the namespace UUID; its length must be 16 bytes
     *      @return returns a generated uuid string
     *
     */
    function v5(name: string, ns: Class_Buffer): string;

    /**
     * @description Creates a uuid with a SHA1 namespace (string format)
     *      @param name the name to use
     *      @param ns the namespace UUID string, or use a predefined namespace
     *      @return returns a generated uuid string
     *
     */
    function v5(name: string, ns: string): string;

    function v5(name: string, ns: Class_Buffer | string): string;

    /**
     * @description Gets the version number of the uuid
     *      @param uuid the uuid string to check
     *      @return returns the uuid version number (0-7), or undefined if the format is invalid
     *
     */
    function version(uuid: string): number;

    /**
     * @description Creates a uuid v6 using a reordered timestamp
     *      @param options optional parameter object; supports the following properties: node (Buffer, node ID), clockseq (Integer, clock sequence), msecs (Integer, millisecond timestamp), nsecs (Integer, nanosecond timestamp)
     *      @return returns a generated uuid string
     *
     */
    function v6(options?: FIBJS.GeneralObject): string;

    /**
     * @description Creates a uuid v7 using a Unix Epoch timestamp
     *      @param options optional parameter object; supports the following properties: msecs (Integer, millisecond timestamp)
     *      @return returns a generated uuid string
     *
     */
    function v7(options?: FIBJS.GeneralObject): string;

    /**
     * @description Converts a uuid v1 to v6
     *      @param uuid the uuid string in v1 format
     *      @return returns the converted v6 uuid string
     *
     */
    function v1ToV6(uuid: string): string;

    /**
     * @description Converts a uuid v6 to v1
     *      @param uuid the uuid string in v6 format
     *      @return returns the converted v1 uuid string
     *
     */
    function v6ToV1(uuid: string): string;

    /**
     * @description Validates whether a uuid string conforms to the specification
     *      @param uuid the uuid string to validate
     *      @return returns true if it conforms to the specification, false otherwise
     *
     */
    function validate(uuid: string): boolean;

    /**
     * @description Creates a uuid using the time and host name
     *      @return returns a generated binary id
     *
     */
    function node(): Class_Buffer;

    /**
     * @description Creates a uuid with md5 for a specific name
     *      @param ns the namespace to use; can be uuid.DNS, uuid.URL, uuid.OID, uuid.X509
     *      @param name the name to use
     *      @return returns a generated binary id
     *
     */
    function md5(ns: number, name: string): Class_Buffer;

    /**
     * @description Creates a uuid using random numbers
     *      @return returns a generated binary id
     *
     */
    function random(): Class_Buffer;

    /**
     * @description Creates a uuid with sha1 for a specific name
     *      @param ns the namespace to use; can be uuid.DNS, uuid.URL, uuid.OID, uuid.X509
     *      @param name the name to use
     *      @return returns a generated binary id
     *
     */
    function sha1(ns: number, name: string): Class_Buffer;

    /**
     * @description Creates a uuid using the Snowflake algorithm
     *      @return returns a generated binary id
     *
     */
    function snowflake(): Class_Buffer;

    /**
     * @description Queries and modifies the host id of the Snowflake algorithm
     */
    var hostID: number;

}

