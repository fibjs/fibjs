/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description The dns module resolves host names to IP addresses: query the first address of a name or all of its addresses, restricted to an address family when needed; useful for connection targets, service discovery and address checks
 *
 *  Main capabilities:
 *
 *  - **Single-address lookup**: `lookup` resolves a name (or an IP literal) to one address and can
 *    restrict the result to IPv4 or IPv6;
 *  - **All-address lookup**: the `all` option of `lookup` returns every address as an
 *    `{address, family}` object;
 *  - **Address list**: `resolve` returns every address of a name as an array of strings.
 *
 *  Concepts:
 *
 *  - **Resolver**: both functions use the system resolver (`getaddrinfo`: the hosts file, NSS and
 *    the configured DNS servers), like the Node.js `dns.lookup`; unlike the Node.js `dns.resolve*`
 *    family no DNS record query is issued (c-ares is not used), so there is no resolver-level
 *    control, no record types and no TTL. The address order is decided by the system and may
 *    change between calls; pass `family` when a specific family is required.
 *  - **Address families**: `family` accepts 0 (any), 4, 6, 'IPv4' or 'IPv6'; with `all` every
 *    entry carries the numeric family 4 or 6. An IP literal is returned unchanged and never causes
 *    DNS traffic, and names such as `localhost` come from the hosts file.
 *  - **Record types**: MX, TXT, SRV, CNAME, NS, SOA, PTR and other records are not queried and TTL
 *    values are not exposed; the result only contains addresses. The operating system may cache
 *    answers internally.
 *  - **Errors and timeouts**: there is no timeout option; a failed resolution throws an error with
 *    `code` (`ENOTFOUND` when the name does not resolve, `EAI_AGAIN` for a temporary failure),
 *    `errno`, `syscall` (`getaddrinfo`) and `hostname`; `resolve` reports `EAI_NONAME` with a
 *    shorter payload. An unsupported `family` is a TypeError before the lookup starts.
 *  - **Call forms**: without a callback the functions block the calling fiber and return the
 *    value; a trailing callback receives `(err, result)`, and the `*Sync`/`*Async` aliases and the
 *    `dns.promises` namespace (`require('dns/promises')`) return the same values. The Node.js
 *    callback of `lookup` is `(err, address, family)`, the fibjs callback only `(err, result)`.
 *
 *  Import:
 *  ```JavaScript
 *  const dns = require('dns');
 *  ```
 *
 *  Example 1 — local names and literals, with the family fixed for determinism:
 *  ```JavaScript
 *  const dns = require('dns');
 *
 *  console.log(dns.lookup('localhost', { family: 4 })); // 127.0.0.1
 *  console.log(dns.lookup('127.0.0.1')); // 127.0.0.1
 *  console.log(dns.lookup('::1', { family: 6 })); // ::1
 *  console.log(JSON.stringify(dns.lookup('127.0.0.1', { all: true })));
 *  // [{"address":"127.0.0.1","family":4}]
 *  ```
 *
 *  Example 2 — the callback and promise forms, and a failed lookup:
 *  ```JavaScript
 *  const dns = require('dns');
 *
 *  dns.lookup('127.0.0.1', { family: 4 }, (err, address) => {
 *      if (err) throw err;
 *      console.log(address); // 127.0.0.1
 *  });
 *
 *  dns.promises.lookup('::1').then((address) => {
 *      console.log(address); // ::1
 *  });
 *
 *  try {
 *      dns.lookup('999.999.999.999');
 *  } catch (err) {
 *      console.log(err.code, err.syscall); // ENOTFOUND getaddrinfo
 *  }
 *  ```
 *
 *  Example 3 — resolve returns the whole address list of a name:
 *  ```JavaScript
 *  const dns = require('dns');
 *
 *  console.log(JSON.stringify(dns.resolve('127.0.0.1'))); // ["127.0.0.1"]
 *  console.log(dns.resolve('localhost').length > 0); // true
 *  ```
 *
 */
declare module 'dns' {
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
    function resolve(name: string): string[];

    function resolve(name: string, callback: (err: Error | undefined | null, retVal: string[])=>any): void;

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
    function lookup(name: string, options?: FIBJS.GeneralObject): any;

    function lookup(name: string, options?: FIBJS.GeneralObject, callback: (err: Error | undefined | null, retVal: any)=>any): void;

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

