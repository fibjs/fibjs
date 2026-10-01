/// <reference path="../_import/_fibjs.d.ts" />
/**
 * @description punycode internationalized domain name conversion module
 *
 *  Punycode is a character encoding scheme defined by RFC 3492, mainly used for internationalized domain names. Because hostnames in URLs are restricted to ASCII characters only, hostnames containing non-ASCII characters must be converted to ASCII using the punycode algorithm.
 *
 *  Usage:
 *  ```JavaScript
 *  var punycode = require('punycode');
 *  ```
 *
 */
declare module 'punycode' {
    /**
     * @description converts a Unicode string into an equivalent Punycode string containing only ASCII characters
     * 	 @param domain the given Unicode string
     * 	 @return returns the encoded Punycode string containing only ASCII characters
     *
     */
    function encode(domain: string): string;

    /**
     * @description converts a Punycode string into an equivalent Unicode string
     * 	 @param domain the given Unicode string
     * 	 @return returns the decoded Unicode string
     *
     */
    function decode(domain: string): string;

    /**
     * @description converts a Unicode string representing a domain name into a string containing only ASCII characters. Only the non-ASCII parts representing the domain name are converted. That is, it is also fine to call this on a string that has already been converted to ASCII.
     * 	 @param domain the given Unicode string
     * 	 @return returns the encoded ASCII string
     *
     */
    function toASCII(domain: string): string;

    /**
     * @description converts a Punycode string representing a domain name into a Unicode string. Only the Punycode parts representing the domain name are converted. That is, it is also fine to call this on a string that has already been converted to Unicode.
     * 	 @param domain the given ASCII string
     * 	 @return returns the decoded Unicode string
     *
     */
    function toUnicode(domain: string): string;

}

