/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/// <reference path="../interface/ZipFile.d.ts" />
/**
 * The promise variant of the zip module: async members return a Promise as their primary form.
 */
declare module 'zip/promises' {
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
    function isZipFile(filename: string): Promise<boolean>;

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
    function open(data: Class_Buffer | Class_SeekableStream | Class_SeekableStreamPromise | string, mod?: string, codec?: string): Promise<Class_ZipFilePromise>;

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


declare module "zip" {
    const promises: typeof import("zip/promises");
}
