/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HeapSnapshot.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description V8 runtime introspection: heap statistics, heap snapshots and value serialization
 *
 * The `v8` module exposes the state of the JavaScript heap and the V8 binary serialization
 * format, so an application can observe its own memory behavior, compare heap states and
 * transfer values compactly. Its capabilities are grouped as follows:
 *
 * - **Heap statistics**: `getHeapStatistics`, `getHeapSpaceStatistics` and
 *   `getHeapCodeStatistics` report heap totals, per-space usage and code metadata;
 * - **Heap snapshots**: `takeSnapshot` captures the live heap as a `HeapSnapshot` object
 *   graph, `saveSnapshot` writes it as a Chrome DevTools `.heapsnapshot` file,
 *   `loadSnapshot` parses such a file back and `diff` compares the heap of a function
 *   before and after it runs;
 * - **Runtime sampling**: `start` appends the call stacks of all fibers to a file at a
 *   fixed interval; this is the sampler behind the `--prof` command line option;
 * - **Serialization**: `serialize` and `deserialize` convert values to and from the V8
 *   binary format, which is compatible with Node.js;
 * - **Graph constants**: the `Node_*` and `Edge_*` constants classify the nodes and
 *   edges of a heap snapshot.
 *
 * Concepts:
 *
 * - **V8 heap and spaces**: V8 manages JavaScript values in a heap divided into spaces
 *   (for example `new_space` for young objects, `old_space` for long lived objects and
 *   `code_space` for compiled code). `getHeapSpaceStatistics` reports the size, used
 *   size, available size and physical size of every space in bytes; the set and the
 *   order of the spaces depend on the V8 version, so match them by name.
 * - **Garbage collection**: memory is reclaimed automatically, therefore
 *   `used_heap_size` includes objects that are unreachable but not collected yet. This
 *   build exposes no `gc()` call, and V8 command line flags cannot be changed at run
 *   time (Node.js has `setFlagsFromString`). The snapshot functions issue a V8
 *   low-memory notification (a full collection) before capturing the graph, which is
 *   part of why they are expensive.
 * - **Serialization vs heap snapshots**: `serialize`/`deserialize` transfer a value
 *   graph and preserve circular and shared references, BigInt, Map, Set, Date, RegExp,
 *   Error, ArrayBuffer, TypedArray and Buffer; functions, Symbol, WeakMap and WeakSet
 *   cannot be serialized. Heap snapshots describe every object in the heap, are much
 *   larger and are meant for analysis, not for transfer.
 * - **Heap snapshot model**: a snapshot is a graph of `HeapGraphNode` objects linked by
 *   `HeapGraphEdge` objects. Nodes carry a type, a name, a stable id and a shallow
 *   size; edges carry a type and a name. A snapshot is read only and independent of the
 *   live heap once captured; see HeapSnapshot for the consumption patterns.
 * - **Leak hunting**: capture a snapshot, run the suspected code, capture a second one
 *   and compare them with `diff` or `HeapSnapshot.diff`. Nodes are matched by id, so
 *   the change lists the objects allocated and released in between, grouped by node
 *   description. A growing `number_of_native_contexts` or `number_of_detached_contexts`
 *   from `getHeapStatistics` is another leak indicator. A comparison only points at
 *   suspects: find the retaining path by walking edges from the root. For CPU hotspots
 *   use the `--prof` sampler described in the profiler guide of the documentation; for
 *   lightweight timing use the performance module.
 * - **Node.js differences**: Node.js exposes `setFlagsFromString`, `gc`,
 *   `writeHeapSnapshot`/`getHeapSnapshot`, `cachedDataVersionTag`, `queryObjects`,
 *   `Serializer`/`Deserializer`, promise hooks, coverage APIs and `GCProfiler`, none of
 *   which exist here. Conversely this module adds a snapshot object model
 *   (`takeSnapshot`, `loadSnapshot`, `HeapSnapshot.diff`) and the fiber sampler
 *   (`start`). `getHeapStatistics` omits the Node.js fields `does_zap_garbage`,
 *   `total_global_handles_size`, `used_global_handles_size` and `total_allocated_bytes`,
 *   and `getHeapCodeStatistics` omits `cpu_profiler_metadata_size`.
 *
 * Import:
 * ```JavaScript
 * const v8 = require('v8');
 * ```
 *
 * Example 1 — heap statistics overview:
 * ```JavaScript
 * const v8 = require('v8');
 *
 * const stats = v8.getHeapStatistics();
 * console.log('used_heap_size:', stats.used_heap_size);
 * console.log('heap_size_limit:', stats.heap_size_limit);
 *
 * // Not every heap space must be in use: match spaces by name.
 * const spaces = v8.getHeapSpaceStatistics();
 * for (const space of spaces) {
 *     if (space.space_size > 0)
 *         console.log(space.space_name, space.space_used_size + '/' + space.space_size);
 * }
 *
 * const code = v8.getHeapCodeStatistics();
 * console.log('bytecode_and_metadata_size:', code.bytecode_and_metadata_size);
 * ```
 *
 * Example 2 — serialize and deserialize a value graph:
 * ```JavaScript
 * const v8 = require('v8');
 *
 * const cache = { users: new Map([['ada', { score: 42 }]]) };
 * cache.self = cache; // circular references are preserved
 *
 * const buf = v8.serialize(cache);
 * const copy = v8.deserialize(buf);
 *
 * console.log(Buffer.isBuffer(buf));               // true
 * console.log(copy.users.get('ada').score);        // 42
 * console.log(copy.self === copy);                 // true
 * console.log(v8.deserialize(v8.serialize(123n))); // 123n
 * ```
 *
 * Example 3 — write a heap snapshot to a file and read it back:
 * ```JavaScript
 * const v8 = require('v8');
 * const fs = require('fs');
 * const os = require('os');
 * const path = require('path');
 *
 * const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-v8-'));
 * const file = path.join(dir, 'app.heapsnapshot');
 *
 * v8.saveSnapshot(file);                  // capture the heap and write it
 * const snapshot = v8.loadSnapshot(file); // parse it back as an object graph
 *
 * console.log(snapshot.nodes.length > 0);                                      // true
 * console.log(snapshot.root.type === v8.Node_Synthetic);                       // true
 * console.log(snapshot.getNodeById(snapshot.root.id).id === snapshot.root.id); // true
 *
 * fs.rmSync(dir, { recursive: true, force: true });
 * ```
 *
 * Notes:
 *
 * - Heap snapshots are large: a trivial process already has tens of thousands of nodes,
 *   and the node and edge objects are views that keep working only while their snapshot
 *   is referenced.
 * - Cache `snapshot.nodes` before iterating it: the getter rebuilds the node array and
 *   re-wraps every node on each access.
 * - `HeapSnapshot.time` is not populated in this implementation and reads as an
 *   Invalid Date.
 * - Paths passed to `saveSnapshot` and `loadSnapshot` are normalized and resolved
 *   against the current working directory.
 *
 */
declare module 'v8' {
    /**
     * @description Hidden node, filtered out when shown to the user
     */
    export const Node_Hidden: 0;

    /**
     * @description Element storage of an array (a JS array itself is an object node)
     */
    export const Node_Array: 1;

    /**
     * @description String
     */
    export const Node_String: 2;

    /**
     * @description JS object, including arrays and functions
     */
    export const Node_Object: 3;

    /**
     * @description Compiled code
     */
    export const Node_Code: 4;

    /**
     * @description Function closure
     */
    export const Node_Closure: 5;

    /**
     * @description Regular expression
     */
    export const Node_RegExp: 6;

    /**
     * @description Number stored in the heap (a boxed double)
     */
    export const Node_HeapNumber: 7;

    /**
     * @description Native object (not from the v8 heap)
     */
    export const Node_Native: 8;

    /**
     * @description Synthetic object, used to group snapshot items
     */
    export const Node_Synthetic: 9;

    /**
     * @description Concatenated string (a pair of pointers to strings)
     */
    export const Node_ConsString: 10;

    /**
     * @description Sliced string (a fragment of another string)
     */
    export const Node_SlicedString: 11;

    /**
     * @description Symbol (ES6)
     */
    export const Node_Symbol: 12;

    /**
     * @description Legacy name of type 13, which current V8 reports for BigInt values
     */
    export const Node_SimdValue: 13;

    /**
     * @description Variable from a function context
     */
    export const Edge_ContextVariable: 0;

    /**
     * @description Element of an array; the edge name is the numeric index
     */
    export const Edge_Element: 1;

    /**
     * @description Named object property
     */
    export const Edge_Property: 2;

    /**
     * @description Link that cannot be accessed from JS
     */
    export const Edge_Internal: 3;

    /**
     * @description Link needed for size calculation, hidden from the user
     */
    export const Edge_Hidden: 4;

    /**
     * @description Link that must not be followed during size calculation
     */
    export const Edge_Shortcut: 5;

    /**
     * @description Weak reference, ignored by the garbage collector
     */
    export const Edge_Weak: 6;

    /**
     * @description Gets statistics of the code and its metadata in the v8 heap
     *
     *      The call is cheap and does not trigger a garbage collection. The returned object
     *      contains three fields, all sizes in bytes:
     *      - `code_and_metadata_size`: compiled code and its metadata;
     *      - `bytecode_and_metadata_size`: bytecode and its metadata;
     *      - `external_script_source_size`: externally loaded script sources.
     *
     *      Node.js also reports `cpu_profiler_metadata_size`, which this implementation does
     *      not provide.
     *      @return returns the statistics of the metadata
     *
     */
    function getHeapCodeStatistics(): FIBJS.GeneralObject;

    /**
     * @description Gets the detailed usage of v8 heap memory
     *
     *      Returns one object per heap space with the fields `space_name`, `space_size`,
     *      `space_used_size`, `space_available_size` and `physical_space_size`, all sizes in
     *      bytes. Empty spaces may report zero size, and the set and the order of the spaces
     *      depend on the V8 version, so match spaces by name rather than by index. The call
     *      does not trigger a garbage collection; see the module Concepts section for the
     *      space model.
     *      @return returns the detailed usage of heap memory
     *
     */
    function getHeapSpaceStatistics(): {
        space_name: string;
        space_size: number;
        space_used_size: number;
        space_available_size: number;
        physical_space_size: number;
    }[];

    /**
     * @description Gets statistics of v8 heap memory usage
     *
     *      Returns an object with the heap sizes (`total_heap_size`, `total_physical_size`,
     *      `used_heap_size` and the `heap_size_limit`), the `malloced_memory` and
     *      `peak_malloced_memory` counters, `external_memory` and the context counters, all
     *      sizes in bytes; see the module Concepts section for their meaning. Because the
     *      call does not trigger a garbage collection, `used_heap_size` includes unreachable
     *      objects that have not been collected yet.
     *
     *      Compared with Node.js the returned object omits `does_zap_garbage`,
     *      `total_global_handles_size`, `used_global_handles_size` and
     *      `total_allocated_bytes`.
     *      @return returns statistics of heap memory usage
     *
     */
    function getHeapStatistics(): {
        total_heap_size: number;
        total_heap_size_executable: number;
        total_physical_size: number;
        total_available_size: number;
        used_heap_size: number;
        heap_size_limit: number;
        malloced_memory: number;
        external_memory: number;
        peak_malloced_memory: number;
        number_of_native_contexts: number;
        number_of_detached_contexts: number;
    };

    /**
     * @description Saves a heap snapshot under the specified name
     *
     *      Captures the current heap with a full garbage collection and writes it as a
     *      Chrome DevTools `.heapsnapshot` JSON file, replacing an existing file. The file
     *      can be several megabytes even for a small process; use `takeSnapshot` and
     *      `HeapSnapshot.save` when the snapshot should be inspected before it is written.
     *      The path is normalized and resolved against the current working directory; throws
     *      an ENOENT error when the parent directory does not exist.
     *      @param fname the heap snapshot name (file path)
     *
     */
    function saveSnapshot(fname: string): void;

    /**
     * @description Reads a heap snapshot under the specified name
     *
     *      Parses a `.heapsnapshot` file written by `saveSnapshot`, `HeapSnapshot.save` or
     *      Chrome DevTools and returns an independent `HeapSnapshot` object graph. Node ids
     *      are preserved, so snapshots loaded from files taken at different times can be
     *      compared with `HeapSnapshot.diff`. A loaded snapshot does not relate to the live
     *      heap.
     *
     *      Throws an ENOENT error for a missing file, a SyntaxError for invalid JSON and an
     *      error with number 20011 for well formed JSON that is not a V8 heap snapshot.
     *      @param fname the heap snapshot name (file path)
     *      @return returns the loaded heap snapshot
     *
     */
    function loadSnapshot(fname: string): Class_HeapSnapshot;

    /**
     * @description Gets the heap snapshot at the current point in time
     *
     *      Issues a V8 low-memory notification (a full garbage collection) and captures the
     *      live heap as a `HeapSnapshot`. This is an expensive blocking operation: the graph
     *      of a trivial process already contains tens of thousands of nodes, and V8 keeps
     *      the snapshot data until the returned object is garbage collected, so avoid
     *      calling it in a loop. Keep the snapshot referenced while its nodes and edges are
     *      in use. Node.js has no equivalent object model; its `writeHeapSnapshot` returns
     *      the path of a serialized snapshot instead.
     *
     *      Example — capture the heap and look up the root node:
     *      ```JavaScript
     *      const v8 = require('v8');
     *
     *      const snapshot = v8.takeSnapshot();
     *      const nodes = snapshot.nodes; // cache it: the getter rebuilds the array
     *
     *      console.log(nodes.length > 0);                                // true
     *      console.log(snapshot.root.type === v8.Node_Synthetic);        // true
     *      console.log(snapshot.getNodeById(snapshot.root.id) !== null); // true
     *      ```
     *      @return returns the obtained heap snapshot
     *
     */
    function takeSnapshot(): Class_HeapSnapshot;

    /**
     * @description Executes a function and compares the v8 heap before and after it runs
     *
     *      A convenience wrapper that captures two snapshots around the call, collecting the
     *      garbage before each capture, and returns the same structure as
     *      `HeapSnapshot.diff` (the `before`, `after` and `change` sections). The function is
     *      called without arguments and its return value is ignored; it may be a regular or
     *      an arrow function.
     *
     *      Note: exceptions thrown by `test` are not propagated in the current
     *      implementation; the comparison is still produced.
     *
     *      Example — measure the heap cost of an allocation:
     *      ```JavaScript
     *      const v8 = require('v8');
     *
     *      const result = v8.diff(() => {
     *          const hold = [];
     *          for (let i = 0; i < 2000; i++) hold.push({ index: i, label: 'row-' + i });
     *          return hold.length;
     *      });
     *
     *      console.log(result.before.nodes > 0);           // true
     *      console.log(result.change.allocated_nodes > 0); // true
     *      console.log(result.change.size);                // human readable, e.g. "126.0 KB"
     *      ```
     *      @param test the function to test
     *      @return returns the comparison result
     *
     */
    function diff(test: ()=>void): FIBJS.GeneralObject;

    /**
     * @description Starts a runtime state sampling log
     *
     *      Every `interval` milliseconds the sampler appends one line to `fname` holding a
     *      JSON array with the call stack of each fiber; the file is opened in append mode.
     *      Sampling stops by itself once `time` milliseconds have elapsed (checked after
     *      each tick) or when the returned timer's `clear` method is called; a `time` of 0
     *      or less disables the automatic stop. This is the same sampler that the `--prof`
     *      command line option starts; the profiler guide of the documentation explains how
     *      `--prof-process` turns the log into a flame graph. The returned timer is unref'ed,
     *      so it does not keep the process alive. Relative paths are resolved against the
     *      current working directory; throws an ENOENT error when the directory of `fname`
     *      does not exist.
     *
     *      Example — sample the fiber stacks into a file:
     *      ```JavaScript
     *      const v8 = require('v8');
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const coroutine = require('coroutine');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-v8-'));
     *      const log = path.join(dir, 'stacks.log');
     *
     *      // Sample every 20ms; time 0 disables the auto-stop deadline.
     *      const timer = v8.start(log, 0, 20);
     *      coroutine.sleep(120);
     *      timer.clear();
     *
     *      const lines = fs.readFileSync(log, 'utf8').trim().split('\n');
     *      console.log(lines.length > 0);           // true
     *      console.log(lines[0].charAt(0) === '['); // true: each line is a JSON array of stacks
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param fname the log storage file name
     *      @param time the sampling time to use, default 1 minute
     *      @param interval the interval time to use, default 100 milliseconds
     *      @return returns the sampling timer; sampling can be stopped early through the clear method
     *
     */
    function start(fname: string, time?: number, interval?: number): Class_Timer;

    /**
     * @description Serializes a value into a Buffer
     *
     *      Uses the V8 serialization format to convert a JavaScript value into binary data.
     *      Circular and shared references, BigInt, Map, Set, Date, RegExp, Error, Buffer,
     *      ArrayBuffer and typed arrays are preserved. Functions, Symbol, WeakMap and WeakSet
     *      cannot be serialized: the call throws an Error whose message ends with
     *      "could not be cloned." (no errno). The format is the same as the Node.js
     *      `v8.serialize`, and the produced Buffer can be deserialized by Node.js and vice
     *      versa.
     *
     *      Example — serialize a value with a Set and a BigInt:
     *      ```JavaScript
     *      const v8 = require('v8');
     *
     *      const buf = v8.serialize({ tags: new Set(['a', 'b']), when: new Date(0) });
     *      console.log(Buffer.isBuffer(buf));                      // true
     *      console.log(v8.deserialize(buf).tags.has('a'));         // true
     *      console.log(v8.deserialize(v8.serialize(42n)) === 42n); // true
     *      ```
     *      @param value the value to serialize
     *      @return returns the serialized Buffer
     *
     */
    function serialize(value: any): Class_Buffer;

    /**
     * @description Deserializes a Buffer or a string into a value
     *
     *      Restores binary data previously serialized by `serialize` back into a JavaScript
     *      value. At run time the argument may be a Buffer, a TypedArray, an ArrayBuffer or a
     *      DataView; the declared string form is decoded as utf8 and is only safe when the
     *      serialized bytes form valid utf8, so prefer a Buffer. The data must contain a
     *      complete serialization: a truncated or corrupted buffer throws an Error with the
     *      message "Unable to deserialize cloned data due to invalid or unsupported
     *      version." (no errno). Buffers produced by the Node.js `v8.serialize` are accepted.
     *
     *      Example — deserialize from a Buffer and from a Uint8Array:
     *      ```JavaScript
     *      const v8 = require('v8');
     *
     *      const bytes = new Uint8Array(v8.serialize({ ok: true }));
     *      console.log(v8.deserialize(bytes).ok);                          // true
     *      console.log(v8.deserialize(v8.serialize([1, 2, 3])).join(',')); // 1,2,3
     *      ```
     *      @param data the data to deserialize
     *      @return returns the deserialized value
     *
     */
    function deserialize(data: Class_Buffer | string): any;

}

