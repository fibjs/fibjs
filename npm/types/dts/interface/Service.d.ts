/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description System service management object
 */
declare class Class_Service extends Class_EventEmitter {
    /**
     * @description System service management object constructor
     *      @param name service name
     *      @param worker service run function
     *      @param event service event handling
     *
     */
    constructor(name: string, worker: (...args: any[])=>any, event?: FIBJS.GeneralObject);

    /**
     * @description Starts running the service entity
     */
    run(): void;

    run(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Starts running the service entity
     */
    runSync(): void;

    /**
     * @description Starts running the service entity
     */
    runAsync(): Promise<void>;

    /**
     * @description Queries and sets the service name
     */
    name: string;

    /**
     * @description Queries and binds the service stop event, equivalent to on("stop", func);
     */
    on(event: "stop", listener: ()=>void): this;

    once(event: "stop", listener: ()=>void): this;

    off(event: "stop", listener: ()=>void): this;

    addListener(event: "stop", listener: ()=>void): this;

    removeListener(event: "stop", listener: ()=>void): this;

    addEventListener(event: "stop", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "stop", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "stop", listener: ()=>void): this;

    prependOnceListener(event: "stop", listener: ()=>void): this;

    /**
     * @description Queries and binds the service stop event, equivalent to on("stop", func);
     */
    onstop: (()=>void) | null;

    /**
     * @description Queries and binds the service pause event, equivalent to on("pause", func);
     */
    on(event: "pause", listener: ()=>void): this;

    once(event: "pause", listener: ()=>void): this;

    off(event: "pause", listener: ()=>void): this;

    addListener(event: "pause", listener: ()=>void): this;

    removeListener(event: "pause", listener: ()=>void): this;

    addEventListener(event: "pause", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "pause", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "pause", listener: ()=>void): this;

    prependOnceListener(event: "pause", listener: ()=>void): this;

    /**
     * @description Queries and binds the service pause event, equivalent to on("pause", func);
     */
    onpause: (()=>void) | null;

    /**
     * @description Queries and binds the service resume event, equivalent to on("continue", func);
     */
    on(event: "continue", listener: ()=>void): this;

    once(event: "continue", listener: ()=>void): this;

    off(event: "continue", listener: ()=>void): this;

    addListener(event: "continue", listener: ()=>void): this;

    removeListener(event: "continue", listener: ()=>void): this;

    addEventListener(event: "continue", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "continue", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "continue", listener: ()=>void): this;

    prependOnceListener(event: "continue", listener: ()=>void): this;

    /**
     * @description Queries and binds the service resume event, equivalent to on("continue", func);
     */
    oncontinue: (()=>void) | null;

    /**
     * @description Installs the service into the system
     *      @param name service name
     *      @param cmd service command line
     *      @param displayName service display name
     *      @param description service description
     *
     */
    static install(name: string, cmd: string, displayName?: string, description?: string): void;

    /**
     * @description Uninstalls the service from the system
     *      @param name service name
     *
     */
    static remove(name: string): void;

    /**
     * @description Starts the service
     *      @param name service name
     *
     */
    static start(name: string): void;

    /**
     * @description Stops the service
     *      @param name service name
     *
     */
    static stop(name: string): void;

    /**
     * @description Restarts the service
     *      @param name service name
     *
     */
    static restart(name: string): void;

    /**
     * @description Checks whether the service is installed
     *      @param name service name
     *      @return returns True if the service is installed
     *
     */
    static isInstalled(name: string): boolean;

    /**
     * @description Checks whether the service is running
     *      @param name service name
     *      @return returns True if the service is running
     *
     */
    static isRunning(name: string): boolean;

    on(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>any, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>any): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * The promise variant of the Service class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_ServicePromise extends Class_EventEmitter {
    /**
     * @description System service management object constructor
     *      @param name service name
     *      @param worker service run function
     *      @param event service event handling
     *
     */
    constructor(name: string, worker: (...args: any[])=>any, event?: FIBJS.GeneralObject);

    /**
     * @description Starts running the service entity
     */
    run(): Promise<void>;

    /**
     * @description Starts running the service entity
     */
    runSync(): void;

    /**
     * @description Starts running the service entity
     */
    runAsync(): Promise<void>;

    /**
     * @description Queries and sets the service name
     */
    name: string;

    /**
     * @description Queries and binds the service stop event, equivalent to on("stop", func);
     */
    onstop: (()=>void) | null;

    /**
     * @description Queries and binds the service pause event, equivalent to on("pause", func);
     */
    onpause: (()=>void) | null;

    /**
     * @description Queries and binds the service resume event, equivalent to on("continue", func);
     */
    oncontinue: (()=>void) | null;

    /**
     * @description Installs the service into the system
     *      @param name service name
     *      @param cmd service command line
     *      @param displayName service display name
     *      @param description service description
     *
     */
    static install(name: string, cmd: string, displayName?: string, description?: string): void;

    /**
     * @description Uninstalls the service from the system
     *      @param name service name
     *
     */
    static remove(name: string): void;

    /**
     * @description Starts the service
     *      @param name service name
     *
     */
    static start(name: string): void;

    /**
     * @description Stops the service
     *      @param name service name
     *
     */
    static stop(name: string): void;

    /**
     * @description Restarts the service
     *      @param name service name
     *
     */
    static restart(name: string): void;

    /**
     * @description Checks whether the service is installed
     *      @param name service name
     *      @return returns True if the service is installed
     *
     */
    static isInstalled(name: string): boolean;

    /**
     * @description Checks whether the service is running
     *      @param name service name
     *      @return returns True if the service is running
     *
     */
    static isRunning(name: string): boolean;

}


declare namespace Class_Service {
    const promises: {
        readonly install: (name: string, cmd: string, displayName?: string, description?: string)=>void;
        readonly remove: (name: string)=>void;
        readonly start: (name: string)=>void;
        readonly stop: (name: string)=>void;
        readonly restart: (name: string)=>void;
        readonly isInstalled: (name: string)=>boolean;
        readonly isRunning: (name: string)=>boolean;
    };

}

