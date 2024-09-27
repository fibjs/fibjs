/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * @description 条件变量对象
 * 
 *  条件变量是利用纤程间共享的全局变量来进行同步的一种机制，主要包括两个动作：
 *  1）一个线程等待某个条件成立，而将自己挂起；
 *  2）另一个线程使条件成立，并通知等待的纤程向下执行。
 * 
 *  为了防止竞争，每个条件变量都需要一个Lock的配合（Lock可自行显式创建并传递进来，也可交由fibjs为您创建）
 * 
 *  通过使用条件变量，可以利用一个条件变量控制一批纤程的开关；
 * 
 *  以下是两个纤程调度的实例：
 *  ```JavaScript
 *  var coroutine = require("coroutine");
 *  var cond = new coroutine.Condition();
 *  var ready = false;
 *  var state = "ready";
 * 
 *  function funcwait() {
 *     cond.acquire();
 *     while (!ready)
 *         cond.wait();
 *     state = "go"
 *     cond.release();
 *  }
 * 
 *  coroutine.start(funcwait);
 * 
 *  cond.acquire();
 *  console.log(state)
 *  ready = true;
 *  cond.notify();
 *  coroutine.sleep();
 *  console.log(state);
 *  ```
 *  will output:
 *  ```sh
 *  ready
 *  go
 *  ```
 *  
 */
declare class Class_Condition extends Class_Lock {
    /**
     * @description 条件变量构造函数（条件变量所需的锁由fibjs内部构造） 
     */
    constructor();

    /**
     * @description 条件变量构造函数
     *      @param lock 使用自行构造的锁
     *      
     */
    constructor(lock: Class_Lock);

    /**
     * @description 等待一个条件变量
     *      @param timeout 指定超时时间，单位毫秒，缺省为 -1，表示永不超时。
     *      @return 获取成功则返回 true，超时返回 false
     *     
     */
    wait(timeout?: number): boolean;

    wait(timeout?: number, callback?: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description 通知一个被阻塞的纤程（最后加入纤程池的）向下继续执行 
     */
    notify(): void;

    /**
     * @description 通知所有被阻塞的纤程向下继续执行 
     */
    notifyAll(): void;

}

