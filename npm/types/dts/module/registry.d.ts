/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The `registry` module is a module for operating the Windows Registry. It provides methods and constants for accessing the registry, supporting read, modify, delete, add and other operations. The ways of operating provided by the `registry` module are similar to those used by Windows applications, but they are provided as capabilities in FibJS. The constants include common Root and data type constants, as well as some constants used as return values for different operations
 *
 * The `registry` module is a module for operating the Windows Registry. The registry is a hierarchical database used to store configuration information of the system and applications. The Windows operating system and many applications rely on the registry to store and retrieve configuration information.
 *
 * The Windows Registry contains multiple root keys (Root Key), and each root key contains multiple sub keys (Sub Key) and values (Value). Common root keys include:
 *
 * - `HKEY_CLASSES_ROOT`: stores information about file types and their associated applications.
 * - `HKEY_CURRENT_USER`: stores configuration information of the current user.
 * - `HKEY_LOCAL_MACHINE`: stores configuration information of all users on the computer.
 * - `HKEY_USERS`: stores configuration information of all users.
 * - `HKEY_CURRENT_CONFIG`: stores information about the current hardware configuration.
 *
 * Data types in the registry include string (SZ), expanded string (EXPAND_SZ), 32-bit value (DWORD), 64-bit value (QWORD), etc.
 *
 * The `registry` module provides a series of functions for reading, modifying, deleting and adding registry entries. Common functions include:
 *
 * - `get(root, key[, flags])`: gets the value of the specified registry entry.
 * - `set(root, key, value[, type])`: sets the value of the specified registry entry.
 * - `del(root, key)`: deletes the specified registry entry.
 *
 * The following is example code using the `registry` module, showing how to verify whether a registry entry exists, write the entry if it does not exist, and read its value:
 *
 * ```JavaScript
 * var registry = require('registry');
 *
 * // Specify key name
 * var key = "Software\\Fibjs\\Test\\KeyName";
 *
 * // Check if registry key exists
 * if (!registry.get(registry.CLASSES_ROOT, key)) {
 *     // If not exists, write to registry
 *     registry.set(registry.CLASSES_ROOT, key, "test_value");
 * }
 *
 * // Read registry key value
 * var value = registry.get(registry.CLASSES_ROOT, key);
 * console.log(value);
 * ```
 *
 * The program first checks whether the registry entry `Software\Fibjs\Test\KeyName` exists; if it does not exist, it sets its value to `test_value`. Finally, it reads the value of the registry entry and outputs it to the console.
 *
 * The `registry` module provides a convenient interface for operating the Windows Registry in FibJS. Through this module, information in the registry can be easily read, modified, added and deleted, so as to manage the configuration of the system and applications.
 *
 */
declare module 'registry' {
    /**
     * @description Registry root; stores a detailed list of file types recognized by Windows and their associated programs
     */
    export const CLASSES_ROOT: 0;

    /**
     * @description Registry root; stores information about the current user settings
     */
    export const CURRENT_USER: 1;

    /**
     * @description Registry root; contains information about the hardware and software installed on the computer
     */
    export const LOCAL_MACHINE: 2;

    /**
     * @description Registry root; contains information about the users using the computer
     */
    export const USERS: 3;

    /**
     * @description Registry root; this branch contains the current hardware configuration information of the computer
     */
    export const CURRENT_CONFIG: 5;

    /**
     * @description Registry data type, string
     */
    export const SZ: 1;

    /**
     * @description Registry data type, expanded string
     */
    export const EXPAND_SZ: 2;

    /**
     * @description Registry data type, 32-bit value
     */
    export const DWORD: 4;

    /**
     * @description Registry data type, 64-bit value
     */
    export const QWORD: 11;

    /**
     * @description Returns all sub keys under the specified key
     *      @param root the registry root to use
     *      @param key the key to use
     *      @return returns all sub keys under the key
     *
     */
    function listSubKey(root: number, key: string): string[];

    /**
     * @description Returns the keys of all data under the specified key
     *      @param root the registry root to use
     *      @param key the key to use
     *      @return returns the keys of all data under the key
     *
     */
    function listValue(root: number, key: string): string[];

    /**
     * @description Queries the value of the specified key
     *      @param root the registry root to use
     *      @param key the key to use
     *      @return returns the value of the specified key
     *
     */
    function get(root: number, key: string): any;

    /**
     * @description Queries the value of the specified key
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param name the value name to use
     *      @return returns the value of the specified key
     *
     */
    function get(root: number, key: string, name: string): any;

    /**
     * @description Sets the specified key to a number
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param value the number to set
     *      @param type the type to use; allowed types are DWORD and QWORD, default DWORD
     *
     */
    function set(root: number, key: string, value: number, type?: number): void;

    /**
     * @description Sets the specified key to a multi-string
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param values the multi-string array to set
     *
     */
    function set(root: number, key: string, values: string[]): void;

    /**
     * @description Sets the specified key to binary
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param value the binary data to set
     *
     */
    function set(root: number, key: string, value: Class_Buffer): void;

    /**
     * @description Sets the specified key to a string
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param value the string to set
     *      @param type the type to use; allowed types are SZ and EXPAND_SZ, default SZ
     *
     */
    function set(root: number, key: string, value: string, type?: number): void;

    /**
     * @description Sets the specified key to a number
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param name the value name to use
     *      @param value the number to set
     *      @param type the type to use; allowed types are DWORD and QWORD, default DWORD
     *
     */
    function set(root: number, key: string, name: string, value: number, type?: number): void;

    /**
     * @description Sets the specified key to a multi-string
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param name the value name to use
     *      @param values the multi-string array to set
     *
     */
    function set(root: number, key: string, name: string, values: string[]): void;

    /**
     * @description Sets the specified key to binary
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param name the value name to use
     *      @param value the binary data to set
     *
     */
    function set(root: number, key: string, name: string, value: Class_Buffer): void;

    /**
     * @description Sets the specified key to a string
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param name the value name to use
     *      @param value the string to set
     *      @param type the type to use; allowed types are SZ and EXPAND_SZ, default SZ
     *
     */
    function set(root: number, key: string, name: string, value: string, type?: number): void;

    /**
     * @description Checks whether the specified key exists
     *      @param root the registry root to use
     *      @param key the key to use
     *      @return returns whether the key exists
     *
     */
    function has(root: number, key: string): boolean;

    /**
     * @description Checks whether the specified key exists
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param name the value name to use
     *      @return returns whether the key exists
     *
     */
    function has(root: number, key: string, name: string): boolean;

    /**
     * @description Deletes the value of the specified key
     *      @param root the registry root to use
     *      @param key the key to use
     *
     */
    function del(root: number, key: string): void;

    /**
     * @description Deletes the value of the specified key
     *      @param root the registry root to use
     *      @param key the key to use
     *      @param name the value name to use
     *
     */
    function del(root: number, key: string, name: string): void;

}

