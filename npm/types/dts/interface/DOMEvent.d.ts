/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description DOMEvent represents a W3C DOM event object
 *
 *  DOMEvent implements the standard Web Event interface, providing standard event properties such as event type, bubbling and cancellation.
 *
 *  ```JavaScript
 *  const ev = new Event('click', { bubbles: true, cancelable: true });
 *  console.log(ev.type);       // 'click'
 *  console.log(ev.bubbles);    // true
 *  console.log(ev.cancelable); // true
 *  ```
 *
 */
declare class Class_DOMEvent extends Class_object {
    /**
     * @description DOMEvent constructor
     *      @param type event type
     *      @param eventInitDict optional event initialization dictionary
     *
     */
    constructor(type: string, eventInitDict?: FIBJS.GeneralObject);

    /**
     * @description Event type
     */
    readonly type: string;

    /**
     * @description Whether the event bubbles
     */
    readonly bubbles: boolean;

    /**
     * @description Whether the event is cancelable
     */
    readonly cancelable: boolean;

    /**
     * @description Whether the event can cross Shadow DOM boundaries
     */
    readonly composed: boolean;

    /**
     * @description Whether preventDefault() has been called
     */
    readonly defaultPrevented: boolean;

    /**
     * @description Event target
     */
    readonly target: any;

    /**
     * @description Current event target
     */
    readonly currentTarget: any;

    /**
     * @description Event creation timestamp
     */
    readonly timeStamp: number;

    /**
     * @description Stops further propagation of the event
     */
    stopPropagation(): void;

    /**
     * @description Prevents other listeners of the same event from being called
     */
    stopImmediatePropagation(): void;

    /**
     * @description Cancels the event if it is cancelable
     */
    preventDefault(): void;

}

