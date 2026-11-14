/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/DnsResolver.d.ts" />
/**
 * The promise variant of the dns module: async members return a Promise as their primary form.
 */
declare module 'dns/promises' {
    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      `rrtype` accepts `A`, `AAAA`, `ANY`, `CAA`, `CNAME`, `MX`, `NAPTR`, `NS`,
     *      `PTR`, `SOA`, `SRV`, `TLSA` and `TXT`; the default is `A`. An unknown type
     *      throws a TypeError before any query starts. Arrays are returned for every
     *      type except `SOA`, which is a single object with `nsname`, `hostmaster`,
     *      `serial`, `refresh`, `retry`, `expire` and `minttl`.
     *
     *      The lookup queries DNS servers directly (it does not read the hosts file);
     *      `ENOTFOUND` means the name does not resolve and `ENODATA` means the name
     *      exists but has no record of that type.
     *
     *      Example — an A query and the SOA object:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      dns.resolve('example.com', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      console.log(dns.resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    function resolve(name: string, rrtype?: string): Promise<any>;

    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      `rrtype` accepts `A`, `AAAA`, `ANY`, `CAA`, `CNAME`, `MX`, `NAPTR`, `NS`,
     *      `PTR`, `SOA`, `SRV`, `TLSA` and `TXT`; the default is `A`. An unknown type
     *      throws a TypeError before any query starts. Arrays are returned for every
     *      type except `SOA`, which is a single object with `nsname`, `hostmaster`,
     *      `serial`, `refresh`, `retry`, `expire` and `minttl`.
     *
     *      The lookup queries DNS servers directly (it does not read the hosts file);
     *      `ENOTFOUND` means the name does not resolve and `ENODATA` means the name
     *      exists but has no record of that type.
     *
     *      Example — an A query and the SOA object:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      dns.resolve('example.com', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      console.log(dns.resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    function resolveSync(name: string, rrtype?: string): any;

    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      `rrtype` accepts `A`, `AAAA`, `ANY`, `CAA`, `CNAME`, `MX`, `NAPTR`, `NS`,
     *      `PTR`, `SOA`, `SRV`, `TLSA` and `TXT`; the default is `A`. An unknown type
     *      throws a TypeError before any query starts. Arrays are returned for every
     *      type except `SOA`, which is a single object with `nsname`, `hostmaster`,
     *      `serial`, `refresh`, `retry`, `expire` and `minttl`.
     *
     *      The lookup queries DNS servers directly (it does not read the hosts file);
     *      `ENOTFOUND` means the name does not resolve and `ENODATA` means the name
     *      exists but has no record of that type.
     *
     *      Example — an A query and the SOA object:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      dns.resolve('example.com', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      console.log(dns.resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    function resolveAsync(name: string, rrtype?: string): Promise<any>;

    /**
     * @description Resolves a host name to its first address, or to every address when all is set
     *
     *      Uses the system resolver. `family` restricts the result (0 any, 4, 6,
     *      'IPv4' or 'IPv6'), `hints` takes the getaddrinfo flags `ADDRCONFIG`, `ALL`
     *      and `V4MAPPED`, and `order` selects `verbatim`, `ipv4first` or `ipv6first`
     *      (the deprecated `verbatim` boolean is also accepted). With `all` the
     *      result is an array of `{address, family}` objects; otherwise one address
     *      is returned. An IP literal is returned without a DNS query.
     *
     *      The callback receives `(err, address, family)`; with `all` it receives
     *      `(err, addresses)`. The promise form returns `{address, family}`, or the
     *      address array with `all`. An empty host name is rejected with
     *      `ERR_INVALID_ARG_VALUE`.
     *
     *      Example — one address, all addresses and a literal:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(dns.lookup('127.0.0.1')); // 127.0.0.1
     *      console.log(JSON.stringify(dns.lookup('127.0.0.1', { all: true })));
     *      // [{"address":"127.0.0.1","family":4}]
     *      ```
     *      @param hostname the host name or IP literal to resolve
     *      @param options the lookup options: family, all, hints, verbatim and order
     *      @return the first address or the address array
     *
     */
    function lookup(hostname: string, options?: {
        family?: string | number;
        all?: boolean;
        hints?: number;
        verbatim?: boolean;
        order?: string;
    }): Promise<{
        address: any;
        family: number;
    }>;

    /**
     * @description Resolves a host name to its first address, or to every address when all is set
     *
     *      Uses the system resolver. `family` restricts the result (0 any, 4, 6,
     *      'IPv4' or 'IPv6'), `hints` takes the getaddrinfo flags `ADDRCONFIG`, `ALL`
     *      and `V4MAPPED`, and `order` selects `verbatim`, `ipv4first` or `ipv6first`
     *      (the deprecated `verbatim` boolean is also accepted). With `all` the
     *      result is an array of `{address, family}` objects; otherwise one address
     *      is returned. An IP literal is returned without a DNS query.
     *
     *      The callback receives `(err, address, family)`; with `all` it receives
     *      `(err, addresses)`. The promise form returns `{address, family}`, or the
     *      address array with `all`. An empty host name is rejected with
     *      `ERR_INVALID_ARG_VALUE`.
     *
     *      Example — one address, all addresses and a literal:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(dns.lookup('127.0.0.1')); // 127.0.0.1
     *      console.log(JSON.stringify(dns.lookup('127.0.0.1', { all: true })));
     *      // [{"address":"127.0.0.1","family":4}]
     *      ```
     *      @param hostname the host name or IP literal to resolve
     *      @param options the lookup options: family, all, hints, verbatim and order
     *      @return the first address or the address array
     *
     */
    function lookupSync(hostname: string, options?: {
        family?: string | number;
        all?: boolean;
        hints?: number;
        verbatim?: boolean;
        order?: string;
    }): {
        address: any;
        family: number;
    };

    /**
     * @description Resolves a host name to its first address, or to every address when all is set
     *
     *      Uses the system resolver. `family` restricts the result (0 any, 4, 6,
     *      'IPv4' or 'IPv6'), `hints` takes the getaddrinfo flags `ADDRCONFIG`, `ALL`
     *      and `V4MAPPED`, and `order` selects `verbatim`, `ipv4first` or `ipv6first`
     *      (the deprecated `verbatim` boolean is also accepted). With `all` the
     *      result is an array of `{address, family}` objects; otherwise one address
     *      is returned. An IP literal is returned without a DNS query.
     *
     *      The callback receives `(err, address, family)`; with `all` it receives
     *      `(err, addresses)`. The promise form returns `{address, family}`, or the
     *      address array with `all`. An empty host name is rejected with
     *      `ERR_INVALID_ARG_VALUE`.
     *
     *      Example — one address, all addresses and a literal:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(dns.lookup('127.0.0.1')); // 127.0.0.1
     *      console.log(JSON.stringify(dns.lookup('127.0.0.1', { all: true })));
     *      // [{"address":"127.0.0.1","family":4}]
     *      ```
     *      @param hostname the host name or IP literal to resolve
     *      @param options the lookup options: family, all, hints, verbatim and order
     *      @return the first address or the address array
     *
     */
    function lookupAsync(hostname: string, options?: {
        family?: string | number;
        all?: boolean;
        hints?: number;
        verbatim?: boolean;
        order?: string;
    }): Promise<{
        address: any;
        family: number;
    }>;

    /**
     * @description Resolves a host name with a numeric address family
     *
     *      This is the legacy `dns.lookup(host, family, callback)` form; the family
     *      must be 0, 4 or 6 and everything else behaves like the options overload.
     *
     *      Example — the numeric family form:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      dns.lookup('127.0.0.1', 4, (err, address, family) => {
     *          if (err) throw err;
     *          console.log(address, family); // 127.0.0.1 4
     *      });
     *      ```
     *      @param hostname the host name or IP literal to resolve
     *      @param family 0 for any family, 4 for IPv4 or 6 for IPv6
     *      @return the first address and its family
     *
     */
    function lookup(hostname: string, family: number): Promise<{
        address: any;
        family: number;
    }>;

    /**
     * @description Resolves a host name with a numeric address family
     *
     *      This is the legacy `dns.lookup(host, family, callback)` form; the family
     *      must be 0, 4 or 6 and everything else behaves like the options overload.
     *
     *      Example — the numeric family form:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      dns.lookup('127.0.0.1', 4, (err, address, family) => {
     *          if (err) throw err;
     *          console.log(address, family); // 127.0.0.1 4
     *      });
     *      ```
     *      @param hostname the host name or IP literal to resolve
     *      @param family 0 for any family, 4 for IPv4 or 6 for IPv6
     *      @return the first address and its family
     *
     */
    function lookupSync(hostname: string, family: number): {
        address: any;
        family: number;
    };

    /**
     * @description Resolves a host name with a numeric address family
     *
     *      This is the legacy `dns.lookup(host, family, callback)` form; the family
     *      must be 0, 4 or 6 and everything else behaves like the options overload.
     *
     *      Example — the numeric family form:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      dns.lookup('127.0.0.1', 4, (err, address, family) => {
     *          if (err) throw err;
     *          console.log(address, family); // 127.0.0.1 4
     *      });
     *      ```
     *      @param hostname the host name or IP literal to resolve
     *      @param family 0 for any family, 4 for IPv4 or 6 for IPv6
     *      @return the first address and its family
     *
     */
    function lookupAsync(hostname: string, family: number): Promise<{
        address: any;
        family: number;
    }>;

    /**
     * @description Resolves the IPv4 addresses of a name
     *
     *      Returns the A records as an array of strings, or as `{address, ttl}`
     *      objects when `ttl` is true. The lookup queries DNS servers directly and
     *      does not consult the hosts file.
     *
     *      Example — the addresses and their TTLs:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolve4('example.com'));
     *      console.log(dns.resolve4('example.com', { ttl: true }));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects instead of strings
     *      @return the A records
     *
     */
    function resolve4(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves the IPv4 addresses of a name
     *
     *      Returns the A records as an array of strings, or as `{address, ttl}`
     *      objects when `ttl` is true. The lookup queries DNS servers directly and
     *      does not consult the hosts file.
     *
     *      Example — the addresses and their TTLs:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolve4('example.com'));
     *      console.log(dns.resolve4('example.com', { ttl: true }));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects instead of strings
     *      @return the A records
     *
     */
    function resolve4Sync(name: string, options?: {
        ttl?: boolean;
    }): any[];

    /**
     * @description Resolves the IPv4 addresses of a name
     *
     *      Returns the A records as an array of strings, or as `{address, ttl}`
     *      objects when `ttl` is true. The lookup queries DNS servers directly and
     *      does not consult the hosts file.
     *
     *      Example — the addresses and their TTLs:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolve4('example.com'));
     *      console.log(dns.resolve4('example.com', { ttl: true }));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects instead of strings
     *      @return the A records
     *
     */
    function resolve4Async(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves the IPv6 addresses of a name
     *
     *      Returns the AAAA records as an array of strings, or as `{address, ttl}`
     *      objects when `ttl` is true. The lookup queries DNS servers directly and
     *      does not consult the hosts file.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolve6('example.com'));
     *      console.log(dns.resolve6('example.com', { ttl: true }));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects instead of strings
     *      @return the AAAA records
     *
     */
    function resolve6(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves the IPv6 addresses of a name
     *
     *      Returns the AAAA records as an array of strings, or as `{address, ttl}`
     *      objects when `ttl` is true. The lookup queries DNS servers directly and
     *      does not consult the hosts file.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolve6('example.com'));
     *      console.log(dns.resolve6('example.com', { ttl: true }));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects instead of strings
     *      @return the AAAA records
     *
     */
    function resolve6Sync(name: string, options?: {
        ttl?: boolean;
    }): any[];

    /**
     * @description Resolves the IPv6 addresses of a name
     *
     *      Returns the AAAA records as an array of strings, or as `{address, ttl}`
     *      objects when `ttl` is true. The lookup queries DNS servers directly and
     *      does not consult the hosts file.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolve6('example.com'));
     *      console.log(dns.resolve6('example.com', { ttl: true }));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects instead of strings
     *      @return the AAAA records
     *
     */
    function resolve6Async(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves every record type of a name in one query
     *
     *      Sends an ANY query and merges the answer records into one array. Each item
     *      carries a `type` field; A/AAAA items have `address` and `ttl`, MX items
     *      have `exchange` and `priority`, SRV items have `name`, `port`, `priority`
     *      and `weight`, TXT items have `entries`, and SOA is returned as one object.
     *
     *      Groups are returned in Node's order: CNAME or A, AAAA, MX, NS, TXT, SRV,
     *      PTR, NAPTR, SOA, TLSA, CAA. When the answer contains a CNAME the A group
     *      is omitted, because the address belongs to the CNAME target.
     *
     *      Example — the mixed ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      dns.resolveAny('example.com', (err, records) => {
     *          if (err) throw err;
     *          console.log(records.map((record) => record.type));
     *      });
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    function resolveAny(name: string): Promise<any[]>;

    /**
     * @description Resolves every record type of a name in one query
     *
     *      Sends an ANY query and merges the answer records into one array. Each item
     *      carries a `type` field; A/AAAA items have `address` and `ttl`, MX items
     *      have `exchange` and `priority`, SRV items have `name`, `port`, `priority`
     *      and `weight`, TXT items have `entries`, and SOA is returned as one object.
     *
     *      Groups are returned in Node's order: CNAME or A, AAAA, MX, NS, TXT, SRV,
     *      PTR, NAPTR, SOA, TLSA, CAA. When the answer contains a CNAME the A group
     *      is omitted, because the address belongs to the CNAME target.
     *
     *      Example — the mixed ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      dns.resolveAny('example.com', (err, records) => {
     *          if (err) throw err;
     *          console.log(records.map((record) => record.type));
     *      });
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    function resolveAnySync(name: string): any[];

    /**
     * @description Resolves every record type of a name in one query
     *
     *      Sends an ANY query and merges the answer records into one array. Each item
     *      carries a `type` field; A/AAAA items have `address` and `ttl`, MX items
     *      have `exchange` and `priority`, SRV items have `name`, `port`, `priority`
     *      and `weight`, TXT items have `entries`, and SOA is returned as one object.
     *
     *      Groups are returned in Node's order: CNAME or A, AAAA, MX, NS, TXT, SRV,
     *      PTR, NAPTR, SOA, TLSA, CAA. When the answer contains a CNAME the A group
     *      is omitted, because the address belongs to the CNAME target.
     *
     *      Example — the mixed ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      dns.resolveAny('example.com', (err, records) => {
     *          if (err) throw err;
     *          console.log(records.map((record) => record.type));
     *      });
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    function resolveAnyAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the CAA records of a name
     *
     *      Each item has a numeric `critical` field and the tag as a property, for
     *      example `{ critical: 0, issue: 'letsencrypt.org' }`. The lookup uses the
     *      DNS servers configured for the resolver.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    function resolveCaa(name: string): Promise<any[]>;

    /**
     * @description Resolves the CAA records of a name
     *
     *      Each item has a numeric `critical` field and the tag as a property, for
     *      example `{ critical: 0, issue: 'letsencrypt.org' }`. The lookup uses the
     *      DNS servers configured for the resolver.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    function resolveCaaSync(name: string): any[];

    /**
     * @description Resolves the CAA records of a name
     *
     *      Each item has a numeric `critical` field and the tag as a property, for
     *      example `{ critical: 0, issue: 'letsencrypt.org' }`. The lookup uses the
     *      DNS servers configured for the resolver.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    function resolveCaaAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the CNAME records of a name
     *
     *      Returns the canonical names as an array of strings. A name without a CNAME
     *      record reports `ENODATA`.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    function resolveCname(name: string): Promise<string[]>;

    /**
     * @description Resolves the CNAME records of a name
     *
     *      Returns the canonical names as an array of strings. A name without a CNAME
     *      record reports `ENODATA`.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    function resolveCnameSync(name: string): string[];

    /**
     * @description Resolves the CNAME records of a name
     *
     *      Returns the canonical names as an array of strings. A name without a CNAME
     *      record reports `ENODATA`.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    function resolveCnameAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the MX records of a name
     *
     *      Each item has `exchange` and `priority` fields. The records are returned in
     *      the order the DNS server sent them.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    function resolveMx(name: string): Promise<any[]>;

    /**
     * @description Resolves the MX records of a name
     *
     *      Each item has `exchange` and `priority` fields. The records are returned in
     *      the order the DNS server sent them.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    function resolveMxSync(name: string): any[];

    /**
     * @description Resolves the MX records of a name
     *
     *      Each item has `exchange` and `priority` fields. The records are returned in
     *      the order the DNS server sent them.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    function resolveMxAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the NAPTR records of a name
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    function resolveNaptr(name: string): Promise<any[]>;

    /**
     * @description Resolves the NAPTR records of a name
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    function resolveNaptrSync(name: string): any[];

    /**
     * @description Resolves the NAPTR records of a name
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    function resolveNaptrAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the NS records of a name
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    function resolveNs(name: string): Promise<string[]>;

    /**
     * @description Resolves the NS records of a name
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    function resolveNsSync(name: string): string[];

    /**
     * @description Resolves the NS records of a name
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    function resolveNsAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the PTR records of a name
     *
     *      Returns the pointer names as an array of strings. This queries the name
     *      exactly as given; use `reverse` to turn an IP address into a name.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    function resolvePtr(name: string): Promise<string[]>;

    /**
     * @description Resolves the PTR records of a name
     *
     *      Returns the pointer names as an array of strings. This queries the name
     *      exactly as given; use `reverse` to turn an IP address into a name.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    function resolvePtrSync(name: string): string[];

    /**
     * @description Resolves the PTR records of a name
     *
     *      Returns the pointer names as an array of strings. This queries the name
     *      exactly as given; use `reverse` to turn an IP address into a name.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    function resolvePtrAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the SOA record of a name
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields. Unlike the other resolve methods
     *      the result is not an array; the counter fields are Numbers.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      const soa = dns.resolveSoa('example.com');
     *      console.log(soa.nsname, soa.serial);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    function resolveSoa(name: string): Promise<any>;

    /**
     * @description Resolves the SOA record of a name
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields. Unlike the other resolve methods
     *      the result is not an array; the counter fields are Numbers.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      const soa = dns.resolveSoa('example.com');
     *      console.log(soa.nsname, soa.serial);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    function resolveSoaSync(name: string): any;

    /**
     * @description Resolves the SOA record of a name
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields. Unlike the other resolve methods
     *      the result is not an array; the counter fields are Numbers.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      const soa = dns.resolveSoa('example.com');
     *      console.log(soa.nsname, soa.serial);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    function resolveSoaAsync(name: string): Promise<any>;

    /**
     * @description Resolves the SRV records of a name
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    function resolveSrv(name: string): Promise<any[]>;

    /**
     * @description Resolves the SRV records of a name
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    function resolveSrvSync(name: string): any[];

    /**
     * @description Resolves the SRV records of a name
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    function resolveSrvAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the TLSA records of a name
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer with the certificate association data. TLSA support
     *      follows the Node.js v25 API.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    function resolveTlsa(name: string): Promise<any[]>;

    /**
     * @description Resolves the TLSA records of a name
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer with the certificate association data. TLSA support
     *      follows the Node.js v25 API.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    function resolveTlsaSync(name: string): any[];

    /**
     * @description Resolves the TLSA records of a name
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer with the certificate association data. TLSA support
     *      follows the Node.js v25 API.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    function resolveTlsaAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the TXT records of a name
     *
     *      Returns one array of strings per TXT record. Each inner array contains the
     *      character-string chunks of that record, exactly as Node.js reports them.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    function resolveTxt(name: string): Promise<any[]>;

    /**
     * @description Resolves the TXT records of a name
     *
     *      Returns one array of strings per TXT record. Each inner array contains the
     *      character-string chunks of that record, exactly as Node.js reports them.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    function resolveTxtSync(name: string): any[];

    /**
     * @description Resolves the TXT records of a name
     *
     *      Returns one array of strings per TXT record. Each inner array contains the
     *      character-string chunks of that record, exactly as Node.js reports them.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    function resolveTxtAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves an IP address to its host names
     *
     *      Sends a PTR query for the reverse name of the address and returns the
     *      aliases reported by the server. The argument must be an IPv4 or IPv6
     *      literal; an invalid literal reports `EINVAL` with syscall
     *      `getHostByAddr`, matching Node.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    function reverse(ip: string): Promise<string[]>;

    /**
     * @description Resolves an IP address to its host names
     *
     *      Sends a PTR query for the reverse name of the address and returns the
     *      aliases reported by the server. The argument must be an IPv4 or IPv6
     *      literal; an invalid literal reports `EINVAL` with syscall
     *      `getHostByAddr`, matching Node.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    function reverseSync(ip: string): string[];

    /**
     * @description Resolves an IP address to its host names
     *
     *      Sends a PTR query for the reverse name of the address and returns the
     *      aliases reported by the server. The argument must be an IPv4 or IPv6
     *      literal; an invalid literal reports `EINVAL` with syscall
     *      `getHostByAddr`, matching Node.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(dns.reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    function reverseAsync(ip: string): Promise<string[]>;

    /**
     * @description Resolves an IP address and port to a host name and service
     *
     *      The address must be an IP literal and the port must be between 0 and
     *      65535. The reverse lookup uses `getnameinfo` with `NI_NAMEREQD`, so a
     *      missing PTR record is an error. The callback receives
     *      `(err, hostname, service)` and the promise form returns
     *      `{hostname, service}`.
     *
     *      Example — a service lookup:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      dns.lookupService('127.0.0.1', 80, (err, hostname, service) => {
     *          if (err) throw err;
     *          console.log(hostname, service);
     *      });
     *      ```
     *      @param address the IPv4 or IPv6 address
     *      @param port the port number
     *      @return the host name and the service name
     *
     */
    function lookupService(address: string, port: number): Promise<{
        hostname: string;
        service: string;
    }>;

    /**
     * @description Resolves an IP address and port to a host name and service
     *
     *      The address must be an IP literal and the port must be between 0 and
     *      65535. The reverse lookup uses `getnameinfo` with `NI_NAMEREQD`, so a
     *      missing PTR record is an error. The callback receives
     *      `(err, hostname, service)` and the promise form returns
     *      `{hostname, service}`.
     *
     *      Example — a service lookup:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      dns.lookupService('127.0.0.1', 80, (err, hostname, service) => {
     *          if (err) throw err;
     *          console.log(hostname, service);
     *      });
     *      ```
     *      @param address the IPv4 or IPv6 address
     *      @param port the port number
     *      @return the host name and the service name
     *
     */
    function lookupServiceSync(address: string, port: number): {
        hostname: string;
        service: string;
    };

    /**
     * @description Resolves an IP address and port to a host name and service
     *
     *      The address must be an IP literal and the port must be between 0 and
     *      65535. The reverse lookup uses `getnameinfo` with `NI_NAMEREQD`, so a
     *      missing PTR record is an error. The callback receives
     *      `(err, hostname, service)` and the promise form returns
     *      `{hostname, service}`.
     *
     *      Example — a service lookup:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      dns.lookupService('127.0.0.1', 80, (err, hostname, service) => {
     *          if (err) throw err;
     *          console.log(hostname, service);
     *      });
     *      ```
     *      @param address the IPv4 or IPv6 address
     *      @param port the port number
     *      @return the host name and the service name
     *
     */
    function lookupServiceAsync(address: string, port: number): Promise<{
        hostname: string;
        service: string;
    }>;

    /**
     * @description Reads the DNS servers of the default resolver
     *
     *      Returns the configured servers in Node.js notation: `8.8.8.8`,
     *      `8.8.8.8:5353`, `[2001:4860:4860::8888]` and
     *      `[2001:4860:4860::8888]:5353`. The port is omitted when it is 53.
     *
     *      Example — read the system servers:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(dns.getServers());
     *      ```
     *      @return the server list
     *
     */
    function getServers(): string[];

    /**
     * @description Replaces the DNS servers of the default resolver
     *
     *      Accepts an array of server strings. Each item is an IPv4 or IPv6 literal,
     *      optionally followed by a port (`8.8.8.8:5353`); an IPv6 address with a
     *      port must be bracketed. Invalid input throws a TypeError and leaves the
     *      previous servers untouched. The call installs a fresh default resolver, so
     *      in-flight `resolve*`/`reverse` queries keep using the old channel and are
     *      not interrupted. The change only affects `resolve*` and `reverse`;
     *      `lookup` always uses the system resolver.
     *
     *      Example — point the resolver at a custom server:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      dns.setServers(['8.8.8.8', '8.8.4.4']);
     *      console.log(dns.getServers());
     *      ```
     *      @param servers the DNS server list
     *
     */
    function setServers(servers: any[]): void;

    /**
     * @description Reads the default result order used by lookup
     *
     *      The value is `verbatim` by default; `ipv4first` and `ipv6first` are the
     *      other accepted values. The setting is scoped to the current isolate.
     *
     *      Example — read the order:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(dns.getDefaultResultOrder()); // verbatim
     *      ```
     *      @return the current default order
     *
     */
    function getDefaultResultOrder(): string;

    /**
     * @description Sets the default result order used by lookup
     *
     *      Only `verbatim`, `ipv4first` and `ipv6first` are accepted; anything else
     *      throws a TypeError. The setting is scoped to the current isolate and does
     *      not change the order of `resolve*` records.
     *
     *      Example — prefer IPv4 addresses:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      dns.setDefaultResultOrder('ipv4first');
     *      console.log(dns.getDefaultResultOrder()); // ipv4first
     *      ```
     *      @param order the default order
     *
     */
    function setDefaultResultOrder(order: string): void;

    /**
     * @description The Resolver class for an independent DNS channel
     *
     *      A Resolver owns its own c-ares channel and server list; it is the class
     *      behind `dns.Resolver` and `dns.promises.Resolver`.
     *
     */
    const Resolver: typeof Class_DnsResolver;

    type Resolver = Class_DnsResolverPromise;

    /**
     * @description The getaddrinfo flag that restricts results to configured families
     *
     *      Pass the value in the `hints` option of `lookup`. The numeric value is the
     *      platform getaddrinfo value.
     *
     */
    const ADDRCONFIG: number;

    /**
     * @description The getaddrinfo flag that returns both address families
     *
     *      Pass the value in the `hints` option of `lookup` together with `V4MAPPED`
     *      to receive IPv4 addresses mapped into IPv6 form.
     *
     */
    const ALL: number;

    /**
     * @description The getaddrinfo flag that maps IPv4 addresses into IPv6 form
     *
     *      Pass the value in the `hints` option of `lookup`. The platform may report
     *      the flag as zero when it does not support it.
     *
     */
    const V4MAPPED: number;

    /**
     * @description No record of the requested type was found
     */
    export const NODATA: "ENODATA";

    /**
     * @description The DNS server reported a format error
     */
    export const FORMERR: "EFORMERR";

    /**
     * @description The DNS server reported a server failure
     */
    export const SERVFAIL: "ESERVFAIL";

    /**
     * @description The name does not resolve
     */
    export const NOTFOUND: "ENOTFOUND";

    /**
     * @description The DNS server does not implement the query
     */
    export const NOTIMP: "ENOTIMP";

    /**
     * @description The DNS server refused the query
     */
    export const REFUSED: "EREFUSED";

    /**
     * @description The query is malformed
     */
    export const BADQUERY: "EBADQUERY";

    /**
     * @description The name is malformed
     */
    export const BADNAME: "EBADNAME";

    /**
     * @description The address family is not supported
     */
    export const BADFAMILY: "EBADFAMILY";

    /**
     * @description The response is malformed
     */
    export const BADRESP: "EBADRESP";

    /**
     * @description The connection was refused
     */
    export const CONNREFUSED: "ECONNREFUSED";

    /**
     * @description The query timed out
     */
    export const TIMEOUT: "ETIMEOUT";

    /**
     * @description The connection closed unexpectedly
     */
    export const EOF: "EOF";

    /**
     * @description A file operation failed
     */
    export const FILE: "EFILE";

    /**
     * @description Memory allocation failed
     */
    export const NOMEM: "ENOMEM";

    /**
     * @description The channel was destroyed
     */
    export const DESTRUCTION: "EDESTRUCTION";

    /**
     * @description A string argument is invalid
     */
    export const BADSTR: "EBADSTR";

    /**
     * @description A flags argument is invalid
     */
    export const BADFLAGS: "EBADFLAGS";

    /**
     * @description A name argument is invalid
     */
    export const NONAME: "ENONAME";

    /**
     * @description The hints argument is invalid
     */
    export const BADHINTS: "EBADHINTS";

    /**
     * @description The library was not initialized
     */
    export const NOTINITIALIZED: "ENOTINITIALIZED";

    /**
     * @description Loading the Windows IP helper API failed
     */
    export const LOADIPHLPAPI: "ELOADIPHLPAPI";

    /**
     * @description Reading the network parameters failed
     */
    export const ADDRGETNETWORKPARAMS: "EADDRGETNETWORKPARAMS";

    /**
     * @description The query was cancelled
     */
    export const CANCELLED: "ECANCELLED";

}


declare module "dns" {
    const promises: typeof import("dns/promises");
}
