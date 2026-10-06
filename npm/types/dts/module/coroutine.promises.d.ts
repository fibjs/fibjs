/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/// <reference path="../interface/Semaphore.d.ts" />
/// <reference path="../interface/Condition.d.ts" />
/// <reference path="../interface/Event.d.ts" />
/// <reference path="../interface/Fiber.d.ts" />
/**
 * The promise variant of the coroutine module: async members return a Promise as their primary form.
 */
declare module 'coroutine/promises' {
    /**
     * @description Reference to the Lock class
     *
     *      A Lock provides mutual exclusion between fibers: one fiber owns it at a time, other fibers that
     *      call `acquire` wait until it is released, and the same fiber may acquire it more than once. Use
     *      a lock around any state that several fibers read and write across a suspension point; see the
     *      Lock class.
     *
     */
    const Lock: typeof Class_Lock;

    /**
     * @description Reference to the Semaphore class
     *
     *      A Semaphore is a counting lock: `post` adds a permit, `wait` consumes one and waits when none
     *      is left, and permits may be posted by any fiber instead of only by the owner. Use it to limit
     *      concurrency or to hand work between fibers; see the Semaphore class.
     *
     */
    const Semaphore: typeof Class_Semaphore;

    /**
     * @description Reference to the Condition class
     *
     *      A Condition lets fibers wait until shared state becomes true: `wait` releases its lock and
     *      parks the fiber, and `notify`/`notifyAll` wake the waiters after the state has been changed.
     *      Use it instead of polling with `sleep`; see the Condition class.
     *
     */
    const Condition: typeof Class_Condition;

    /**
     * @description Reference to the Event class
     *
     *      An Event is a broadcast gate: `wait` parks a fiber until `set` is called, and one `set` wakes
     *      every waiter at once. It carries no count and no payload, so it is the simplest way to start or
     *      finish a group of fibers; see the Event class.
     *
     */
    const Event: typeof Class_Event;

    /**
     * @description Starts a fiber and returns its Fiber object
     *
     *      The new fiber is queued, not executed immediately: it starts running when the current fiber
     *      yields — for example by calling `sleep` or `join` — or when the current script ends. The values
     *      in `args` are passed to `func` as they are, and inside the fiber `this` is the returned Fiber
     *      object, so properties set on that object before or after `start` are its fiber-local storage.
     *
     *      An exception that escapes `func` does not propagate to the caller or to `join`: it is printed
     *      to stderr with the fiber stack, the fiber ends, and the rest of the process keeps running.
     *
     *      Example — arguments, fiber-local state and the deferred start:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const order = [];
     *
     *      const task = coroutine.start(function (a, b) {
     *          order.push('fiber: ' + (a + b));
     *          order.push('fiber reads this.state: ' + this.state);
     *          this.result = a + b;
     *      }, 100, 200);
     *
     *      task.state = 'ready';
     *      order.push('main continues before the fiber starts');
     *      task.join();
     *      order.push('joined with result ' + task.result);
     *
     *      console.log(order.join('\n'));
     *      ```
     *      will output:
     *      ```sh
     *      main continues before the fiber starts
     *      fiber: 300
     *      fiber reads this.state: ready
     *      joined with result 300
     *      ```
     *
     *      @param func the function executed by the new fiber
     *      @param args arguments passed to the function inside the new fiber
     *      @return the Fiber object of the new fiber
     *
     */
    function start(func: (...args: any[])=>void, ...args: any[]): Class_Fiber;

    /**
     * @description Runs a set of functions in parallel and returns their results
     *
     *      The four call forms of `parallel` are:
     *
     *      - `parallel(funcs, fibers)` — an array of functions, each called without arguments;
     *      - `parallel(datas, func, fibers)` — `func(data)` is called once per element of the data array;
     *      - `parallel(func, num, fibers)` — `func(index)` is called `num` times with 0-based indexes;
     *      - `parallel(...funcs)` — the functions given directly as arguments.
     *
     *      The calling fiber blocks until every task has finished, and the result array keeps the input
     *      order no matter in which order the tasks complete. A task that returns a value stores it in
     *      the array; a task that returns nothing leaves the slot `undefined`. An empty input returns an
     *      empty array. `fibers` limits how many tasks run at the same time; values that are not positive
     *      or that exceed the number of tasks mean "one fiber per task". An entry that is not a function,
     *      or a non-array first argument, throws `TypeError` (20004).
     *
     *      The tasks run on fibers of the same isolate, so they are concurrent but not parallel:
     *      CPU-bound tasks still share the one JavaScript thread. Use worker_threads when several cores
     *      are required. The async context of the calling fiber (see AsyncLocalStorage) is propagated to
     *      the tasks.
     *
     *      If a task throws, its error is printed as an uncaught fiber exception, the remaining tasks
     *      still run to completion, and this call then throws `Error` (20020, internal error); the
     *      individual results are lost.
     *
     *      @param funcs array of functions to run in parallel
     *      @param fibers maximum number of concurrent fibers, one per task by default
     *      @return array of results in the order of the input
     *
     */
    function parallel(funcs: any[], fibers?: number): any[];

    /**
     * @description Runs a function over a set of data in parallel and returns the results
     *
     *      `func` is called once per element of the data array and receives that element as its only
     *      argument; the results stay in the order of the input. The optional `fibers` argument limits the
     *      concurrency exactly like the array-of-functions form; see `parallel(funcs, fibers)` for the
     *      scheduling, error and concurrency rules.
     *
     *      Example — results keep the input order even though the tasks finish out of order:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const result = coroutine.parallel([5, 1, 4, 2], function (v) {
     *          coroutine.sleep(v * 5); // the first task takes the longest
     *          return v;
     *      });
     *
     *      console.log('result:', result.join(','));
     *      ```
     *      will output:
     *      ```sh
     *      result: 5,1,4,2
     *      ```
     *
     *      @param datas array of data processed in parallel
     *      @param func the function called once per data element
     *      @param fibers maximum number of concurrent fibers, one per element by default
     *      @return array of results in the order of the input
     *
     */
    function parallel(datas: any[], func: (data: any)=>any, fibers?: number): any[];

    /**
     * @description Runs a function num times in parallel and returns the results
     *
     *      `func` is called `num` times and receives the task index (`0` … `num - 1`) as its only
     *      argument; the results stay in index order. The optional `fibers` argument limits the
     *      concurrency exactly like the array-of-functions form; see `parallel(funcs, fibers)` for the
     *      scheduling, error and concurrency rules. A `num` of 0 returns an empty array.
     *
     *      @param func the function called once per index
     *      @param num number of tasks
     *      @param fibers maximum number of concurrent fibers, one per task by default
     *      @return array of results in index order
     *
     */
    function parallel(func: (index: number)=>any, num: number, fibers?: number): any[];

    /**
     * @description Runs the given functions in parallel and returns their results
     *
     *      Each argument is a function without parameters; the call is equivalent to
     *      `parallel([func1, func2, ...])`, and the results stay in argument order; see
     *      `parallel(funcs, fibers)` for the scheduling, error and concurrency rules.
     *      @param funcs the functions to run in parallel
     *      @return array of results in argument order
     *
     */
    function parallel(...funcs: any[]): any[];

    /**
     * @description Returns the Fiber object of the calling fiber
     *
     *      Inside a fiber the returned object is the one that `coroutine.start` returned and that the
     *      fiber function receives as `this`, so it can be used directly as fiber-local storage. In the
     *      main script the returned object is the main fiber, which is also listed in `fibers`.
     *
     *      Example — the current fiber is the same object as the one held by the caller:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      function worker() {
     *          const self = coroutine.current();
     *          self.name = 'worker-1';
     *          console.log('same object as this:', self === this);
     *      }
     *
     *      const task = coroutine.start(worker);
     *      task.join();
     *      console.log('property visible outside:', task.name);
     *      ```
     *      will output:
     *      ```sh
     *      same object as this: true
     *      property visible outside: worker-1
     *      ```
     *
     *      @return the Fiber object of the calling fiber
     *
     */
    function current(): Class_Fiber;

    /**
     * @description Pauses the current fiber for the specified time
     *
     *      While the fiber is suspended the runtime runs the other fibers, and pending timers keep the
     *      process alive until the sleep finishes. `ms` defaults to 0: a value that is not positive
     *      (`0`, a negative number or no argument at all) does not wait but simply yields the CPU for one
     *      scheduling round. `sleepAsync(ms)` is the promise form and does not block the calling fiber.
     *
     *      Example — yielding with no delay and sleeping with a deadline:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      let ran = false;
     *      coroutine.start(() => { ran = true; });
     *
     *      coroutine.sleep(); // no argument: yield the CPU once
     *      console.log('queued fiber had a chance to run:', ran);
     *
     *      const start = Date.now();
     *      coroutine.sleep(30);
     *      console.log('slept for at least 25ms:', Date.now() - start >= 25);
     *      ```
     *      will output:
     *      ```sh
     *      queued fiber had a chance to run: true
     *      slept for at least 25ms: true
     *      ```
     *
     *      @param ms pause time in milliseconds; 0 or less only yields the CPU
     *
     */
    function sleep(ms?: number): Promise<void>;

    /**
     * @description Pauses the current fiber for the specified time
     *
     *      While the fiber is suspended the runtime runs the other fibers, and pending timers keep the
     *      process alive until the sleep finishes. `ms` defaults to 0: a value that is not positive
     *      (`0`, a negative number or no argument at all) does not wait but simply yields the CPU for one
     *      scheduling round. `sleepAsync(ms)` is the promise form and does not block the calling fiber.
     *
     *      Example — yielding with no delay and sleeping with a deadline:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      let ran = false;
     *      coroutine.start(() => { ran = true; });
     *
     *      coroutine.sleep(); // no argument: yield the CPU once
     *      console.log('queued fiber had a chance to run:', ran);
     *
     *      const start = Date.now();
     *      coroutine.sleep(30);
     *      console.log('slept for at least 25ms:', Date.now() - start >= 25);
     *      ```
     *      will output:
     *      ```sh
     *      queued fiber had a chance to run: true
     *      slept for at least 25ms: true
     *      ```
     *
     *      @param ms pause time in milliseconds; 0 or less only yields the CPU
     *
     */
    function sleepSync(ms?: number): void;

    /**
     * @description Pauses the current fiber for the specified time
     *
     *      While the fiber is suspended the runtime runs the other fibers, and pending timers keep the
     *      process alive until the sleep finishes. `ms` defaults to 0: a value that is not positive
     *      (`0`, a negative number or no argument at all) does not wait but simply yields the CPU for one
     *      scheduling round. `sleepAsync(ms)` is the promise form and does not block the calling fiber.
     *
     *      Example — yielding with no delay and sleeping with a deadline:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      let ran = false;
     *      coroutine.start(() => { ran = true; });
     *
     *      coroutine.sleep(); // no argument: yield the CPU once
     *      console.log('queued fiber had a chance to run:', ran);
     *
     *      const start = Date.now();
     *      coroutine.sleep(30);
     *      console.log('slept for at least 25ms:', Date.now() - start >= 25);
     *      ```
     *      will output:
     *      ```sh
     *      queued fiber had a chance to run: true
     *      slept for at least 25ms: true
     *      ```
     *
     *      @param ms pause time in milliseconds; 0 or less only yields the CPU
     *
     */
    function sleepAsync(ms?: number): Promise<void>;

    /**
     * @description Returns the live fibers of the current isolate
     *
     *      The result is a snapshot array that includes the calling fiber; it shrinks as fibers finish and
     *      grows as they are started. A fiber remains listed until its function returns or throws.
     *
     *      Example — inspecting the live fibers:
     *      ```JavaScript
     *      const coroutine = require('coroutine');
     *
     *      const task = coroutine.start(function () { coroutine.sleep(40); });
     *      coroutine.sleep(); // let the new fiber start
     *      const live = coroutine.fibers;
     *
     *      console.log('includes the current fiber:', live.indexOf(coroutine.current()) >= 0);
     *      console.log('includes the new fiber:', live.some((fb) => fb.id === task.id));
     *
     *      task.join();
     *      console.log('live fibers now:', coroutine.fibers.length);
     *      ```
     *      will output:
     *      ```sh
     *      includes the current fiber: true
     *      includes the new fiber: true
     *      live fibers now: 1
     *      ```
     *
     */
    const fibers: any[];

    /**
     * @description Maximum number of idle worker fibers kept per isolate; the default is 256
     *
     *      The runtime caches the fibers whose jobs have finished and reuses them for the next job instead
     *      of creating an OS-level fiber each time. A larger pool absorbs bursts of short jobs, such as
     *      server request handlers, at the cost of idle stacks; a smaller pool lowers the memory watermark
     *      but makes new jobs allocate a fiber. The value is a process-wide setting (each isolate applies
     *      it to its own pool) and is not validated, so keep it non-negative. It is a fibjs extension
     *      with no Node.js equivalent.
     *
     */
    var spareFibers: number;

    /**
     * @description Id of the current isolate
     *
     *      The main process is isolate 1, and every Worker created with worker_threads receives the next
     *      number, so `vmid` distinguishes the isolates of a process. It is typically used to derive
     *      per-isolate resources such as the ports or directories of a test run.
     *
     */
    const vmid: number;

}


declare module "coroutine" {
    const promises: typeof import("coroutine/promises");
}
