/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * @description A Windows system service: run a JavaScript function under the Service Control Manager
 *
 *  A Service connects the current process to the Windows Service Control Manager (SCM).
 *  The worker function runs when the SCM starts the service, and the SCM commands `stop`,
 *  `pause` and `continue` are delivered as events. Unlike TcpServer or HttpServer —
 *  ordinary classes that accept connections — a Service serves no requests and is not
 *  their base class; it only presents the process to the operating system as a managed
 *  service.
 *
 *  Concepts:
 *
 *  - **Platform**: the class is published on every platform as `os.Service`, but
 *  construction, `run` and all static methods are Windows-only; elsewhere they throw
 *  `Error` with number 20009 (invalid procedure call). A service script must therefore be
 *  guarded with `process.platform === 'win32'`, as in the examples below.
 *  - **Lifecycle**: `install` registers a command line that the SCM launches at boot;
 *  when that process calls `run()`, the SCM connects to it and dispatches control events.
 *  `isInstalled` and `isRunning` query the service database, `start`, `stop` and
 *  `restart` control a registered service, and `remove` unregisters it. Managing services
 *  normally requires administrator rights.
 *  - **Running**: `run()` must be called by the process the SCM launched; it does not
 *  return until the service stops, and only one Service may run per process. The worker
 *  function is invoked when the service starts, and the `stop` handler is the place to
 *  close resources. `runAsync()` is the promise form.
 *  - **Events**: `stop`, `pause` and `continue` mirror the SCM controls and are also
 *  exposed as the `onstop`, `onpause` and `oncontinue` properties. The `event` object of
 *  the constructor is the map form of EventEmitter#on, a shortcut for registering them.
 *  - **Name**: `name` is the identifier registered with the SCM and may be changed at any
 *  time; the static methods take that name as a string instead of using an instance.
 *  - **Node.js**: there is no Node.js counterpart — system service management is outside
 *  the Node.js standard library.
 *
 *  Obtained from:
 *  - `new os.Service(name, worker, event = {})` — creates the service object (Windows only);
 *  - the class is published as `os.Service`; there is no module of that name.
 *
 *  Example 1 — the class is reachable everywhere but construction only works on Windows:
 *  ```JavaScript
 *  const os = require('os');
 *
 *  console.log('os.Service is a', typeof os.Service);
 *
 *  if (process.platform === 'win32') {
 *      const service = new os.Service('fibjs-demo', function () {
 *          console.log('service worker');
 *      });
 *      console.log('created:', service.name);
 *  } else {
 *      try {
 *          new os.Service('fibjs-demo', function () { });
 *          console.log('created');
 *      } catch (err) {
 *          console.log('not available on this platform, error number:', err.number);
 *      }
 *  }
 *  ```
 *  will output on Linux:
 *  ```sh
 *  os.Service is a function
 *  not available on this platform, error number: 20009
 *  ```
 *
 *  Example 2 — the entry point of a service process:
 *  ```JavaScript
 *  // requires: windows
 *  const os = require('os');
 *  const fs = require('fs');
 *
 *  const log = 'C:\\temp\\fibjs-service.log';
 *
 *  const service = new os.Service('fibjs-demo', function () {
 *      // runs when the SCM starts the service, on its own fiber
 *      fs.appendFile(log, 'worker started\n');
 *  }, {
 *      stop: function () {
 *          fs.appendFile(log, 'service stopping\n');
 *      }
 *  });
 *
 *  service.run(); // returns when the service stops
 *  ```
 *
 *  Example 3 — install, control and remove a service:
 *  ```JavaScript
 *  // requires: windows
 *  const os = require('os');
 *
 *  const name = 'fibjs-demo';
 *  const cmd = process.execPath + ' C:\\services\\demo.js';
 *
 *  if (!os.Service.isInstalled(name)) {
 *      os.Service.install(name, cmd, 'fibjs demo service', 'A fibjs sample service');
 *      console.log('installed');
 *  }
 *
 *  console.log('installed:', os.Service.isInstalled(name));
 *  os.Service.start(name);
 *  console.log('running:', os.Service.isRunning(name));
 *  os.Service.stop(name);
 *  os.Service.remove(name);
 *  console.log('removed:', os.Service.isInstalled(name) === false);
 *  ```
 *
 */
declare class Class_Service extends Class_EventEmitter {
    /**
     * @description Creates the service object
     *
     *      `name` is the service name registered with the SCM and is also the initial value of
     *      the `name` property. `worker` is the function executed when the service starts; it is
     *      called without arguments, with the Service object as `this`, and it may block for the
     *      whole life of the service. `event` is an optional map of event names to handlers,
     *      equivalent to calling `on` for each entry. The constructor only creates the object: it
     *      does not install or start anything, and on platforms other than Windows it throws
     *      `Error` (20009).
     *      @param name service name
     *      @param worker function executed when the service starts
     *      @param event map of event handlers to register, for example `{ stop: fn }`
     *
     */
    constructor(name: string, worker: ()=>void, event?: FIBJS.GeneralObject);

    /**
     * @description Runs the service control dispatcher and blocks until the service stops
     *
     *      The call must come from the process that the SCM launched; it connects to the Service
     *      Control Manager and waits for its control events, invoking the worker function when
     *      the service is started and the event handlers when the service is stopped, paused or
     *      continued. The method returns when the service stops, and only one Service may be
     *      running per process. `runAsync()` is the promise form; both are Windows-only (`Error`
     *      20009 elsewhere).
     *
     */
    run(): void;

    run(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Runs the service control dispatcher and blocks until the service stops
     *
     *      The call must come from the process that the SCM launched; it connects to the Service
     *      Control Manager and waits for its control events, invoking the worker function when
     *      the service is started and the event handlers when the service is stopped, paused or
     *      continued. The method returns when the service stops, and only one Service may be
     *      running per process. `runAsync()` is the promise form; both are Windows-only (`Error`
     *      20009 elsewhere).
     *
     */
    runSync(): void;

    /**
     * @description Runs the service control dispatcher and blocks until the service stops
     *
     *      The call must come from the process that the SCM launched; it connects to the Service
     *      Control Manager and waits for its control events, invoking the worker function when
     *      the service is started and the event handlers when the service is stopped, paused or
     *      continued. The method returns when the service stops, and only one Service may be
     *      running per process. `runAsync()` is the promise form; both are Windows-only (`Error`
     *      20009 elsewhere).
     *
     */
    runAsync(): Promise<void>;

    /**
     * @description The service name
     *
     *      The name is the identifier used by the SCM and may be read or replaced at any time;
     *      changing it does not rename an already installed service. This property is available
     *      on every platform on a service object; the creation of that object is the Windows-only
     *      part.
     *
     */
    name: string;

    /**
     * @description Binds the service stop handler, equivalent to on("stop", func)
     *
     *      The handler runs on its own fiber when the SCM stops the service, after which `run`
     *      returns. The property accessor is `onstop`, and the constructor's event map can
     *      register the same handler.
     *
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
     * @description Binds the service stop handler, equivalent to on("stop", func)
     *
     *      The handler runs on its own fiber when the SCM stops the service, after which `run`
     *      returns. The property accessor is `onstop`, and the constructor's event map can
     *      register the same handler.
     *
     */
    onstop: (()=>void) | null;

    /**
     * @description Binds the service pause handler, equivalent to on("pause", func)
     *
     *      The handler runs on its own fiber when the SCM pauses the service; use it to suspend
     *      work that should not continue while the service is paused. The property accessor is
     *      `onpause`.
     *
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
     * @description Binds the service pause handler, equivalent to on("pause", func)
     *
     *      The handler runs on its own fiber when the SCM pauses the service; use it to suspend
     *      work that should not continue while the service is paused. The property accessor is
     *      `onpause`.
     *
     */
    onpause: (()=>void) | null;

    /**
     * @description Binds the service resume handler, equivalent to on("continue", func)
     *
     *      The handler runs on its own fiber when the SCM resumes a paused service. The property
     *      accessor is `oncontinue`.
     *
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
     * @description Binds the service resume handler, equivalent to on("continue", func)
     *
     *      The handler runs on its own fiber when the SCM resumes a paused service. The property
     *      accessor is `oncontinue`.
     *
     */
    oncontinue: (()=>void) | null;

    /**
     * @description Installs the service into the system
     *
     *      Registers the service `name` with the SCM so that it can be started manually or at
     *      boot, and stores `displayName` and `description` for the service manager. `cmd` is the
     *      complete command line to launch, including the executable and its arguments (for
     *      example `process.execPath + ' C:\\srv\\app.js'`). A failure is reported with the Windows
     *      error code of the SCM operation, for example `ERROR_SERVICE_EXISTS` when the name is
     *      already registered or an access-denied error when the process lacks the rights. The
     *      method is Windows-only.
     *      @param name service name
     *      @param cmd command line the SCM will launch
     *      @param displayName name shown by the service manager, empty to use `name`
     *      @param description description shown by the service manager, empty by default
     *
     */
    static install(name: string, cmd: string, displayName?: string, description?: string): void;

    /**
     * @description Uninstalls the service from the system
     *
     *      Removes the service registration from the SCM; it fails with the SCM error when the
     *      service is running or the process has no permission. Windows-only (`Error` 20009
     *      elsewhere).
     *      @param name service name
     *
     */
    static remove(name: string): void;

    /**
     * @description Starts the service
     *
     *      Asks the SCM to launch the registered command line; the new process calls `run` and
     *      the service begins to work. Windows-only (`Error` 20009 elsewhere).
     *      @param name service name
     *
     */
    static start(name: string): void;

    /**
     * @description Stops the service
     *
     *      Sends the stop control to the running service and waits until the SCM reports it as
     *      stopped, so the call may block for a while. Windows-only (`Error` 20009 elsewhere).
     *      @param name service name
     *
     */
    static stop(name: string): void;

    /**
     * @description Restarts the service, equivalent to stop followed by start
     *
     *      Windows-only (`Error` 20009 elsewhere).
     *      @param name service name
     *
     */
    static restart(name: string): void;

    /**
     * @description Checks whether the service is installed
     *
     *      Opens the service in the SCM database and returns true when the registration exists,
     *      no matter whether the service is running. Windows-only (`Error` 20009 elsewhere).
     *      @param name service name
     *      @return true when the service is registered with the SCM
     *
     */
    static isInstalled(name: string): boolean;

    /**
     * @description Checks whether the service is running
     *
     *      Queries the current SCM status of the service and returns true only while it is in the
     *      running state, so a paused service reports false. Windows-only (`Error` 20009
     *      elsewhere).
     *      @param name service name
     *      @return true when the service is running
     *
     */
    static isRunning(name: string): boolean;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/**
 * The promise variant of the Service class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_ServicePromise extends Class_EventEmitter {
    /**
     * @description Creates the service object
     *
     *      `name` is the service name registered with the SCM and is also the initial value of
     *      the `name` property. `worker` is the function executed when the service starts; it is
     *      called without arguments, with the Service object as `this`, and it may block for the
     *      whole life of the service. `event` is an optional map of event names to handlers,
     *      equivalent to calling `on` for each entry. The constructor only creates the object: it
     *      does not install or start anything, and on platforms other than Windows it throws
     *      `Error` (20009).
     *      @param name service name
     *      @param worker function executed when the service starts
     *      @param event map of event handlers to register, for example `{ stop: fn }`
     *
     */
    constructor(name: string, worker: ()=>void, event?: FIBJS.GeneralObject);

    /**
     * @description Runs the service control dispatcher and blocks until the service stops
     *
     *      The call must come from the process that the SCM launched; it connects to the Service
     *      Control Manager and waits for its control events, invoking the worker function when
     *      the service is started and the event handlers when the service is stopped, paused or
     *      continued. The method returns when the service stops, and only one Service may be
     *      running per process. `runAsync()` is the promise form; both are Windows-only (`Error`
     *      20009 elsewhere).
     *
     */
    run(): Promise<void>;

    /**
     * @description Runs the service control dispatcher and blocks until the service stops
     *
     *      The call must come from the process that the SCM launched; it connects to the Service
     *      Control Manager and waits for its control events, invoking the worker function when
     *      the service is started and the event handlers when the service is stopped, paused or
     *      continued. The method returns when the service stops, and only one Service may be
     *      running per process. `runAsync()` is the promise form; both are Windows-only (`Error`
     *      20009 elsewhere).
     *
     */
    runSync(): void;

    /**
     * @description Runs the service control dispatcher and blocks until the service stops
     *
     *      The call must come from the process that the SCM launched; it connects to the Service
     *      Control Manager and waits for its control events, invoking the worker function when
     *      the service is started and the event handlers when the service is stopped, paused or
     *      continued. The method returns when the service stops, and only one Service may be
     *      running per process. `runAsync()` is the promise form; both are Windows-only (`Error`
     *      20009 elsewhere).
     *
     */
    runAsync(): Promise<void>;

    /**
     * @description The service name
     *
     *      The name is the identifier used by the SCM and may be read or replaced at any time;
     *      changing it does not rename an already installed service. This property is available
     *      on every platform on a service object; the creation of that object is the Windows-only
     *      part.
     *
     */
    name: string;

    /**
     * @description Binds the service stop handler, equivalent to on("stop", func)
     *
     *      The handler runs on its own fiber when the SCM stops the service, after which `run`
     *      returns. The property accessor is `onstop`, and the constructor's event map can
     *      register the same handler.
     *
     */
    onstop: (()=>void) | null;

    /**
     * @description Binds the service pause handler, equivalent to on("pause", func)
     *
     *      The handler runs on its own fiber when the SCM pauses the service; use it to suspend
     *      work that should not continue while the service is paused. The property accessor is
     *      `onpause`.
     *
     */
    onpause: (()=>void) | null;

    /**
     * @description Binds the service resume handler, equivalent to on("continue", func)
     *
     *      The handler runs on its own fiber when the SCM resumes a paused service. The property
     *      accessor is `oncontinue`.
     *
     */
    oncontinue: (()=>void) | null;

    /**
     * @description Installs the service into the system
     *
     *      Registers the service `name` with the SCM so that it can be started manually or at
     *      boot, and stores `displayName` and `description` for the service manager. `cmd` is the
     *      complete command line to launch, including the executable and its arguments (for
     *      example `process.execPath + ' C:\\srv\\app.js'`). A failure is reported with the Windows
     *      error code of the SCM operation, for example `ERROR_SERVICE_EXISTS` when the name is
     *      already registered or an access-denied error when the process lacks the rights. The
     *      method is Windows-only.
     *      @param name service name
     *      @param cmd command line the SCM will launch
     *      @param displayName name shown by the service manager, empty to use `name`
     *      @param description description shown by the service manager, empty by default
     *
     */
    static install(name: string, cmd: string, displayName?: string, description?: string): void;

    /**
     * @description Uninstalls the service from the system
     *
     *      Removes the service registration from the SCM; it fails with the SCM error when the
     *      service is running or the process has no permission. Windows-only (`Error` 20009
     *      elsewhere).
     *      @param name service name
     *
     */
    static remove(name: string): void;

    /**
     * @description Starts the service
     *
     *      Asks the SCM to launch the registered command line; the new process calls `run` and
     *      the service begins to work. Windows-only (`Error` 20009 elsewhere).
     *      @param name service name
     *
     */
    static start(name: string): void;

    /**
     * @description Stops the service
     *
     *      Sends the stop control to the running service and waits until the SCM reports it as
     *      stopped, so the call may block for a while. Windows-only (`Error` 20009 elsewhere).
     *      @param name service name
     *
     */
    static stop(name: string): void;

    /**
     * @description Restarts the service, equivalent to stop followed by start
     *
     *      Windows-only (`Error` 20009 elsewhere).
     *      @param name service name
     *
     */
    static restart(name: string): void;

    /**
     * @description Checks whether the service is installed
     *
     *      Opens the service in the SCM database and returns true when the registration exists,
     *      no matter whether the service is running. Windows-only (`Error` 20009 elsewhere).
     *      @param name service name
     *      @return true when the service is registered with the SCM
     *
     */
    static isInstalled(name: string): boolean;

    /**
     * @description Checks whether the service is running
     *
     *      Queries the current SCM status of the service and returns true only while it is in the
     *      running state, so a paused service reports false. Windows-only (`Error` 20009
     *      elsewhere).
     *      @param name service name
     *      @return true when the service is running
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

