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

    /**
     * @description Queries and binds the service pause event, equivalent to on("pause", func);
     */
    on(event: "pause", listener: ()=>void): this;

    /**
     * @description Queries and binds the service resume event, equivalent to on("continue", func);
     */
    on(event: "continue", listener: ()=>void): this;

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

