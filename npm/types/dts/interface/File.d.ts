/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description An in-memory file: a Blob with a file name and a modification time
 *
 *  File implements the Web File API on top of Blob, so it has every Blob capability (size,
 *  type, slice, text, arrayBuffer) plus the read-only name and lastModified properties. It is
 *  the value type behind upload, download and FormData flows, and it never touches the disk:
 *  use fs.readFile to load bytes from a real file and wrap them in a File only when the Web API
 *  shape is needed.
 *
 *  Concepts:
 *
 *  - **Blob first**: File only adds metadata to Blob; slice() still returns a Blob, text() and
 *    arrayBuffer() are still promises. `File` is a global class in fibjs.
 *  - **Immutable snapshot**: the parts are concatenated into one buffer at construction. A
 *    string part contributes its utf8 bytes, Buffer/TypedArray/Blob parts contribute their
 *    bytes, and any other value contributes its DOM string form (null becomes 'null'). name and
 *    lastModified are read-only afterwards.
 *  - **Options**: `type` is lowercased and defaults to an empty string; `lastModified` is a
 *    number of milliseconds since the Unix epoch and defaults to the current time. A Date is
 *    rejected, unlike Node.js, where it is converted (plans/compat-differences.md 2.17).
 *  - **Not in Node.js**: the single-object constructor `new File({ data, name, ... })` is a
 *    fibjs extension; Node.js also exports the class as `buffer.File`, which fibjs does not
 *    provide.
 *
 *  Obtained from:
 *  - `new File(blobParts, name[, options])` — an array of parts;
 *  - `new File(blobData, name[, options])` — one Buffer or string, utf8 for a string;
 *  - `new File(options)` — fibjs extension, `data` is required and `name` defaults to '';
 *  - FormData values (`FormData#get`, `FormData#getAll`) are File objects when the entry was
 *    appended as a File.
 *
 *  Example 1 — create a text file and read it back:
 *  ```JavaScript
 *  const file = new File(['hello'], 'greeting.txt', { type: 'text/plain' });
 *  console.log(file.name, file.type, file.size); // greeting.txt text/plain 5
 *
 *  file.text().then((text) => console.log(text)); // hello
 *  ```
 *
 *  Example 2 — Blob inheritance: slice a binary file and read the slice:
 *  ```JavaScript
 *  const file = new File([Buffer.from('hello world')], 'data.bin');
 *  const part = file.slice(6, 11); // slice() still returns a Blob
 *  console.log(part.size, file instanceof Blob); // 5 true
 *
 *  part.text().then((text) => console.log(text)); // world
 *  ```
 *
 *  Example 3 — build a File from one piece of data with explicit metadata:
 *  ```JavaScript
 *  const file = new File({
 *      data: 'notes',
 *      name: 'notes.txt',
 *      lastModified: 1600000000000
 *  });
 *  console.log(file.name, file.size, file.lastModified);
 *  // notes.txt 5 1600000000000
 *  ```
 *
 */
declare class Class_File extends Class_Blob {
    /**
     * @description Creates a File from an array of Blob parts
     *
     *      The parts are concatenated in order into one buffer; see the class for how each part
     *      type contributes bytes. name must be a string and may be empty, while a missing or
     *      non-string name throws a TypeError. options supports the properties below; the type is
     *      lowercased before it is stored.
     *
     *      The other constructors differ only in how the data is passed: one Buffer|String
     *      blobData (string parts are encoded as utf8), or a single options object whose `data`
     *      property is required and whose `name` property defaults to an empty string.
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "type": "", // the MIME type, lowercased; default an empty string
     *          "lastModified": 0 // ms since the Unix epoch; default the current time
     *      })
     *      ```
     *
     *      @param blobParts the initial data array: strings, Buffers, TypedArrays, Blobs and other values
     *      @param name the file name, a string such as "a.txt"
     *      @param options optional parameter object
     *
     */
    constructor(blobParts: any[], name: string, options?: FIBJS.GeneralObject);

    /**
     * @description Creates a File from one Buffer or string
     *
     *      blobData is the whole content: a string is encoded as utf8 and a Buffer is used as it
     *      is. name must be a string and options is the same object as in the parts form. Use this
     *      form when the bytes are already available, for example from fs.readFile.
     *      @param blobData the initial binary data
     *      @param name the file name, a string such as "a.txt"
     *      @param options optional parameter object
     *
     */
    constructor(blobData: Class_Buffer | string, name: string, options?: FIBJS.GeneralObject);

    /**
     * @description Creates a File from a single options object (fibjs extension)
     *
     *      `data` is required and is treated as one Buffer|String part; `name` is optional and
     *      defaults to an empty string, unlike the positional forms. The remaining properties are
     *      the same options as in the parts form. This form is not part of the Web File API or
     *      Node.js.
     *      @param options optional parameter object
     *
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * @description File name, a read-only string
     *
     *      Identifies the file in display, upload and download flows. It is a name, not a path, and
     *      it is never resolved against the file system. Read-only: an assignment throws in strict
     *      mode. Same property as the Web File API and Node.js `buffer.File`.
     *
     */
    readonly name: string;

    /**
     * @description Last modification time in milliseconds since the Unix epoch
     *
     *      Set from the `lastModified` option at construction and defaulted to the current time.
     *      Only a number is accepted; Node.js also converts a Date (plans/compat-differences.md
     *      2.17). Read-only.
     *
     */
    readonly lastModified: number;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Blob.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * The promise variant of the File class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_FilePromise extends Class_BlobPromise {
    /**
     * @description Creates a File from an array of Blob parts
     *
     *      The parts are concatenated in order into one buffer; see the class for how each part
     *      type contributes bytes. name must be a string and may be empty, while a missing or
     *      non-string name throws a TypeError. options supports the properties below; the type is
     *      lowercased before it is stored.
     *
     *      The other constructors differ only in how the data is passed: one Buffer|String
     *      blobData (string parts are encoded as utf8), or a single options object whose `data`
     *      property is required and whose `name` property defaults to an empty string.
     *
     *      options supports the following properties:
     *      ```JavaScript
     *      // fragment: options
     *      ({
     *          "type": "", // the MIME type, lowercased; default an empty string
     *          "lastModified": 0 // ms since the Unix epoch; default the current time
     *      })
     *      ```
     *
     *      @param blobParts the initial data array: strings, Buffers, TypedArrays, Blobs and other values
     *      @param name the file name, a string such as "a.txt"
     *      @param options optional parameter object
     *
     */
    constructor(blobParts: any[], name: string, options?: FIBJS.GeneralObject);

    /**
     * @description Creates a File from one Buffer or string
     *
     *      blobData is the whole content: a string is encoded as utf8 and a Buffer is used as it
     *      is. name must be a string and options is the same object as in the parts form. Use this
     *      form when the bytes are already available, for example from fs.readFile.
     *      @param blobData the initial binary data
     *      @param name the file name, a string such as "a.txt"
     *      @param options optional parameter object
     *
     */
    constructor(blobData: Class_Buffer | string, name: string, options?: FIBJS.GeneralObject);

    /**
     * @description Creates a File from a single options object (fibjs extension)
     *
     *      `data` is required and is treated as one Buffer|String part; `name` is optional and
     *      defaults to an empty string, unlike the positional forms. The remaining properties are
     *      the same options as in the parts form. This form is not part of the Web File API or
     *      Node.js.
     *      @param options optional parameter object
     *
     */
    constructor(options?: FIBJS.GeneralObject);

    /**
     * @description File name, a read-only string
     *
     *      Identifies the file in display, upload and download flows. It is a name, not a path, and
     *      it is never resolved against the file system. Read-only: an assignment throws in strict
     *      mode. Same property as the Web File API and Node.js `buffer.File`.
     *
     */
    readonly name: string;

    /**
     * @description Last modification time in milliseconds since the Unix epoch
     *
     *      Set from the `lastModified` option at construction and defaulted to the current time.
     *      Only a number is accepted; Node.js also converts a Date (plans/compat-differences.md
     *      2.17). Read-only.
     *
     */
    readonly lastModified: number;

}


declare namespace Class_File {
    const promises: FIBJS.GeneralObject;
}
