/// <reference path="../_import/_fibjs.d.ts" />
// Hand-written `buffer` module (no IDL unit: fibjs/scripts/internal/buffer.js
// provides the class and the module object).
//
// The runtime module is `{ Buffer, SlowBuffer, constants, kMaxLength,
// kStringMaxLength, INSPECT_MAX_BYTES, transcode, atob, btoa, isUtf8, isAscii,
// Blob }`; `Buffer` is the *same* class as the global one
// (fibjs/scripts/internal/buffer.js, `class Buffer extends Uint8Array`), so the
// declaration points at the existing Class_Buffer instead of re-declaring it.

declare module 'buffer' {
    /** The Buffer class: `require('buffer').Buffer === Buffer` (the global). */
    const Buffer: typeof Class_Buffer;

    /**
     * The historical alias of Buffer. Node deprecated it in favour of
     * `Buffer.allocUnsafeSlow()`; fibjs keeps it as a separate binding to the
     * same class, so `new SlowBuffer(n)` still works.
     */
    const SlowBuffer: {
        new (size: number): Class_Buffer;
        (size: number): Class_Buffer;
    };

    /** `{ MAX_LENGTH, MAX_STRING_LENGTH }`: the Node buffer constants. */
    const constants: {
        MAX_LENGTH: number;
        MAX_STRING_LENGTH: number;
    };

    /** The maximum length of a Buffer instance (2^53 - 1). */
    const kMaxLength: number;

    /** The maximum length of a string (2^29 - 24). */
    const kStringMaxLength: number;

    /** How many bytes `util.inspect()` shows before truncating (50). */
    let INSPECT_MAX_BYTES: number;

    /**
     * Re-encodes a buffer from one encoding to another
     * (`buffer.transcode(source, fromEnc, toEnc)`); the source is not modified.
     */
    function transcode(source: Uint8Array, fromEnc: string, toEnc: string): Class_Buffer;

    /** Decodes a base64 string into a Buffer (a global in fibjs as well). */
    function atob(data: string): Class_Buffer;

    /** Encodes a Buffer (or a string, taken as utf8) into base64. */
    function btoa(data: string | Uint8Array): string;

    /** Whether the buffer is valid UTF-8. */
    function isUtf8(input: Uint8Array | ArrayBufferView | ArrayBufferLike): boolean;

    /** Whether every byte is ASCII (0x00 - 0x7F). */
    function isAscii(input: Uint8Array | ArrayBufferView | ArrayBufferLike): boolean;

    /**
     * The Blob class, the same one the global `Blob` is. The type twin is what
     * lets `import { Blob as B } from 'buffer'` be used in a type position too.
     */
    const Blob: typeof Class_Blob;
    type Blob = Class_Blob;

    export {
        Buffer,
        SlowBuffer,
        constants,
        kMaxLength,
        kStringMaxLength,
        INSPECT_MAX_BYTES,
        transcode,
        atob,
        btoa,
        isUtf8,
        isAscii,
        Blob,
    };
}
