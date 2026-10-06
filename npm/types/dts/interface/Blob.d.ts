/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description An immutable container of raw bytes, the Web Blob API of fibjs
 *
 *  Blob holds a fixed byte sequence plus a MIME type. The bytes are concatenated and copied at
 *  construction time, so a Blob never changes afterwards and can be passed around, sliced and
 *  re-read at will. It is the binary value type of the Web surface of fibjs: the body helpers of
 *  Message return one, FormData stores file entries as File (a Blob subclass) and
 *  FormData#encode produces one.
 *
 *  File in fibjs is a Blob with a name and a modification time: File extends Blob, every File is
 *  accepted wherever a Blob is expected, and slice() of a File returns a plain Blob. Use Blob for
 *  anonymous binary data and File when a file name matters (uploads, downloads, FormData).
 *
 *  Concepts:
 *
 *  - **Blob parts**: the constructor takes an array of parts - strings (utf8), Buffers,
 *    TypedArrays, ArrayBuffers and other Blobs; a value of any other type falls back to its DOM
 *    string form, so null becomes "null". The parts are concatenated in order.
 *  - **MIME type**: the `type` option is lowercased and exposed by the read-only type property.
 *    fibjs keeps the value as given; the Web standard resets a type containing characters
 *    outside U+0020-U+007E to an empty string (plans/compat-differences.md).
 *  - **Reading**: text() decodes the bytes as utf8 and arrayBuffer() copies them into an
 *    ArrayBuffer, both as promises; the fibjs calling forms textSync()/textAsync() and
 *    arrayBufferSync()/arrayBufferAsync() are generated from the promise declarations.
 *  - **Slicing**: slice() copies a byte range into a new Blob and never returns a File. Indices
 *    follow the Buffer conventions: a negative value counts from the end, an index past the end
 *    clamps, and a start beyond end yields an empty Blob.
 *  - **Not in the standard**: Blob.stream() and Blob.bytes() are not implemented; use
 *    arrayBuffer() and read the bytes from it.
 *
 *  Obtained from:
 *  - `new Blob(blobParts[, options])` — the parts form;
 *  - `new Blob(blobData[, options])` — one Buffer or string (utf8);
 *  - `Blob#slice` — the sliced copy;
 *  - `Message#blob` (`http.Request#blob`, `http.Response#blob`, `mq.Message#blob`) — the body;
 *  - `FormData#encode` — the encoded body;
 *  - `File` — a Blob with metadata, since File extends Blob.
 *
 *  Example 1 — build a Blob from mixed parts and read it:
 *  ```JavaScript
 *  const blob = new Blob(['hello ', new Uint8Array([119, 111, 114, 108, 100])],
 *      { type: 'TEXT/Plain' });
 *
 *  console.log(blob.type, blob.size); // text/plain 11
 *  (async () => {
 *      console.log(await blob.text()); // hello world
 *  })();
 *  ```
 *
 *  Example 2 — slice a byte range and keep or replace the type:
 *  ```JavaScript
 *  const source = new Blob(['hello world'], { type: 'text/plain' });
 *  const part = source.slice(6);          // to the end
 *  const tail = source.slice(-5, 10);     // negative start counts from the end
 *  const typed = source.slice(0, 5, 'text/css');
 *
 *  console.log(part.size, tail.size, typed.size); // 5 4 5
 *  console.log(typed.type);                       // text/css
 *  console.log(tail.textSync());                  // worl
 *  ```
 *
 *  Example 3 — a binary round trip through arrayBuffer:
 *  ```JavaScript
 *  const source = new Blob([new Uint8Array([1, 2, 3])], { type: 'application/octet-stream' });
 *
 *  (async () => {
 *      const buffer = await source.arrayBuffer();
 *      const copy = new Blob([buffer]);
 *      console.log(buffer.byteLength, copy.size); // 3 3
 *      console.log(new Uint8Array(buffer).join(',')); // 1,2,3
 *  })();
 *  ```
 *
 */
declare class Class_Blob extends Class_object {
    /**
     * @description Creates a Blob from an array of parts
     *
     *      The parts are concatenated in order into one buffer: strings contribute their utf8 bytes,
     *      Buffer, TypedArray and ArrayBuffer parts contribute their bytes, and a Blob part
     *      contributes its bytes. Any other value falls back to its DOM string form, so null becomes
     *      "null" and a plain object becomes "[object Object]". The array itself is required: a
     *      missing, null or undefined argument is accepted as an empty Blob, but a non-array value
     *      such as a number or a Set throws TypeError 20005 (the Web BlobPart sequence accepts any
     *      iterable, fibjs requires an array).
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "type": "", // the MIME type, lowercased; default an empty string
     *          "endings": "transparent" // accepted and ignored: line endings are never converted
     *      })
     *      ```
     *
     *      Example — the DOM string fallback of a non-binary part:
     *      ```JavaScript
     *      const blob = new Blob(['a', null, new Uint8Array([98])]);
     *
     *      console.log(blob.size);        // 6 ('a' + 'null' + 'b')
     *      console.log(blob.textSync());  // anullb
     *      ```
     *
     *      @param blobParts the initial parts: strings, Buffers, TypedArrays, ArrayBuffers or Blobs
     *      @param options optional parameter object
     *
     */
    constructor(blobParts?: any[], options?: FIBJS.GeneralObject);

    /**
     * @description Creates a Blob from one Buffer or string
     *
     *      blobData is the whole content: a Buffer is used as it is and a string is encoded as utf8.
     *      The options object is the same as in the parts form, so only `type` has an effect. Use
     *      this form when the bytes are already available, for example from fs.readFile or a Buffer
     *      built by hand.
     *
     *      Example — wrap a Buffer in a typed Blob:
     *      ```JavaScript
     *      const blob = new Blob(Buffer.from('abc'), { type: 'X/Plain' });
     *
     *      console.log(blob.size, blob.type); // 3 x/plain
     *      console.log(blob.textSync());      // abc
     *      ```
     *
     *      @param blobData the initial binary data
     *      @param options optional parameter object
     *
     */
    constructor(blobData: Class_Buffer | string, options?: FIBJS.GeneralObject);

    /**
     * @description The MIME type of the Blob, read-only
     *
     *      The lowercased `type` option given to the constructor, or an empty string when the option
     *      is missing or not a string. fibjs keeps the value as given (apart from the lower case
     *      form); the Web standard resets a type containing characters outside U+0020-U+007E to an
     *      empty string.
     *
     */
    readonly type: string;

    /**
     * @description The byte length of the Blob, read-only
     *
     *      The total size of the concatenated parts; 0 for an empty Blob. It is a plain number and
     *      is recomputed from the stored buffer, so it is constant for the lifetime of the Blob.
     *
     */
    readonly size: number;

    /**
     * @description Returns a new Blob with a copy of a byte range
     *
     *      The bytes of the range are copied; the source Blob is untouched. start and end are byte
     *      indices that follow the Buffer conventions: a negative value counts from the end, an
     *      index past the end clamps to the size, and start beyond end yields an empty Blob. One
     *      fibjs detail: the default end value -1 always means the end of the Blob, so slice(x, -1)
     *      behaves like slice(x) even when -1 is passed explicitly, while any other negative value
     *      counts from the end (the Web standard counts -1 from the end as well). The optional
     *      contentType replaces the type of the result; when it is omitted (or empty) the type of
     *      the source Blob is kept. slice() always returns a plain Blob, even when the source is a
     *      File, so File#slice drops name and lastModified.
     *
     *      Example — extract a range and retype it:
     *      ```JavaScript
     *      const source = new Blob(['abcdef'], { type: 'text/plain' });
     *      const part = source.slice(2, 5, 'text/css');
     *
     *      console.log(part.size, part.type); // 3 text/css
     *      console.log(part.textSync());      // cde
     *      console.log(source.size);          // 6
     *      ```
     *
     *      @param start the start byte index, default 0; a negative value counts from the end
     *      @param end the end byte index (exclusive), default -1 meaning the end of the Blob
     *      @param contentType the MIME type of the new Blob, default keeps the type of the source
     *      @return the new Blob with the copied range
     *
     */
    slice(start?: number, end?: number, contentType?: string): Class_Blob;

    /**
     * @description Reads the whole Blob as text
     *
     *      The bytes are decoded as utf8 and returned as a string; decoding never throws and an
     *      invalid byte sequence becomes the replacement character U+FFFD. The declared calling
     *      form returns a Promise<String>; the generated textSync()/textAsync() aliases are
     *      available too. The stored bytes are read without consuming them, so the method can be
     *      called any number of times.
     *
     *      Example — read a Blob synchronously and asynchronously:
     *      ```JavaScript
     *      const blob = new Blob(['héllo'], { type: 'text/plain' });
     *
     *      console.log(blob.textSync()); // héllo
     *      (async () => {
     *          console.log(await blob.text()); // héllo
     *      })();
     *      ```
     *
     *      @return a Promise that resolves to the text content
     *
     */
    text(): Promise<string>;

    /**
     * @description Reads the whole Blob as text
     *
     *      The bytes are decoded as utf8 and returned as a string; decoding never throws and an
     *      invalid byte sequence becomes the replacement character U+FFFD. The declared calling
     *      form returns a Promise<String>; the generated textSync()/textAsync() aliases are
     *      available too. The stored bytes are read without consuming them, so the method can be
     *      called any number of times.
     *
     *      Example — read a Blob synchronously and asynchronously:
     *      ```JavaScript
     *      const blob = new Blob(['héllo'], { type: 'text/plain' });
     *
     *      console.log(blob.textSync()); // héllo
     *      (async () => {
     *          console.log(await blob.text()); // héllo
     *      })();
     *      ```
     *
     *      @return a Promise that resolves to the text content
     *
     */
    textSync(): string;

    /**
     * @description Reads the whole Blob as text
     *
     *      The bytes are decoded as utf8 and returned as a string; decoding never throws and an
     *      invalid byte sequence becomes the replacement character U+FFFD. The declared calling
     *      form returns a Promise<String>; the generated textSync()/textAsync() aliases are
     *      available too. The stored bytes are read without consuming them, so the method can be
     *      called any number of times.
     *
     *      Example — read a Blob synchronously and asynchronously:
     *      ```JavaScript
     *      const blob = new Blob(['héllo'], { type: 'text/plain' });
     *
     *      console.log(blob.textSync()); // héllo
     *      (async () => {
     *          console.log(await blob.text()); // héllo
     *      })();
     *      ```
     *
     *      @return a Promise that resolves to the text content
     *
     */
    textAsync(): Promise<string>;

    /**
     * @description Reads the whole Blob as an ArrayBuffer
     *
     *      The bytes are copied into a new ArrayBuffer, so the result does not share memory with the
     *      Blob and stays valid after the Blob is collected. An empty Blob yields a zero-length
     *      ArrayBuffer. The declared calling form returns a Promise<ArrayBuffer>; the generated
     *      arrayBufferSync()/arrayBufferAsync() aliases are available too. The MDN members
     *      Blob.bytes() (a Uint8Array view) and Blob.stream() do not exist in fibjs.
     *
     *      @return a Promise that resolves to an ArrayBuffer with the Blob bytes
     *
     */
    arrayBuffer(): Promise<ArrayBuffer>;

    /**
     * @description Reads the whole Blob as an ArrayBuffer
     *
     *      The bytes are copied into a new ArrayBuffer, so the result does not share memory with the
     *      Blob and stays valid after the Blob is collected. An empty Blob yields a zero-length
     *      ArrayBuffer. The declared calling form returns a Promise<ArrayBuffer>; the generated
     *      arrayBufferSync()/arrayBufferAsync() aliases are available too. The MDN members
     *      Blob.bytes() (a Uint8Array view) and Blob.stream() do not exist in fibjs.
     *
     *      @return a Promise that resolves to an ArrayBuffer with the Blob bytes
     *
     */
    arrayBufferSync(): ArrayBuffer;

    /**
     * @description Reads the whole Blob as an ArrayBuffer
     *
     *      The bytes are copied into a new ArrayBuffer, so the result does not share memory with the
     *      Blob and stays valid after the Blob is collected. An empty Blob yields a zero-length
     *      ArrayBuffer. The declared calling form returns a Promise<ArrayBuffer>; the generated
     *      arrayBufferSync()/arrayBufferAsync() aliases are available too. The MDN members
     *      Blob.bytes() (a Uint8Array view) and Blob.stream() do not exist in fibjs.
     *
     *      @return a Promise that resolves to an ArrayBuffer with the Blob bytes
     *
     */
    arrayBufferAsync(): Promise<ArrayBuffer>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the Blob class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_BlobPromise extends Class_object {
    /**
     * @description Creates a Blob from an array of parts
     *
     *      The parts are concatenated in order into one buffer: strings contribute their utf8 bytes,
     *      Buffer, TypedArray and ArrayBuffer parts contribute their bytes, and a Blob part
     *      contributes its bytes. Any other value falls back to its DOM string form, so null becomes
     *      "null" and a plain object becomes "[object Object]". The array itself is required: a
     *      missing, null or undefined argument is accepted as an empty Blob, but a non-array value
     *      such as a number or a Set throws TypeError 20005 (the Web BlobPart sequence accepts any
     *      iterable, fibjs requires an array).
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "type": "", // the MIME type, lowercased; default an empty string
     *          "endings": "transparent" // accepted and ignored: line endings are never converted
     *      })
     *      ```
     *
     *      Example — the DOM string fallback of a non-binary part:
     *      ```JavaScript
     *      const blob = new Blob(['a', null, new Uint8Array([98])]);
     *
     *      console.log(blob.size);        // 6 ('a' + 'null' + 'b')
     *      console.log(blob.textSync());  // anullb
     *      ```
     *
     *      @param blobParts the initial parts: strings, Buffers, TypedArrays, ArrayBuffers or Blobs
     *      @param options optional parameter object
     *
     */
    constructor(blobParts?: any[], options?: FIBJS.GeneralObject);

    /**
     * @description Creates a Blob from one Buffer or string
     *
     *      blobData is the whole content: a Buffer is used as it is and a string is encoded as utf8.
     *      The options object is the same as in the parts form, so only `type` has an effect. Use
     *      this form when the bytes are already available, for example from fs.readFile or a Buffer
     *      built by hand.
     *
     *      Example — wrap a Buffer in a typed Blob:
     *      ```JavaScript
     *      const blob = new Blob(Buffer.from('abc'), { type: 'X/Plain' });
     *
     *      console.log(blob.size, blob.type); // 3 x/plain
     *      console.log(blob.textSync());      // abc
     *      ```
     *
     *      @param blobData the initial binary data
     *      @param options optional parameter object
     *
     */
    constructor(blobData: Class_Buffer | string, options?: FIBJS.GeneralObject);

    /**
     * @description The MIME type of the Blob, read-only
     *
     *      The lowercased `type` option given to the constructor, or an empty string when the option
     *      is missing or not a string. fibjs keeps the value as given (apart from the lower case
     *      form); the Web standard resets a type containing characters outside U+0020-U+007E to an
     *      empty string.
     *
     */
    readonly type: string;

    /**
     * @description The byte length of the Blob, read-only
     *
     *      The total size of the concatenated parts; 0 for an empty Blob. It is a plain number and
     *      is recomputed from the stored buffer, so it is constant for the lifetime of the Blob.
     *
     */
    readonly size: number;

    /**
     * @description Returns a new Blob with a copy of a byte range
     *
     *      The bytes of the range are copied; the source Blob is untouched. start and end are byte
     *      indices that follow the Buffer conventions: a negative value counts from the end, an
     *      index past the end clamps to the size, and start beyond end yields an empty Blob. One
     *      fibjs detail: the default end value -1 always means the end of the Blob, so slice(x, -1)
     *      behaves like slice(x) even when -1 is passed explicitly, while any other negative value
     *      counts from the end (the Web standard counts -1 from the end as well). The optional
     *      contentType replaces the type of the result; when it is omitted (or empty) the type of
     *      the source Blob is kept. slice() always returns a plain Blob, even when the source is a
     *      File, so File#slice drops name and lastModified.
     *
     *      Example — extract a range and retype it:
     *      ```JavaScript
     *      const source = new Blob(['abcdef'], { type: 'text/plain' });
     *      const part = source.slice(2, 5, 'text/css');
     *
     *      console.log(part.size, part.type); // 3 text/css
     *      console.log(part.textSync());      // cde
     *      console.log(source.size);          // 6
     *      ```
     *
     *      @param start the start byte index, default 0; a negative value counts from the end
     *      @param end the end byte index (exclusive), default -1 meaning the end of the Blob
     *      @param contentType the MIME type of the new Blob, default keeps the type of the source
     *      @return the new Blob with the copied range
     *
     */
    slice(start?: number, end?: number, contentType?: string): Class_Blob;

    /**
     * @description Reads the whole Blob as text
     *
     *      The bytes are decoded as utf8 and returned as a string; decoding never throws and an
     *      invalid byte sequence becomes the replacement character U+FFFD. The declared calling
     *      form returns a Promise<String>; the generated textSync()/textAsync() aliases are
     *      available too. The stored bytes are read without consuming them, so the method can be
     *      called any number of times.
     *
     *      Example — read a Blob synchronously and asynchronously:
     *      ```JavaScript
     *      const blob = new Blob(['héllo'], { type: 'text/plain' });
     *
     *      console.log(blob.textSync()); // héllo
     *      (async () => {
     *          console.log(await blob.text()); // héllo
     *      })();
     *      ```
     *
     *      @return a Promise that resolves to the text content
     *
     */
    text(): Promise<string>;

    /**
     * @description Reads the whole Blob as text
     *
     *      The bytes are decoded as utf8 and returned as a string; decoding never throws and an
     *      invalid byte sequence becomes the replacement character U+FFFD. The declared calling
     *      form returns a Promise<String>; the generated textSync()/textAsync() aliases are
     *      available too. The stored bytes are read without consuming them, so the method can be
     *      called any number of times.
     *
     *      Example — read a Blob synchronously and asynchronously:
     *      ```JavaScript
     *      const blob = new Blob(['héllo'], { type: 'text/plain' });
     *
     *      console.log(blob.textSync()); // héllo
     *      (async () => {
     *          console.log(await blob.text()); // héllo
     *      })();
     *      ```
     *
     *      @return a Promise that resolves to the text content
     *
     */
    textSync(): string;

    /**
     * @description Reads the whole Blob as text
     *
     *      The bytes are decoded as utf8 and returned as a string; decoding never throws and an
     *      invalid byte sequence becomes the replacement character U+FFFD. The declared calling
     *      form returns a Promise<String>; the generated textSync()/textAsync() aliases are
     *      available too. The stored bytes are read without consuming them, so the method can be
     *      called any number of times.
     *
     *      Example — read a Blob synchronously and asynchronously:
     *      ```JavaScript
     *      const blob = new Blob(['héllo'], { type: 'text/plain' });
     *
     *      console.log(blob.textSync()); // héllo
     *      (async () => {
     *          console.log(await blob.text()); // héllo
     *      })();
     *      ```
     *
     *      @return a Promise that resolves to the text content
     *
     */
    textAsync(): Promise<string>;

    /**
     * @description Reads the whole Blob as an ArrayBuffer
     *
     *      The bytes are copied into a new ArrayBuffer, so the result does not share memory with the
     *      Blob and stays valid after the Blob is collected. An empty Blob yields a zero-length
     *      ArrayBuffer. The declared calling form returns a Promise<ArrayBuffer>; the generated
     *      arrayBufferSync()/arrayBufferAsync() aliases are available too. The MDN members
     *      Blob.bytes() (a Uint8Array view) and Blob.stream() do not exist in fibjs.
     *
     *      @return a Promise that resolves to an ArrayBuffer with the Blob bytes
     *
     */
    arrayBuffer(): Promise<ArrayBuffer>;

    /**
     * @description Reads the whole Blob as an ArrayBuffer
     *
     *      The bytes are copied into a new ArrayBuffer, so the result does not share memory with the
     *      Blob and stays valid after the Blob is collected. An empty Blob yields a zero-length
     *      ArrayBuffer. The declared calling form returns a Promise<ArrayBuffer>; the generated
     *      arrayBufferSync()/arrayBufferAsync() aliases are available too. The MDN members
     *      Blob.bytes() (a Uint8Array view) and Blob.stream() do not exist in fibjs.
     *
     *      @return a Promise that resolves to an ArrayBuffer with the Blob bytes
     *
     */
    arrayBufferSync(): ArrayBuffer;

    /**
     * @description Reads the whole Blob as an ArrayBuffer
     *
     *      The bytes are copied into a new ArrayBuffer, so the result does not share memory with the
     *      Blob and stays valid after the Blob is collected. An empty Blob yields a zero-length
     *      ArrayBuffer. The declared calling form returns a Promise<ArrayBuffer>; the generated
     *      arrayBufferSync()/arrayBufferAsync() aliases are available too. The MDN members
     *      Blob.bytes() (a Uint8Array view) and Blob.stream() do not exist in fibjs.
     *
     *      @return a Promise that resolves to an ArrayBuffer with the Blob bytes
     *
     */
    arrayBufferAsync(): Promise<ArrayBuffer>;

}


declare namespace Class_Blob {
    const promises: FIBJS.GeneralObject;
}
