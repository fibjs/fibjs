/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Service.d.ts" />
/// <reference path="../module/os_constants.d.ts" />
/**
 * @description The os module reports the operating system the process runs on: platform and kernel identity, CPU and memory resources, user and network information, plus fibjs time helpers
 *
 *  Main capabilities:
 *
 *  - **Identity**: `platform`, `arch`, `type`, `release`, `hostname`, `endianness`, `EOL`,
 *    `timezone`;
 *  - **Resources**: `cpus`, `cpuNumbers`, `loadavg`, `totalmem`, `freemem`;
 *  - **Paths and user**: `homedir`, `tmpdir`, `userInfo`;
 *  - **Network**: `networkInterfaces`;
 *  - **Time helpers**: `time`, `dateAdd`;
 *  - **System integration**: `constants` and `Service`.
 *
 *  Concepts:
 *
 *  - **Portable versus platform-dependent values**: `platform` returns 'linux', 'darwin',
 *    'win32', 'freebsd', 'android' or 'ios'; `arch` returns 'x64', 'ia32', 'arm', 'arm64',
 *    'mips', 'mips64', 'ppc64', 'riscv64' or 'loong64'. `type` and `release` come from the
 *    kernel (Linux, Darwin, Windows_NT, FreeBSD), so their text is not portable, while
 *    `endianness` ('LE'/'BE') and `EOL` ('\n'/'\r\n') are. `loadavg` is all zeros on
 *    Windows. Branch on `platform`/`arch`, never on the text of `type`/`release`.
 *  - **CPU and load shapes**: `cpus` returns one object per logical CPU, re-read on every
 *    call, with `model`, `speed` in MHz and `times` (`user`, `nice`, `sys`, `idle`, `irq`)
 *    in milliseconds since boot. `cpuNumbers` is the same count, cached after the first
 *    call, and is a fibjs extension; Node.js has `os.availableParallelism` and
 *    `os.cpus().length` for that. `loadavg` is the 1, 5 and 15 minute run-queue average,
 *    where a value of 1.0 means one runnable task on average.
 *  - **Memory units**: `totalmem` and `freemem` are bytes of physical memory, and `freemem`
 *    is the memory the operating system reports as available, so on Linux it includes
 *    reclaimable cache and can be much larger than the truly unused pages.
 *  - **Network interface shape**: `networkInterfaces` maps an interface name to an array of
 *    address records: `address`, `netmask`, `family` ('IPv4' or 'IPv6'), `mac` (lowercase
 *    hex, all zeros for the loopback interface), `internal` and, on IPv6 records,
 *    `scopeid`. The Node.js-only `cidr` field is not provided.
 *  - **User information and encodings**: `userInfo` returns `uid`, `gid`, `username`,
 *    `homedir` and `shell`; on Windows uid/gid are -1 and shell is null, and on systems
 *    without a passwd entry the call throws. `options.encoding` selects the text encoding of
 *    the string fields, with 'buffer' returning Buffer values and any other supported
 *    encoding (such as 'base64') encoding the strings, matching Node.js.
 *  - **fibjs extensions**: `time`, `dateAdd`, `timezone`, `cpuNumbers` and `Service` are not
 *    part of the Node.js os module. `time` parses a date string and `dateAdd` shifts a date
 *    by a calendar part with month-end clamping: 2000-01-31 plus one month is 2000-02-29.
 *    `timezone` is the local UTC offset in hundredths of an hour (UTC+8 is 800), unlike
 *    Date#getTimezoneOffset, which counts minutes west of UTC.
 *  - **Node.js gaps**: `os.availableParallelism`, `os.machine`, `os.version`, `os.devNull`
 *    and `os.getPriority`/`setPriority` do not exist; use `cpus().length` when a parallelism
 *    hint is needed.
 *
 *  Import:
 *  ```JavaScript
 *  const os = require('os');
 *  ```
 *
 *  Example 1 — report the platform and CPU identity:
 *  ```JavaScript
 *  const os = require('os');
 *
 *  console.log(os.platform(), os.arch(), os.type(), os.release());
 *  console.log(os.endianness(), JSON.stringify(os.EOL), os.cpuNumbers());
 *  console.log(os.hostname());
 *  ```
 *
 *  Example 2 — inspect memory and load:
 *  ```JavaScript
 *  const os = require('os');
 *
 *  const mib = (bytes) => Math.round(bytes / 1024 / 1024);
 *  console.log('total', mib(os.totalmem()), 'MiB, free', mib(os.freemem()), 'MiB');
 *  console.log('load', os.loadavg().map((v) => v.toFixed(2)).join(' '));
 *
 *  const cpu = os.cpus()[0];
 *  console.log(cpu.model, cpu.speed, 'MHz', Object.keys(cpu.times).join(','));
 *  ```
 *
 *  Example 3 — summarize the effective user and the network interfaces:
 *  ```JavaScript
 *  const os = require('os');
 *
 *  const user = os.userInfo();
 *  console.log(user.username, user.uid, user.gid, user.homedir, user.shell);
 *  console.log(os.tmpdir());
 *
 *  const interfaces = os.networkInterfaces();
 *  Object.keys(interfaces).forEach((name) => {
 *      const addresses = interfaces[name].map((i) => i.family + ' ' + i.address);
 *      console.log(name, addresses.join(', '));
 *  });
 *  ```
 *
 *  Example 4 — parse and shift dates with the fibjs time helpers:
 *  ```JavaScript
 *  const os = require('os');
 *
 *  const start = os.time('2000-1-31T10:10:10');
 *  const february = os.dateAdd(start, 1, 'month'); // 2000 is a leap year
 *  console.log(february.getFullYear(), february.getMonth() + 1, february.getDate());
 *  // 2000 2 29
 *
 *  console.log(isNaN(os.time('2000-1-32').getTime())); // true
 *  ```
 *
 *  Notes:
 *
 *  - Every function is read-only and reflects the machine at call time, except `cpuNumbers`
 *    (cached) and `platform`/`arch` (fixed at build time); nothing here changes the system.
 *  - `os.arch()` matches `process.arch` and `os.platform()` matches `process.platform`.
 *  - `networkInterfaces`, `cpus` and `userInfo` can return empty results or throw on systems
 *    without the corresponding kernel interface (containers, iOS).
 *
 */
declare module 'os' {
    /**
     * @description Reference to the Service class for system service management
     *
     *      The property holds the Service constructor itself, so `new os.Service(name, worker)`
     *      creates a service that can be installed and controlled by the platform service
     *      manager; see the Service class. This is a fibjs extension; Node.js has no service
     *      API.
     *
     */
    const Service: typeof Class_Service;

    /**
     * @description Queries the hostname of the current runtime environment
     *
     *      The value comes from the operating system and is not fully qualified on most systems.
     *      The Node.js method is identical.
     *      @return returns the hostname
     *
     */
    function hostname(): string;

    /**
     * @description Queries the byte order of the current CPU
     *
     *      The result is 'LE' for little-endian architectures (x64, arm64) or 'BE' for
     *      big-endian ones, matching Node.js; it describes the build target, not the memory of a
     *      particular buffer.
     *      @return returns the byte order
     *
     */
    function endianness(): string;

    /**
     * @description Queries the operating system name of the current runtime environment
     *
     *      The string is the kernel name from uname: 'Linux', 'Darwin', 'Windows_NT' or
     *      'FreeBSD'. Use `platform` when a lowercase switchable name is needed, because the
     *      text of `type` is platform specific.
     *      @return returns the system name
     *
     */
    function type(): string;

    /**
     * @description Queries the operating system version of the current runtime environment
     *
     *      The value is the kernel release (for example '5.15.0-139-generic' on Linux) or the
     *      Windows version, and its format varies by system; use it for display and diagnostics,
     *      never to parse a version number.
     *      @return returns the version information
     *
     */
    function release(): string;

    /**
     * @description Queries the home directory of the current user
     *
     *      The value follows $HOME on POSIX and USERPROFILE on Windows, falling back to the
     *      passwd entry; on iOS it is the application documents directory. The string does not
     *      end with a path separator, matching Node.js.
     *      @return returns the directory string
     *
     */
    function homedir(): string;

    /**
     * @description Queries the CPU architecture of the runtime environment
     *
     *      This is the build target of the running binary and matches `process.arch`; see the
     *      module Concepts for the possible values ('x64', 'ia32', 'arm', 'arm64', 'mips',
     *      'mips64', 'ppc64', 'riscv64', 'loong64').
     *      @return returns the CPU architecture name
     *
     */
    function arch(): string;

    /**
     * @description Queries the current time zone of the runtime environment
     *
     *      The value is the local offset from UTC in hundredths of an hour: UTC+8 reports 800
     *      and UTC-5 reports -500. It is a fibjs extension: Node.js exposes nothing similar on
     *      os, and Date#getTimezoneOffset reports minutes west of UTC instead (-480 for UTC+8).
     *
     */
    const timezone: number;

    /**
     * @description Queries the line ending of the current runtime environment, posix:'\n'; windows:'\r\n'
     *
     *      The value is '\n' on POSIX systems and '\r\n' on Windows, matching Node.js; use it for
     *      text files and protocols that require the platform separator.
     *
     */
    const EOL: string;

    /**
     * ! The constants object of the os module, see os_constants
     *
     *      It groups the platform constants used by the low-level APIs: `errno` (error codes),
     *      `signals` (signal numbers), `priority` (process priorities) and `dlopen` (library
     *      loading flags), plus libuv's `UV_UDP_REUSEADDR`. The sub-objects mirror Node.js, while
     *      their entries follow the platform and the bundled libuv.
     *
     */
    const constants: typeof import ('os_constants');

    /**
     * @description Queries the 1-minute, 5-minute and 15-minute average load of the runtime environment
     *
     *      The result is an array of three numbers describing the run queue of the whole system,
     *      not just this process; on Windows the implementation returns [0, 0, 0] because the
     *      kernel does not provide the value.
     *      @return returns an array containing three load values
     *
     */
    function loadavg(): number[];

    /**
     * @description Queries the total memory of the runtime environment, in bytes
     *
     *      The value is the physical memory installed in the machine, not a limit of the current
     *      container or cgroup, matching Node.js; see `freemem` for the available part and
     *      process.memoryUsage for this process.
     *      @return returns the memory value
     *
     */
    function totalmem(): number;

    /**
     * @description Queries the available memory of the runtime environment, in bytes
     *
     *      The value is what the operating system reports as free (on Linux the MemAvailable
     *      estimate), so it includes reclaimable cache; Node.js reports the same value.
     *      @return returns the memory value
     *
     */
    function freemem(): number;

    /**
     * @description Queries the number and parameters of CPUs in the current runtime environment
     *
     *      One object per logical CPU is returned on every call, each with `model`, `speed` in
     *      MHz and `times`; the times are absolute counters in milliseconds since boot (`user`,
     *      `nice`, `sys`, `idle`, `irq`), so sample the array twice to compute a usage ratio. The
     *      shape matches Node.js os.cpus().
     *
     *      Example — summarize the first logical CPU:
     *      ```JavaScript
     *      const os = require('os');
     *
     *      const cpu = os.cpus()[0];
     *      console.log(cpu.model, cpu.speed, 'MHz');
     *      console.log(cpu.times.user, cpu.times.idle); // milliseconds since boot
     *      ```
     *      @return returns an array containing cpu parameters, each item corresponding to one cpu
     *
     */
    function cpus(): any[];

    /**
     * @description Queries the number of CPUs in the current runtime environment
     *
     *      The number of logical CPUs is counted once and then cached for the lifetime of the
     *      process. This is a fibjs extension; Node.js code uses os.cpus().length or
     *      os.availableParallelism() instead.
     *      @return returns the number of CPUs
     *
     */
    function cpuNumbers(): number;

    /**
     * @description Queries the temporary file directory of the current runtime environment
     *
     *      The directory is resolved from the environment on every call: TMPDIR, TMP, TEMP or
     *      TEMPDIR, then the platform fallback (/tmp on POSIX, GetTempPath on Windows); the
     *      returned path does not end with a separator. The directory is suitable for
     *      fs.mkdtemp.
     *      @return returns the temporary file directory
     *
     */
    function tmpdir(): string;

    /**
     * @description Returns information about the currently effective user
     *
     *      The object has `uid` and `gid` numbers plus `username`, `homedir` and `shell`
     *      strings; on Windows uid/gid are -1 and shell is null, and on systems without a passwd
     *      entry the call throws. `encoding` selects the text encoding of the string fields:
     *      'buffer' returns Buffer values and any other supported encoding (such as 'base64')
     *      encodes them, as in Node.js.
     *
     *      Example — print the effective user:
     *      ```JavaScript
     *      const os = require('os');
     *
     *      const user = os.userInfo();
     *      console.log(user.username, user.uid, user.gid);
     *      console.log(user.homedir, user.shell); // shell is null on Windows
     *      ```
     *      @param options character encoding used to interpret the result strings; 'buffer' returns Buffers
     *      @return information about the currently effective user
     *
     */
    function userInfo(options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    /**
     * @description Queries the network information of the current runtime environment
     *
     *      The result maps every interface name to its address records; see the module Concepts
     *      for the fields. Interfaces with several addresses (IPv4 and IPv6, or several aliases)
     *      contribute one record each, grouped in the order the kernel reports them. The
     *      Node.js-only `cidr` field is not provided.
     *
     *      Example — list the addresses of every interface:
     *      ```JavaScript
     *      const os = require('os');
     *
     *      const interfaces = os.networkInterfaces();
     *      Object.keys(interfaces).forEach((name) => {
     *          interfaces[name].forEach((i) => console.log(name, i.family, i.address, i.internal));
     *      });
     *      ```
     *      @return returns the network interface information
     *
     */
    function networkInterfaces(): FIBJS.GeneralObject;

    /**
     * @description Queries the current platform name
     *
     *      Possible results are 'linux', 'win32', 'darwin', 'freebsd', 'android' and 'ios'; the
     *      value is fixed at build time and matches `process.platform`. Node.js uses the same
     *      names for these platforms.
     *      @return returns the platform name
     *
     */
    function platform(): string;

    /**
     * @description Parses a time string or queries the current time of the runtime environment
     *
     *      Without an argument the current time is returned; otherwise tmString is parsed with
     *      the fibjs date parser, which accepts the same forms as the Date constructor (`1998-4-14`,
     *      `4/14/1998 1:12:12.123 pm`, `Tue Apr 14 1998 09:46:05 GMT+0800` and so on). An
     *      unparsable value yields a Date whose getTime() is NaN instead of throwing. This is a
     *      fibjs extension.
     *
     *      Example — parse a date string and detect an invalid one:
     *      ```JavaScript
     *      const os = require('os');
     *
     *      console.log(os.time('1998-4-14').getFullYear()); // 1998
     *      console.log(isNaN(os.time('2000-1-32').getTime())); // true, invalid date
     *      ```
     *      @param tmString time string; if omitted, queries the current time
     *      @return returns a javascript Date object
     *
     */
    function time(tmString?: string): Date;

    /**
     * @description Time calculation function; calculates the time according to part
     *
     *      The date is shifted by num units of part; the accepted parts are 'year', 'month',
     *      'day', 'hour', 'minute' and 'second', and any other value throws Error 20004
     *      ("Invalid date part: ..."). A month or day shift clamps to the end of the target
     *      month (2000-01-31 plus one month is 2000-02-29, 2000-02-29 plus one month is
     *      2000-03-31), so the result is always a valid date. This is a fibjs extension.
     *
     *      Example — add calendar parts and let month ends clamp:
     *      ```JavaScript
     *      const os = require('os');
     *
     *      const jan31 = os.time('2000-1-31T10:10:10');
     *      const february = os.dateAdd(jan31, 1, 'month'); // 2000 is a leap year
     *      console.log(february.getFullYear(), february.getMonth() + 1, february.getDate());
     *      // 2000 2 29
     *      ```
     *      @param d specifies the Date object used for the calculation
     *      @param num specifies the value of the operation
     *      @param part specifies the time part of the operation; accepted values are: "year", "month", "day", "hour", "minute", "second"
     *      @return returns a javascript Date object
     *
     */
    function dateAdd(d: Date, num: number, part: string): Date;

}

