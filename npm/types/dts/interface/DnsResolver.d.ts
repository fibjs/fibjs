/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description An independent DNS resolver with its own c-ares channel
 *
 *  A Resolver owns a c-ares channel, its own timeout/tries/maxTimeout settings and
 *  its own server list. Module-level `dns.resolve*` calls use a default resolver;
 *  every `new dns.Resolver()` is isolated from it.
 *
 *  The promise form is available as `dns.promises.Resolver`.
 *
 *  Example 1 — an isolated resolver with custom servers:
 *  ```JavaScript
 *  const dns = require('dns');
 *
 *  const resolver = new dns.Resolver();
 *  resolver.setServers(['127.0.0.1']);
 *  console.log(resolver.getServers());
 *  ```
 *
 *  Example 2 — a query with its own timeout:
 *  ```JavaScript
 *  const dns = require('dns');
 *
 *  const resolver = new dns.Resolver({ timeout: 2000, tries: 2 });
 *  resolver.resolve4('localhost', (err, addresses) => {
 *      console.log(err, addresses);
 *  });
 *  ```
 *
 *  Example 3 — cancel outstanding queries:
 *  ```JavaScript
 *  const dns = require('dns');
 *
 *  const resolver = new dns.Resolver();
 *  resolver.cancel();
 *  ```
 *
 */
declare class Class_DnsResolver extends Class_object {
    /**
     * @description Creates a resolver with its own timeout, retries and channel
     *
     *      `timeout` is the per-query timeout in milliseconds (or -1 for the c-ares
     *      default), `tries` is the number of tries per name server and `maxTimeout`
     *      is the maximum retry timeout (0 disables it). All three must be integers;
     *      out-of-range values throw `ERR_OUT_OF_RANGE`. The constructor creates the
     *      c-ares channel immediately.
     *
     *      Example — a resolver with a timeout:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      const resolver = new dns.Resolver({ timeout: 1000 });
     *      console.log(resolver.getServers());
     *      ```
     *      @param options timeout, tries and maxTimeout for the channel
     *
     */
    constructor(options?: {
        timeout?: number;
        tries?: number;
        maxTimeout?: number;
    });

    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      Same record types and output shapes as the module-level `dns.resolve`,
     *      but the query uses this resolver's channel and server list.
     *
     *      Example — an A query:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      new dns.Resolver().resolve('example.com', 'A', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    resolve(name: string, rrtype?: string): any;

    resolve(name: string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    resolve(name: string, rrtype: string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      Same record types and output shapes as the module-level `dns.resolve`,
     *      but the query uses this resolver's channel and server list.
     *
     *      Example — an A query:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      new dns.Resolver().resolve('example.com', 'A', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    resolveSync(name: string, rrtype?: string): any;

    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      Same record types and output shapes as the module-level `dns.resolve`,
     *      but the query uses this resolver's channel and server list.
     *
     *      Example — an A query:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      new dns.Resolver().resolve('example.com', 'A', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    resolveAsync(name: string, rrtype?: string): Promise<any>;

    /**
     * @description Resolves the IPv4 addresses through this resolver
     *
     *      Returns A records as strings, or `{address, ttl}` objects with the `ttl`
     *      option. The resolver's own servers are used.
     *
     *      Example — the A records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve4('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the A records
     *
     */
    resolve4(name: string, options?: {
        ttl?: boolean;
    }): any[];

    resolve4(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    resolve4(name: string, options: {
        ttl?: boolean;
    }, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves the IPv4 addresses through this resolver
     *
     *      Returns A records as strings, or `{address, ttl}` objects with the `ttl`
     *      option. The resolver's own servers are used.
     *
     *      Example — the A records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve4('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the A records
     *
     */
    resolve4Sync(name: string, options?: {
        ttl?: boolean;
    }): any[];

    /**
     * @description Resolves the IPv4 addresses through this resolver
     *
     *      Returns A records as strings, or `{address, ttl}` objects with the `ttl`
     *      option. The resolver's own servers are used.
     *
     *      Example — the A records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve4('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the A records
     *
     */
    resolve4Async(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves the IPv6 addresses through this resolver
     *
     *      Returns AAAA records as strings, or `{address, ttl}` objects with the
     *      `ttl` option.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve6('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the AAAA records
     *
     */
    resolve6(name: string, options?: {
        ttl?: boolean;
    }): any[];

    resolve6(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    resolve6(name: string, options: {
        ttl?: boolean;
    }, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves the IPv6 addresses through this resolver
     *
     *      Returns AAAA records as strings, or `{address, ttl}` objects with the
     *      `ttl` option.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve6('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the AAAA records
     *
     */
    resolve6Sync(name: string, options?: {
        ttl?: boolean;
    }): any[];

    /**
     * @description Resolves the IPv6 addresses through this resolver
     *
     *      Returns AAAA records as strings, or `{address, ttl}` objects with the
     *      `ttl` option.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve6('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the AAAA records
     *
     */
    resolve6Async(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves every record type through this resolver
     *
     *      Sends an ANY query and returns the mixed record array with a `type` field
     *      on each item.
     *
     *      Example — the ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveAny('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    resolveAny(name: string): any[];

    resolveAny(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves every record type through this resolver
     *
     *      Sends an ANY query and returns the mixed record array with a `type` field
     *      on each item.
     *
     *      Example — the ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveAny('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    resolveAnySync(name: string): any[];

    /**
     * @description Resolves every record type through this resolver
     *
     *      Sends an ANY query and returns the mixed record array with a `type` field
     *      on each item.
     *
     *      Example — the ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveAny('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    resolveAnyAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the CAA records through this resolver
     *
     *      Each item has a numeric `critical` field and the tag as a property.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    resolveCaa(name: string): any[];

    resolveCaa(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves the CAA records through this resolver
     *
     *      Each item has a numeric `critical` field and the tag as a property.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    resolveCaaSync(name: string): any[];

    /**
     * @description Resolves the CAA records through this resolver
     *
     *      Each item has a numeric `critical` field and the tag as a property.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    resolveCaaAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the CNAME records through this resolver
     *
     *      Returns the canonical names as an array of strings.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    resolveCname(name: string): string[];

    resolveCname(name: string, callback: (err: Error | undefined | null, retVal: string[])=>any): void;

    /**
     * @description Resolves the CNAME records through this resolver
     *
     *      Returns the canonical names as an array of strings.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    resolveCnameSync(name: string): string[];

    /**
     * @description Resolves the CNAME records through this resolver
     *
     *      Returns the canonical names as an array of strings.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    resolveCnameAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the MX records through this resolver
     *
     *      Each item has `exchange` and `priority` fields.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    resolveMx(name: string): any[];

    resolveMx(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves the MX records through this resolver
     *
     *      Each item has `exchange` and `priority` fields.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    resolveMxSync(name: string): any[];

    /**
     * @description Resolves the MX records through this resolver
     *
     *      Each item has `exchange` and `priority` fields.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    resolveMxAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the NAPTR records through this resolver
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    resolveNaptr(name: string): any[];

    resolveNaptr(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves the NAPTR records through this resolver
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    resolveNaptrSync(name: string): any[];

    /**
     * @description Resolves the NAPTR records through this resolver
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    resolveNaptrAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the NS records through this resolver
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    resolveNs(name: string): string[];

    resolveNs(name: string, callback: (err: Error | undefined | null, retVal: string[])=>any): void;

    /**
     * @description Resolves the NS records through this resolver
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    resolveNsSync(name: string): string[];

    /**
     * @description Resolves the NS records through this resolver
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    resolveNsAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the PTR records through this resolver
     *
     *      Returns the pointer names as an array of strings.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    resolvePtr(name: string): string[];

    resolvePtr(name: string, callback: (err: Error | undefined | null, retVal: string[])=>any): void;

    /**
     * @description Resolves the PTR records through this resolver
     *
     *      Returns the pointer names as an array of strings.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    resolvePtrSync(name: string): string[];

    /**
     * @description Resolves the PTR records through this resolver
     *
     *      Returns the pointer names as an array of strings.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    resolvePtrAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the SOA record through this resolver
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    resolveSoa(name: string): any;

    resolveSoa(name: string, callback: (err: Error | undefined | null, retVal: any)=>any): void;

    /**
     * @description Resolves the SOA record through this resolver
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    resolveSoaSync(name: string): any;

    /**
     * @description Resolves the SOA record through this resolver
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    resolveSoaAsync(name: string): Promise<any>;

    /**
     * @description Resolves the SRV records through this resolver
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    resolveSrv(name: string): any[];

    resolveSrv(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves the SRV records through this resolver
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    resolveSrvSync(name: string): any[];

    /**
     * @description Resolves the SRV records through this resolver
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    resolveSrvAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the TLSA records through this resolver
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    resolveTlsa(name: string): any[];

    resolveTlsa(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves the TLSA records through this resolver
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    resolveTlsaSync(name: string): any[];

    /**
     * @description Resolves the TLSA records through this resolver
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    resolveTlsaAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the TXT records through this resolver
     *
     *      Returns one array of strings per TXT record.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    resolveTxt(name: string): any[];

    resolveTxt(name: string, callback: (err: Error | undefined | null, retVal: any[])=>any): void;

    /**
     * @description Resolves the TXT records through this resolver
     *
     *      Returns one array of strings per TXT record.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    resolveTxtSync(name: string): any[];

    /**
     * @description Resolves the TXT records through this resolver
     *
     *      Returns one array of strings per TXT record.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    resolveTxtAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves an IP address to host names through this resolver
     *
     *      The argument must be an IPv4 or IPv6 literal; an invalid literal reports
     *      `EINVAL` with syscall `getHostByAddr`, matching Node. The query uses this
     *      resolver's servers.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    reverse(ip: string): string[];

    reverse(ip: string, callback: (err: Error | undefined | null, retVal: string[])=>any): void;

    /**
     * @description Resolves an IP address to host names through this resolver
     *
     *      The argument must be an IPv4 or IPv6 literal; an invalid literal reports
     *      `EINVAL` with syscall `getHostByAddr`, matching Node. The query uses this
     *      resolver's servers.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    reverseSync(ip: string): string[];

    /**
     * @description Resolves an IP address to host names through this resolver
     *
     *      The argument must be an IPv4 or IPv6 literal; an invalid literal reports
     *      `EINVAL` with syscall `getHostByAddr`, matching Node. The query uses this
     *      resolver's servers.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    reverseAsync(ip: string): Promise<string[]>;

    /**
     * @description Cancels all outstanding queries of this resolver
     *
     *      The callbacks of the cancelled queries receive an ECANCELLED error. The
     *      resolver itself stays usable for new queries.
     *
     *      Example — cancel outstanding work:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      const resolver = new dns.Resolver();
     *      resolver.cancel();
     *      ```
     *
     */
    cancel(): void;

    /**
     * @description Sets the local addresses used for outgoing queries
     *
     *      The first address may be IPv4 or IPv6; when the second is present it must
     *      be the other family. When it is omitted the other family is reset to any.
     *      Both arguments must be valid IP literals.
     *
     *      Example — bind to a local IPv4 address:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      const resolver = new dns.Resolver();
     *      resolver.setLocalAddress('127.0.0.1');
     *      ```
     *      @param ipv4 the IPv4 or IPv6 local address
     *      @param ipv6 the address of the other family
     *
     */
    setLocalAddress(ipv4: string, ipv6?: string): void;

    /**
     * @description Reads the DNS servers of this resolver
     *
     *      Returns the servers in Node.js notation, with the port omitted when it is
     *      53 and IPv6 addresses bracketed when a port is present.
     *
     *      Example — read the servers:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().getServers());
     *      ```
     *      @return the server list
     *
     */
    getServers(): string[];

    /**
     * @description Replaces the DNS servers of this resolver
     *
     *      Accepts the same server strings as the module-level `dns.setServers`:
     *      `8.8.8.8`, `8.8.8.8:5353`, `[2001:4860:4860::8888]` and
     *      `[2001:4860:4860::8888]:5353`. Invalid input throws a TypeError and leaves
     *      the previous list untouched. Changing the list while a query is still
     *      pending throws `ERR_DNS_SET_SERVERS_FAILED` ("There are pending
     *      queries."), again leaving the previous list in place.
     *
     *      Example — set a custom server:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      const resolver = new dns.Resolver();
     *      resolver.setServers(['8.8.8.8']);
     *      console.log(resolver.getServers());
     *      ```
     *      @param servers the DNS server list
     *
     */
    setServers(servers: any[]): void;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * The promise variant of the DnsResolver class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_DnsResolverPromise extends Class_object {
    /**
     * @description Creates a resolver with its own timeout, retries and channel
     *
     *      `timeout` is the per-query timeout in milliseconds (or -1 for the c-ares
     *      default), `tries` is the number of tries per name server and `maxTimeout`
     *      is the maximum retry timeout (0 disables it). All three must be integers;
     *      out-of-range values throw `ERR_OUT_OF_RANGE`. The constructor creates the
     *      c-ares channel immediately.
     *
     *      Example — a resolver with a timeout:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      const resolver = new dns.Resolver({ timeout: 1000 });
     *      console.log(resolver.getServers());
     *      ```
     *      @param options timeout, tries and maxTimeout for the channel
     *
     */
    constructor(options?: {
        timeout?: number;
        tries?: number;
        maxTimeout?: number;
    });

    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      Same record types and output shapes as the module-level `dns.resolve`,
     *      but the query uses this resolver's channel and server list.
     *
     *      Example — an A query:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      new dns.Resolver().resolve('example.com', 'A', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    resolve(name: string, rrtype?: string): Promise<any>;

    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      Same record types and output shapes as the module-level `dns.resolve`,
     *      but the query uses this resolver's channel and server list.
     *
     *      Example — an A query:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      new dns.Resolver().resolve('example.com', 'A', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    resolveSync(name: string, rrtype?: string): any;

    /**
     * @description Resolves a host name to the records of the requested type
     *
     *      Same record types and output shapes as the module-level `dns.resolve`,
     *      but the query uses this resolver's channel and server list.
     *
     *      Example — an A query:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      new dns.Resolver().resolve('example.com', 'A', (err, addresses) => {
     *          if (err) throw err;
     *          console.log(addresses);
     *      });
     *      ```
     *      @param name the host name to query
     *      @param rrtype the record type, A by default
     *      @return the record array, or the SOA object
     *
     */
    resolveAsync(name: string, rrtype?: string): Promise<any>;

    /**
     * @description Resolves the IPv4 addresses through this resolver
     *
     *      Returns A records as strings, or `{address, ttl}` objects with the `ttl`
     *      option. The resolver's own servers are used.
     *
     *      Example — the A records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve4('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the A records
     *
     */
    resolve4(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves the IPv4 addresses through this resolver
     *
     *      Returns A records as strings, or `{address, ttl}` objects with the `ttl`
     *      option. The resolver's own servers are used.
     *
     *      Example — the A records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve4('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the A records
     *
     */
    resolve4Sync(name: string, options?: {
        ttl?: boolean;
    }): any[];

    /**
     * @description Resolves the IPv4 addresses through this resolver
     *
     *      Returns A records as strings, or `{address, ttl}` objects with the `ttl`
     *      option. The resolver's own servers are used.
     *
     *      Example — the A records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve4('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the A records
     *
     */
    resolve4Async(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves the IPv6 addresses through this resolver
     *
     *      Returns AAAA records as strings, or `{address, ttl}` objects with the
     *      `ttl` option.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve6('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the AAAA records
     *
     */
    resolve6(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves the IPv6 addresses through this resolver
     *
     *      Returns AAAA records as strings, or `{address, ttl}` objects with the
     *      `ttl` option.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve6('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the AAAA records
     *
     */
    resolve6Sync(name: string, options?: {
        ttl?: boolean;
    }): any[];

    /**
     * @description Resolves the IPv6 addresses through this resolver
     *
     *      Returns AAAA records as strings, or `{address, ttl}` objects with the
     *      `ttl` option.
     *
     *      Example — the AAAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolve6('example.com'));
     *      ```
     *      @param name the host name to query
     *      @param options ttl selects `{address, ttl}` objects
     *      @return the AAAA records
     *
     */
    resolve6Async(name: string, options?: {
        ttl?: boolean;
    }): Promise<any[]>;

    /**
     * @description Resolves every record type through this resolver
     *
     *      Sends an ANY query and returns the mixed record array with a `type` field
     *      on each item.
     *
     *      Example — the ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveAny('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    resolveAny(name: string): Promise<any[]>;

    /**
     * @description Resolves every record type through this resolver
     *
     *      Sends an ANY query and returns the mixed record array with a `type` field
     *      on each item.
     *
     *      Example — the ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveAny('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    resolveAnySync(name: string): any[];

    /**
     * @description Resolves every record type through this resolver
     *
     *      Sends an ANY query and returns the mixed record array with a `type` field
     *      on each item.
     *
     *      Example — the ANY result:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveAny('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the mixed record array
     *
     */
    resolveAnyAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the CAA records through this resolver
     *
     *      Each item has a numeric `critical` field and the tag as a property.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    resolveCaa(name: string): Promise<any[]>;

    /**
     * @description Resolves the CAA records through this resolver
     *
     *      Each item has a numeric `critical` field and the tag as a property.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    resolveCaaSync(name: string): any[];

    /**
     * @description Resolves the CAA records through this resolver
     *
     *      Each item has a numeric `critical` field and the tag as a property.
     *
     *      Example — the CAA records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCaa('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the CAA records
     *
     */
    resolveCaaAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the CNAME records through this resolver
     *
     *      Returns the canonical names as an array of strings.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    resolveCname(name: string): Promise<string[]>;

    /**
     * @description Resolves the CNAME records through this resolver
     *
     *      Returns the canonical names as an array of strings.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    resolveCnameSync(name: string): string[];

    /**
     * @description Resolves the CNAME records through this resolver
     *
     *      Returns the canonical names as an array of strings.
     *
     *      Example — the canonical name:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveCname('www.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the canonical names
     *
     */
    resolveCnameAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the MX records through this resolver
     *
     *      Each item has `exchange` and `priority` fields.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    resolveMx(name: string): Promise<any[]>;

    /**
     * @description Resolves the MX records through this resolver
     *
     *      Each item has `exchange` and `priority` fields.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    resolveMxSync(name: string): any[];

    /**
     * @description Resolves the MX records through this resolver
     *
     *      Each item has `exchange` and `priority` fields.
     *
     *      Example — the mail exchangers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveMx('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the MX records
     *
     */
    resolveMxAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the NAPTR records through this resolver
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    resolveNaptr(name: string): Promise<any[]>;

    /**
     * @description Resolves the NAPTR records through this resolver
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    resolveNaptrSync(name: string): any[];

    /**
     * @description Resolves the NAPTR records through this resolver
     *
     *      Each item has `flags`, `service`, `regexp`, `replacement`, `order` and
     *      `preference` fields.
     *
     *      Example — the NAPTR records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNaptr('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the NAPTR records
     *
     */
    resolveNaptrAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the NS records through this resolver
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    resolveNs(name: string): Promise<string[]>;

    /**
     * @description Resolves the NS records through this resolver
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    resolveNsSync(name: string): string[];

    /**
     * @description Resolves the NS records through this resolver
     *
     *      Returns the authoritative name servers as an array of strings.
     *
     *      Example — the name servers:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveNs('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the name servers
     *
     */
    resolveNsAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the PTR records through this resolver
     *
     *      Returns the pointer names as an array of strings.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    resolvePtr(name: string): Promise<string[]>;

    /**
     * @description Resolves the PTR records through this resolver
     *
     *      Returns the pointer names as an array of strings.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    resolvePtrSync(name: string): string[];

    /**
     * @description Resolves the PTR records through this resolver
     *
     *      Returns the pointer names as an array of strings.
     *
     *      Example — the pointer records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolvePtr('1.0.0.127.in-addr.arpa'));
     *      ```
     *      @param name the host name to query
     *      @return the pointer names
     *
     */
    resolvePtrAsync(name: string): Promise<string[]>;

    /**
     * @description Resolves the SOA record through this resolver
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    resolveSoa(name: string): Promise<any>;

    /**
     * @description Resolves the SOA record through this resolver
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    resolveSoaSync(name: string): any;

    /**
     * @description Resolves the SOA record through this resolver
     *
     *      Returns one object with `nsname`, `hostmaster`, `serial`, `refresh`,
     *      `retry`, `expire` and `minttl` fields.
     *
     *      Example — the start of authority:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSoa('example.com').nsname);
     *      ```
     *      @param name the host name to query
     *      @return the SOA record object
     *
     */
    resolveSoaAsync(name: string): Promise<any>;

    /**
     * @description Resolves the SRV records through this resolver
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    resolveSrv(name: string): Promise<any[]>;

    /**
     * @description Resolves the SRV records through this resolver
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    resolveSrvSync(name: string): any[];

    /**
     * @description Resolves the SRV records through this resolver
     *
     *      Each item has `name`, `port`, `priority` and `weight` fields.
     *
     *      Example — the service records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveSrv('_http._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the SRV records
     *
     */
    resolveSrvAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the TLSA records through this resolver
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    resolveTlsa(name: string): Promise<any[]>;

    /**
     * @description Resolves the TLSA records through this resolver
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    resolveTlsaSync(name: string): any[];

    /**
     * @description Resolves the TLSA records through this resolver
     *
     *      Each item has numeric `certUsage`, `selector` and `match` fields and a
     *      `data` ArrayBuffer.
     *
     *      Example — the certificate associations:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTlsa('_443._tcp.example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the TLSA records
     *
     */
    resolveTlsaAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves the TXT records through this resolver
     *
     *      Returns one array of strings per TXT record.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    resolveTxt(name: string): Promise<any[]>;

    /**
     * @description Resolves the TXT records through this resolver
     *
     *      Returns one array of strings per TXT record.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    resolveTxtSync(name: string): any[];

    /**
     * @description Resolves the TXT records through this resolver
     *
     *      Returns one array of strings per TXT record.
     *
     *      Example — the text records:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().resolveTxt('example.com'));
     *      ```
     *      @param name the host name to query
     *      @return the text records
     *
     */
    resolveTxtAsync(name: string): Promise<any[]>;

    /**
     * @description Resolves an IP address to host names through this resolver
     *
     *      The argument must be an IPv4 or IPv6 literal; an invalid literal reports
     *      `EINVAL` with syscall `getHostByAddr`, matching Node. The query uses this
     *      resolver's servers.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    reverse(ip: string): Promise<string[]>;

    /**
     * @description Resolves an IP address to host names through this resolver
     *
     *      The argument must be an IPv4 or IPv6 literal; an invalid literal reports
     *      `EINVAL` with syscall `getHostByAddr`, matching Node. The query uses this
     *      resolver's servers.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    reverseSync(ip: string): string[];

    /**
     * @description Resolves an IP address to host names through this resolver
     *
     *      The argument must be an IPv4 or IPv6 literal; an invalid literal reports
     *      `EINVAL` with syscall `getHostByAddr`, matching Node. The query uses this
     *      resolver's servers.
     *
     *      Example — reverse resolution:
     *      ```JavaScript
     *      // requires: network
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().reverse('8.8.8.8'));
     *      ```
     *      @param ip the IPv4 or IPv6 address to reverse
     *      @return the host names
     *
     */
    reverseAsync(ip: string): Promise<string[]>;

    /**
     * @description Cancels all outstanding queries of this resolver
     *
     *      The callbacks of the cancelled queries receive an ECANCELLED error. The
     *      resolver itself stays usable for new queries.
     *
     *      Example — cancel outstanding work:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      const resolver = new dns.Resolver();
     *      resolver.cancel();
     *      ```
     *
     */
    cancel(): void;

    /**
     * @description Sets the local addresses used for outgoing queries
     *
     *      The first address may be IPv4 or IPv6; when the second is present it must
     *      be the other family. When it is omitted the other family is reset to any.
     *      Both arguments must be valid IP literals.
     *
     *      Example — bind to a local IPv4 address:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      const resolver = new dns.Resolver();
     *      resolver.setLocalAddress('127.0.0.1');
     *      ```
     *      @param ipv4 the IPv4 or IPv6 local address
     *      @param ipv6 the address of the other family
     *
     */
    setLocalAddress(ipv4: string, ipv6?: string): void;

    /**
     * @description Reads the DNS servers of this resolver
     *
     *      Returns the servers in Node.js notation, with the port omitted when it is
     *      53 and IPv6 addresses bracketed when a port is present.
     *
     *      Example — read the servers:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      console.log(new dns.Resolver().getServers());
     *      ```
     *      @return the server list
     *
     */
    getServers(): string[];

    /**
     * @description Replaces the DNS servers of this resolver
     *
     *      Accepts the same server strings as the module-level `dns.setServers`:
     *      `8.8.8.8`, `8.8.8.8:5353`, `[2001:4860:4860::8888]` and
     *      `[2001:4860:4860::8888]:5353`. Invalid input throws a TypeError and leaves
     *      the previous list untouched. Changing the list while a query is still
     *      pending throws `ERR_DNS_SET_SERVERS_FAILED` ("There are pending
     *      queries."), again leaving the previous list in place.
     *
     *      Example — set a custom server:
     *      ```JavaScript
     *      const dns = require('dns');
     *
     *      const resolver = new dns.Resolver();
     *      resolver.setServers(['8.8.8.8']);
     *      console.log(resolver.getServers());
     *      ```
     *      @param servers the DNS server list
     *
     */
    setServers(servers: any[]): void;

}


declare namespace Class_DnsResolver {
    const promises: FIBJS.GeneralObject;
}
