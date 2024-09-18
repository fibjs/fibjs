declare namespace FIBJS {
    type TypedArray = Promise | Int8Array | Uint8Array | Int16Array | Uint16Array | Int32Array | Uint32Array | Uint8ClampedArray | Float32Array | Float64Array;

    interface GeneralObject {
        [k: string]: any;
    }

}

