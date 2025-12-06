/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/AsyncLocalStorage.d.ts" />
/**
 * @description 定义异步钩子模块 
 */
declare module 'async_hooks' {
    /**
     * @description AsyncLocalStorage 对象，参见 AsyncLocalStorage 
     */
    const AsyncLocalStorage: typeof Class_AsyncLocalStorage;

}

