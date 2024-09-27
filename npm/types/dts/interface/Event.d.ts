/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Lock.d.ts" />
/**
 * @description Event 模块提供了一个事件对象，用于协同共享数据操作。它允许多个纤程（协程）之间进行同步操作，以实现协作式多任务。事件对象有 wait，pulse 和 clear 三个方法，wait 方法将阻塞当前纤程，直至事件被触发，pulse 方法将唤醒所有等待该事件的纤程，而 clear 方法则将事件标志重置为 false。通过使用 coroutine.Event 模块，开发者可以控制纤程之间的执行顺序和数据共享，实现复杂业务逻辑。
 * 
 * 举个例子，假设我们需要两个纤程间进行数据共享，然而它们执行的顺序是不确定的，你可以通过一个事件对象，控制纤程的执行顺序，保证一个纤程执行之前，触发另一个纤程的事件。
 * ```JavaScript
 * const coroutine = require('coroutine');
 * 
 * var evt = new coroutine.Event();
 * 
 * coroutine.start(function() {
 *    console.log('[1] wait for event');
 *    evt.wait();
 *    console.log('[1] receive event');
 * });
 * 
 * coroutine.start(function() {
 *    loop:for (var i = 0; i < 10; i++) {
 *       console.log('[2] do some work');
 *       if (i === 5) {
 *          evt.pulse();
 *       }
 *       coroutine.sleep(1000);
 *    }
 * });
 * ```
 * 在上面的例子中，我们创建了一个事件对象 evt，在纤程1中使用wait方法等待事件触发，而在纤程2中通过pulse方法触发事件，当i等于5的时候，纤程2触发事件，纤程1通过事件监听被唤醒，继续执行。在这个过程中，两个纤程之间没有使用任何的锁或其它同步工具，但是它们在纤程级别上保证了数据的同步。
 *  
 */
declare class Class_Event extends Class_Lock {
    /**
     * @description 事件对象构造函数
     *      @param value 指定是否等待，为 true 时等待，缺省为 false
     *      
     */
    constructor(value?: boolean);

    /**
     * @description 判断事件对象是否为真
     *      @return 如果事件为真，返回 true
     *      
     */
    isSet(): boolean;

    /**
     * @description 激活事件（将事件状态改为true），并调用pulse() 
     */
    set(): void;

    /**
     * @description 激活等待该事件的所有纤程 
     */
    pulse(): void;

    /**
     * @description 重置事件（将事件状态改为false） 
     */
    clear(): void;

    /**
     * @description 等待一个事件 
     */
    wait(): void;

    wait(callback: (err: Error | undefined | null)=>any): void;

}

