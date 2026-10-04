/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/HeapSnapshot.d.ts" />
/// <reference path="../interface/Timer.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description v8 memory module
 *
 * The `v8` module is a tool for analyzing and monitoring the memory usage of JavaScript applications. It provides a series of methods that help developers gain insight into the memory allocation and garbage collection behavior of applications, so as to optimize performance and memory usage.
 *
 * In JavaScript, memory management is handled automatically by the garbage collection mechanism. However, for complex applications, especially those that process large amounts of data or run for a long time, memory leaks and unnecessary memory usage may become performance bottlenecks. By using the `v8` module, developers can obtain detailed memory usage information and identify and solve potential memory problems.
 *
 * The `v8` module provides the following main features:
 *
 * 1. **Get heap memory statistics**: including the overall heap memory usage, the detailed usage of each heap space, and statistics of the code in the heap.
 * 2. **Heap snapshot**: can save and load heap snapshots, recording the heap memory state at a specific point in time.
 * 3. **Heap snapshot comparison**: executes the given function and compares the heap memory changes before and after execution, helping identify differences in memory allocation and reclamation.
 * 4. **Runtime state sampling**: starts a runtime state sampling log that records memory usage within a specified period of time.
 *
 * The following is some example code using the `v8` module, showing how to get heap memory statistics, save and load heap snapshots, and start runtime state sampling.
 *
 * ```javascript
 * // Import v8 module
 * var v8 = require('v8');
 *
 * // Get heap memory statistics
 * var heapStats = v8.getHeapStatistics();
 * console.log('Heap Statistics:', heapStats);
 *
 * // Get heap space statistics
 * var heapSpaceStats = v8.getHeapSpaceStatistics();
 * console.log('Heap Space Statistics:', heapSpaceStats);
 *
 * // Save heap snapshot
 * v8.saveSnapshot('snapshot1');
 *
 * // Load heap snapshot
 * var snapshot = v8.loadSnapshot('snapshot1');
 * console.log('Loaded Snapshot:', snapshot);
 *
 * // Get current heap snapshot
 * var currentSnapshot = v8.takeSnapshot();
 * console.log('Current Snapshot:', currentSnapshot);
 *
 * // Compare heap memory changes before and after execution
 * function testFunction() {
 *     // Simulate some memory allocation operations
 *     var arr = [];
 *     for (var i = 0; i < 10000; i++) {
 *         arr.push({index: i});
 *     }
 * }
 * var diffResult = v8.diff(testFunction);
 * console.log('Heap Diff Result:', diffResult);
 *
 * // Start runtime sampling log
 * var timer = v8.start('samplingLog', 60000, 100);
 * console.log('Sampling started, timer:', timer);
 *
 * // Stop sampling
 * // timer.clear();
 * ```
 *
 * Through these features and methods, developers can better monitor and optimize the memory usage of JavaScript applications, improving application performance and stability.
 *
 */
declare module 'v8' {
    /**
     * @description Hidden node, can be filtered out when shown to the user
     */
    export const Node_Hidden: 0;

    /**
     * @description Array
     */
    export const Node_Array: 1;

    /**
     * @description String
     */
    export const Node_String: 2;

    /**
     * @description JS object (other than strings and arrays)
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
     * @description Sorted number in the heap
     */
    export const Node_HeapNumber: 7;

    /**
     * @description Native object (not on the v8 heap)
     */
    export const Node_Native: 8;

    /**
     * @description Synthetic object
     */
    export const Node_Synthetic: 9;

    /**
     * @description Concatenated string
     */
    export const Node_ConsString: 10;

    /**
     * @description Sliced string
     */
    export const Node_SlicedString: 11;

    /**
     * @description Symbol (ES6)
     */
    export const Node_Symbol: 12;

    /**
     * @description Sorted SIMD value in the heap (ES7)
     */
    export const Node_SimdValue: 13;

    /**
     * @description Variable in a function
     */
    export const Edge_ContextVariable: 0;

    /**
     * @description Element in an array
     */
    export const Edge_Element: 1;

    /**
     * @description Property of a named object
     */
    export const Edge_Property: 2;

    /**
     * @description Link that JS cannot enter
     */
    export const Edge_Internal: 3;

    /**
     * @description Points to a node whose space size must be computed in advance
     */
    export const Edge_Hidden: 4;

    /**
     * @description Points to a node whose space size cannot be computed in advance
     */
    export const Edge_Shortcut: 5;

    /**
     * @description A weak reference (ignored by the GC)
     */
    export const Edge_Weak: 6;

    /**
     * @description Gets = statistics of the code and its metadata in the v8 heap
     *      @return returns the statistics of the metadata
     *
     */
    function getHeapCodeStatistics(): FIBJS.GeneralObject;

    /**
     * @description Gets the detailed usage of v8 heap memory
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
     * 	 @param fname the heap snapshot name
     *
     */
    function saveSnapshot(fname: string): void;

    /**
     * @description Reads a heap snapshot under the specified name
     * 	 @param fname the heap snapshot name
     * 	 @return returns the loaded heap snapshot
     *
     */
    function loadSnapshot(fname: string): Class_HeapSnapshot;

    /**
     * @description Gets the heap snapshot at the current point in time; the heap snapshot records the state of the JS heap at the current moment
     * 	 @return returns the obtained heap snapshot
     *
     */
    function takeSnapshot(): Class_HeapSnapshot;

    /**
     * @description Executes the given function and compares the changes of the v8 heap before and after execution
     * 	 @param test the function to test
     * 	 @return returns the comparison result
     *
     */
    function diff(test: ()=>void): FIBJS.GeneralObject;

    /**
     * @description Starts a runtime state sampling log
     * 	 @param fname the log storage file name
     * 	 @param time the sampling time to use, default 1 minute
     * 	 @param interval the interval time to use, default 100 milliseconds
     *      @return returns the sampling timer; sampling can be stopped early through the clear method
     *
     */
    function start(fname: string, time?: number, interval?: number): Class_Timer;

    /**
     * @description Serializes a value into a Buffer
     *
     *      Uses the V8 serialization format to convert any JavaScript value into binary data. Supports circular references, TypedArray, Map, Set, Date, RegExp, Error and other types.
     *      Does not support functions, Symbol, WeakMap, WeakSet and other types.
     *      @param value the value to serialize
     *      @return returns the serialized Buffer
     *
     */
    function serialize(value: any): Class_Buffer;

    /**
     * @description Deserializes a Buffer or a string into a value
     *
     *      Restores binary data previously serialized by serialize back into a JavaScript value.
     *      @param data the Buffer to deserialize; a string is encoded as utf8
     *      @return returns the deserialized value
     *
     */
    function deserialize(data: Class_Buffer | string): any;

}

