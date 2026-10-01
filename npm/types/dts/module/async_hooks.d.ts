/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/AsyncLocalStorage.d.ts" />
/// <reference path="../interface/AsyncResource.d.ts" />
/**
 * @description Defines the asynchronous hooks module
 */
declare module 'async_hooks' {
    /**
     * @description AsyncLocalStorage object, see AsyncLocalStorage
     */
    const AsyncLocalStorage: typeof Class_AsyncLocalStorage;

    /**
     * @description AsyncResource object, see AsyncResource
     */
    const AsyncResource: typeof Class_AsyncResource;

}

