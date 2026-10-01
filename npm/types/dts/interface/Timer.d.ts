/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/**
 * @description Timer handler object
 */
declare class Class_Timer extends Class_object {
    /**
     * @description Keeps the fibjs process alive; prevents the fibjs process from exiting during the timer wait
     *      @return returns the timer object
     *
     */
    ref(): Class_Timer;

    /**
     * @description Allows the fibjs process to exit; permits the fibjs process to exit during the timer wait
     *      @return returns the timer object
     *
     */
    unref(): Class_Timer;

    /**
     * @description Cancels the current timer
     */
    clear(): void;

    /**
     * @description Queries whether the current timer has been stopped
     */
    readonly stopped: boolean;

}

