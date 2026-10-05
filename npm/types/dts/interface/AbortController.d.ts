/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/AbortSignal.d.ts" />
/**
 * @description The controller object used to abort one or more Web requests on demand
 *
 * The AbortController object is a global base class and can be created at any time directly with new AbortController():
 *  ```JavaScript
 *  var buf = new AbortController();
 *  ```
 *
 */
declare class Class_AbortController extends Class_object {
    /**
     * @description The AbortSignal object used to abort one or more Web requests
     */
    constructor();

    /**
     * @description The AbortSignal object used to abort one or more Web requests
     */
    readonly signal: Class_AbortSignal;

    /**
     * @description Aborts one or more Web requests
     *      reason may be a string, or a value of any type.
     *      @param reason the reason for aborting the request
     *
     */
    abort(reason?: string | any): void;

}

