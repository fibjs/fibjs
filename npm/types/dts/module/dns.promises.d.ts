/// <reference path="../_import/_fibjs.d.ts" />
/**
 * The promise variant of the dns module: async members return a Promise as their primary form.
 */
declare module 'dns/promises' {
    /**
     * @description queries the address of the given hostname
     *      @param name specifies the hostname
     *      @return returns the array of queried ip strings
     *
     */
    function resolve(name: string): Promise<string[]>;

    /**
     * @description queries the address of the given hostname
     *      @param name specifies the hostname
     *      @return returns the array of queried ip strings
     *
     */
    function resolveSync(name: string): string[];

    /**
     * @description queries the address of the given hostname
     *      @param name specifies the hostname
     *      @return returns the array of queried ip strings
     *
     */
    function resolveAsync(name: string): Promise<string[]>;

    /**
     * @description queries the address of the given hostname
     *
     *      The supported options of options are as follows:
     *      ```JavaScript
     *      {
     *          "family": 0, // specify the address family: 0 for any, 4 for IPv4, 6 for IPv6, "IPv4"/"IPv6" is also allowed. Default: 0
     *          "all": false // when true, returns an object array of all addresses, otherwise returns a string of the first address. Default: false
     *      }
     *      ```
     *
     *      When all is true, the returned array elements contain the `address` (ip string) and `family` (address family number) fields.
     *      @param name specifies the hostname
     *      @param options query options
     *      @return returns the queried ip string
     *
     */
    function lookup(name: string, options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description queries the address of the given hostname
     *
     *      The supported options of options are as follows:
     *      ```JavaScript
     *      {
     *          "family": 0, // specify the address family: 0 for any, 4 for IPv4, 6 for IPv6, "IPv4"/"IPv6" is also allowed. Default: 0
     *          "all": false // when true, returns an object array of all addresses, otherwise returns a string of the first address. Default: false
     *      }
     *      ```
     *
     *      When all is true, the returned array elements contain the `address` (ip string) and `family` (address family number) fields.
     *      @param name specifies the hostname
     *      @param options query options
     *      @return returns the queried ip string
     *
     */
    function lookupSync(name: string, options?: FIBJS.GeneralObject): any;

    /**
     * @description queries the address of the given hostname
     *
     *      The supported options of options are as follows:
     *      ```JavaScript
     *      {
     *          "family": 0, // specify the address family: 0 for any, 4 for IPv4, 6 for IPv6, "IPv4"/"IPv6" is also allowed. Default: 0
     *          "all": false // when true, returns an object array of all addresses, otherwise returns a string of the first address. Default: false
     *      }
     *      ```
     *
     *      When all is true, the returned array elements contain the `address` (ip string) and `family` (address family number) fields.
     *      @param name specifies the hostname
     *      @param options query options
     *      @return returns the queried ip string
     *
     */
    function lookupAsync(name: string, options?: FIBJS.GeneralObject): Promise<any>;

}


declare module "dns" {
    const promises: typeof import("dns/promises");
}
