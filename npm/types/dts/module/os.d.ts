/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Service.d.ts" />
/// <reference path="../module/os_constants.d.ts" />
/**
 * @description The `os` module is one of the core modules, providing functions for the running operating system. It offers utility features for interacting with the operating system, including file addresses, file paths, network interfaces, hostname, operating system type, etc.
 *
 * ### Common methods
 *
 * The `os` module provides many methods; the following are a few of the more commonly used ones:
 *
 * #### os.hostname()
 *
 * Gets the hostname of the current computer.
 *
 * Example:
 *
 * ```JavaScript
 * const os = require('os');
 * const hostname = os.hostname();
 * console.log(hostname);
 * ```
 *
 * The returned result is similar to:
 *
 * ```sh
 * localhost
 * ```
 *
 * #### os.type()
 *
 * Gets the name of the current operating system.
 *
 * Example:
 *
 * ```JavaScript
 * const os = require('os');
 * const type = os.type();
 * console.log(type);
 * ```
 *
 * The returned result is similar to:
 *
 * ```sh
 * Windows_NT
 * ```
 *
 * #### os.release()
 *
 * Gets the version of the current operating system.
 *
 * Example:
 *
 * ```JavaScript
 * const os = require('os');
 * const release = os.release();
 * console.log(release);
 * ```
 *
 * The returned result is similar to:
 *
 * ```sh
 * 10.0.18362
 * ```
 *
 * #### os.arch()
 *
 * Gets the processor architecture of the operating system.
 *
 * Example:
 *
 * ```JavaScript
 * const os = require('os');
 * const arch = os.arch();
 * console.log(arch);
 * ```
 *
 * The returned result is similar to:
 *
 * ```sh
 * x64
 * ```
 *
 * #### os.cpus()
 *
 * Gets CPU information.
 *
 * Example:
 *
 * ```JavaScript
 * const os = require('os');
 * const cpus = os.cpus();
 * console.log(cpus);
 * ```
 *
 * The returned result is similar to:
 *
 * ```sh
 * [
 *   { model: 'Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz', speed: 2592, times: { user: 2400298, nice: 0, sys: 9684894, idle: 91516801, irq: 0 } },
 *   { model: 'Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz', speed: 2592, times: { user: 464927, nice: 0, sys: 1454926, idle: 95119061, irq: 0 } },
 *   { model: 'Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz', speed: 2592, times: { user: 232077, nice: 0, sys: 898942, idle: 95482112, irq: 0 } },
 *   { model: 'Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz', speed: 2592, times: { user: 950448, nice: 0, sys: 1875169, idle: 93117788, irq: 0 } }
 * ]
 * ```
 *
 */
declare module 'os' {
    /**
     * @description Service constructor, see Service
     */
    const Service: typeof Class_Service;

    /**
     * @description Queries the hostname of the current runtime environment
     *      @return returns the hostname
     *
     */
    function hostname(): string;

    /**
     * @description Queries the byte order of the current CPU
     *      @return returns the byte order
     *
     */
    function endianness(): string;

    /**
     * @description Queries the operating system name of the current runtime environment
     *      @return returns the system name
     *
     */
    function type(): string;

    /**
     * @description Queries the operating system version of the current runtime environment
     *      @return returns the version information
     *
     */
    function release(): string;

    /**
     * @description Queries the home directory of the current user
     *      @return returns the directory string
     *
     */
    function homedir(): string;

    /**
     * @description Queries the current cpu environment
     *      @return returns the cpu type; possible results are 'amd64', 'arm', 'arm64', 'ia32'
     *
     */
    function arch(): string;

    /**
     * @description Queries the current time zone of the runtime environment
     */
    const timezone: number;

    /**
     * @description Queries the line ending of the current runtime environment, posix:\"\\n\"; windows:\"\\r\\n\"
     */
    const EOL: string;

    /**
     * ! The constants object of the os module, see os_constants
     */
    const constants: typeof import ('os_constants');

    /**
     * @description Queries the 1-minute, 5-minute and 15-minute average load of the runtime environment
     *      @return returns an array containing three load values
     *
     */
    function loadavg(): any[];

    /**
     * @description Queries the total memory of the runtime environment, in bytes
     *      @return returns the memory value
     *
     */
    function totalmem(): number;

    /**
     * @description Queries the available memory of the runtime environment, in bytes
     *      @return returns the memory value
     *
     */
    function freemem(): number;

    /**
     * @description Queries the number and parameters of CPUs in the current runtime environment
     *      @return returns an array containing cpu parameters, each item corresponding to one cpu
     *
     */
    function cpus(): any[];

    /**
     * @description Queries the number of CPUs in the current runtime environment
     *      @return returns the number of CPUs
     *
     */
    function cpuNumbers(): number;

    /**
     * @description Queries the temporary file directory of the current runtime environment
     *      @return returns the temporary file directory
     *
     */
    function tmpdir(): string;

    /**
     * @description Returns information about the currently effective user
     *      @param options character encoding used to interpret the result strings
     *      @return information about the currently effective user
     *
     */
    function userInfo(options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Queries the network information of the current runtime environment
     *      @return returns the network interface information
     *
     */
    function networkInterfaces(): FIBJS.GeneralObject;

    /**
     * @description Queries the current platform name
     *      @return returns the platform name; possible results are 'darwin', 'freebsd', 'linux', or 'win32'
     *
     */
    function platform(): string;

    /**
     * @description Parses a time string or queries the current time of the runtime environment
     *      @param tmString time string; if omitted, queries the current time
     *      @return returns a javascript Date object
     *
     */
    function time(tmString?: string): typeof Date;

    /**
     * @description Time calculation function; calculates the time according to part
     *      @param d specifies the Date object used for the calculation
     *      @param num specifies the value of the operation
     *      @param part specifies the time part of the operation; accepted values are: "year", "month", "day", "hour", "minute", "second"
     *      @return returns a javascript Date object
     *
     */
    function dateAdd(d: typeof Date, num: number, part: string): typeof Date;

}

