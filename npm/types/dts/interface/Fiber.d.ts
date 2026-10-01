/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Fiber operation object; this object cannot be created directly
 *
 *  After creating a fiber with coroutine.start, this object is returned and used for fiber handling and inter-fiber communication.
 *  The fiber main function can access this fiber object through this, or get the current fiber through coroutine.current.
 *  ```JavaScript
 *  function func(v1)
 *  {
 *    console.log(v1 + this.v);
 *  }
 *
 *  var fb = coroutine.start(func,100);
 *
 *  fb.v = 123;
 *
 *  fb.join();
 *  ```
 *
 *  Fiber-local storage is implemented through the shared Fiber object; get the current fiber through coroutine.current and share data by modifying and querying its variables.
 *
 *  ```JavaScript
 *  function func()
 *  {
 *    console.log(coroutine.current().v);
 *  }
 *
 *  coroutine.current().v = 100;
 *
 *  func();
 *  ```
 *
 *  When a fiber is created, the local variables of the current fiber are automatically copied to the new fiber; afterwards, modifications to their respective local variables do not affect each other, unless the variable itself is an object reference.
 *
 *  ```JavaScript
 *  function func()
 *  {
 *    console.log(coroutine.current().v);
 *  }
 *
 *  coroutine.current().v = 100;
 *
 *  var fb = coroutine.start(func);
 *
 *  coroutine.current().v = 200;
 *
 *  fb.join();
 *  ```
 *
 */
declare class Class_Fiber extends Class_object {
    /**
     * @description Waits for the fiber to finish
     */
    join(): void;

    /**
     * @description Queries the unique id of the fiber
     */
    readonly id: number;

    /**
     * @description Queries the call stack of the fiber
     */
    readonly stack: string;

    /**
     * @description Queries the used stack size of the fiber
     */
    readonly stack_usage: number;

}

