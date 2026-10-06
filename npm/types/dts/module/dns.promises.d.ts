/// <reference path="../_import/_fibjs.d.ts" />
/**
 * The promise variant of the dns module: async members return a Promise as their primary form.
 */
declare module 'dns/promises' {
    /**
     * @description Resolves a host name to all of its addresses
     *
     *      Returns every address the system resolver finds for name as strings, IPv4 and IPv6 mixed
     *      and in a system-dependent order; an IP literal is returned unchanged. The name can also be
     *      resolved with a trailing callback or through `dns.promises`. On failure the error carries
     *      `code` (EAI_NONAME when the name does not resolve) and `number`, but no `syscall` or
     *      `hostname`.
     *
     *      Unlike the Node.js `dns.resolve`, there is no record-type argument and no c-ares query: the
     *      result is the complete address list of the name, comparable to `lookup` with `all: true`.
     *
     *      Example — the address list of a numeric literal is stable:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(JSON.stringify(dns.resolve('127.0.0.1'))); // ["127.0.0.1"]
     *      console.log(JSON.stringify(dns.resolve('::1'))); // ["::1"]
     *
     *      try {
     *          dns.resolve('999.999.999.999');
     *      } catch (err) {
     *          console.log(err.code); // EAI_NONAME
     *      }
     *      ```
     *      @param name the host name or IP literal to resolve
     *      @return the addresses of the name
     *
     */
    function resolve(name: string): Promise<string[]>;

    /**
     * @description Resolves a host name to all of its addresses
     *
     *      Returns every address the system resolver finds for name as strings, IPv4 and IPv6 mixed
     *      and in a system-dependent order; an IP literal is returned unchanged. The name can also be
     *      resolved with a trailing callback or through `dns.promises`. On failure the error carries
     *      `code` (EAI_NONAME when the name does not resolve) and `number`, but no `syscall` or
     *      `hostname`.
     *
     *      Unlike the Node.js `dns.resolve`, there is no record-type argument and no c-ares query: the
     *      result is the complete address list of the name, comparable to `lookup` with `all: true`.
     *
     *      Example — the address list of a numeric literal is stable:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(JSON.stringify(dns.resolve('127.0.0.1'))); // ["127.0.0.1"]
     *      console.log(JSON.stringify(dns.resolve('::1'))); // ["::1"]
     *
     *      try {
     *          dns.resolve('999.999.999.999');
     *      } catch (err) {
     *          console.log(err.code); // EAI_NONAME
     *      }
     *      ```
     *      @param name the host name or IP literal to resolve
     *      @return the addresses of the name
     *
     */
    function resolveSync(name: string): string[];

    /**
     * @description Resolves a host name to all of its addresses
     *
     *      Returns every address the system resolver finds for name as strings, IPv4 and IPv6 mixed
     *      and in a system-dependent order; an IP literal is returned unchanged. The name can also be
     *      resolved with a trailing callback or through `dns.promises`. On failure the error carries
     *      `code` (EAI_NONAME when the name does not resolve) and `number`, but no `syscall` or
     *      `hostname`.
     *
     *      Unlike the Node.js `dns.resolve`, there is no record-type argument and no c-ares query: the
     *      result is the complete address list of the name, comparable to `lookup` with `all: true`.
     *
     *      Example — the address list of a numeric literal is stable:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(JSON.stringify(dns.resolve('127.0.0.1'))); // ["127.0.0.1"]
     *      console.log(JSON.stringify(dns.resolve('::1'))); // ["::1"]
     *
     *      try {
     *          dns.resolve('999.999.999.999');
     *      } catch (err) {
     *          console.log(err.code); // EAI_NONAME
     *      }
     *      ```
     *      @param name the host name or IP literal to resolve
     *      @return the addresses of the name
     *
     */
    function resolveAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves a host name to its first address, or to every address when the all option is set
     *
     *      By default the first address the system resolver returns is delivered as a string; with
     *      `all` the result is an array of `{address, family}` objects whose family is 4 or 6. The
     *      `family` option restricts the result (0 any, 4, 6, 'IPv4', 'IPv6'); any other value throws
     *      a TypeError with the message `Invalid family: <value>`. An IP literal is returned without a
     *      DNS query.
     *
     *      A failed lookup throws an error with `code` (ENOTFOUND when the name does not resolve,
     *      EAI_AGAIN for a temporary failure), `errno`, `syscall` ('getaddrinfo'), `hostname` and an
     *      `args` object with the stringified hostname, family and all values. Compared with Node.js,
     *      the trailing callback receives only `(err, result)` (no separate family argument), and the
     *      hints/order/verbatim options are not supported.
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          family: 0, // 0 any, 4 IPv4, 6 IPv6, 'IPv4' or 'IPv6'; default 0
     *          all: false // true returns [{ address, family }], not one string; default false
     *      })
     *      ```
     *
     *      Example — one address, all addresses and an invalid family:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(dns.lookup('127.0.0.1')); // 127.0.0.1
     *      console.log(JSON.stringify(dns.lookup('127.0.0.1', { all: true })));
     *      // [{"address":"127.0.0.1","family":4}]
     *
     *      try {
     *          dns.lookup('localhost', { family: 5 });
     *      } catch (err) {
     *          console.log(err.message); // Invalid family: 5
     *      }
     *      ```
     *      @param name the host name or IP literal to resolve
     *      @param options the query options
     *      @return the first address, or all addresses as objects when all is true
     *
     */
    function lookup(name: string, options?: FIBJS.GeneralObject): Promise<any>;

    /**
     * @description Resolves a host name to its first address, or to every address when the all option is set
     *
     *      By default the first address the system resolver returns is delivered as a string; with
     *      `all` the result is an array of `{address, family}` objects whose family is 4 or 6. The
     *      `family` option restricts the result (0 any, 4, 6, 'IPv4', 'IPv6'); any other value throws
     *      a TypeError with the message `Invalid family: <value>`. An IP literal is returned without a
     *      DNS query.
     *
     *      A failed lookup throws an error with `code` (ENOTFOUND when the name does not resolve,
     *      EAI_AGAIN for a temporary failure), `errno`, `syscall` ('getaddrinfo'), `hostname` and an
     *      `args` object with the stringified hostname, family and all values. Compared with Node.js,
     *      the trailing callback receives only `(err, result)` (no separate family argument), and the
     *      hints/order/verbatim options are not supported.
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          family: 0, // 0 any, 4 IPv4, 6 IPv6, 'IPv4' or 'IPv6'; default 0
     *          all: false // true returns [{ address, family }], not one string; default false
     *      })
     *      ```
     *
     *      Example — one address, all addresses and an invalid family:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(dns.lookup('127.0.0.1')); // 127.0.0.1
     *      console.log(JSON.stringify(dns.lookup('127.0.0.1', { all: true })));
     *      // [{"address":"127.0.0.1","family":4}]
     *
     *      try {
     *          dns.lookup('localhost', { family: 5 });
     *      } catch (err) {
     *          console.log(err.message); // Invalid family: 5
     *      }
     *      ```
     *      @param name the host name or IP literal to resolve
     *      @param options the query options
     *      @return the first address, or all addresses as objects when all is true
     *
     */
    function lookupSync(name: string, options?: FIBJS.GeneralObject): any;

    /**
     * @description Resolves a host name to its first address, or to every address when the all option is set
     *
     *      By default the first address the system resolver returns is delivered as a string; with
     *      `all` the result is an array of `{address, family}` objects whose family is 4 or 6. The
     *      `family` option restricts the result (0 any, 4, 6, 'IPv4', 'IPv6'); any other value throws
     *      a TypeError with the message `Invalid family: <value>`. An IP literal is returned without a
     *      DNS query.
     *
     *      A failed lookup throws an error with `code` (ENOTFOUND when the name does not resolve,
     *      EAI_AGAIN for a temporary failure), `errno`, `syscall` ('getaddrinfo'), `hostname` and an
     *      `args` object with the stringified hostname, family and all values. Compared with Node.js,
     *      the trailing callback receives only `(err, result)` (no separate family argument), and the
     *      hints/order/verbatim options are not supported.
     *
     *      The options object accepts:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          family: 0, // 0 any, 4 IPv4, 6 IPv6, 'IPv4' or 'IPv6'; default 0
     *          all: false // true returns [{ address, family }], not one string; default false
     *      })
     *      ```
     *
     *      Example — one address, all addresses and an invalid family:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(dns.lookup('127.0.0.1')); // 127.0.0.1
     *      console.log(JSON.stringify(dns.lookup('127.0.0.1', { all: true })));
     *      // [{"address":"127.0.0.1","family":4}]
     *
     *      try {
     *          dns.lookup('localhost', { family: 5 });
     *      } catch (err) {
     *          console.log(err.message); // Invalid family: 5
     *      }
     *      ```
     *      @param name the host name or IP literal to resolve
     *      @param options the query options
     *      @return the first address, or all addresses as objects when all is true
     *
     */
    function lookupAsync(name: string, options?: FIBJS.GeneralObject): Promise<any>;

}


declare module "dns" {
    const promises: typeof import("dns/promises");
}
