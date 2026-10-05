/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description Isolated sandbox object, used to manage an independent runtime space
 *
 *   All code runs in its own sandbox; the global require loads modules through the current sandbox, and the sandbox is passed to loaded sandboxes through require. The following example creates a sandbox that allows access only to the assert module among the global basic modules, and adds two custom modules a and b:
 *  ```JavaScript
 *   var vm = require('vm');
 *   var sbox = new vm.SandBox({
 *    a: 100,
 *    b: 200,
 *    assert: require('assert')
 *   });
 *
 *   var mod_in_sbox = sbox.require('./path/to/mod');
 *   ```
 *  Note that SandBox is not a security sandbox against attacks; SandBox is only an independent runtime space that can be used to isolate different code and avoid mutual interference, but it cannot protect against malicious code.
 *  To load ECMAScript modules, use the following code:
 *  ```JavaScript
 *   import vm from 'vm';
 *   var sbox = new vm.SandBox();
 *   var mod = await sbox.import('./a.mjs', __dirname);
 *   ```
 *
 */
declare class Class_SandBox extends Class_object {
    /**
     * @description Constructs a new isolated sandbox object and initializes the basic modules
     *      @param mods the module object dictionary to add
     *
     */
    constructor(mods?: FIBJS.GeneralObject);

    /**
     * @description Constructs a new isolated sandbox object and initializes the basic modules
     *      @param mods the module object dictionary to add
     *      @param require a custom require function; when a module does not exist, the custom function is called first, and if it returns nothing the module is loaded from files
     *
     */
    constructor(mods: FIBJS.GeneralObject, require: (id: string)=>any);

    /**
     * @description Constructs a new isolated sandbox object with an independent Global and initializes the basic modules
     *      @param mods the module object dictionary to add
     *      @param require a custom require function; when a module does not exist, the custom function is called first, and if it returns nothing the module is loaded from files
     *      @param global the initial Global properties to set
     *
     */
    constructor(mods: FIBJS.GeneralObject, require: (id: string)=>any, global: FIBJS.GeneralObject);

    /**
     * @description Constructs a new isolated sandbox object with an independent Global and initializes the basic modules
     *      @param mods the module object dictionary to add
     *      @param global the initial Global properties to set
     *
     */
    constructor(mods: FIBJS.GeneralObject, global: FIBJS.GeneralObject);

    /**
     * @description Adds the built-in basic modules to the sandbox
     */
    addBuiltinModules(): void;

    /**
     * @description Adds a basic module to the sandbox
     *      @param id the name of the module to add; this path is unrelated to the running script and must be an absolute path or a module name
     *      @param mod the module object to add
     *
     */
    add(id: string, mod: any): void;

    /**
     * @description Adds a group of basic modules to the sandbox
     *      @param mods the module object dictionary to add; added javascript modules are copied so that modifications made by the sandbox do not interfere with each other
     *
     */
    add(mods: FIBJS.GeneralObject): void;

    /**
     * @description Adds a script module to the sandbox; a string script is encoded as utf8
     *      @param srcname the script name to add; srcname must include an extension, such as json, js or jsc
     *      script may be a Buffer, or a string encoded as utf8.
     *      @param script the binary code to add
     *      @return returns the loaded module object
     *
     */
    addScript(srcname: string, script: Class_Buffer | string): any;

    /**
     * @description Removes the specified basic module from the sandbox
     *      @param id the name of the module to remove; this path is unrelated to the running script and must be an absolute path or a module name
     *
     */
    remove(id: string): void;

    /**
     * @description Checks whether a basic module exists in the sandbox
     *      @param id the name of the module to check; this path is unrelated to the running script and must be an absolute path or a module name
     *      @return whether it exists
     *
     */
    has(id: string): boolean;

    /**
     * @description Clones the current sandbox; the new sandbox contains the modules of the current sandbox, and has the same name and require function
     *      @return the new cloned sandbox
     *
     */
    clone(): Class_SandBox;

    /**
     * @description Freezes the current sandbox; after freezing, modifications made to global are ignored
     */
    freeze(): void;

    /**
     * @description Runs a script
     *      @param fname the path of the script to run; this path is unrelated to the running script and must be an absolute path
     *
     */
    run(fname: string): void;

    /**
     * @description Queries a module and returns the full file name of the module
     *      @param id the name of the module to load
     *      @param base the lookup path
     *      @return returns the full file name of the loaded module
     *
     */
    resolve(id: string, base: string): string;

    /**
     * @description Loads a module and returns the module object; require cannot load ECMAScript modules
     *      @param id the name of the module to load
     *      @param base the lookup path
     *      @return returns the loaded module object
     *
     */
    require(id: string, base: string): any;

    /**
     * @description Asynchronously loads a module and returns the module object; can load both ECMAScript modules and CommonJS modules
     *      @param id the name of the module to load
     *      @param base the lookup path
     *      @return returns the loaded module object
     *
     */
    import(id: string, base: string): Promise;

    /**
     * @description Adds a compiler for the specified extname; extname cannot be a system built-in extension (including {'.js', '.json', '.jsc', '.wasm'}), and compiler must return a valid javascript script.
     *
     *       ```JavaScript
     *       var vm = require('vm');
     *       var sbox = new vm.SandBox({
     *       });
     *
     *       // compile ts to js and load
     *       sbox.setModuleCompiler('.ts', tsCompiler);
     *       var mod_ts = sbox.require('./a.ts');
     *
     *       // compile coffee to js and load
     *       sbox.setModuleCompiler('.coffee', cafeCompiler);
     *       var mod_coffee = sbox.require('./a.coffee');
     *
     *       // compile jsx to js and load
     *       sbox.setModuleCompiler('.jsx', reactCompiler);
     *       var mod_react = sbox.require('./a.jsx');
     *
     *       // compile yaml to rest and load
     *       sbox.setModuleCompiler('.yml', yaml2Rest)
     *       sbox.setModuleCompiler('.yaml', yaml2Rest)
     *
     *       // compile markdown to html and load
     *       sbox.setModuleCompiler('.md', mdCompiler)
     *       sbox.setModuleCompiler('.markdown', mdCompiler)
     *       ```
     *
     *      @param extname the extname, which must start with '.' and must not be a system built-in extension
     *      @param compiler the compile callback; files with this extname are required only once. The callback format is `compiler(buf, requireInfo)`, where buf is the read file Buffer and requireInfo has the structure `{filename: string}`.
     *
     */
    setModuleCompiler(extname: string, compiler: (buf: Class_Buffer, requireInfo: FIBJS.GeneralObject)=>any): void;

    /**
     * @description Queries the global object of the sandbox
     */
    readonly global: FIBJS.GeneralObject;

    /**
     * @description Queries the dictionary object of all modules currently in the sandbox
     *
     */
    readonly modules: FIBJS.GeneralObject;

}

