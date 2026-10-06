/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/// <reference path="../interface/ZipFile.d.ts" />
/**
 * @description The zip module opens, creates and inspects zip archives and mounts them as a read-only FS
 *  mapping that is then read through the fs module with a `$` suffix
 *
 *  Main capabilities:
 *
 *  - **Opening archives**: `open` opens a path, a Buffer or a SeekableStream in "r", "w" or "a"
 *    mode and returns a ZipFile for reading, writing and extracting entries;
 *  - **Probing files**: `isZipFile` checks whether a file is a zip archive without throwing;
 *  - **Virtual file system**: archive data registered with `fs.setZipFS` is read through a mapping
 *    path with a `$` suffix, and `fs.clearZipFS` removes the mapping (see the fs module).
 *
 *  Concepts:
 *
 *  - **Zip as a virtual file system**: `fs.setZipFS(fname, data)` parses the archive once and keeps
 *    its entries in memory under the normalized path fname; reads of `fname + '$/' + entry` are
 *    then served from that cache and do not touch a real file, so fname does not have to exist on
 *    disk. The mapping is not visible to directory APIs such as `fs.readdir` or `fs.exists`.
 *  - **The `$` suffix**: `$` must be immediately followed by `/`. `/archive.zip$/dir/file.txt`
 *    resolves to the member `dir/file.txt` of the archive mounted at `/archive.zip`; a path
 *    without the slash after `$` or the bare `/archive.zip$/` (empty member) falls back to the
 *    real file system and fails with ENOENT. The `stat`/`lstat`, `readFile` and `createReadStream`
 *    functions and `openFile` with string flags are zip-aware; the integer-flags form of
 *    `openFile` bypasses the mapping and reads the real file system.
 *  - **Lifecycle of mounted archives**: a mapping stays valid until `clearZipFS(fname)` removes one
 *    mapping or `clearZipFS()` removes all of them. The data is pinned at mount time, so rewriting
 *    the source file does not change what the mapping serves. A `$` path that was never mounted is
 *    resolved lazily from the real archive file, whose entries are re-checked at most once every
 *    3 seconds through its mtime.
 *  - **Memory and file archives**: `open` accepts a file path, the archive bytes as a Buffer or any
 *    SeekableStream (for example from `fs.openFile`); a Buffer source is read-only — writes to it
 *    fail, so writes ("w"/"a") need a path or a writable stream. Writes are completed by close,
 *    which appends the central directory that makes the new entries visible.
 *  - **Entry name encoding**: codec is the character set used for entry names, default "utf8";
 *    names are encoded with it when writing and decoded with it when listing, so archives with
 *    legacy names must be opened with the matching codec.
 *
 *  Import:
 *  ```JavaScript
 *  const zip = require('zip');
 *  ```
 *
 *  Example 1 — create an archive and read its entries back:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const zip = require('zip');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zip-'));
 *  fs.writeFileSync(path.join(dir, 'src1.txt'), 'written to disk first');
 *
 *  let zipfile = zip.open(path.join(dir, 'dest.zip'), 'w');
 *  zipfile.write(path.join(dir, 'src1.txt'), 'src1.txt'); // from a file path
 *  zipfile.write(Buffer.from('written from memory'), 'src2.txt'); // from a Buffer
 *  zipfile.close();
 *
 *  zipfile = zip.open(path.join(dir, 'dest.zip')); // "r" by default
 *  console.log(zipfile.namelist().join(', ')); // src1.txt, src2.txt
 *  console.log(zipfile.read('src2.txt').toString()); // written from memory
 *  zipfile.close();
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — build an archive in memory and reopen it from a Buffer:
 *  ```JavaScript
 *  const io = require('io');
 *  const zip = require('zip');
 *
 *  const ms = new io.MemoryStream();
 *  let zipfile = zip.open(ms, 'w');
 *  zipfile.write(Buffer.from('kept in memory'), 'mem.txt');
 *  zipfile.close(); // the archive bytes now live in the stream
 *
 *  ms.rewind();
 *  zipfile = zip.open(ms.readAll()); // reopen the bytes as a Buffer source
 *  console.log(zipfile.namelist().join(', ')); // mem.txt
 *  console.log(zipfile.read('mem.txt').toString()); // kept in memory
 *  zipfile.close();
 *  ms.close();
 *  ```
 *
 *  Example 3 — mount an archive as a virtual file system:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const zip = require('zip');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zip-'));
 *  const file = path.join(dir, 'app.zip');
 *
 *  let zipfile = zip.open(file, 'w');
 *  zipfile.write(Buffer.from('{"name":"fibjs"}'), 'config/app.json');
 *  zipfile.close();
 *
 *  // mount the archive under a virtual name and read it through the "$" suffix
 *  fs.setZipFS('app.zip', fs.readFileSync(file));
 *  console.log(fs.readFile('app.zip$/config/app.json', 'utf8')); // {"name":"fibjs"}
 *  console.log(fs.stat('app.zip$/config/app.json').size); // 16
 *  fs.clearZipFS('app.zip');
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Notes:
 *
 *  - An archive appended after other data is recognized as well, which is how a packed executable
 *    can serve its embedded archive at `process.execPath + '$/...'`.
 *  - The returned ZipFile is only valid until `close()`; operations on a closed or wrongly opened
 *    object throw `ZipFile: file is closed.` (see the ZipFile interface for the entry API).
 *
 */
declare module 'zip' {
    /**
     * @description Determines whether a file is in zip format
     *
     *      The file is probed with the zip reader, so a missing file, a directory, an empty file or any
     *      other non-archive data simply returns false instead of throwing. Data appended after a zip
     *      stream is found as well, because the reader scans for the end-of-central-directory record.
     *
     *      Example — probe a real archive and a text file:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zip-'));
     *
     *      const archive = path.join(dir, 'a.zip');
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close();
     *
     *      const text = path.join(dir, 'a.txt');
     *      fs.writeFileSync(text, 'not an archive');
     *
     *      console.log(zip.isZipFile(archive)); // true
     *      console.log(zip.isZipFile(text)); // false
     *      console.log(zip.isZipFile(path.join(dir, 'missing.zip'))); // false
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename file name
     *      @return returns true if the file is a zip file
     *
     */
    function isZipFile(filename: string): boolean;

    function isZipFile(filename: string, callback: (err: Error | undefined | null, retVal: boolean)=>any): void;

    /**
     * @description Determines whether a file is in zip format
     *
     *      The file is probed with the zip reader, so a missing file, a directory, an empty file or any
     *      other non-archive data simply returns false instead of throwing. Data appended after a zip
     *      stream is found as well, because the reader scans for the end-of-central-directory record.
     *
     *      Example — probe a real archive and a text file:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zip-'));
     *
     *      const archive = path.join(dir, 'a.zip');
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close();
     *
     *      const text = path.join(dir, 'a.txt');
     *      fs.writeFileSync(text, 'not an archive');
     *
     *      console.log(zip.isZipFile(archive)); // true
     *      console.log(zip.isZipFile(text)); // false
     *      console.log(zip.isZipFile(path.join(dir, 'missing.zip'))); // false
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename file name
     *      @return returns true if the file is a zip file
     *
     */
    function isZipFileSync(filename: string): boolean;

    /**
     * @description Determines whether a file is in zip format
     *
     *      The file is probed with the zip reader, so a missing file, a directory, an empty file or any
     *      other non-archive data simply returns false instead of throwing. Data appended after a zip
     *      stream is found as well, because the reader scans for the end-of-central-directory record.
     *
     *      Example — probe a real archive and a text file:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zip-'));
     *
     *      const archive = path.join(dir, 'a.zip');
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close();
     *
     *      const text = path.join(dir, 'a.txt');
     *      fs.writeFileSync(text, 'not an archive');
     *
     *      console.log(zip.isZipFile(archive)); // true
     *      console.log(zip.isZipFile(text)); // false
     *      console.log(zip.isZipFile(path.join(dir, 'missing.zip'))); // false
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename file name
     *      @return returns true if the file is a zip file
     *
     */
    function isZipFileAsync(filename: string): Promise<boolean>;

    /**
     * @description Opens a zip file
     *
     *      data may be the path of the archive, the archive bytes as a Buffer, or a SeekableStream
     *      (for example the result of `fs.openFile` on a file). Reading works on all three sources;
     *      writing ("w"/"a") needs a path or a writable stream, because the bytes written to a Buffer
     *      source cannot be read back.
     *
     *      mod selects the mode: "r" reads an existing archive (the default), "w" creates or truncates
     *      the target, and "a"/"a+" appends new entries to an existing archive, throwing
     *      `ZipFile: zip file not exists!` if the target is missing. Any other mode returns an object
     *      whose operations fail with `ZipFile: file is closed.`; a missing file in "r" mode reports
     *      ENOENT.
     *
     *      codec is the character set used to decode entry names while reading and to encode them while
     *      writing (default "utf8"); an unknown codec throws `encoding: Unknown charset`.
     *
     *      The returned ZipFile lives until `ZipFile.close()`, which also writes the central directory
     *      that makes the new entries visible to other readers.
     *
     *      Example — create, append and read an archive:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zip-'));
     *      const file = path.join(dir, 'log.zip');
     *
     *      let zipfile = zip.open(file, 'w');
     *      zipfile.write(Buffer.from('first'), 'first.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(file, 'a'); // append to the existing archive
     *      zipfile.write(Buffer.from('second'), 'second.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(file); // "r" is the default
     *      console.log(zipfile.namelist().join(', ')); // first.txt, second.txt
     *      console.log(zipfile.read('second.txt').toString()); // second
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the zip file: a path, a Buffer or a SeekableStream
     *      @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     *      @param codec sets the encoding of the zip file, default "utf8"
     *      @return returns the zip file object
     *
     */
    function open(data: Class_Buffer | Class_SeekableStream | Class_SeekableStreamPromise | string, mod?: string, codec?: string): Class_ZipFile;

    function open(data: Class_Buffer | Class_SeekableStream | Class_SeekableStreamPromise | string, mod?: string, codec?: string, callback: (err: Error | undefined | null, retVal: Class_ZipFile)=>any): void;

    /**
     * @description Opens a zip file
     *
     *      data may be the path of the archive, the archive bytes as a Buffer, or a SeekableStream
     *      (for example the result of `fs.openFile` on a file). Reading works on all three sources;
     *      writing ("w"/"a") needs a path or a writable stream, because the bytes written to a Buffer
     *      source cannot be read back.
     *
     *      mod selects the mode: "r" reads an existing archive (the default), "w" creates or truncates
     *      the target, and "a"/"a+" appends new entries to an existing archive, throwing
     *      `ZipFile: zip file not exists!` if the target is missing. Any other mode returns an object
     *      whose operations fail with `ZipFile: file is closed.`; a missing file in "r" mode reports
     *      ENOENT.
     *
     *      codec is the character set used to decode entry names while reading and to encode them while
     *      writing (default "utf8"); an unknown codec throws `encoding: Unknown charset`.
     *
     *      The returned ZipFile lives until `ZipFile.close()`, which also writes the central directory
     *      that makes the new entries visible to other readers.
     *
     *      Example — create, append and read an archive:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zip-'));
     *      const file = path.join(dir, 'log.zip');
     *
     *      let zipfile = zip.open(file, 'w');
     *      zipfile.write(Buffer.from('first'), 'first.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(file, 'a'); // append to the existing archive
     *      zipfile.write(Buffer.from('second'), 'second.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(file); // "r" is the default
     *      console.log(zipfile.namelist().join(', ')); // first.txt, second.txt
     *      console.log(zipfile.read('second.txt').toString()); // second
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the zip file: a path, a Buffer or a SeekableStream
     *      @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     *      @param codec sets the encoding of the zip file, default "utf8"
     *      @return returns the zip file object
     *
     */
    function openSync(data: Class_Buffer | Class_SeekableStream | Class_SeekableStreamPromise | string, mod?: string, codec?: string): Class_ZipFile;

    /**
     * @description Opens a zip file
     *
     *      data may be the path of the archive, the archive bytes as a Buffer, or a SeekableStream
     *      (for example the result of `fs.openFile` on a file). Reading works on all three sources;
     *      writing ("w"/"a") needs a path or a writable stream, because the bytes written to a Buffer
     *      source cannot be read back.
     *
     *      mod selects the mode: "r" reads an existing archive (the default), "w" creates or truncates
     *      the target, and "a"/"a+" appends new entries to an existing archive, throwing
     *      `ZipFile: zip file not exists!` if the target is missing. Any other mode returns an object
     *      whose operations fail with `ZipFile: file is closed.`; a missing file in "r" mode reports
     *      ENOENT.
     *
     *      codec is the character set used to decode entry names while reading and to encode them while
     *      writing (default "utf8"); an unknown codec throws `encoding: Unknown charset`.
     *
     *      The returned ZipFile lives until `ZipFile.close()`, which also writes the central directory
     *      that makes the new entries visible to other readers.
     *
     *      Example — create, append and read an archive:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zip-'));
     *      const file = path.join(dir, 'log.zip');
     *
     *      let zipfile = zip.open(file, 'w');
     *      zipfile.write(Buffer.from('first'), 'first.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(file, 'a'); // append to the existing archive
     *      zipfile.write(Buffer.from('second'), 'second.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(file); // "r" is the default
     *      console.log(zipfile.namelist().join(', ')); // first.txt, second.txt
     *      console.log(zipfile.read('second.txt').toString()); // second
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the zip file: a path, a Buffer or a SeekableStream
     *      @param mod open mode, "r" for reading, "w" for creating, "a" for appending after the zip file
     *      @param codec sets the encoding of the zip file, default "utf8"
     *      @return returns the zip file object
     *
     */
    function openAsync(data: Class_Buffer | Class_SeekableStream | Class_SeekableStreamPromise | string, mod?: string, codec?: string): Promise<Class_ZipFilePromise>;

}

