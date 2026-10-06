/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description Binary file stream: reads, writes and positions one open file
 *
 *  FileStream is the stream behind fs.openFile, fs.createReadStream and
 *  fs.createWriteStream. It owns an open file descriptor, so the operating system
 *  keeps the position: each read/write advances it, seek/tell/rewind move it, and
 *  size/truncate/eof inspect the file. The class is not exposed as a global in
 *  fibjs; for descriptor calls without stream behavior use the FileHandle that
 *  fs.open returns instead.
 *
 *  Concepts:
 *
 *  - **Position**: one descriptor and one position shared by reads and writes. A
 *    write at the position extends the file (a seek past the end then a write
 *    creates a zero-filled hole); `seek` uses fs.SEEK_SET/CUR/END and, unlike
 *    MemoryStream, does not clamp the result. `truncate` resizes the file and
 *    keeps the position. See SeekableStream for the shared positioning contract.
 *  - **close**: close() closes the descriptor and is idempotent. Every member
 *    then throws an Error [20009] with the message "FileStream: file is closed."
 *  - **name vs stat().name**: `name` is the path the file was opened with
 *    (normalized, still relative when a relative path was given), while
 *    stat().name is only the base name (see Stat).
 *  - **Node comparison**: Node.js has no FileStream class: fs.open returns a
 *    FileHandle (not a stream) and fs.createReadStream returns a Readable without
 *    seek/tell/size/truncate. fibjs createReadStream takes an inclusive
 *    start/end range and returns a positioned stream instead.
 *
 *  Obtained from:
 *  - `fs.openFile(path[, flags])` — the binary file stream, positioned at 0;
 *  - `fs.createReadStream(path[, options])` — a FileStream, or a RangeStream over
 *    one when start/end are given;
 *  - `fs.createWriteStream(path[, options])` — a FileStream opened for writing.
 *
 *  Example 1 — write, seek and read the same file:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-filestream-'));
 *  const file = path.join(dir, 'data.bin');
 *
 *  const stm = fs.openFile(file, 'w+');
 *  stm.write(Buffer.from('hello world'));
 *  stm.seek(6, fs.SEEK_SET);
 *  console.log(stm.readAll().toString()); // world
 *  console.log(stm.name.endsWith('data.bin'), stm.size()); // true 11
 *
 *  stm.close();
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — read an inclusive slice through createReadStream:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-filestream-'));
 *  const file = path.join(dir, 'data.txt');
 *  fs.writeFile(file, '0123456789');
 *
 *  const stm = fs.createReadStream(file, { start: 2, end: 5 });
 *  console.log(stm.readAll().toString()); // 2345
 *
 *  stm.close();
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 */
declare class Class_FileStream extends Class_SeekableStream {
    /**
     * @description Queries the current file name
     *
     *      The path passed to the open function, with platform separators normalized
     *      but not resolved against the current directory: opening 'data.txt' keeps
     *      'data.txt', an absolute path stays absolute. Reading it after close()
     *      throws [20009]. Node.js read streams expose the same idea as `.path`.
     *
     *      Example — inspect the name before and after close:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-name-'));
     *      const file = path.join(dir, 'notes.txt');
     *      fs.writeFile(file, 'x');
     *
     *      const stm = fs.openFile(file);
     *      console.log(stm.name === file); // true
     *
     *      stm.close();
     *      try {
     *          stm.name;
     *      } catch (err) {
     *          console.log(err.number); // 20009
     *      }
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    readonly name: string;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *
     *      Applies fchmod to the open descriptor, so the change reaches the file on
     *      disk immediately and does not depend on the path (a rename in between does
     *      not matter). mode holds the permission bits, for example 0o600; on Windows
     *      the call fails with [20009]. Node.js offers the same operation as
     *      filehandle.chmod.
     *
     *      Example — restrict a file to its owner:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-chmod-'));
     *      const file = path.join(dir, 'run.sh');
     *      fs.writeFile(file, '#!/bin/sh\n');
     *
     *      const stm = fs.openFile(file, 'r+');
     *      if (process.platform !== 'win32') {
     *          stm.chmod(0o700);
     *          console.log((fs.stat(file).mode & 0o777).toString(8)); // 700
     *      }
     *
     *      stm.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param mode the access permission to set
     *
     */
    chmod(mode: number): void;

    chmod(mode: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *
     *      Applies fchmod to the open descriptor, so the change reaches the file on
     *      disk immediately and does not depend on the path (a rename in between does
     *      not matter). mode holds the permission bits, for example 0o600; on Windows
     *      the call fails with [20009]. Node.js offers the same operation as
     *      filehandle.chmod.
     *
     *      Example — restrict a file to its owner:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-chmod-'));
     *      const file = path.join(dir, 'run.sh');
     *      fs.writeFile(file, '#!/bin/sh\n');
     *
     *      const stm = fs.openFile(file, 'r+');
     *      if (process.platform !== 'win32') {
     *          stm.chmod(0o700);
     *          console.log((fs.stat(file).mode & 0o777).toString(8)); // 700
     *      }
     *
     *      stm.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param mode the access permission to set
     *
     */
    chmodSync(mode: number): void;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *
     *      Applies fchmod to the open descriptor, so the change reaches the file on
     *      disk immediately and does not depend on the path (a rename in between does
     *      not matter). mode holds the permission bits, for example 0o600; on Windows
     *      the call fails with [20009]. Node.js offers the same operation as
     *      filehandle.chmod.
     *
     *      Example — restrict a file to its owner:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-chmod-'));
     *      const file = path.join(dir, 'run.sh');
     *      fs.writeFile(file, '#!/bin/sh\n');
     *
     *      const stm = fs.openFile(file, 'r+');
     *      if (process.platform !== 'win32') {
     *          stm.chmod(0o700);
     *          console.log((fs.stat(file).mode & 0o777).toString(8)); // 700
     *      }
     *
     *      stm.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param mode the access permission to set
     *
     */
    chmodAsync(mode: number): Promise<void>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * The promise variant of the FileStream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_FileStreamPromise extends Class_SeekableStreamPromise {
    /**
     * @description Queries the current file name
     *
     *      The path passed to the open function, with platform separators normalized
     *      but not resolved against the current directory: opening 'data.txt' keeps
     *      'data.txt', an absolute path stays absolute. Reading it after close()
     *      throws [20009]. Node.js read streams expose the same idea as `.path`.
     *
     *      Example — inspect the name before and after close:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-name-'));
     *      const file = path.join(dir, 'notes.txt');
     *      fs.writeFile(file, 'x');
     *
     *      const stm = fs.openFile(file);
     *      console.log(stm.name === file); // true
     *
     *      stm.close();
     *      try {
     *          stm.name;
     *      } catch (err) {
     *          console.log(err.number); // 20009
     *      }
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    readonly name: string;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *
     *      Applies fchmod to the open descriptor, so the change reaches the file on
     *      disk immediately and does not depend on the path (a rename in between does
     *      not matter). mode holds the permission bits, for example 0o600; on Windows
     *      the call fails with [20009]. Node.js offers the same operation as
     *      filehandle.chmod.
     *
     *      Example — restrict a file to its owner:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-chmod-'));
     *      const file = path.join(dir, 'run.sh');
     *      fs.writeFile(file, '#!/bin/sh\n');
     *
     *      const stm = fs.openFile(file, 'r+');
     *      if (process.platform !== 'win32') {
     *          stm.chmod(0o700);
     *          console.log((fs.stat(file).mode & 0o777).toString(8)); // 700
     *      }
     *
     *      stm.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param mode the access permission to set
     *
     */
    chmod(mode: number): Promise<void>;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *
     *      Applies fchmod to the open descriptor, so the change reaches the file on
     *      disk immediately and does not depend on the path (a rename in between does
     *      not matter). mode holds the permission bits, for example 0o600; on Windows
     *      the call fails with [20009]. Node.js offers the same operation as
     *      filehandle.chmod.
     *
     *      Example — restrict a file to its owner:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-chmod-'));
     *      const file = path.join(dir, 'run.sh');
     *      fs.writeFile(file, '#!/bin/sh\n');
     *
     *      const stm = fs.openFile(file, 'r+');
     *      if (process.platform !== 'win32') {
     *          stm.chmod(0o700);
     *          console.log((fs.stat(file).mode & 0o777).toString(8)); // 700
     *      }
     *
     *      stm.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param mode the access permission to set
     *
     */
    chmodSync(mode: number): void;

    /**
     * @description Queries the access permission of the current file; not supported on Windows
     *
     *      Applies fchmod to the open descriptor, so the change reaches the file on
     *      disk immediately and does not depend on the path (a rename in between does
     *      not matter). mode holds the permission bits, for example 0o600; on Windows
     *      the call fails with [20009]. Node.js offers the same operation as
     *      filehandle.chmod.
     *
     *      Example — restrict a file to its owner:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-chmod-'));
     *      const file = path.join(dir, 'run.sh');
     *      fs.writeFile(file, '#!/bin/sh\n');
     *
     *      const stm = fs.openFile(file, 'r+');
     *      if (process.platform !== 'win32') {
     *          stm.chmod(0o700);
     *          console.log((fs.stat(file).mode & 0o777).toString(8)); // 700
     *      }
     *
     *      stm.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param mode the access permission to set
     *
     */
    chmodAsync(mode: number): Promise<void>;

}


declare namespace Class_FileStream {
    const promises: FIBJS.GeneralObject;
}
