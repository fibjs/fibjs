/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description The WASI preview1 host: binds a WebAssembly instance to the guest system-call ABI
 *
 *  A WASI host is the host side of a wasm module that imports `wasi_snapshot_preview1`. It holds the
 *  guest arguments, the environment and the preopened directories, exposes the syscall methods that
 *  the wasm module imports, and knows the instance memory through `start()` or `initialize()`.
 *  Instantiate the wasm module with `getImportObject()`; the module then calls back into this object
 *  for every file, clock, random and process operation. The class is exported by the wasi module as
 *  `WASI`.
 *
 *  Concepts:
 *
 *  - **Construction**: the options object accepts `version` (`'preview1'`, the default, or `'unstable'`
 *    for the legacy `wasi_unstable` namespace), `args` (the guest argv, an empty array by default),
 *    `env` (the guest environment, an empty object by default; entries whose value is `undefined`
 *    are dropped), `preopens` (a map from guest path to host path, empty by default), `sockets`
 *    (an array of `{ address, port }` listeners that are exposed as preopened socket descriptors
 *    after the directory preopens) and the `stdin`/`stdout`/`stderr` descriptors that back guest
 *    descriptors 0/1/2 (0/1/2 by default). `returnOnExit` (true by default) selects what
 *    `proc_exit` does: when true the exit unwinds back to `start()` as an `ERR_WASI_EXIT` error and
 *    `start()` returns the guest exit code; when false the host calls `process.exit(code)`, matching
 *    uvwasi, so the current process terminates with the guest status.
 *    Invalid option types throw a `TypeError`, matching the Node.js constructor.
 *  - **Binding**: `start(instance)` is the command model, requires `_start`, rejects an `_initialize`
 *    export and returns the guest exit code; `initialize(instance)` is the reactor model, rejects
 *    `_start` and calls `_initialize` when present. Both go through `finalizeBindings()`, which may be
 *    called only once per host (a second call throws an already-started error) and accepts a
 *    `{ memory }` override for embedders. The memory must be a `WebAssembly.Memory` object.
 *  - **Syscalls**: every method mirrors one import of the preview1 ABI and takes the raw wasm
 *    parameters. Pointers are guest-memory offsets, not host pointers; the iovec list passed to
 *    `fd_read`/`fd_write` is an array of `{ buf, buf_len }` records. The return value is 0 on success
 *    or one of the WASI `errno` values (`8` BADF, `21` FAULT, `28` INVAL, `29` IO, `31` ISDIR,
 *    `42` NOBUFS, `44` NOENT, `52` NOSYS, `61` OVERFLOW, `76` NOTCAPABLE); the methods do not throw
 *    for guest errors. Out-of-range guest pointers report `61` and a too-small output buffer reports
 *    `42`, matching uvwasi.
 *  - **Rights**: each descriptor carries the `rights_base`/`rights_inheriting` masks granted by
 *    `path_open` (intersected with what the opened file type supports). A syscall that needs a right
 *    the descriptor does not have returns `76` instead of touching the host, and `path_open` only
 *    succeeds when the directory's inheriting rights cover the request. This mirrors uvwasi and is
 *    what wasi-libc expects.
 *  - **Blocking and fibers**: a syscall that waits blocks only the calling fiber. The host starts the
 *    uv request, suspends the fiber and resumes it when the request completes, so timers, sockets and
 *    other fibers continue to run during a `poll_oneoff` or a slow read. Unlike most fibjs objects the
 *    host is lock free (`FIBER_FREE`): it does not take the per-object reentrancy lock, so a fiber
 *    blocked in one syscall does not stop other fibers from calling other syscalls on the same
 *    instance. A wasm instance is single threaded and calls its imports from one fiber at a time, so
 *    concurrency only arises when host code shares one `WASIHost` between fibers.
 *
 *    What the host guarantees to such host code: the descriptor table is only mutated from the JS
 *    thread, and every descriptor carries an id, so a call that was suspended resolves its descriptor
 *    by id on completion instead of trusting the guest fd number. Closing a descriptor that another
 *    fiber is using is therefore safe: a second `fd_close` reports `8` (BADF) instead of closing the
 *    same host file twice, a pending `path_open` whose placeholder is closed returns `8` and closes
 *    the host file, and `fd_renumber` moves the descriptor without confusing in-flight calls.
 *    Sockets are guarded by a per-socket mutex, uv completions are one-shot (a close racing a
 *    connection handoff settles each waiting fiber exactly once), and closing a listener wakes the
 *    fibers parked in `sock_accept` with `8`. The host does not serialize concurrent syscalls on the
 *    *same* descriptor, so host code that shares an instance must not read and seek one descriptor
 *    from two fibers at the same time; a wasm guest never does.
 *  - **Scope**: wasm imports are called as plain functions, so each syscall method is bound to the
 *    host instance in the constructor and stored as an own property of the instance.
 *    `getImportObject()` and the `wasiImport` property therefore return the host itself:
 *    `wasiImport === wasi`, and the value stays recognizable by `instanceof` and `getInstance()`.
 *    This differs from Node, where `wasiImport` is a separate wrapper object.
 *
 *  Import:
 *  ```JavaScript
 *  const { WASI } = require('wasi');
 *  ```
 *
 *  Example 1 — build the import object and print the ABI key:
 *  ```JavaScript
 *  const { WASI } = require('wasi');
 *
 *  const wasi = new WASI({ version: 'preview1', args: ['app'], env: { LANG: 'C' } });
 *  const imports = wasi.getImportObject();
 *  console.log(Object.keys(imports)); // [ 'wasi_snapshot_preview1' ]
 *  ```
 *
 *  Example 2 — construct and inspect a syscall method:
 *  ```JavaScript
 *  const { WASI } = require('wasi');
 *
 *  const wasi = new WASI({ version: 'preview1' });
 *  console.log(typeof wasi.fd_read); // function
 *  console.log(typeof wasi.poll_oneoff); // function
 *  ```
 *
 *  Notes:
 *
 *  - Guest descriptors 0, 1 and 2 are backed by the `stdin`/`stdout`/`stderr` options (host
 *    descriptor numbers); their `fd_fdstat_get` rights follow the host file type, like uvwasi (a tty
 *    gets the tty set, a regular file gets the regular-file set, a pipe or socket gets the socket
 *    set).
 *  - Preopened directories are virtual descriptors: the host path is stored and directory scans are
 *    driven from it, so no host descriptor is consumed per preopen. Sockets from the `sockets` option
 *    are real libuv TCP listeners; accepted connections become guest socket descriptors and are
 *    driven by a host-side queue so `sock_accept` can suspend without dropping connections.
 *  - `fd_read` and `fd_write` copy the guest iovecs into a host buffer, perform the operation on the
 *    uv worker and copy the result back into guest memory after the fiber resumes.
 *  - `poll_oneoff` probes descriptor readiness with a non-blocking host `poll()`; regular files and
 *    directories are always ready, and a hung-up pipe reports the HANGUP event flag. If nothing is
 *    ready, the fiber suspends on `uv_poll` handles (one per host descriptor, watching only the
 *    subscribed direction) and/or a `uv_timer` for the earliest clock subscription. A clock timeout
 *    reports one clock event; ready descriptors win over a concurrent timeout.
 *  - Two behaviors are deliberately more permissive than Node/uvwasi: `fd_renumber` may target a
 *    free descriptor (the preview1 specification allows it while uvwasi returns BADF), and
 *    `path_open` can open a directory read-only (uvwasi rejects it with INVAL on macOS).
 *  - The host follows the preview1 specification and current uvwasi where Node 23.x still differs:
 *    an absolute guest path or a `..` that escapes the preopen reports NOTCAPABLE (`76`) instead of
 *    opening it or reporting NOENT, an embedded NUL reports INVAL (`28`), and guest pointer range
 *    failures report OVERFLOW (`61`) instead of INVAL. `fd_fdstat_get` reports the WASI
 *    `fs_flags` the guest set (APPEND `1`, DSYNC `2`, NONBLOCK `4`, RSYNC `8`, SYNC `16`);
 *    uvwasi copies the raw host `fcntl(F_GETFL)` bits there, which leaks host flags into the guest
 *    ABI.
 *  - `version` is required, like Node: it selects the import namespace and cannot be guessed.
 *    Preopen host paths are resolved at construction; a missing path or a non-directory throws
 *    `ERR_WASI_PREOPEN` (Node reports the uvwasi error code instead).
 *  - A syscall invoked before `start()`/`initialize()`/`finalizeBindings()` binds a memory throws
 *    `ERR_WASI_NOT_STARTED` (`wasi.start() has not been called`), matching Node.
 *  - The socket syscalls validate like uvwasi: the flag arguments first (NOTSUP/INVAL), then BADF,
 *    then the socket rights (NOTCAPABLE), then the descriptor kind (NOTSOCK). A non-socket
 *    descriptor therefore reports NOTCAPABLE rather than NOTSOCK.
 *
 */
declare class Class_WASIHost extends Class_object {
    /**
     * @description Creates a WASI preview1 host from an options object
     *
     *      `version` is required and selects the import namespace: `'preview1'` (`wasi_snapshot_preview1`)
     *      or `'unstable'` (the legacy `wasi_unstable`), matching Node. `args` is the guest argument
     *      vector (without the program name), `env` is the guest environment, `preopens` maps guest paths
     *      to host paths, `sockets` lists the TCP listeners to expose as preopened socket descriptors and
     *      `stdin`/`stdout`/`stderr` select the host descriptors behind guest 0/1/2. Every preopen must
     *      resolve to an existing directory; the host paths are canonicalised at construction. Wrong
     *      option types throw `TypeError`, an unsupported version throws `ERR_INVALID_ARG_VALUE`, a
     *      missing or non-directory preopen throws `ERR_WASI_PREOPEN`, and a listener that cannot bind
     *      throws `ERR_WASI_SOCKET`.
     *      @param options the version, argument vector, environment, preopened directories, sockets and stdio descriptors
     *
     */
    constructor(options: FIBJS.GeneralObject);

    /**
     * @description Returns the import object to pass to WebAssembly.instantiate
     *
     *      The result has one key, `wasi_snapshot_preview1` (or `wasi_unstable` for the legacy version),
     *      whose value is this host instance. Its syscall methods are bound to the instance in the
     *      constructor, so wasm can call them as plain functions; the same instance is exposed as the
     *      `wasiImport` property (`wasiImport === wasi`).
     *      @return the import object keyed by the ABI name
     *
     */
    getImportObject(): FIBJS.GeneralObject;

    /**
     * @description Binds a command-style instance and runs its `_start` export
     *
     *      The instance must export `memory` and `_start`, and must not export `_initialize` (use
     *      `initialize()` for reactors). The call runs synchronously in the current fiber; syscalls that
     *      need the host may suspend and resume the fiber without blocking other fibers. The return value
     *      is the guest exit code, 0 when `_start` returns normally. Binding twice throws
     *      `ERR_WASI_ALREADY_STARTED`.
     *      @param instance the instance to bind: a `WebAssembly.Instance`, or the exports returned by
     *      `require()` of a `.wasm` file
     *      @return the guest exit code
     *
     */
    start(instance: any): number;

    /**
     * @description Binds a reactor-style instance and runs its `_initialize` export
     *
     *      The instance must export `memory` and must not export `_start`; `_initialize` is optional in
     *      the reactor model and is called when present. The instance stays alive after the call, so its
     *      exported functions can call the syscalls later through the import object. Binding twice throws
     *      `ERR_WASI_ALREADY_STARTED`.
     *      @param instance the instance to bind: a `WebAssembly.Instance`, or the exports returned by
     *      `require()` of a `.wasm` file
     *
     */
    initialize(instance: any): void;

    /**
     * @description Binds an existing instance for compatibility with embedders
     *
     *      This is the hook Node.js uses when the import object is linked after instantiation. It
     *      captures `options.memory` when supplied, otherwise `instance.exports.memory` (which must be a
     *      `WebAssembly.Memory`), and does not call any export. A host can be bound only once; later calls
     *      throw `ERR_WASI_ALREADY_STARTED`.
     *      @param instance the instance to bind: a `WebAssembly.Instance`, or the exports returned by
     *      `require()` of a `.wasm` file
     *      @param options optional `{ memory }` override
     *
     */
    finalizeBindings(instance: any, options?: FIBJS.GeneralObject): void;

    /**
     * @description Registers this host as the import namespace for `.wasm` modules
     *
     *      `require('./app.wasm')` compiles a wasm binary and instantiates it with the
     *      namespaces its imports ask for. Calling `registerAsImport()` stores this host
     *      in the isolate-wide registry under the host ABI namespace
     *      (`wasi_snapshot_preview1`, or `wasi_unstable` for the legacy version), so a
     *      command module can be loaded without any extra glue:
     *
     *      ```JavaScript
     *      const { WASI } = require('wasi');
     *      const wasi = new WASI({ version: 'preview1', args: ['app'] });
     *      wasi.registerAsImport();
     *      const app = require('./app.wasm');       // instance exports
     *      wasi.start(app);                         // binds the instance behind them
     *      ```
     *
     *      Registration is per isolate (workers have their own registry) and lasts
     *      until `unregisterAsImport()` or the end of the isolate. A registered
     *      namespace takes precedence over module resolution; namespaces that are not
     *      registered are resolved with `require(name)` from the directory of the wasm
     *      file, so `import_module("./helper.js")` works too.
     *
     */
    registerAsImport(): void;

    /**
     * @description Removes a namespace registered by `registerAsImport()`
     *
     *      The registry entry is only removed when it still holds this host, so
     *      unregistering an old host cannot drop a newer registration.
     *
     */
    unregisterAsImport(): void;

    /**
     * @description The import object, the host instance itself
     *
     *      Read-only alias of `getImportObject()['wasi_snapshot_preview1']`. The value is the host
     *      instance: the syscall methods are bound own properties, so calling `wasi.fd_read` works even
     *      though wasm calls them without a receiver. `wasiImport === wasi` and `instanceof` hold;
     *      Node.js uses a separate wrapper here, which is the only shape difference.
     *
     */
    readonly wasiImport: FIBJS.GeneralObject;

    /**
     * @description Implements the WASI `fd_read` syscall
     *
     *      Reads from the descriptor into the guest iovec list, which is an array of 8-byte records
     *      (`buf`, `buf_len` as little-endian u32). The host copies a contiguous range, performs the read
     *      on the uv worker and writes the number of bytes read to the `nread` guest address. The
     *      descriptor is validated against the open table and a bad descriptor returns BADF instead of
     *      throwing.
     *      @param fd the guest descriptor to read from
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_read(fd: number, iovs: number, iovs_len: number, nread: number): number;

    fd_read(fd: number, iovs: number, iovs_len: number, nread: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_read` syscall
     *
     *      Reads from the descriptor into the guest iovec list, which is an array of 8-byte records
     *      (`buf`, `buf_len` as little-endian u32). The host copies a contiguous range, performs the read
     *      on the uv worker and writes the number of bytes read to the `nread` guest address. The
     *      descriptor is validated against the open table and a bad descriptor returns BADF instead of
     *      throwing.
     *      @param fd the guest descriptor to read from
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readSync(fd: number, iovs: number, iovs_len: number, nread: number): number;

    /**
     * @description Implements the WASI `fd_read` syscall
     *
     *      Reads from the descriptor into the guest iovec list, which is an array of 8-byte records
     *      (`buf`, `buf_len` as little-endian u32). The host copies a contiguous range, performs the read
     *      on the uv worker and writes the number of bytes read to the `nread` guest address. The
     *      descriptor is validated against the open table and a bad descriptor returns BADF instead of
     *      throwing.
     *      @param fd the guest descriptor to read from
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readAsync(fd: number, iovs: number, iovs_len: number, nread: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_write` syscall
     *
     *      Gathers the bytes described by the guest iovec list and writes them to the descriptor. The
     *      iovec records are `{ buf, buf_len }` pairs of little-endian u32 values; the host bounds-checks
     *      every record against the instance memory, copies the payload to a staging buffer, writes it on
     *      the uv worker and stores the byte count at `nwritten`. A descriptor that is not open returns
     *      BADF.
     *      @param fd the guest descriptor to write to
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_write(fd: number, iovs: number, iovs_len: number, nwritten: number): number;

    fd_write(fd: number, iovs: number, iovs_len: number, nwritten: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_write` syscall
     *
     *      Gathers the bytes described by the guest iovec list and writes them to the descriptor. The
     *      iovec records are `{ buf, buf_len }` pairs of little-endian u32 values; the host bounds-checks
     *      every record against the instance memory, copies the payload to a staging buffer, writes it on
     *      the uv worker and stores the byte count at `nwritten`. A descriptor that is not open returns
     *      BADF.
     *      @param fd the guest descriptor to write to
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_writeSync(fd: number, iovs: number, iovs_len: number, nwritten: number): number;

    /**
     * @description Implements the WASI `fd_write` syscall
     *
     *      Gathers the bytes described by the guest iovec list and writes them to the descriptor. The
     *      iovec records are `{ buf, buf_len }` pairs of little-endian u32 values; the host bounds-checks
     *      every record against the instance memory, copies the payload to a staging buffer, writes it on
     *      the uv worker and stores the byte count at `nwritten`. A descriptor that is not open returns
     *      BADF.
     *      @param fd the guest descriptor to write to
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_writeAsync(fd: number, iovs: number, iovs_len: number, nwritten: number): Promise<number>;

    /**
     * @description Implements the WASI `poll_oneoff` syscall
     *
     *      Parses the 48-byte subscriptions. Clock subscriptions are served by a uv timer; descriptor
     *      subscriptions are probed with a non-blocking host `poll()` (regular files and directories are
     *      always ready) and their events carry the HANGUP flag for a closed pipe. Ready descriptor events
     *      are written immediately; otherwise the fiber suspends on `uv_poll` and the earliest clock
     *      timer, and the ready descriptors are collected when it wakes up. A descriptor subscription
     *      without POLL_FD_READWRITE plus FD_READ/FD_WRITE reports NOTCAPABLE; an unknown descriptor is
     *      reported as a BADF event. On Windows the host keeps no pollable handle for a socket, so every
     *      descriptor subscription is reported ready without waiting; the guest still blocks in
     *      `sock_recv` until data arrives.
     *      @param in_ptr the guest address of the subscription array
     *      @param out_ptr the guest address of the event array
     *      @param nsubscriptions the number of subscriptions
     *      @param nevents the guest address that receives the event count
     *      @return 0 on success, or a WASI errno value
     *
     */
    poll_oneoff(in_ptr: number, out_ptr: number, nsubscriptions: number, nevents: number): number;

    poll_oneoff(in_ptr: number, out_ptr: number, nsubscriptions: number, nevents: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `poll_oneoff` syscall
     *
     *      Parses the 48-byte subscriptions. Clock subscriptions are served by a uv timer; descriptor
     *      subscriptions are probed with a non-blocking host `poll()` (regular files and directories are
     *      always ready) and their events carry the HANGUP flag for a closed pipe. Ready descriptor events
     *      are written immediately; otherwise the fiber suspends on `uv_poll` and the earliest clock
     *      timer, and the ready descriptors are collected when it wakes up. A descriptor subscription
     *      without POLL_FD_READWRITE plus FD_READ/FD_WRITE reports NOTCAPABLE; an unknown descriptor is
     *      reported as a BADF event. On Windows the host keeps no pollable handle for a socket, so every
     *      descriptor subscription is reported ready without waiting; the guest still blocks in
     *      `sock_recv` until data arrives.
     *      @param in_ptr the guest address of the subscription array
     *      @param out_ptr the guest address of the event array
     *      @param nsubscriptions the number of subscriptions
     *      @param nevents the guest address that receives the event count
     *      @return 0 on success, or a WASI errno value
     *
     */
    poll_oneoffSync(in_ptr: number, out_ptr: number, nsubscriptions: number, nevents: number): number;

    /**
     * @description Implements the WASI `poll_oneoff` syscall
     *
     *      Parses the 48-byte subscriptions. Clock subscriptions are served by a uv timer; descriptor
     *      subscriptions are probed with a non-blocking host `poll()` (regular files and directories are
     *      always ready) and their events carry the HANGUP flag for a closed pipe. Ready descriptor events
     *      are written immediately; otherwise the fiber suspends on `uv_poll` and the earliest clock
     *      timer, and the ready descriptors are collected when it wakes up. A descriptor subscription
     *      without POLL_FD_READWRITE plus FD_READ/FD_WRITE reports NOTCAPABLE; an unknown descriptor is
     *      reported as a BADF event. On Windows the host keeps no pollable handle for a socket, so every
     *      descriptor subscription is reported ready without waiting; the guest still blocks in
     *      `sock_recv` until data arrives.
     *      @param in_ptr the guest address of the subscription array
     *      @param out_ptr the guest address of the event array
     *      @param nsubscriptions the number of subscriptions
     *      @param nevents the guest address that receives the event count
     *      @return 0 on success, or a WASI errno value
     *
     */
    poll_oneoffAsync(in_ptr: number, out_ptr: number, nsubscriptions: number, nevents: number): Promise<number>;

    /**
     * @description Implements the WASI `args_sizes_get` syscall
     *
     *      Writes the argument count and the size of the buffer needed to store the arguments with their
     *      NUL terminators. The guest allocates `argv` and the buffer from these two values before
     *      calling `args_get`. The arguments come from the `args` option of the constructor.
     *      @param argc the guest address that receives the argument count
     *      @param argv_buf_size the guest address that receives the buffer size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    args_sizes_get(argc: number, argv_buf_size: number): number;

    /**
     * @description Implements the WASI `args_get` syscall
     *
     *      Writes the argument vector and the argument bytes. `argv` receives one u32 pointer per
     *      argument, each pointing into the buffer at `argv_buf`; the strings are NUL separated and each
     *      one is preceded by its pointer. The host bounds-checks both guest ranges before writing.
     *      @param argv the guest address of the pointer array
     *      @param argv_buf the guest address of the string buffer
     *      @return 0 on success, or a WASI errno value
     *
     */
    args_get(argv: number, argv_buf: number): number;

    /**
     * @description Implements the WASI `environ_sizes_get` syscall
     *
     *      Writes the environment variable count and the size of the buffer needed to store the
     *      `key=value` strings with their NUL terminators. The environment comes from the `env` option
     *      of the constructor.
     *      @param environ_count the guest address that receives the variable count
     *      @param environ_buf_size the guest address that receives the buffer size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    environ_sizes_get(environ_count: number, environ_buf_size: number): number;

    /**
     * @description Implements the WASI `environ_get` syscall
     *
     *      Writes the environment vector and the `key=value` strings. `environ` receives one u32 pointer
     *      per variable, each pointing into `environ_buf`; entries are NUL separated. The host
     *      bounds-checks both guest ranges before writing.
     *      @param environ the guest address of the pointer array
     *      @param environ_buf the guest address of the string buffer
     *      @return 0 on success, or a WASI errno value
     *
     */
    environ_get(environ: number, environ_buf: number): number;

    /**
     * @description Implements the WASI `clock_res_get` syscall
     *
     *      Writes the resolution of the requested clock in nanoseconds. The realtime and monotonic
     *      clocks are supported; the process and thread CPU clocks fall back to the monotonic
     *      resolution on platforms without a separate source.
     *      @param clock_id the WASI clock identifier
     *      @param resolution the guest address that receives the resolution in nanoseconds
     *      @return 0 on success, or a WASI errno value
     *
     */
    clock_res_get(clock_id: number, resolution: number): number;

    /**
     * @description Implements the WASI `clock_time_get` syscall
     *
     *      Writes the current time of the requested clock in nanoseconds. The precision argument is
     *      accepted for compatibility and ignored; the realtime clock uses the system wall clock and the
     *      others use the monotonic high-resolution timer.
     *      @param clock_id the WASI clock identifier
     *      @param precision the requested precision in nanoseconds (ignored)
     *      @param time the guest address that receives the timestamp in nanoseconds
     *      @return 0 on success, or a WASI errno value
     *
     */
    clock_time_get(clock_id: number, precision: number, time: number): number;

    /**
     * @description Implements the WASI `random_get` syscall
     *
     *      Fills the guest buffer with random bytes from the host entropy source. The whole range is
     *      bounds-checked before any byte is written, so a bad pointer returns FAULT without touching
     *      memory.
     *      @param buf the guest address of the buffer
     *      @param buf_len the number of bytes to generate
     *      @return 0 on success, or a WASI errno value
     *
     */
    random_get(buf: number, buf_len: number): number;

    /**
     * @description Implements the WASI `sched_yield` syscall
     *
     *      Yields the calling fiber so that other fibers of the isolate can run. In fibjs this is a
     *      cooperative suspension of the current fiber, not an operating-system thread yield.
     *      @return 0 on success, or a WASI errno value
     *
     */
    sched_yield(): number;

    /**
     * @description Implements the WASI `proc_exit` syscall
     *
     *      Terminates the guest by unwinding `_start` with the given exit code. The host records the code
     *      and throws a JavaScript exception to leave the wasm frames; `start()` catches it and returns
     *      the recorded code instead of reporting a trap.
     *      @param code the guest exit code
     *
     */
    proc_exit(code: number): void;

    /**
     * @description Implements the WASI `proc_raise` syscall
     *
     *      Sends a signal to the host process, mirroring uvwasi: the WASI signal number is translated to
     *      the host signal and delivered with `uv_kill(getpid(), sig)`, so a guest `raise()` follows the
     *      host disposition (a fatal signal terminates the process, a blocked or ignored one does not).
     *      Signals the host has no mapping for return `52` (NOSYS). Unlike `proc_exit`, this syscall does
     *      not unwind the guest.
     *      @param sig the WASI signal number (`1` HUP ... `30` SYS)
     *      @return 0 on success, or a WASI errno value
     *
     */
    proc_raise(sig: number): number;

    /**
     * @description Implements the WASI `fd_close` syscall
     *
     *      Releases the descriptor. A file opened by `path_open` closes its host descriptor and is
     *      removed from the table; the standard descriptors and the preopened directories remain valid
     *      and closing them only removes the guest entry, matching the preview1 table semantics.
     *      @param fd the guest descriptor to close
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_close(fd: number): number;

    fd_close(fd: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_close` syscall
     *
     *      Releases the descriptor. A file opened by `path_open` closes its host descriptor and is
     *      removed from the table; the standard descriptors and the preopened directories remain valid
     *      and closing them only removes the guest entry, matching the preview1 table semantics.
     *      @param fd the guest descriptor to close
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_closeSync(fd: number): number;

    /**
     * @description Implements the WASI `fd_close` syscall
     *
     *      Releases the descriptor. A file opened by `path_open` closes its host descriptor and is
     *      removed from the table; the standard descriptors and the preopened directories remain valid
     *      and closing them only removes the guest entry, matching the preview1 table semantics.
     *      @param fd the guest descriptor to close
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_closeAsync(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_fdstat_get` syscall
     *
     *      Writes the 24-byte `fdstat` record: the file type, the descriptor flags and the base and
     *      inheriting rights. The record is built from the host `fstat` result plus the rights and flags
     *      recorded when the descriptor was opened.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `fdstat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_get(fd: number, buf: number): number;

    fd_fdstat_get(fd: number, buf: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_fdstat_get` syscall
     *
     *      Writes the 24-byte `fdstat` record: the file type, the descriptor flags and the base and
     *      inheriting rights. The record is built from the host `fstat` result plus the rights and flags
     *      recorded when the descriptor was opened.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `fdstat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_getSync(fd: number, buf: number): number;

    /**
     * @description Implements the WASI `fd_fdstat_get` syscall
     *
     *      Writes the 24-byte `fdstat` record: the file type, the descriptor flags and the base and
     *      inheriting rights. The record is built from the host `fstat` result plus the rights and flags
     *      recorded when the descriptor was opened.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `fdstat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_getAsync(fd: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for the descriptor: device, inode, file type, link
     *      count, size and the access, modification and change timestamps. The values come from the host
     *      `fstat` call on the underlying descriptor.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_get(fd: number, buf: number): number;

    fd_filestat_get(fd: number, buf: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for the descriptor: device, inode, file type, link
     *      count, size and the access, modification and change timestamps. The values come from the host
     *      `fstat` call on the underlying descriptor.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_getSync(fd: number, buf: number): number;

    /**
     * @description Implements the WASI `fd_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for the descriptor: device, inode, file type, link
     *      count, size and the access, modification and change timestamps. The values come from the host
     *      `fstat` call on the underlying descriptor.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_getAsync(fd: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_prestat_get` syscall
     *
     *      Writes the 8-byte prestat record for a preopened directory: the tag (always `dir`) and the
     *      length of the guest path returned by `fd_prestat_dir_name`. A missing descriptor returns BADF
     *      (`8`) and an existing descriptor that is not a preopen returns INVAL (`28`), matching uvwasi;
     *      `fd_prestat_dir_name` uses BADF for both cases.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the prestat record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_prestat_get(fd: number, buf: number): number;

    /**
     * @description Implements the WASI `fd_prestat_dir_name` syscall
     *
     *      Writes the guest name of a preopened directory. The name is not NUL-terminated and a buffer
     *      smaller than the name reports NOBUFS (42), matching uvwasi.
     *      @param fd the preopened descriptor
     *      @param path the guest address of the output buffer
     *      @param path_len the output buffer length
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_prestat_dir_name(fd: number, path: number, path_len: number): number;

    /**
     * @description Implements the WASI `path_open` syscall
     *
     *      Resolves the guest path below a directory descriptor and opens it with the requested flags
     *      and rights. The `oflags` bits map to create/exclusive/truncate/directory, `fdflags` maps to
     *      append and the sync modes, and the new descriptor is written to `opened_fd`. Access is
     *      confined to the preopened directory tree. Like uvwasi, the directory must hold PATH_OPEN plus
     *      the rights implied by the flags, and its inheriting rights must cover the requested base and
     *      inheriting masks (otherwise NOTCAPABLE); the granted rights are intersected with the maximum
     *      the opened file type supports.
     *      @param fd the directory descriptor
     *      @param dirflags the lookup flags (symlink follow)
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param oflags the open flags
     *      @param rights_base the requested base rights
     *      @param rights_inheriting the requested inheriting rights
     *      @param fdflags the descriptor flags
     *      @param opened_fd the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_open(fd: number, dirflags: number, path: number, path_len: number, oflags: number, rights_base: number, rights_inheriting: number, fdflags: number, opened_fd: number): number;

    path_open(fd: number, dirflags: number, path: number, path_len: number, oflags: number, rights_base: number, rights_inheriting: number, fdflags: number, opened_fd: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_open` syscall
     *
     *      Resolves the guest path below a directory descriptor and opens it with the requested flags
     *      and rights. The `oflags` bits map to create/exclusive/truncate/directory, `fdflags` maps to
     *      append and the sync modes, and the new descriptor is written to `opened_fd`. Access is
     *      confined to the preopened directory tree. Like uvwasi, the directory must hold PATH_OPEN plus
     *      the rights implied by the flags, and its inheriting rights must cover the requested base and
     *      inheriting masks (otherwise NOTCAPABLE); the granted rights are intersected with the maximum
     *      the opened file type supports.
     *      @param fd the directory descriptor
     *      @param dirflags the lookup flags (symlink follow)
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param oflags the open flags
     *      @param rights_base the requested base rights
     *      @param rights_inheriting the requested inheriting rights
     *      @param fdflags the descriptor flags
     *      @param opened_fd the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_openSync(fd: number, dirflags: number, path: number, path_len: number, oflags: number, rights_base: number, rights_inheriting: number, fdflags: number, opened_fd: number): number;

    /**
     * @description Implements the WASI `path_open` syscall
     *
     *      Resolves the guest path below a directory descriptor and opens it with the requested flags
     *      and rights. The `oflags` bits map to create/exclusive/truncate/directory, `fdflags` maps to
     *      append and the sync modes, and the new descriptor is written to `opened_fd`. Access is
     *      confined to the preopened directory tree. Like uvwasi, the directory must hold PATH_OPEN plus
     *      the rights implied by the flags, and its inheriting rights must cover the requested base and
     *      inheriting masks (otherwise NOTCAPABLE); the granted rights are intersected with the maximum
     *      the opened file type supports.
     *      @param fd the directory descriptor
     *      @param dirflags the lookup flags (symlink follow)
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param oflags the open flags
     *      @param rights_base the requested base rights
     *      @param rights_inheriting the requested inheriting rights
     *      @param fdflags the descriptor flags
     *      @param opened_fd the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_openAsync(fd: number, dirflags: number, path: number, path_len: number, oflags: number, rights_base: number, rights_inheriting: number, fdflags: number, opened_fd: number): Promise<number>;

    /**
     * @description Implements the WASI `path_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for a path below a directory descriptor. The lookup
     *      flags select whether a symbolic link is followed; without the follow bit the host `lstat`
     *      result describes the link itself.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_get(fd: number, flags: number, path: number, path_len: number, buf: number): number;

    path_filestat_get(fd: number, flags: number, path: number, path_len: number, buf: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for a path below a directory descriptor. The lookup
     *      flags select whether a symbolic link is followed; without the follow bit the host `lstat`
     *      result describes the link itself.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_getSync(fd: number, flags: number, path: number, path_len: number, buf: number): number;

    /**
     * @description Implements the WASI `path_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for a path below a directory descriptor. The lookup
     *      flags select whether a symbolic link is followed; without the follow bit the host `lstat`
     *      result describes the link itself.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_getAsync(fd: number, flags: number, path: number, path_len: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_seek` syscall
     *
     *      Moves the file position of a regular file. `whence` is 0 (set), 1 (current) or 2 (end); the
     *      end form stats the file first, the other two update the tracked position directly. The new
     *      position is written to `newoffset`.
     *      @param fd the guest descriptor
     *      @param offset the signed byte offset
     *      @param whence the seek origin
     *      @param newoffset the guest address that receives the new position
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_seek(fd: number, offset: number, whence: number, newoffset: number): number;

    fd_seek(fd: number, offset: number, whence: number, newoffset: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_seek` syscall
     *
     *      Moves the file position of a regular file. `whence` is 0 (set), 1 (current) or 2 (end); the
     *      end form stats the file first, the other two update the tracked position directly. The new
     *      position is written to `newoffset`.
     *      @param fd the guest descriptor
     *      @param offset the signed byte offset
     *      @param whence the seek origin
     *      @param newoffset the guest address that receives the new position
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_seekSync(fd: number, offset: number, whence: number, newoffset: number): number;

    /**
     * @description Implements the WASI `fd_seek` syscall
     *
     *      Moves the file position of a regular file. `whence` is 0 (set), 1 (current) or 2 (end); the
     *      end form stats the file first, the other two update the tracked position directly. The new
     *      position is written to `newoffset`.
     *      @param fd the guest descriptor
     *      @param offset the signed byte offset
     *      @param whence the seek origin
     *      @param newoffset the guest address that receives the new position
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_seekAsync(fd: number, offset: number, whence: number, newoffset: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_tell` syscall
     *
     *      Writes the current file position of a regular file. Standard streams and directories are not
     *      seekable and return SPIPE, matching the preview1 behavior on pipes and terminals.
     *      @param fd the guest descriptor
     *      @param offset the guest address that receives the position
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_tell(fd: number, offset: number): number;

    /**
     * @description Implements the WASI `fd_pread` syscall
     *
     *      Reads from the descriptor at the given absolute offset without changing the file position.
     *      The iovec list is validated and copied exactly like `fd_read`; the byte count is written to
     *      `nread`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_pread(fd: number, iovs: number, iovs_len: number, offset: number, nread: number): number;

    fd_pread(fd: number, iovs: number, iovs_len: number, offset: number, nread: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_pread` syscall
     *
     *      Reads from the descriptor at the given absolute offset without changing the file position.
     *      The iovec list is validated and copied exactly like `fd_read`; the byte count is written to
     *      `nread`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_preadSync(fd: number, iovs: number, iovs_len: number, offset: number, nread: number): number;

    /**
     * @description Implements the WASI `fd_pread` syscall
     *
     *      Reads from the descriptor at the given absolute offset without changing the file position.
     *      The iovec list is validated and copied exactly like `fd_read`; the byte count is written to
     *      `nread`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_preadAsync(fd: number, iovs: number, iovs_len: number, offset: number, nread: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_pwrite` syscall
     *
     *      Writes to the descriptor at the given absolute offset without changing the file position.
     *      The iovec payload is gathered into a staging buffer first, then written on the uv worker; the
     *      byte count is written to `nwritten`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_pwrite(fd: number, iovs: number, iovs_len: number, offset: number, nwritten: number): number;

    fd_pwrite(fd: number, iovs: number, iovs_len: number, offset: number, nwritten: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_pwrite` syscall
     *
     *      Writes to the descriptor at the given absolute offset without changing the file position.
     *      The iovec payload is gathered into a staging buffer first, then written on the uv worker; the
     *      byte count is written to `nwritten`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_pwriteSync(fd: number, iovs: number, iovs_len: number, offset: number, nwritten: number): number;

    /**
     * @description Implements the WASI `fd_pwrite` syscall
     *
     *      Writes to the descriptor at the given absolute offset without changing the file position.
     *      The iovec payload is gathered into a staging buffer first, then written on the uv worker; the
     *      byte count is written to `nwritten`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_pwriteAsync(fd: number, iovs: number, iovs_len: number, offset: number, nwritten: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_readdir` syscall
     *
     *      Writes directory entries as 24-byte records (`d_next`, `d_ino`, `d_namlen`, `d_type`)
     *      followed by the name bytes. `cookie` selects the starting entry and `bufused` receives the
     *      number of bytes written; the caller repeats the call with the last `d_next` while the buffer
     *      was full.
     *      @param fd the directory descriptor
     *      @param buf the guest address of the output buffer
     *      @param buf_len the buffer length in bytes
     *      @param cookie the directory cookie to start from
     *      @param bufused the guest address that receives the bytes written
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readdir(fd: number, buf: number, buf_len: number, cookie: number, bufused: number): number;

    fd_readdir(fd: number, buf: number, buf_len: number, cookie: number, bufused: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_readdir` syscall
     *
     *      Writes directory entries as 24-byte records (`d_next`, `d_ino`, `d_namlen`, `d_type`)
     *      followed by the name bytes. `cookie` selects the starting entry and `bufused` receives the
     *      number of bytes written; the caller repeats the call with the last `d_next` while the buffer
     *      was full.
     *      @param fd the directory descriptor
     *      @param buf the guest address of the output buffer
     *      @param buf_len the buffer length in bytes
     *      @param cookie the directory cookie to start from
     *      @param bufused the guest address that receives the bytes written
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readdirSync(fd: number, buf: number, buf_len: number, cookie: number, bufused: number): number;

    /**
     * @description Implements the WASI `fd_readdir` syscall
     *
     *      Writes directory entries as 24-byte records (`d_next`, `d_ino`, `d_namlen`, `d_type`)
     *      followed by the name bytes. `cookie` selects the starting entry and `bufused` receives the
     *      number of bytes written; the caller repeats the call with the last `d_next` while the buffer
     *      was full.
     *      @param fd the directory descriptor
     *      @param buf the guest address of the output buffer
     *      @param buf_len the buffer length in bytes
     *      @param cookie the directory cookie to start from
     *      @param bufused the guest address that receives the bytes written
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readdirAsync(fd: number, buf: number, buf_len: number, cookie: number, bufused: number): Promise<number>;

    /**
     * @description Implements the WASI `path_create_directory` syscall
     *
     *      Creates a directory below the descriptor. The path is resolved inside the preopened tree and
     *      the host `mkdir` is performed on the uv worker.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_create_directory(fd: number, path: number, path_len: number): number;

    path_create_directory(fd: number, path: number, path_len: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_create_directory` syscall
     *
     *      Creates a directory below the descriptor. The path is resolved inside the preopened tree and
     *      the host `mkdir` is performed on the uv worker.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_create_directorySync(fd: number, path: number, path_len: number): number;

    /**
     * @description Implements the WASI `path_create_directory` syscall
     *
     *      Creates a directory below the descriptor. The path is resolved inside the preopened tree and
     *      the host `mkdir` is performed on the uv worker.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_create_directoryAsync(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_remove_directory` syscall
     *
     *      Removes an empty directory below the descriptor. A non-empty directory returns NOTEMPTY and
     *      a path that leaves the preopen returns NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_remove_directory(fd: number, path: number, path_len: number): number;

    path_remove_directory(fd: number, path: number, path_len: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_remove_directory` syscall
     *
     *      Removes an empty directory below the descriptor. A non-empty directory returns NOTEMPTY and
     *      a path that leaves the preopen returns NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_remove_directorySync(fd: number, path: number, path_len: number): number;

    /**
     * @description Implements the WASI `path_remove_directory` syscall
     *
     *      Removes an empty directory below the descriptor. A non-empty directory returns NOTEMPTY and
     *      a path that leaves the preopen returns NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_remove_directoryAsync(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_unlink_file` syscall
     *
     *      Removes a file below the descriptor. A directory returns ISDIR; symbolic links are removed
     *      themselves, not their targets.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_unlink_file(fd: number, path: number, path_len: number): number;

    path_unlink_file(fd: number, path: number, path_len: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_unlink_file` syscall
     *
     *      Removes a file below the descriptor. A directory returns ISDIR; symbolic links are removed
     *      themselves, not their targets.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_unlink_fileSync(fd: number, path: number, path_len: number): number;

    /**
     * @description Implements the WASI `path_unlink_file` syscall
     *
     *      Removes a file below the descriptor. A directory returns ISDIR; symbolic links are removed
     *      themselves, not their targets.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_unlink_fileAsync(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_rename` syscall
     *
     *      Renames a path inside one preopened tree to a path inside another. Both paths are resolved
     *      before the host rename runs; the call does not allow either side to escape its descriptor.
     *      @param fd the source directory descriptor
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_rename(fd: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): number;

    path_rename(fd: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_rename` syscall
     *
     *      Renames a path inside one preopened tree to a path inside another. Both paths are resolved
     *      before the host rename runs; the call does not allow either side to escape its descriptor.
     *      @param fd the source directory descriptor
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_renameSync(fd: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): number;

    /**
     * @description Implements the WASI `path_rename` syscall
     *
     *      Renames a path inside one preopened tree to a path inside another. Both paths are resolved
     *      before the host rename runs; the call does not allow either side to escape its descriptor.
     *      @param fd the source directory descriptor
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_renameAsync(fd: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_link` syscall
     *
     *      Creates a hard link between two paths. The source lookup flags are accepted for
     *      compatibility; both paths are resolved inside their preopened trees before the host link
     *      runs.
     *      @param old_fd the source directory descriptor
     *      @param old_flags the lookup flags
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_link(old_fd: number, old_flags: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): number;

    path_link(old_fd: number, old_flags: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_link` syscall
     *
     *      Creates a hard link between two paths. The source lookup flags are accepted for
     *      compatibility; both paths are resolved inside their preopened trees before the host link
     *      runs.
     *      @param old_fd the source directory descriptor
     *      @param old_flags the lookup flags
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_linkSync(old_fd: number, old_flags: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): number;

    /**
     * @description Implements the WASI `path_link` syscall
     *
     *      Creates a hard link between two paths. The source lookup flags are accepted for
     *      compatibility; both paths are resolved inside their preopened trees before the host link
     *      runs.
     *      @param old_fd the source directory descriptor
     *      @param old_flags the lookup flags
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_linkAsync(old_fd: number, old_flags: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_readlink` syscall
     *
     *      Reads a symbolic link below a directory descriptor into the guest buffer. When the target does
     *      not fit, the call reports NOBUFS (42) instead of truncating, matching uvwasi; on success the
     *      target is NUL-terminated and `bufused` excludes the terminator. A descriptor without
     *      PATH_READLINK reports NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the link path
     *      @param path_len the path length in bytes
     *      @param buf the guest address of the output buffer
     *      @param buf_len the output buffer length
     *      @param bufused the guest address that receives the target length
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_readlink(fd: number, path: number, path_len: number, buf: number, buf_len: number, bufused: number): number;

    path_readlink(fd: number, path: number, path_len: number, buf: number, buf_len: number, bufused: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_readlink` syscall
     *
     *      Reads a symbolic link below a directory descriptor into the guest buffer. When the target does
     *      not fit, the call reports NOBUFS (42) instead of truncating, matching uvwasi; on success the
     *      target is NUL-terminated and `bufused` excludes the terminator. A descriptor without
     *      PATH_READLINK reports NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the link path
     *      @param path_len the path length in bytes
     *      @param buf the guest address of the output buffer
     *      @param buf_len the output buffer length
     *      @param bufused the guest address that receives the target length
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_readlinkSync(fd: number, path: number, path_len: number, buf: number, buf_len: number, bufused: number): number;

    /**
     * @description Implements the WASI `path_readlink` syscall
     *
     *      Reads a symbolic link below a directory descriptor into the guest buffer. When the target does
     *      not fit, the call reports NOBUFS (42) instead of truncating, matching uvwasi; on success the
     *      target is NUL-terminated and `bufused` excludes the terminator. A descriptor without
     *      PATH_READLINK reports NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the link path
     *      @param path_len the path length in bytes
     *      @param buf the guest address of the output buffer
     *      @param buf_len the output buffer length
     *      @param bufused the guest address that receives the target length
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_readlinkAsync(fd: number, path: number, path_len: number, buf: number, buf_len: number, bufused: number): Promise<number>;

    /**
     * @description Implements the WASI `path_symlink` syscall
     *
     *      Creates a symbolic link. The link target is the raw `old_path` string and is not resolved;
     *      the new link path is resolved inside the preopened tree of `fd`.
     *      @param old_path the guest address of the link target bytes
     *      @param old_path_len the link target length in bytes
     *      @param fd the directory descriptor for the new link
     *      @param new_path the guest address of the new link path bytes
     *      @param new_path_len the new link path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_symlink(old_path: number, old_path_len: number, fd: number, new_path: number, new_path_len: number): number;

    path_symlink(old_path: number, old_path_len: number, fd: number, new_path: number, new_path_len: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_symlink` syscall
     *
     *      Creates a symbolic link. The link target is the raw `old_path` string and is not resolved;
     *      the new link path is resolved inside the preopened tree of `fd`.
     *      @param old_path the guest address of the link target bytes
     *      @param old_path_len the link target length in bytes
     *      @param fd the directory descriptor for the new link
     *      @param new_path the guest address of the new link path bytes
     *      @param new_path_len the new link path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_symlinkSync(old_path: number, old_path_len: number, fd: number, new_path: number, new_path_len: number): number;

    /**
     * @description Implements the WASI `path_symlink` syscall
     *
     *      Creates a symbolic link. The link target is the raw `old_path` string and is not resolved;
     *      the new link path is resolved inside the preopened tree of `fd`.
     *      @param old_path the guest address of the link target bytes
     *      @param old_path_len the link target length in bytes
     *      @param fd the directory descriptor for the new link
     *      @param new_path the guest address of the new link path bytes
     *      @param new_path_len the new link path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_symlinkAsync(old_path: number, old_path_len: number, fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_filestat_set_size` syscall
     *
     *      Truncates or extends a regular file to the given size. The host `ftruncate` runs on the uv
     *      worker and the descriptor rights are not enforced in this milestone.
     *      @param fd the guest descriptor
     *      @param size the new file size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_size(fd: number, size: number): number;

    fd_filestat_set_size(fd: number, size: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_filestat_set_size` syscall
     *
     *      Truncates or extends a regular file to the given size. The host `ftruncate` runs on the uv
     *      worker and the descriptor rights are not enforced in this milestone.
     *      @param fd the guest descriptor
     *      @param size the new file size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_sizeSync(fd: number, size: number): number;

    /**
     * @description Implements the WASI `fd_filestat_set_size` syscall
     *
     *      Truncates or extends a regular file to the given size. The host `ftruncate` runs on the uv
     *      worker and the descriptor rights are not enforced in this milestone.
     *      @param fd the guest descriptor
     *      @param size the new file size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_sizeAsync(fd: number, size: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_sync` syscall
     *
     *      Flushes the file data and metadata to the storage device. Standard streams and preopened
     *      directories have no host descriptor to flush and return success.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_sync(fd: number): number;

    fd_sync(fd: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_sync` syscall
     *
     *      Flushes the file data and metadata to the storage device. Standard streams and preopened
     *      directories have no host descriptor to flush and return success.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_syncSync(fd: number): number;

    /**
     * @description Implements the WASI `fd_sync` syscall
     *
     *      Flushes the file data and metadata to the storage device. Standard streams and preopened
     *      directories have no host descriptor to flush and return success.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_syncAsync(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_datasync` syscall
     *
     *      Flushes the file data to the storage device without forcing a metadata update. The call is
     *      the data-only counterpart of `fd_sync` and uses the host `fdatasync`.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_datasync(fd: number): number;

    fd_datasync(fd: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_datasync` syscall
     *
     *      Flushes the file data to the storage device without forcing a metadata update. The call is
     *      the data-only counterpart of `fd_sync` and uses the host `fdatasync`.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_datasyncSync(fd: number): number;

    /**
     * @description Implements the WASI `fd_datasync` syscall
     *
     *      Flushes the file data to the storage device without forcing a metadata update. The call is
     *      the data-only counterpart of `fd_sync` and uses the host `fdatasync`.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_datasyncAsync(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_advise` syscall
     *
     *      Supplies a usage hint (`normal`, `sequential`, `random`, `willneed`, `dontneed`,
     *      `noreuse`) for a range of a file. The advice value is validated before the rights check and
     *      the call is a no-op otherwise; a directory reports BADF. A descriptor without FD_ADVISE
     *      reports NOTCAPABLE.
     *      @param fd the guest descriptor
     *      @param offset the range offset
     *      @param len the range length
     *      @param advice the advice value
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_advise(fd: number, offset: number, len: number, advice: number): number;

    /**
     * @description Implements the WASI `fd_allocate` syscall
     *
     *      Grows the file to `offset + len` when it is shorter. macOS has no `posix_fallocate`, so the
     *      host uses the uvwasi fallback (`fstat` + `ftruncate`); the file is not physically reserved.
     *      Directories report ISDIR, a descriptor without FD_ALLOCATE reports NOTCAPABLE.
     *      @param fd the descriptor to allocate for
     *      @param offset the start of the range
     *      @param len the number of bytes to add
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_allocate(fd: number, offset: number, len: number): number;

    /**
     * @description Implements the WASI `fd_fdstat_set_flags` syscall
     *
     *      Stores the descriptor flags and, on POSIX, applies them to the host descriptor with `fcntl`
     *      (append, sync modes, non-blocking) like uvwasi. A descriptor without FD_FDSTAT_SET_FLAGS
     *      reports NOTCAPABLE.
     *      @param fd the descriptor to update
     *      @param flags the new descriptor flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_set_flags(fd: number, flags: number): number;

    /**
     * @description Implements the WASI `fd_fdstat_set_rights` syscall
     *
     *      Reduces the rights of a descriptor. WASI only allows dropping rights: a requested right that
     *      the descriptor does not already have returns NOTCAPABLE.
     *      @param fd the guest descriptor
     *      @param rights_base the new base rights
     *      @param rights_inheriting the new inheriting rights
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_set_rights(fd: number, rights_base: number, rights_inheriting: number): number;

    /**
     * @description Implements the WASI `fd_filestat_set_times` syscall
     *
     *      Updates the access and modification times. `fst_flags` accepts SET_ATIM/SET_MTIM and their
     *      NOW variants; conflicting or unknown bits report INVAL before the rights check, like uvwasi.
     *      Omitted times keep the current value, read with `fstat`.
     *      @param fd the descriptor to update
     *      @param atim the access time in nanoseconds (when SET_ATIM is set)
     *      @param mtim the modification time in nanoseconds (when SET_MTIM is set)
     *      @param flags the fstflags mask
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_times(fd: number, atim: number, mtim: number, flags: number): number;

    fd_filestat_set_times(fd: number, atim: number, mtim: number, flags: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_filestat_set_times` syscall
     *
     *      Updates the access and modification times. `fst_flags` accepts SET_ATIM/SET_MTIM and their
     *      NOW variants; conflicting or unknown bits report INVAL before the rights check, like uvwasi.
     *      Omitted times keep the current value, read with `fstat`.
     *      @param fd the descriptor to update
     *      @param atim the access time in nanoseconds (when SET_ATIM is set)
     *      @param mtim the modification time in nanoseconds (when SET_MTIM is set)
     *      @param flags the fstflags mask
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_timesSync(fd: number, atim: number, mtim: number, flags: number): number;

    /**
     * @description Implements the WASI `fd_filestat_set_times` syscall
     *
     *      Updates the access and modification times. `fst_flags` accepts SET_ATIM/SET_MTIM and their
     *      NOW variants; conflicting or unknown bits report INVAL before the rights check, like uvwasi.
     *      Omitted times keep the current value, read with `fstat`.
     *      @param fd the descriptor to update
     *      @param atim the access time in nanoseconds (when SET_ATIM is set)
     *      @param mtim the modification time in nanoseconds (when SET_MTIM is set)
     *      @param flags the fstflags mask
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_timesAsync(fd: number, atim: number, mtim: number, flags: number): Promise<number>;

    /**
     * @description Implements the WASI `path_filestat_set_times` syscall
     *
     *      Updates the access and modification times of a path below a directory descriptor. The
     *      lookup flags select whether a symbolic link is followed; the timestamp flags have the same
     *      meaning as in `fd_filestat_set_times`.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param atim the access time in nanoseconds
     *      @param mtim the modification time in nanoseconds
     *      @param fst_flags the timestamp update flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_set_times(fd: number, flags: number, path: number, path_len: number, atim: number, mtim: number, fst_flags: number): number;

    path_filestat_set_times(fd: number, flags: number, path: number, path_len: number, atim: number, mtim: number, fst_flags: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `path_filestat_set_times` syscall
     *
     *      Updates the access and modification times of a path below a directory descriptor. The
     *      lookup flags select whether a symbolic link is followed; the timestamp flags have the same
     *      meaning as in `fd_filestat_set_times`.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param atim the access time in nanoseconds
     *      @param mtim the modification time in nanoseconds
     *      @param fst_flags the timestamp update flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_set_timesSync(fd: number, flags: number, path: number, path_len: number, atim: number, mtim: number, fst_flags: number): number;

    /**
     * @description Implements the WASI `path_filestat_set_times` syscall
     *
     *      Updates the access and modification times of a path below a directory descriptor. The
     *      lookup flags select whether a symbolic link is followed; the timestamp flags have the same
     *      meaning as in `fd_filestat_set_times`.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param atim the access time in nanoseconds
     *      @param mtim the modification time in nanoseconds
     *      @param fst_flags the timestamp update flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_set_timesAsync(fd: number, flags: number, path: number, path_len: number, atim: number, mtim: number, fst_flags: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_renumber` syscall
     *
     *      Moves the source descriptor to the target number, closing the previous target when it exists.
     *      Unlike uvwasi, the target may be free; the preview1 specification allows renumbering onto an
     *      unopened descriptor and wasi-libc's `dup2` relies on it.
     *      @param from the descriptor to move
     *      @param to the new descriptor number
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_renumber(from: number, to: number): number;

    fd_renumber(from: number, to: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `fd_renumber` syscall
     *
     *      Moves the source descriptor to the target number, closing the previous target when it exists.
     *      Unlike uvwasi, the target may be free; the preview1 specification allows renumbering onto an
     *      unopened descriptor and wasi-libc's `dup2` relies on it.
     *      @param from the descriptor to move
     *      @param to the new descriptor number
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_renumberSync(from: number, to: number): number;

    /**
     * @description Implements the WASI `fd_renumber` syscall
     *
     *      Moves the source descriptor to the target number, closing the previous target when it exists.
     *      Unlike uvwasi, the target may be free; the preview1 specification allows renumbering onto an
     *      unopened descriptor and wasi-libc's `dup2` relies on it.
     *      @param from the descriptor to move
     *      @param to the new descriptor number
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_renumberAsync(from: number, to: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_accept` syscall
     *
     *      Accepts a pending connection on a preopened listener. The listener is created from the
     *      `sockets` constructor option (each entry `{ address, port }`), and the new connection is
     *      inserted as a socket descriptor with the socket rights. The call suspends the fiber until a
     *      connection arrives; with FDFLAG_NONBLOCK it returns EAGAIN instead. A non-listener descriptor
     *      reports NOTSOCK and a descriptor without the socket rights reports NOTCAPABLE.
     *      @param fd the listener descriptor
     *      @param flags the accept flags (only NONBLOCK is supported)
     *      @param fd_out the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_accept(fd: number, flags: number, fd_out: number): number;

    sock_accept(fd: number, flags: number, fd_out: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `sock_accept` syscall
     *
     *      Accepts a pending connection on a preopened listener. The listener is created from the
     *      `sockets` constructor option (each entry `{ address, port }`), and the new connection is
     *      inserted as a socket descriptor with the socket rights. The call suspends the fiber until a
     *      connection arrives; with FDFLAG_NONBLOCK it returns EAGAIN instead. A non-listener descriptor
     *      reports NOTSOCK and a descriptor without the socket rights reports NOTCAPABLE.
     *      @param fd the listener descriptor
     *      @param flags the accept flags (only NONBLOCK is supported)
     *      @param fd_out the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_acceptSync(fd: number, flags: number, fd_out: number): number;

    /**
     * @description Implements the WASI `sock_accept` syscall
     *
     *      Accepts a pending connection on a preopened listener. The listener is created from the
     *      `sockets` constructor option (each entry `{ address, port }`), and the new connection is
     *      inserted as a socket descriptor with the socket rights. The call suspends the fiber until a
     *      connection arrives; with FDFLAG_NONBLOCK it returns EAGAIN instead. A non-listener descriptor
     *      reports NOTSOCK and a descriptor without the socket rights reports NOTCAPABLE.
     *      @param fd the listener descriptor
     *      @param flags the accept flags (only NONBLOCK is supported)
     *      @param fd_out the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_acceptAsync(fd: number, flags: number, fd_out: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_recv` syscall
     *
     *      Reads stream bytes into the first iovec of the guest array, like uvwasi. Non-zero `ri_flags`
     *      report NOTSUP. The fiber suspends until data arrives; a zero-length read reports EAGAIN.
     *      `ro_datalen` receives the byte count and `ro_flags` is written as 0.
     *      @param fd the socket descriptor
     *      @param ri_data the guest address of the iovec array
     *      @param ri_data_len the number of iovec records (only the first is used)
     *      @param ri_flags the receive flags
     *      @param ro_datalen the guest address that receives the byte count
     *      @param ro_flags the guest address that receives the receive flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_recv(fd: number, ri_data: number, ri_data_len: number, ri_flags: number, ro_datalen: number, ro_flags: number): number;

    sock_recv(fd: number, ri_data: number, ri_data_len: number, ri_flags: number, ro_datalen: number, ro_flags: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `sock_recv` syscall
     *
     *      Reads stream bytes into the first iovec of the guest array, like uvwasi. Non-zero `ri_flags`
     *      report NOTSUP. The fiber suspends until data arrives; a zero-length read reports EAGAIN.
     *      `ro_datalen` receives the byte count and `ro_flags` is written as 0.
     *      @param fd the socket descriptor
     *      @param ri_data the guest address of the iovec array
     *      @param ri_data_len the number of iovec records (only the first is used)
     *      @param ri_flags the receive flags
     *      @param ro_datalen the guest address that receives the byte count
     *      @param ro_flags the guest address that receives the receive flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_recvSync(fd: number, ri_data: number, ri_data_len: number, ri_flags: number, ro_datalen: number, ro_flags: number): number;

    /**
     * @description Implements the WASI `sock_recv` syscall
     *
     *      Reads stream bytes into the first iovec of the guest array, like uvwasi. Non-zero `ri_flags`
     *      report NOTSUP. The fiber suspends until data arrives; a zero-length read reports EAGAIN.
     *      `ro_datalen` receives the byte count and `ro_flags` is written as 0.
     *      @param fd the socket descriptor
     *      @param ri_data the guest address of the iovec array
     *      @param ri_data_len the number of iovec records (only the first is used)
     *      @param ri_flags the receive flags
     *      @param ro_datalen the guest address that receives the byte count
     *      @param ro_flags the guest address that receives the receive flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_recvAsync(fd: number, ri_data: number, ri_data_len: number, ri_flags: number, ro_datalen: number, ro_flags: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_send` syscall
     *
     *      Sends the guest iovecs through the socket. Like uvwasi, the write is non-blocking
     *      (`uv_try_write`) and may be partial; `so_datalen` receives the number of bytes queued.
     *      Non-zero `si_flags` report INVAL.
     *      @param fd the socket descriptor
     *      @param si_data the guest address of the iovec array
     *      @param si_data_len the number of iovec records
     *      @param si_flags the send flags
     *      @param so_datalen the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_send(fd: number, si_data: number, si_data_len: number, si_flags: number, so_datalen: number): number;

    sock_send(fd: number, si_data: number, si_data_len: number, si_flags: number, so_datalen: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `sock_send` syscall
     *
     *      Sends the guest iovecs through the socket. Like uvwasi, the write is non-blocking
     *      (`uv_try_write`) and may be partial; `so_datalen` receives the number of bytes queued.
     *      Non-zero `si_flags` report INVAL.
     *      @param fd the socket descriptor
     *      @param si_data the guest address of the iovec array
     *      @param si_data_len the number of iovec records
     *      @param si_flags the send flags
     *      @param so_datalen the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_sendSync(fd: number, si_data: number, si_data_len: number, si_flags: number, so_datalen: number): number;

    /**
     * @description Implements the WASI `sock_send` syscall
     *
     *      Sends the guest iovecs through the socket. Like uvwasi, the write is non-blocking
     *      (`uv_try_write`) and may be partial; `so_datalen` receives the number of bytes queued.
     *      Non-zero `si_flags` report INVAL.
     *      @param fd the socket descriptor
     *      @param si_data the guest address of the iovec array
     *      @param si_data_len the number of iovec records
     *      @param si_flags the send flags
     *      @param so_datalen the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_sendAsync(fd: number, si_data: number, si_data_len: number, si_flags: number, so_datalen: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_shutdown` syscall
     *
     *      Shuts down the write direction of a socket and suspends the fiber until libuv completes the
     *      shutdown. `how == 0` is a successful no-op; other unsupported bits report NOTSUP.
     *      @param fd the socket descriptor
     *      @param how the shutdown flags (0 or SHUT_WR)
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_shutdown(fd: number, how: number): number;

    sock_shutdown(fd: number, how: number, callback: (err: Error | undefined | null, retVal: number)=>any): void;

    /**
     * @description Implements the WASI `sock_shutdown` syscall
     *
     *      Shuts down the write direction of a socket and suspends the fiber until libuv completes the
     *      shutdown. `how == 0` is a successful no-op; other unsupported bits report NOTSUP.
     *      @param fd the socket descriptor
     *      @param how the shutdown flags (0 or SHUT_WR)
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_shutdownSync(fd: number, how: number): number;

    /**
     * @description Implements the WASI `sock_shutdown` syscall
     *
     *      Shuts down the write direction of a socket and suspends the fiber until libuv completes the
     *      shutdown. `how == 0` is a successful no-op; other unsupported bits report NOTSUP.
     *      @param fd the socket descriptor
     *      @param how the shutdown flags (0 or SHUT_WR)
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_shutdownAsync(fd: number, how: number): Promise<number>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * The promise variant of the WASIHost class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_WASIHostPromise extends Class_object {
    /**
     * @description Creates a WASI preview1 host from an options object
     *
     *      `version` is required and selects the import namespace: `'preview1'` (`wasi_snapshot_preview1`)
     *      or `'unstable'` (the legacy `wasi_unstable`), matching Node. `args` is the guest argument
     *      vector (without the program name), `env` is the guest environment, `preopens` maps guest paths
     *      to host paths, `sockets` lists the TCP listeners to expose as preopened socket descriptors and
     *      `stdin`/`stdout`/`stderr` select the host descriptors behind guest 0/1/2. Every preopen must
     *      resolve to an existing directory; the host paths are canonicalised at construction. Wrong
     *      option types throw `TypeError`, an unsupported version throws `ERR_INVALID_ARG_VALUE`, a
     *      missing or non-directory preopen throws `ERR_WASI_PREOPEN`, and a listener that cannot bind
     *      throws `ERR_WASI_SOCKET`.
     *      @param options the version, argument vector, environment, preopened directories, sockets and stdio descriptors
     *
     */
    constructor(options: FIBJS.GeneralObject);

    /**
     * @description Returns the import object to pass to WebAssembly.instantiate
     *
     *      The result has one key, `wasi_snapshot_preview1` (or `wasi_unstable` for the legacy version),
     *      whose value is this host instance. Its syscall methods are bound to the instance in the
     *      constructor, so wasm can call them as plain functions; the same instance is exposed as the
     *      `wasiImport` property (`wasiImport === wasi`).
     *      @return the import object keyed by the ABI name
     *
     */
    getImportObject(): FIBJS.GeneralObject;

    /**
     * @description Binds a command-style instance and runs its `_start` export
     *
     *      The instance must export `memory` and `_start`, and must not export `_initialize` (use
     *      `initialize()` for reactors). The call runs synchronously in the current fiber; syscalls that
     *      need the host may suspend and resume the fiber without blocking other fibers. The return value
     *      is the guest exit code, 0 when `_start` returns normally. Binding twice throws
     *      `ERR_WASI_ALREADY_STARTED`.
     *      @param instance the instance to bind: a `WebAssembly.Instance`, or the exports returned by
     *      `require()` of a `.wasm` file
     *      @return the guest exit code
     *
     */
    start(instance: any): number;

    /**
     * @description Binds a reactor-style instance and runs its `_initialize` export
     *
     *      The instance must export `memory` and must not export `_start`; `_initialize` is optional in
     *      the reactor model and is called when present. The instance stays alive after the call, so its
     *      exported functions can call the syscalls later through the import object. Binding twice throws
     *      `ERR_WASI_ALREADY_STARTED`.
     *      @param instance the instance to bind: a `WebAssembly.Instance`, or the exports returned by
     *      `require()` of a `.wasm` file
     *
     */
    initialize(instance: any): void;

    /**
     * @description Binds an existing instance for compatibility with embedders
     *
     *      This is the hook Node.js uses when the import object is linked after instantiation. It
     *      captures `options.memory` when supplied, otherwise `instance.exports.memory` (which must be a
     *      `WebAssembly.Memory`), and does not call any export. A host can be bound only once; later calls
     *      throw `ERR_WASI_ALREADY_STARTED`.
     *      @param instance the instance to bind: a `WebAssembly.Instance`, or the exports returned by
     *      `require()` of a `.wasm` file
     *      @param options optional `{ memory }` override
     *
     */
    finalizeBindings(instance: any, options?: FIBJS.GeneralObject): void;

    /**
     * @description Registers this host as the import namespace for `.wasm` modules
     *
     *      `require('./app.wasm')` compiles a wasm binary and instantiates it with the
     *      namespaces its imports ask for. Calling `registerAsImport()` stores this host
     *      in the isolate-wide registry under the host ABI namespace
     *      (`wasi_snapshot_preview1`, or `wasi_unstable` for the legacy version), so a
     *      command module can be loaded without any extra glue:
     *
     *      ```JavaScript
     *      const { WASI } = require('wasi');
     *      const wasi = new WASI({ version: 'preview1', args: ['app'] });
     *      wasi.registerAsImport();
     *      const app = require('./app.wasm');       // instance exports
     *      wasi.start(app);                         // binds the instance behind them
     *      ```
     *
     *      Registration is per isolate (workers have their own registry) and lasts
     *      until `unregisterAsImport()` or the end of the isolate. A registered
     *      namespace takes precedence over module resolution; namespaces that are not
     *      registered are resolved with `require(name)` from the directory of the wasm
     *      file, so `import_module("./helper.js")` works too.
     *
     */
    registerAsImport(): void;

    /**
     * @description Removes a namespace registered by `registerAsImport()`
     *
     *      The registry entry is only removed when it still holds this host, so
     *      unregistering an old host cannot drop a newer registration.
     *
     */
    unregisterAsImport(): void;

    /**
     * @description The import object, the host instance itself
     *
     *      Read-only alias of `getImportObject()['wasi_snapshot_preview1']`. The value is the host
     *      instance: the syscall methods are bound own properties, so calling `wasi.fd_read` works even
     *      though wasm calls them without a receiver. `wasiImport === wasi` and `instanceof` hold;
     *      Node.js uses a separate wrapper here, which is the only shape difference.
     *
     */
    readonly wasiImport: FIBJS.GeneralObject;

    /**
     * @description Implements the WASI `fd_read` syscall
     *
     *      Reads from the descriptor into the guest iovec list, which is an array of 8-byte records
     *      (`buf`, `buf_len` as little-endian u32). The host copies a contiguous range, performs the read
     *      on the uv worker and writes the number of bytes read to the `nread` guest address. The
     *      descriptor is validated against the open table and a bad descriptor returns BADF instead of
     *      throwing.
     *      @param fd the guest descriptor to read from
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_read(fd: number, iovs: number, iovs_len: number, nread: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_read` syscall
     *
     *      Reads from the descriptor into the guest iovec list, which is an array of 8-byte records
     *      (`buf`, `buf_len` as little-endian u32). The host copies a contiguous range, performs the read
     *      on the uv worker and writes the number of bytes read to the `nread` guest address. The
     *      descriptor is validated against the open table and a bad descriptor returns BADF instead of
     *      throwing.
     *      @param fd the guest descriptor to read from
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readSync(fd: number, iovs: number, iovs_len: number, nread: number): number;

    /**
     * @description Implements the WASI `fd_read` syscall
     *
     *      Reads from the descriptor into the guest iovec list, which is an array of 8-byte records
     *      (`buf`, `buf_len` as little-endian u32). The host copies a contiguous range, performs the read
     *      on the uv worker and writes the number of bytes read to the `nread` guest address. The
     *      descriptor is validated against the open table and a bad descriptor returns BADF instead of
     *      throwing.
     *      @param fd the guest descriptor to read from
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readAsync(fd: number, iovs: number, iovs_len: number, nread: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_write` syscall
     *
     *      Gathers the bytes described by the guest iovec list and writes them to the descriptor. The
     *      iovec records are `{ buf, buf_len }` pairs of little-endian u32 values; the host bounds-checks
     *      every record against the instance memory, copies the payload to a staging buffer, writes it on
     *      the uv worker and stores the byte count at `nwritten`. A descriptor that is not open returns
     *      BADF.
     *      @param fd the guest descriptor to write to
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_write(fd: number, iovs: number, iovs_len: number, nwritten: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_write` syscall
     *
     *      Gathers the bytes described by the guest iovec list and writes them to the descriptor. The
     *      iovec records are `{ buf, buf_len }` pairs of little-endian u32 values; the host bounds-checks
     *      every record against the instance memory, copies the payload to a staging buffer, writes it on
     *      the uv worker and stores the byte count at `nwritten`. A descriptor that is not open returns
     *      BADF.
     *      @param fd the guest descriptor to write to
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_writeSync(fd: number, iovs: number, iovs_len: number, nwritten: number): number;

    /**
     * @description Implements the WASI `fd_write` syscall
     *
     *      Gathers the bytes described by the guest iovec list and writes them to the descriptor. The
     *      iovec records are `{ buf, buf_len }` pairs of little-endian u32 values; the host bounds-checks
     *      every record against the instance memory, copies the payload to a staging buffer, writes it on
     *      the uv worker and stores the byte count at `nwritten`. A descriptor that is not open returns
     *      BADF.
     *      @param fd the guest descriptor to write to
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_writeAsync(fd: number, iovs: number, iovs_len: number, nwritten: number): Promise<number>;

    /**
     * @description Implements the WASI `poll_oneoff` syscall
     *
     *      Parses the 48-byte subscriptions. Clock subscriptions are served by a uv timer; descriptor
     *      subscriptions are probed with a non-blocking host `poll()` (regular files and directories are
     *      always ready) and their events carry the HANGUP flag for a closed pipe. Ready descriptor events
     *      are written immediately; otherwise the fiber suspends on `uv_poll` and the earliest clock
     *      timer, and the ready descriptors are collected when it wakes up. A descriptor subscription
     *      without POLL_FD_READWRITE plus FD_READ/FD_WRITE reports NOTCAPABLE; an unknown descriptor is
     *      reported as a BADF event. On Windows the host keeps no pollable handle for a socket, so every
     *      descriptor subscription is reported ready without waiting; the guest still blocks in
     *      `sock_recv` until data arrives.
     *      @param in_ptr the guest address of the subscription array
     *      @param out_ptr the guest address of the event array
     *      @param nsubscriptions the number of subscriptions
     *      @param nevents the guest address that receives the event count
     *      @return 0 on success, or a WASI errno value
     *
     */
    poll_oneoff(in_ptr: number, out_ptr: number, nsubscriptions: number, nevents: number): Promise<number>;

    /**
     * @description Implements the WASI `poll_oneoff` syscall
     *
     *      Parses the 48-byte subscriptions. Clock subscriptions are served by a uv timer; descriptor
     *      subscriptions are probed with a non-blocking host `poll()` (regular files and directories are
     *      always ready) and their events carry the HANGUP flag for a closed pipe. Ready descriptor events
     *      are written immediately; otherwise the fiber suspends on `uv_poll` and the earliest clock
     *      timer, and the ready descriptors are collected when it wakes up. A descriptor subscription
     *      without POLL_FD_READWRITE plus FD_READ/FD_WRITE reports NOTCAPABLE; an unknown descriptor is
     *      reported as a BADF event. On Windows the host keeps no pollable handle for a socket, so every
     *      descriptor subscription is reported ready without waiting; the guest still blocks in
     *      `sock_recv` until data arrives.
     *      @param in_ptr the guest address of the subscription array
     *      @param out_ptr the guest address of the event array
     *      @param nsubscriptions the number of subscriptions
     *      @param nevents the guest address that receives the event count
     *      @return 0 on success, or a WASI errno value
     *
     */
    poll_oneoffSync(in_ptr: number, out_ptr: number, nsubscriptions: number, nevents: number): number;

    /**
     * @description Implements the WASI `poll_oneoff` syscall
     *
     *      Parses the 48-byte subscriptions. Clock subscriptions are served by a uv timer; descriptor
     *      subscriptions are probed with a non-blocking host `poll()` (regular files and directories are
     *      always ready) and their events carry the HANGUP flag for a closed pipe. Ready descriptor events
     *      are written immediately; otherwise the fiber suspends on `uv_poll` and the earliest clock
     *      timer, and the ready descriptors are collected when it wakes up. A descriptor subscription
     *      without POLL_FD_READWRITE plus FD_READ/FD_WRITE reports NOTCAPABLE; an unknown descriptor is
     *      reported as a BADF event. On Windows the host keeps no pollable handle for a socket, so every
     *      descriptor subscription is reported ready without waiting; the guest still blocks in
     *      `sock_recv` until data arrives.
     *      @param in_ptr the guest address of the subscription array
     *      @param out_ptr the guest address of the event array
     *      @param nsubscriptions the number of subscriptions
     *      @param nevents the guest address that receives the event count
     *      @return 0 on success, or a WASI errno value
     *
     */
    poll_oneoffAsync(in_ptr: number, out_ptr: number, nsubscriptions: number, nevents: number): Promise<number>;

    /**
     * @description Implements the WASI `args_sizes_get` syscall
     *
     *      Writes the argument count and the size of the buffer needed to store the arguments with their
     *      NUL terminators. The guest allocates `argv` and the buffer from these two values before
     *      calling `args_get`. The arguments come from the `args` option of the constructor.
     *      @param argc the guest address that receives the argument count
     *      @param argv_buf_size the guest address that receives the buffer size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    args_sizes_get(argc: number, argv_buf_size: number): number;

    /**
     * @description Implements the WASI `args_get` syscall
     *
     *      Writes the argument vector and the argument bytes. `argv` receives one u32 pointer per
     *      argument, each pointing into the buffer at `argv_buf`; the strings are NUL separated and each
     *      one is preceded by its pointer. The host bounds-checks both guest ranges before writing.
     *      @param argv the guest address of the pointer array
     *      @param argv_buf the guest address of the string buffer
     *      @return 0 on success, or a WASI errno value
     *
     */
    args_get(argv: number, argv_buf: number): number;

    /**
     * @description Implements the WASI `environ_sizes_get` syscall
     *
     *      Writes the environment variable count and the size of the buffer needed to store the
     *      `key=value` strings with their NUL terminators. The environment comes from the `env` option
     *      of the constructor.
     *      @param environ_count the guest address that receives the variable count
     *      @param environ_buf_size the guest address that receives the buffer size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    environ_sizes_get(environ_count: number, environ_buf_size: number): number;

    /**
     * @description Implements the WASI `environ_get` syscall
     *
     *      Writes the environment vector and the `key=value` strings. `environ` receives one u32 pointer
     *      per variable, each pointing into `environ_buf`; entries are NUL separated. The host
     *      bounds-checks both guest ranges before writing.
     *      @param environ the guest address of the pointer array
     *      @param environ_buf the guest address of the string buffer
     *      @return 0 on success, or a WASI errno value
     *
     */
    environ_get(environ: number, environ_buf: number): number;

    /**
     * @description Implements the WASI `clock_res_get` syscall
     *
     *      Writes the resolution of the requested clock in nanoseconds. The realtime and monotonic
     *      clocks are supported; the process and thread CPU clocks fall back to the monotonic
     *      resolution on platforms without a separate source.
     *      @param clock_id the WASI clock identifier
     *      @param resolution the guest address that receives the resolution in nanoseconds
     *      @return 0 on success, or a WASI errno value
     *
     */
    clock_res_get(clock_id: number, resolution: number): number;

    /**
     * @description Implements the WASI `clock_time_get` syscall
     *
     *      Writes the current time of the requested clock in nanoseconds. The precision argument is
     *      accepted for compatibility and ignored; the realtime clock uses the system wall clock and the
     *      others use the monotonic high-resolution timer.
     *      @param clock_id the WASI clock identifier
     *      @param precision the requested precision in nanoseconds (ignored)
     *      @param time the guest address that receives the timestamp in nanoseconds
     *      @return 0 on success, or a WASI errno value
     *
     */
    clock_time_get(clock_id: number, precision: number, time: number): number;

    /**
     * @description Implements the WASI `random_get` syscall
     *
     *      Fills the guest buffer with random bytes from the host entropy source. The whole range is
     *      bounds-checked before any byte is written, so a bad pointer returns FAULT without touching
     *      memory.
     *      @param buf the guest address of the buffer
     *      @param buf_len the number of bytes to generate
     *      @return 0 on success, or a WASI errno value
     *
     */
    random_get(buf: number, buf_len: number): number;

    /**
     * @description Implements the WASI `sched_yield` syscall
     *
     *      Yields the calling fiber so that other fibers of the isolate can run. In fibjs this is a
     *      cooperative suspension of the current fiber, not an operating-system thread yield.
     *      @return 0 on success, or a WASI errno value
     *
     */
    sched_yield(): number;

    /**
     * @description Implements the WASI `proc_exit` syscall
     *
     *      Terminates the guest by unwinding `_start` with the given exit code. The host records the code
     *      and throws a JavaScript exception to leave the wasm frames; `start()` catches it and returns
     *      the recorded code instead of reporting a trap.
     *      @param code the guest exit code
     *
     */
    proc_exit(code: number): void;

    /**
     * @description Implements the WASI `proc_raise` syscall
     *
     *      Sends a signal to the host process, mirroring uvwasi: the WASI signal number is translated to
     *      the host signal and delivered with `uv_kill(getpid(), sig)`, so a guest `raise()` follows the
     *      host disposition (a fatal signal terminates the process, a blocked or ignored one does not).
     *      Signals the host has no mapping for return `52` (NOSYS). Unlike `proc_exit`, this syscall does
     *      not unwind the guest.
     *      @param sig the WASI signal number (`1` HUP ... `30` SYS)
     *      @return 0 on success, or a WASI errno value
     *
     */
    proc_raise(sig: number): number;

    /**
     * @description Implements the WASI `fd_close` syscall
     *
     *      Releases the descriptor. A file opened by `path_open` closes its host descriptor and is
     *      removed from the table; the standard descriptors and the preopened directories remain valid
     *      and closing them only removes the guest entry, matching the preview1 table semantics.
     *      @param fd the guest descriptor to close
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_close(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_close` syscall
     *
     *      Releases the descriptor. A file opened by `path_open` closes its host descriptor and is
     *      removed from the table; the standard descriptors and the preopened directories remain valid
     *      and closing them only removes the guest entry, matching the preview1 table semantics.
     *      @param fd the guest descriptor to close
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_closeSync(fd: number): number;

    /**
     * @description Implements the WASI `fd_close` syscall
     *
     *      Releases the descriptor. A file opened by `path_open` closes its host descriptor and is
     *      removed from the table; the standard descriptors and the preopened directories remain valid
     *      and closing them only removes the guest entry, matching the preview1 table semantics.
     *      @param fd the guest descriptor to close
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_closeAsync(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_fdstat_get` syscall
     *
     *      Writes the 24-byte `fdstat` record: the file type, the descriptor flags and the base and
     *      inheriting rights. The record is built from the host `fstat` result plus the rights and flags
     *      recorded when the descriptor was opened.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `fdstat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_get(fd: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_fdstat_get` syscall
     *
     *      Writes the 24-byte `fdstat` record: the file type, the descriptor flags and the base and
     *      inheriting rights. The record is built from the host `fstat` result plus the rights and flags
     *      recorded when the descriptor was opened.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `fdstat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_getSync(fd: number, buf: number): number;

    /**
     * @description Implements the WASI `fd_fdstat_get` syscall
     *
     *      Writes the 24-byte `fdstat` record: the file type, the descriptor flags and the base and
     *      inheriting rights. The record is built from the host `fstat` result plus the rights and flags
     *      recorded when the descriptor was opened.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `fdstat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_getAsync(fd: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for the descriptor: device, inode, file type, link
     *      count, size and the access, modification and change timestamps. The values come from the host
     *      `fstat` call on the underlying descriptor.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_get(fd: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for the descriptor: device, inode, file type, link
     *      count, size and the access, modification and change timestamps. The values come from the host
     *      `fstat` call on the underlying descriptor.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_getSync(fd: number, buf: number): number;

    /**
     * @description Implements the WASI `fd_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for the descriptor: device, inode, file type, link
     *      count, size and the access, modification and change timestamps. The values come from the host
     *      `fstat` call on the underlying descriptor.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_getAsync(fd: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_prestat_get` syscall
     *
     *      Writes the 8-byte prestat record for a preopened directory: the tag (always `dir`) and the
     *      length of the guest path returned by `fd_prestat_dir_name`. A missing descriptor returns BADF
     *      (`8`) and an existing descriptor that is not a preopen returns INVAL (`28`), matching uvwasi;
     *      `fd_prestat_dir_name` uses BADF for both cases.
     *      @param fd the guest descriptor
     *      @param buf the guest address that receives the prestat record
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_prestat_get(fd: number, buf: number): number;

    /**
     * @description Implements the WASI `fd_prestat_dir_name` syscall
     *
     *      Writes the guest name of a preopened directory. The name is not NUL-terminated and a buffer
     *      smaller than the name reports NOBUFS (42), matching uvwasi.
     *      @param fd the preopened descriptor
     *      @param path the guest address of the output buffer
     *      @param path_len the output buffer length
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_prestat_dir_name(fd: number, path: number, path_len: number): number;

    /**
     * @description Implements the WASI `path_open` syscall
     *
     *      Resolves the guest path below a directory descriptor and opens it with the requested flags
     *      and rights. The `oflags` bits map to create/exclusive/truncate/directory, `fdflags` maps to
     *      append and the sync modes, and the new descriptor is written to `opened_fd`. Access is
     *      confined to the preopened directory tree. Like uvwasi, the directory must hold PATH_OPEN plus
     *      the rights implied by the flags, and its inheriting rights must cover the requested base and
     *      inheriting masks (otherwise NOTCAPABLE); the granted rights are intersected with the maximum
     *      the opened file type supports.
     *      @param fd the directory descriptor
     *      @param dirflags the lookup flags (symlink follow)
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param oflags the open flags
     *      @param rights_base the requested base rights
     *      @param rights_inheriting the requested inheriting rights
     *      @param fdflags the descriptor flags
     *      @param opened_fd the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_open(fd: number, dirflags: number, path: number, path_len: number, oflags: number, rights_base: number, rights_inheriting: number, fdflags: number, opened_fd: number): Promise<number>;

    /**
     * @description Implements the WASI `path_open` syscall
     *
     *      Resolves the guest path below a directory descriptor and opens it with the requested flags
     *      and rights. The `oflags` bits map to create/exclusive/truncate/directory, `fdflags` maps to
     *      append and the sync modes, and the new descriptor is written to `opened_fd`. Access is
     *      confined to the preopened directory tree. Like uvwasi, the directory must hold PATH_OPEN plus
     *      the rights implied by the flags, and its inheriting rights must cover the requested base and
     *      inheriting masks (otherwise NOTCAPABLE); the granted rights are intersected with the maximum
     *      the opened file type supports.
     *      @param fd the directory descriptor
     *      @param dirflags the lookup flags (symlink follow)
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param oflags the open flags
     *      @param rights_base the requested base rights
     *      @param rights_inheriting the requested inheriting rights
     *      @param fdflags the descriptor flags
     *      @param opened_fd the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_openSync(fd: number, dirflags: number, path: number, path_len: number, oflags: number, rights_base: number, rights_inheriting: number, fdflags: number, opened_fd: number): number;

    /**
     * @description Implements the WASI `path_open` syscall
     *
     *      Resolves the guest path below a directory descriptor and opens it with the requested flags
     *      and rights. The `oflags` bits map to create/exclusive/truncate/directory, `fdflags` maps to
     *      append and the sync modes, and the new descriptor is written to `opened_fd`. Access is
     *      confined to the preopened directory tree. Like uvwasi, the directory must hold PATH_OPEN plus
     *      the rights implied by the flags, and its inheriting rights must cover the requested base and
     *      inheriting masks (otherwise NOTCAPABLE); the granted rights are intersected with the maximum
     *      the opened file type supports.
     *      @param fd the directory descriptor
     *      @param dirflags the lookup flags (symlink follow)
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param oflags the open flags
     *      @param rights_base the requested base rights
     *      @param rights_inheriting the requested inheriting rights
     *      @param fdflags the descriptor flags
     *      @param opened_fd the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_openAsync(fd: number, dirflags: number, path: number, path_len: number, oflags: number, rights_base: number, rights_inheriting: number, fdflags: number, opened_fd: number): Promise<number>;

    /**
     * @description Implements the WASI `path_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for a path below a directory descriptor. The lookup
     *      flags select whether a symbolic link is followed; without the follow bit the host `lstat`
     *      result describes the link itself.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_get(fd: number, flags: number, path: number, path_len: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `path_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for a path below a directory descriptor. The lookup
     *      flags select whether a symbolic link is followed; without the follow bit the host `lstat`
     *      result describes the link itself.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_getSync(fd: number, flags: number, path: number, path_len: number, buf: number): number;

    /**
     * @description Implements the WASI `path_filestat_get` syscall
     *
     *      Writes the 64-byte `filestat` record for a path below a directory descriptor. The lookup
     *      flags select whether a symbolic link is followed; without the follow bit the host `lstat`
     *      result describes the link itself.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param buf the guest address that receives the `filestat` record
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_getAsync(fd: number, flags: number, path: number, path_len: number, buf: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_seek` syscall
     *
     *      Moves the file position of a regular file. `whence` is 0 (set), 1 (current) or 2 (end); the
     *      end form stats the file first, the other two update the tracked position directly. The new
     *      position is written to `newoffset`.
     *      @param fd the guest descriptor
     *      @param offset the signed byte offset
     *      @param whence the seek origin
     *      @param newoffset the guest address that receives the new position
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_seek(fd: number, offset: number, whence: number, newoffset: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_seek` syscall
     *
     *      Moves the file position of a regular file. `whence` is 0 (set), 1 (current) or 2 (end); the
     *      end form stats the file first, the other two update the tracked position directly. The new
     *      position is written to `newoffset`.
     *      @param fd the guest descriptor
     *      @param offset the signed byte offset
     *      @param whence the seek origin
     *      @param newoffset the guest address that receives the new position
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_seekSync(fd: number, offset: number, whence: number, newoffset: number): number;

    /**
     * @description Implements the WASI `fd_seek` syscall
     *
     *      Moves the file position of a regular file. `whence` is 0 (set), 1 (current) or 2 (end); the
     *      end form stats the file first, the other two update the tracked position directly. The new
     *      position is written to `newoffset`.
     *      @param fd the guest descriptor
     *      @param offset the signed byte offset
     *      @param whence the seek origin
     *      @param newoffset the guest address that receives the new position
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_seekAsync(fd: number, offset: number, whence: number, newoffset: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_tell` syscall
     *
     *      Writes the current file position of a regular file. Standard streams and directories are not
     *      seekable and return SPIPE, matching the preview1 behavior on pipes and terminals.
     *      @param fd the guest descriptor
     *      @param offset the guest address that receives the position
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_tell(fd: number, offset: number): number;

    /**
     * @description Implements the WASI `fd_pread` syscall
     *
     *      Reads from the descriptor at the given absolute offset without changing the file position.
     *      The iovec list is validated and copied exactly like `fd_read`; the byte count is written to
     *      `nread`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_pread(fd: number, iovs: number, iovs_len: number, offset: number, nread: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_pread` syscall
     *
     *      Reads from the descriptor at the given absolute offset without changing the file position.
     *      The iovec list is validated and copied exactly like `fd_read`; the byte count is written to
     *      `nread`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_preadSync(fd: number, iovs: number, iovs_len: number, offset: number, nread: number): number;

    /**
     * @description Implements the WASI `fd_pread` syscall
     *
     *      Reads from the descriptor at the given absolute offset without changing the file position.
     *      The iovec list is validated and copied exactly like `fd_read`; the byte count is written to
     *      `nread`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nread the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_preadAsync(fd: number, iovs: number, iovs_len: number, offset: number, nread: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_pwrite` syscall
     *
     *      Writes to the descriptor at the given absolute offset without changing the file position.
     *      The iovec payload is gathered into a staging buffer first, then written on the uv worker; the
     *      byte count is written to `nwritten`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_pwrite(fd: number, iovs: number, iovs_len: number, offset: number, nwritten: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_pwrite` syscall
     *
     *      Writes to the descriptor at the given absolute offset without changing the file position.
     *      The iovec payload is gathered into a staging buffer first, then written on the uv worker; the
     *      byte count is written to `nwritten`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_pwriteSync(fd: number, iovs: number, iovs_len: number, offset: number, nwritten: number): number;

    /**
     * @description Implements the WASI `fd_pwrite` syscall
     *
     *      Writes to the descriptor at the given absolute offset without changing the file position.
     *      The iovec payload is gathered into a staging buffer first, then written on the uv worker; the
     *      byte count is written to `nwritten`.
     *      @param fd the guest descriptor
     *      @param iovs the guest address of the iovec array
     *      @param iovs_len the number of iovec records
     *      @param offset the absolute byte offset
     *      @param nwritten the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_pwriteAsync(fd: number, iovs: number, iovs_len: number, offset: number, nwritten: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_readdir` syscall
     *
     *      Writes directory entries as 24-byte records (`d_next`, `d_ino`, `d_namlen`, `d_type`)
     *      followed by the name bytes. `cookie` selects the starting entry and `bufused` receives the
     *      number of bytes written; the caller repeats the call with the last `d_next` while the buffer
     *      was full.
     *      @param fd the directory descriptor
     *      @param buf the guest address of the output buffer
     *      @param buf_len the buffer length in bytes
     *      @param cookie the directory cookie to start from
     *      @param bufused the guest address that receives the bytes written
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readdir(fd: number, buf: number, buf_len: number, cookie: number, bufused: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_readdir` syscall
     *
     *      Writes directory entries as 24-byte records (`d_next`, `d_ino`, `d_namlen`, `d_type`)
     *      followed by the name bytes. `cookie` selects the starting entry and `bufused` receives the
     *      number of bytes written; the caller repeats the call with the last `d_next` while the buffer
     *      was full.
     *      @param fd the directory descriptor
     *      @param buf the guest address of the output buffer
     *      @param buf_len the buffer length in bytes
     *      @param cookie the directory cookie to start from
     *      @param bufused the guest address that receives the bytes written
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readdirSync(fd: number, buf: number, buf_len: number, cookie: number, bufused: number): number;

    /**
     * @description Implements the WASI `fd_readdir` syscall
     *
     *      Writes directory entries as 24-byte records (`d_next`, `d_ino`, `d_namlen`, `d_type`)
     *      followed by the name bytes. `cookie` selects the starting entry and `bufused` receives the
     *      number of bytes written; the caller repeats the call with the last `d_next` while the buffer
     *      was full.
     *      @param fd the directory descriptor
     *      @param buf the guest address of the output buffer
     *      @param buf_len the buffer length in bytes
     *      @param cookie the directory cookie to start from
     *      @param bufused the guest address that receives the bytes written
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_readdirAsync(fd: number, buf: number, buf_len: number, cookie: number, bufused: number): Promise<number>;

    /**
     * @description Implements the WASI `path_create_directory` syscall
     *
     *      Creates a directory below the descriptor. The path is resolved inside the preopened tree and
     *      the host `mkdir` is performed on the uv worker.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_create_directory(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_create_directory` syscall
     *
     *      Creates a directory below the descriptor. The path is resolved inside the preopened tree and
     *      the host `mkdir` is performed on the uv worker.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_create_directorySync(fd: number, path: number, path_len: number): number;

    /**
     * @description Implements the WASI `path_create_directory` syscall
     *
     *      Creates a directory below the descriptor. The path is resolved inside the preopened tree and
     *      the host `mkdir` is performed on the uv worker.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_create_directoryAsync(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_remove_directory` syscall
     *
     *      Removes an empty directory below the descriptor. A non-empty directory returns NOTEMPTY and
     *      a path that leaves the preopen returns NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_remove_directory(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_remove_directory` syscall
     *
     *      Removes an empty directory below the descriptor. A non-empty directory returns NOTEMPTY and
     *      a path that leaves the preopen returns NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_remove_directorySync(fd: number, path: number, path_len: number): number;

    /**
     * @description Implements the WASI `path_remove_directory` syscall
     *
     *      Removes an empty directory below the descriptor. A non-empty directory returns NOTEMPTY and
     *      a path that leaves the preopen returns NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_remove_directoryAsync(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_unlink_file` syscall
     *
     *      Removes a file below the descriptor. A directory returns ISDIR; symbolic links are removed
     *      themselves, not their targets.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_unlink_file(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_unlink_file` syscall
     *
     *      Removes a file below the descriptor. A directory returns ISDIR; symbolic links are removed
     *      themselves, not their targets.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_unlink_fileSync(fd: number, path: number, path_len: number): number;

    /**
     * @description Implements the WASI `path_unlink_file` syscall
     *
     *      Removes a file below the descriptor. A directory returns ISDIR; symbolic links are removed
     *      themselves, not their targets.
     *      @param fd the directory descriptor
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_unlink_fileAsync(fd: number, path: number, path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_rename` syscall
     *
     *      Renames a path inside one preopened tree to a path inside another. Both paths are resolved
     *      before the host rename runs; the call does not allow either side to escape its descriptor.
     *      @param fd the source directory descriptor
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_rename(fd: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_rename` syscall
     *
     *      Renames a path inside one preopened tree to a path inside another. Both paths are resolved
     *      before the host rename runs; the call does not allow either side to escape its descriptor.
     *      @param fd the source directory descriptor
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_renameSync(fd: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): number;

    /**
     * @description Implements the WASI `path_rename` syscall
     *
     *      Renames a path inside one preopened tree to a path inside another. Both paths are resolved
     *      before the host rename runs; the call does not allow either side to escape its descriptor.
     *      @param fd the source directory descriptor
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_renameAsync(fd: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_link` syscall
     *
     *      Creates a hard link between two paths. The source lookup flags are accepted for
     *      compatibility; both paths are resolved inside their preopened trees before the host link
     *      runs.
     *      @param old_fd the source directory descriptor
     *      @param old_flags the lookup flags
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_link(old_fd: number, old_flags: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_link` syscall
     *
     *      Creates a hard link between two paths. The source lookup flags are accepted for
     *      compatibility; both paths are resolved inside their preopened trees before the host link
     *      runs.
     *      @param old_fd the source directory descriptor
     *      @param old_flags the lookup flags
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_linkSync(old_fd: number, old_flags: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): number;

    /**
     * @description Implements the WASI `path_link` syscall
     *
     *      Creates a hard link between two paths. The source lookup flags are accepted for
     *      compatibility; both paths are resolved inside their preopened trees before the host link
     *      runs.
     *      @param old_fd the source directory descriptor
     *      @param old_flags the lookup flags
     *      @param old_path the guest address of the source path bytes
     *      @param old_path_len the source path length in bytes
     *      @param new_fd the target directory descriptor
     *      @param new_path the guest address of the target path bytes
     *      @param new_path_len the target path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_linkAsync(old_fd: number, old_flags: number, old_path: number, old_path_len: number, new_fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_readlink` syscall
     *
     *      Reads a symbolic link below a directory descriptor into the guest buffer. When the target does
     *      not fit, the call reports NOBUFS (42) instead of truncating, matching uvwasi; on success the
     *      target is NUL-terminated and `bufused` excludes the terminator. A descriptor without
     *      PATH_READLINK reports NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the link path
     *      @param path_len the path length in bytes
     *      @param buf the guest address of the output buffer
     *      @param buf_len the output buffer length
     *      @param bufused the guest address that receives the target length
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_readlink(fd: number, path: number, path_len: number, buf: number, buf_len: number, bufused: number): Promise<number>;

    /**
     * @description Implements the WASI `path_readlink` syscall
     *
     *      Reads a symbolic link below a directory descriptor into the guest buffer. When the target does
     *      not fit, the call reports NOBUFS (42) instead of truncating, matching uvwasi; on success the
     *      target is NUL-terminated and `bufused` excludes the terminator. A descriptor without
     *      PATH_READLINK reports NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the link path
     *      @param path_len the path length in bytes
     *      @param buf the guest address of the output buffer
     *      @param buf_len the output buffer length
     *      @param bufused the guest address that receives the target length
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_readlinkSync(fd: number, path: number, path_len: number, buf: number, buf_len: number, bufused: number): number;

    /**
     * @description Implements the WASI `path_readlink` syscall
     *
     *      Reads a symbolic link below a directory descriptor into the guest buffer. When the target does
     *      not fit, the call reports NOBUFS (42) instead of truncating, matching uvwasi; on success the
     *      target is NUL-terminated and `bufused` excludes the terminator. A descriptor without
     *      PATH_READLINK reports NOTCAPABLE.
     *      @param fd the directory descriptor
     *      @param path the guest address of the link path
     *      @param path_len the path length in bytes
     *      @param buf the guest address of the output buffer
     *      @param buf_len the output buffer length
     *      @param bufused the guest address that receives the target length
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_readlinkAsync(fd: number, path: number, path_len: number, buf: number, buf_len: number, bufused: number): Promise<number>;

    /**
     * @description Implements the WASI `path_symlink` syscall
     *
     *      Creates a symbolic link. The link target is the raw `old_path` string and is not resolved;
     *      the new link path is resolved inside the preopened tree of `fd`.
     *      @param old_path the guest address of the link target bytes
     *      @param old_path_len the link target length in bytes
     *      @param fd the directory descriptor for the new link
     *      @param new_path the guest address of the new link path bytes
     *      @param new_path_len the new link path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_symlink(old_path: number, old_path_len: number, fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `path_symlink` syscall
     *
     *      Creates a symbolic link. The link target is the raw `old_path` string and is not resolved;
     *      the new link path is resolved inside the preopened tree of `fd`.
     *      @param old_path the guest address of the link target bytes
     *      @param old_path_len the link target length in bytes
     *      @param fd the directory descriptor for the new link
     *      @param new_path the guest address of the new link path bytes
     *      @param new_path_len the new link path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_symlinkSync(old_path: number, old_path_len: number, fd: number, new_path: number, new_path_len: number): number;

    /**
     * @description Implements the WASI `path_symlink` syscall
     *
     *      Creates a symbolic link. The link target is the raw `old_path` string and is not resolved;
     *      the new link path is resolved inside the preopened tree of `fd`.
     *      @param old_path the guest address of the link target bytes
     *      @param old_path_len the link target length in bytes
     *      @param fd the directory descriptor for the new link
     *      @param new_path the guest address of the new link path bytes
     *      @param new_path_len the new link path length in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_symlinkAsync(old_path: number, old_path_len: number, fd: number, new_path: number, new_path_len: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_filestat_set_size` syscall
     *
     *      Truncates or extends a regular file to the given size. The host `ftruncate` runs on the uv
     *      worker and the descriptor rights are not enforced in this milestone.
     *      @param fd the guest descriptor
     *      @param size the new file size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_size(fd: number, size: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_filestat_set_size` syscall
     *
     *      Truncates or extends a regular file to the given size. The host `ftruncate` runs on the uv
     *      worker and the descriptor rights are not enforced in this milestone.
     *      @param fd the guest descriptor
     *      @param size the new file size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_sizeSync(fd: number, size: number): number;

    /**
     * @description Implements the WASI `fd_filestat_set_size` syscall
     *
     *      Truncates or extends a regular file to the given size. The host `ftruncate` runs on the uv
     *      worker and the descriptor rights are not enforced in this milestone.
     *      @param fd the guest descriptor
     *      @param size the new file size in bytes
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_sizeAsync(fd: number, size: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_sync` syscall
     *
     *      Flushes the file data and metadata to the storage device. Standard streams and preopened
     *      directories have no host descriptor to flush and return success.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_sync(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_sync` syscall
     *
     *      Flushes the file data and metadata to the storage device. Standard streams and preopened
     *      directories have no host descriptor to flush and return success.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_syncSync(fd: number): number;

    /**
     * @description Implements the WASI `fd_sync` syscall
     *
     *      Flushes the file data and metadata to the storage device. Standard streams and preopened
     *      directories have no host descriptor to flush and return success.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_syncAsync(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_datasync` syscall
     *
     *      Flushes the file data to the storage device without forcing a metadata update. The call is
     *      the data-only counterpart of `fd_sync` and uses the host `fdatasync`.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_datasync(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_datasync` syscall
     *
     *      Flushes the file data to the storage device without forcing a metadata update. The call is
     *      the data-only counterpart of `fd_sync` and uses the host `fdatasync`.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_datasyncSync(fd: number): number;

    /**
     * @description Implements the WASI `fd_datasync` syscall
     *
     *      Flushes the file data to the storage device without forcing a metadata update. The call is
     *      the data-only counterpart of `fd_sync` and uses the host `fdatasync`.
     *      @param fd the guest descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_datasyncAsync(fd: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_advise` syscall
     *
     *      Supplies a usage hint (`normal`, `sequential`, `random`, `willneed`, `dontneed`,
     *      `noreuse`) for a range of a file. The advice value is validated before the rights check and
     *      the call is a no-op otherwise; a directory reports BADF. A descriptor without FD_ADVISE
     *      reports NOTCAPABLE.
     *      @param fd the guest descriptor
     *      @param offset the range offset
     *      @param len the range length
     *      @param advice the advice value
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_advise(fd: number, offset: number, len: number, advice: number): number;

    /**
     * @description Implements the WASI `fd_allocate` syscall
     *
     *      Grows the file to `offset + len` when it is shorter. macOS has no `posix_fallocate`, so the
     *      host uses the uvwasi fallback (`fstat` + `ftruncate`); the file is not physically reserved.
     *      Directories report ISDIR, a descriptor without FD_ALLOCATE reports NOTCAPABLE.
     *      @param fd the descriptor to allocate for
     *      @param offset the start of the range
     *      @param len the number of bytes to add
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_allocate(fd: number, offset: number, len: number): number;

    /**
     * @description Implements the WASI `fd_fdstat_set_flags` syscall
     *
     *      Stores the descriptor flags and, on POSIX, applies them to the host descriptor with `fcntl`
     *      (append, sync modes, non-blocking) like uvwasi. A descriptor without FD_FDSTAT_SET_FLAGS
     *      reports NOTCAPABLE.
     *      @param fd the descriptor to update
     *      @param flags the new descriptor flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_set_flags(fd: number, flags: number): number;

    /**
     * @description Implements the WASI `fd_fdstat_set_rights` syscall
     *
     *      Reduces the rights of a descriptor. WASI only allows dropping rights: a requested right that
     *      the descriptor does not already have returns NOTCAPABLE.
     *      @param fd the guest descriptor
     *      @param rights_base the new base rights
     *      @param rights_inheriting the new inheriting rights
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_fdstat_set_rights(fd: number, rights_base: number, rights_inheriting: number): number;

    /**
     * @description Implements the WASI `fd_filestat_set_times` syscall
     *
     *      Updates the access and modification times. `fst_flags` accepts SET_ATIM/SET_MTIM and their
     *      NOW variants; conflicting or unknown bits report INVAL before the rights check, like uvwasi.
     *      Omitted times keep the current value, read with `fstat`.
     *      @param fd the descriptor to update
     *      @param atim the access time in nanoseconds (when SET_ATIM is set)
     *      @param mtim the modification time in nanoseconds (when SET_MTIM is set)
     *      @param flags the fstflags mask
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_times(fd: number, atim: number, mtim: number, flags: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_filestat_set_times` syscall
     *
     *      Updates the access and modification times. `fst_flags` accepts SET_ATIM/SET_MTIM and their
     *      NOW variants; conflicting or unknown bits report INVAL before the rights check, like uvwasi.
     *      Omitted times keep the current value, read with `fstat`.
     *      @param fd the descriptor to update
     *      @param atim the access time in nanoseconds (when SET_ATIM is set)
     *      @param mtim the modification time in nanoseconds (when SET_MTIM is set)
     *      @param flags the fstflags mask
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_timesSync(fd: number, atim: number, mtim: number, flags: number): number;

    /**
     * @description Implements the WASI `fd_filestat_set_times` syscall
     *
     *      Updates the access and modification times. `fst_flags` accepts SET_ATIM/SET_MTIM and their
     *      NOW variants; conflicting or unknown bits report INVAL before the rights check, like uvwasi.
     *      Omitted times keep the current value, read with `fstat`.
     *      @param fd the descriptor to update
     *      @param atim the access time in nanoseconds (when SET_ATIM is set)
     *      @param mtim the modification time in nanoseconds (when SET_MTIM is set)
     *      @param flags the fstflags mask
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_filestat_set_timesAsync(fd: number, atim: number, mtim: number, flags: number): Promise<number>;

    /**
     * @description Implements the WASI `path_filestat_set_times` syscall
     *
     *      Updates the access and modification times of a path below a directory descriptor. The
     *      lookup flags select whether a symbolic link is followed; the timestamp flags have the same
     *      meaning as in `fd_filestat_set_times`.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param atim the access time in nanoseconds
     *      @param mtim the modification time in nanoseconds
     *      @param fst_flags the timestamp update flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_set_times(fd: number, flags: number, path: number, path_len: number, atim: number, mtim: number, fst_flags: number): Promise<number>;

    /**
     * @description Implements the WASI `path_filestat_set_times` syscall
     *
     *      Updates the access and modification times of a path below a directory descriptor. The
     *      lookup flags select whether a symbolic link is followed; the timestamp flags have the same
     *      meaning as in `fd_filestat_set_times`.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param atim the access time in nanoseconds
     *      @param mtim the modification time in nanoseconds
     *      @param fst_flags the timestamp update flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_set_timesSync(fd: number, flags: number, path: number, path_len: number, atim: number, mtim: number, fst_flags: number): number;

    /**
     * @description Implements the WASI `path_filestat_set_times` syscall
     *
     *      Updates the access and modification times of a path below a directory descriptor. The
     *      lookup flags select whether a symbolic link is followed; the timestamp flags have the same
     *      meaning as in `fd_filestat_set_times`.
     *      @param fd the directory descriptor
     *      @param flags the lookup flags
     *      @param path the guest address of the path bytes
     *      @param path_len the path length in bytes
     *      @param atim the access time in nanoseconds
     *      @param mtim the modification time in nanoseconds
     *      @param fst_flags the timestamp update flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    path_filestat_set_timesAsync(fd: number, flags: number, path: number, path_len: number, atim: number, mtim: number, fst_flags: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_renumber` syscall
     *
     *      Moves the source descriptor to the target number, closing the previous target when it exists.
     *      Unlike uvwasi, the target may be free; the preview1 specification allows renumbering onto an
     *      unopened descriptor and wasi-libc's `dup2` relies on it.
     *      @param from the descriptor to move
     *      @param to the new descriptor number
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_renumber(from: number, to: number): Promise<number>;

    /**
     * @description Implements the WASI `fd_renumber` syscall
     *
     *      Moves the source descriptor to the target number, closing the previous target when it exists.
     *      Unlike uvwasi, the target may be free; the preview1 specification allows renumbering onto an
     *      unopened descriptor and wasi-libc's `dup2` relies on it.
     *      @param from the descriptor to move
     *      @param to the new descriptor number
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_renumberSync(from: number, to: number): number;

    /**
     * @description Implements the WASI `fd_renumber` syscall
     *
     *      Moves the source descriptor to the target number, closing the previous target when it exists.
     *      Unlike uvwasi, the target may be free; the preview1 specification allows renumbering onto an
     *      unopened descriptor and wasi-libc's `dup2` relies on it.
     *      @param from the descriptor to move
     *      @param to the new descriptor number
     *      @return 0 on success, or a WASI errno value
     *
     */
    fd_renumberAsync(from: number, to: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_accept` syscall
     *
     *      Accepts a pending connection on a preopened listener. The listener is created from the
     *      `sockets` constructor option (each entry `{ address, port }`), and the new connection is
     *      inserted as a socket descriptor with the socket rights. The call suspends the fiber until a
     *      connection arrives; with FDFLAG_NONBLOCK it returns EAGAIN instead. A non-listener descriptor
     *      reports NOTSOCK and a descriptor without the socket rights reports NOTCAPABLE.
     *      @param fd the listener descriptor
     *      @param flags the accept flags (only NONBLOCK is supported)
     *      @param fd_out the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_accept(fd: number, flags: number, fd_out: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_accept` syscall
     *
     *      Accepts a pending connection on a preopened listener. The listener is created from the
     *      `sockets` constructor option (each entry `{ address, port }`), and the new connection is
     *      inserted as a socket descriptor with the socket rights. The call suspends the fiber until a
     *      connection arrives; with FDFLAG_NONBLOCK it returns EAGAIN instead. A non-listener descriptor
     *      reports NOTSOCK and a descriptor without the socket rights reports NOTCAPABLE.
     *      @param fd the listener descriptor
     *      @param flags the accept flags (only NONBLOCK is supported)
     *      @param fd_out the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_acceptSync(fd: number, flags: number, fd_out: number): number;

    /**
     * @description Implements the WASI `sock_accept` syscall
     *
     *      Accepts a pending connection on a preopened listener. The listener is created from the
     *      `sockets` constructor option (each entry `{ address, port }`), and the new connection is
     *      inserted as a socket descriptor with the socket rights. The call suspends the fiber until a
     *      connection arrives; with FDFLAG_NONBLOCK it returns EAGAIN instead. A non-listener descriptor
     *      reports NOTSOCK and a descriptor without the socket rights reports NOTCAPABLE.
     *      @param fd the listener descriptor
     *      @param flags the accept flags (only NONBLOCK is supported)
     *      @param fd_out the guest address that receives the new descriptor
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_acceptAsync(fd: number, flags: number, fd_out: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_recv` syscall
     *
     *      Reads stream bytes into the first iovec of the guest array, like uvwasi. Non-zero `ri_flags`
     *      report NOTSUP. The fiber suspends until data arrives; a zero-length read reports EAGAIN.
     *      `ro_datalen` receives the byte count and `ro_flags` is written as 0.
     *      @param fd the socket descriptor
     *      @param ri_data the guest address of the iovec array
     *      @param ri_data_len the number of iovec records (only the first is used)
     *      @param ri_flags the receive flags
     *      @param ro_datalen the guest address that receives the byte count
     *      @param ro_flags the guest address that receives the receive flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_recv(fd: number, ri_data: number, ri_data_len: number, ri_flags: number, ro_datalen: number, ro_flags: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_recv` syscall
     *
     *      Reads stream bytes into the first iovec of the guest array, like uvwasi. Non-zero `ri_flags`
     *      report NOTSUP. The fiber suspends until data arrives; a zero-length read reports EAGAIN.
     *      `ro_datalen` receives the byte count and `ro_flags` is written as 0.
     *      @param fd the socket descriptor
     *      @param ri_data the guest address of the iovec array
     *      @param ri_data_len the number of iovec records (only the first is used)
     *      @param ri_flags the receive flags
     *      @param ro_datalen the guest address that receives the byte count
     *      @param ro_flags the guest address that receives the receive flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_recvSync(fd: number, ri_data: number, ri_data_len: number, ri_flags: number, ro_datalen: number, ro_flags: number): number;

    /**
     * @description Implements the WASI `sock_recv` syscall
     *
     *      Reads stream bytes into the first iovec of the guest array, like uvwasi. Non-zero `ri_flags`
     *      report NOTSUP. The fiber suspends until data arrives; a zero-length read reports EAGAIN.
     *      `ro_datalen` receives the byte count and `ro_flags` is written as 0.
     *      @param fd the socket descriptor
     *      @param ri_data the guest address of the iovec array
     *      @param ri_data_len the number of iovec records (only the first is used)
     *      @param ri_flags the receive flags
     *      @param ro_datalen the guest address that receives the byte count
     *      @param ro_flags the guest address that receives the receive flags
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_recvAsync(fd: number, ri_data: number, ri_data_len: number, ri_flags: number, ro_datalen: number, ro_flags: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_send` syscall
     *
     *      Sends the guest iovecs through the socket. Like uvwasi, the write is non-blocking
     *      (`uv_try_write`) and may be partial; `so_datalen` receives the number of bytes queued.
     *      Non-zero `si_flags` report INVAL.
     *      @param fd the socket descriptor
     *      @param si_data the guest address of the iovec array
     *      @param si_data_len the number of iovec records
     *      @param si_flags the send flags
     *      @param so_datalen the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_send(fd: number, si_data: number, si_data_len: number, si_flags: number, so_datalen: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_send` syscall
     *
     *      Sends the guest iovecs through the socket. Like uvwasi, the write is non-blocking
     *      (`uv_try_write`) and may be partial; `so_datalen` receives the number of bytes queued.
     *      Non-zero `si_flags` report INVAL.
     *      @param fd the socket descriptor
     *      @param si_data the guest address of the iovec array
     *      @param si_data_len the number of iovec records
     *      @param si_flags the send flags
     *      @param so_datalen the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_sendSync(fd: number, si_data: number, si_data_len: number, si_flags: number, so_datalen: number): number;

    /**
     * @description Implements the WASI `sock_send` syscall
     *
     *      Sends the guest iovecs through the socket. Like uvwasi, the write is non-blocking
     *      (`uv_try_write`) and may be partial; `so_datalen` receives the number of bytes queued.
     *      Non-zero `si_flags` report INVAL.
     *      @param fd the socket descriptor
     *      @param si_data the guest address of the iovec array
     *      @param si_data_len the number of iovec records
     *      @param si_flags the send flags
     *      @param so_datalen the guest address that receives the byte count
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_sendAsync(fd: number, si_data: number, si_data_len: number, si_flags: number, so_datalen: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_shutdown` syscall
     *
     *      Shuts down the write direction of a socket and suspends the fiber until libuv completes the
     *      shutdown. `how == 0` is a successful no-op; other unsupported bits report NOTSUP.
     *      @param fd the socket descriptor
     *      @param how the shutdown flags (0 or SHUT_WR)
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_shutdown(fd: number, how: number): Promise<number>;

    /**
     * @description Implements the WASI `sock_shutdown` syscall
     *
     *      Shuts down the write direction of a socket and suspends the fiber until libuv completes the
     *      shutdown. `how == 0` is a successful no-op; other unsupported bits report NOTSUP.
     *      @param fd the socket descriptor
     *      @param how the shutdown flags (0 or SHUT_WR)
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_shutdownSync(fd: number, how: number): number;

    /**
     * @description Implements the WASI `sock_shutdown` syscall
     *
     *      Shuts down the write direction of a socket and suspends the fiber until libuv completes the
     *      shutdown. `how == 0` is a successful no-op; other unsupported bits report NOTSUP.
     *      @param fd the socket descriptor
     *      @param how the shutdown flags (0 or SHUT_WR)
     *      @return 0 on success, or a WASI errno value
     *
     */
    sock_shutdownAsync(fd: number, how: number): Promise<number>;

}


declare namespace Class_WASIHost {
    const promises: FIBJS.GeneralObject;
}
