/// <reference path="../interface/Handler.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
declare namespace FIBJS {
    type TypedArray = Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Uint8ClampedArray | Float32Array | Float64Array | BigInt64Array | BigUint64Array;

    interface GeneralObject {
        [k: string]: any;
    }

    interface EventMap {
        [pattern: string]: (...args: any[])=>any;
    }

    interface HandlerMap {
        [pattern: string]: Class_Handler | Class_HandlerPromise | any[] | FIBJS.HandlerMap | string | ((req: any, ...params: any[])=>void);
    }

    interface HeaderMap {
        [pattern: string]: string | string[] | number | boolean | Date;
    }

    interface KVMap {
        [pattern: string]: string | Class_Buffer;
    }

    interface StringMap {
        [pattern: string]: string;
    }

    interface ValueMap {
        [pattern: string]: any;
    }

}

