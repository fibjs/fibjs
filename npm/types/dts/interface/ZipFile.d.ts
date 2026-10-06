/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * @description The ZipFile object gives read and write access to the entries of a single zip archive
 *
 *  A ZipFile is the handle returned by zip.open for a path, a Buffer or a SeekableStream, and
 *  it is never constructed directly. One object has two very different roles: opened with "r"
 *  it is a reader that walks the stored entries, opened with "w" or "a" it is a writer that
 *  adds entries and publishes them when the archive is closed.
 *
 *  Concepts:
 *
 *  - **Reader and writer objects**: the mode passed to zip.open decides which members work.
 *    In "r" mode the reading members (namelist, infolist, getinfo, read, readAll, extract,
 *    extractAll) operate on the stored entries; in "w"/"a" mode only write may be used.
 *    Calling a member in the wrong direction throws `ZipFile: file is closed.`, the same
 *    error as any use after close(). Opening a path with "a" requires the archive to exist,
 *    otherwise zip.open throws `ZipFile: zip file not exists!`.
 *  - **Entries**: an entry is one stored item whose name uses `/` separators
 *    ("dir/file.txt"); by convention a name that ends with `/` marks a directory, and write
 *    stores it like any other entry. Names are encoded and decoded with the codec chosen when
 *    the archive was opened (default "utf8") and are matched exactly, so an absent name
 *    reports `End of file.`
 *  - **Metadata**: getinfo and infolist describe an entry with filename, date,
 *    compress_type ("Stored", "Deflate", "BZip2" or "Unknown"), compress_size (bytes stored
 *    in the archive), file_size (bytes after decompression) and password (true when the entry
 *    is encrypted).
 *  - **Passwords and encryption**: write creates a ZipCrypto-encrypted entry when its
 *    password is not empty, and such an entry reports password == true. Reading it back needs
 *    exactly that password; a plain entry must be read without one, because the reader
 *    decrypts whenever a password is supplied and fails with `data error` on plain data.
 *    readAll and extractAll apply the password to every entry, so they only work when all
 *    entries of the archive were encrypted with the same password.
 *  - **Writes complete at close()**: entries are compressed with Deflate and stamped with the
 *    current local time as they are written, but only close() appends the central directory
 *    that makes them visible to other readers. close() is idempotent; afterwards every member
 *    throws `ZipFile: file is closed.`
 *  - **Extraction never overwrites**: extractAll(path) needs path to exist, creates the
 *    sub-directories found in the entry names and writes an entry whose target name is
 *    already taken to a new name with a trailing "?" (appended until the name is free).
 *
 *  Obtained from:
 *  - `zip.open(data, mod, codec)` — the only factory; the class has no constructor.
 *
 *  Import:
 *  ```JavaScript
 *  const zip = require('zip');
 *  ```
 *
 *  Example 1 — build an archive from a file, a Buffer and a stream, then inspect it:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const io = require('io');
 *  const os = require('os');
 *  const path = require('path');
 *  const zip = require('zip');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
 *  fs.writeFileSync(path.join(dir, 'notes.txt'), 'a local file');
 *
 *  const stream = new io.MemoryStream();
 *  stream.write(Buffer.from('streamed data'));
 *
 *  let zipfile = zip.open(path.join(dir, 'bundle.zip'), 'w');
 *  zipfile.write(path.join(dir, 'notes.txt'), 'notes.txt');    // from a file path
 *  zipfile.write(Buffer.from('in memory'), 'data/memory.txt'); // from a Buffer
 *  zipfile.write(stream, 'data/stream.txt');                   // from a stream
 *  zipfile.close();
 *
 *  zipfile = zip.open(path.join(dir, 'bundle.zip'));
 *  console.log(zipfile.namelist().join(', '));
 *  // notes.txt, data/memory.txt, data/stream.txt
 *  console.log(zipfile.getinfo('data/memory.txt').file_size); // 9
 *  console.log(zipfile.read('notes.txt').toString()); // a local file
 *  zipfile.close();
 *
 *  stream.close();
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Example 2 — a password-protected entry, extracted with the same password:
 *  ```JavaScript
 *  const fs = require('fs');
 *  const os = require('os');
 *  const path = require('path');
 *  const zip = require('zip');
 *
 *  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
 *  const archive = path.join(dir, 'secret.zip');
 *
 *  let zipfile = zip.open(archive, 'w');
 *  zipfile.write(Buffer.from('classified'), 'secret.txt', 'hunter2');
 *  zipfile.close();
 *
 *  zipfile = zip.open(archive);
 *  console.log(zipfile.getinfo('secret.txt').password); // true
 *  console.log(zipfile.read('secret.txt', 'hunter2').toString()); // classified
 *  try {
 *      zipfile.read('secret.txt');
 *  } catch (e) {
 *      console.log(e.message); // data error
 *  }
 *
 *  const out = path.join(dir, 'out');
 *  fs.mkdirSync(out);
 *  zipfile.extractAll(out, 'hunter2');
 *  zipfile.close();
 *  console.log(fs.readFileSync(path.join(out, 'secret.txt'), 'utf8')); // classified
 *
 *  fs.rmSync(dir, { recursive: true, force: true });
 *  ```
 *
 *  Notes:
 *
 *  - An archive that holds no entries is rejected by the reader, so zip.open returns a
 *    ZipFile whose members report `ZipFile: file is closed.` until it is closed. A file that
 *    is not a zip archive at all opens the same way: the failure surfaces on the first read
 *    member, not from zip.open.
 *  - Data appended after the zip records is found by the reader, which is how an executable
 *    serves its embedded archive through `process.execPath + '$/...'`.
 *
 */
declare class Class_ZipFile extends Class_object {
    /**
     * @description Lists the names of all entries in archive order
     *
     *      One name per entry, exactly as stored: names keep their `/` separators and a directory
     *      entry ends with `/`. namelist, infolist, readAll and extractAll walk the entries in the
     *      same stored order. Available only on a reader ("r" mode); on a writer and after close()
     *      the call throws `ZipFile: file is closed.`
     *
     *      Example — list the entries of an archive built in memory:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.namelist().join(', ')); // one.txt, dir/two.txt
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the entry names in archive order
     *
     */
    namelist(): string[];

    namelist(callback: (err: Error | undefined | null, retVal: string[])=>any): void;

    /**
     * @description Lists the names of all entries in archive order
     *
     *      One name per entry, exactly as stored: names keep their `/` separators and a directory
     *      entry ends with `/`. namelist, infolist, readAll and extractAll walk the entries in the
     *      same stored order. Available only on a reader ("r" mode); on a writer and after close()
     *      the call throws `ZipFile: file is closed.`
     *
     *      Example — list the entries of an archive built in memory:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.namelist().join(', ')); // one.txt, dir/two.txt
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the entry names in archive order
     *
     */
    namelistSync(): string[];

    /**
     * @description Lists the names of all entries in archive order
     *
     *      One name per entry, exactly as stored: names keep their `/` separators and a directory
     *      entry ends with `/`. namelist, infolist, readAll and extractAll walk the entries in the
     *      same stored order. Available only on a reader ("r" mode); on a writer and after close()
     *      the call throws `ZipFile: file is closed.`
     *
     *      Example — list the entries of an archive built in memory:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.namelist().join(', ')); // one.txt, dir/two.txt
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the entry names in archive order
     *
     */
    namelistAsync(): Promise<string[]>;

    /**
     * @description Lists the metadata of all entries in archive order
     *
     *      Returns one information object per entry, in the same order as namelist, with the fields
     *      filename, date, compress_type, compress_size, file_size and password; see getinfo for
     *      their meaning. Use namelist when only the names are needed and getinfo to look up a
     *      single entry by name.
     *
     *      Example — report the name, compression method and uncompressed size of each entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello'), 'hello.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.infolist().forEach((entry) => {
     *          console.log(entry.filename, entry.compress_type, entry.file_size);
     *      });
     *      // hello.txt Deflate 5
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the information objects of the entries in archive order
     *
     */
    infolist(): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }[];

    infolist(callback: (err: Error | undefined | null, retVal: {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }[])=>any): void;

    /**
     * @description Lists the metadata of all entries in archive order
     *
     *      Returns one information object per entry, in the same order as namelist, with the fields
     *      filename, date, compress_type, compress_size, file_size and password; see getinfo for
     *      their meaning. Use namelist when only the names are needed and getinfo to look up a
     *      single entry by name.
     *
     *      Example — report the name, compression method and uncompressed size of each entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello'), 'hello.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.infolist().forEach((entry) => {
     *          console.log(entry.filename, entry.compress_type, entry.file_size);
     *      });
     *      // hello.txt Deflate 5
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the information objects of the entries in archive order
     *
     */
    infolistSync(): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }[];

    /**
     * @description Lists the metadata of all entries in archive order
     *
     *      Returns one information object per entry, in the same order as namelist, with the fields
     *      filename, date, compress_type, compress_size, file_size and password; see getinfo for
     *      their meaning. Use namelist when only the names are needed and getinfo to look up a
     *      single entry by name.
     *
     *      Example — report the name, compression method and uncompressed size of each entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello'), 'hello.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.infolist().forEach((entry) => {
     *          console.log(entry.filename, entry.compress_type, entry.file_size);
     *      });
     *      // hello.txt Deflate 5
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the information objects of the entries in archive order
     *
     */
    infolistAsync(): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }[]>;

    /**
     * @description Gets the metadata of one entry
     *
     *      The object carries filename (the stored name), date (the DOS timestamp of the entry),
     *      compress_type ("Stored", "Deflate", "BZip2" or "Unknown"), compress_size (bytes stored
     *      in the archive), file_size (bytes after decompression) and password (true when the entry
     *      is encrypted). member is encoded with the codec of the archive and matched exactly; a
     *      name that is not present throws `End of file.`
     *
     *      Example — inspect one entry, including its encryption flag:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('classified'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      const info = zipfile.getinfo('secret.txt');
     *      console.log(info.filename, info.file_size, info.password); // secret.txt 10 true
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry inside the archive
     *      @return the information object of the entry
     *
     */
    getinfo(member: string): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    };

    getinfo(member: string, callback: (err: Error | undefined | null, retVal: {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    })=>any): void;

    /**
     * @description Gets the metadata of one entry
     *
     *      The object carries filename (the stored name), date (the DOS timestamp of the entry),
     *      compress_type ("Stored", "Deflate", "BZip2" or "Unknown"), compress_size (bytes stored
     *      in the archive), file_size (bytes after decompression) and password (true when the entry
     *      is encrypted). member is encoded with the codec of the archive and matched exactly; a
     *      name that is not present throws `End of file.`
     *
     *      Example — inspect one entry, including its encryption flag:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('classified'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      const info = zipfile.getinfo('secret.txt');
     *      console.log(info.filename, info.file_size, info.password); // secret.txt 10 true
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry inside the archive
     *      @return the information object of the entry
     *
     */
    getinfoSync(member: string): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    };

    /**
     * @description Gets the metadata of one entry
     *
     *      The object carries filename (the stored name), date (the DOS timestamp of the entry),
     *      compress_type ("Stored", "Deflate", "BZip2" or "Unknown"), compress_size (bytes stored
     *      in the archive), file_size (bytes after decompression) and password (true when the entry
     *      is encrypted). member is encoded with the codec of the archive and matched exactly; a
     *      name that is not present throws `End of file.`
     *
     *      Example — inspect one entry, including its encryption flag:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('classified'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      const info = zipfile.getinfo('secret.txt');
     *      console.log(info.filename, info.file_size, info.password); // secret.txt 10 true
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry inside the archive
     *      @return the information object of the entry
     *
     */
    getinfoAsync(member: string): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }>;

    /**
     * @description Reads one entry and returns its uncompressed data
     *
     *      The whole entry is decompressed into a Buffer. An encrypted entry needs its password; an
     *      unencrypted entry must be read without one, because the reader decrypts whenever a
     *      password is supplied and plain data then fails with `data error`. An unknown member name
     *      throws `End of file.`; readAll reads every entry in one call.
     *
     *      Example — read an entry by name:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.read('greeting.txt').toString()); // hello, zip
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to read
     *      @param password password of an encrypted entry, empty for a plain entry
     *      @return the uncompressed data of the entry
     *
     */
    read(member: string, password?: string): Class_Buffer;

    read(member: string, password?: string, callback: (err: Error | undefined | null, retVal: Class_Buffer)=>any): void;

    /**
     * @description Reads one entry and returns its uncompressed data
     *
     *      The whole entry is decompressed into a Buffer. An encrypted entry needs its password; an
     *      unencrypted entry must be read without one, because the reader decrypts whenever a
     *      password is supplied and plain data then fails with `data error`. An unknown member name
     *      throws `End of file.`; readAll reads every entry in one call.
     *
     *      Example — read an entry by name:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.read('greeting.txt').toString()); // hello, zip
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to read
     *      @param password password of an encrypted entry, empty for a plain entry
     *      @return the uncompressed data of the entry
     *
     */
    readSync(member: string, password?: string): Class_Buffer;

    /**
     * @description Reads one entry and returns its uncompressed data
     *
     *      The whole entry is decompressed into a Buffer. An encrypted entry needs its password; an
     *      unencrypted entry must be read without one, because the reader decrypts whenever a
     *      password is supplied and plain data then fails with `data error`. An unknown member name
     *      throws `End of file.`; readAll reads every entry in one call.
     *
     *      Example — read an entry by name:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.read('greeting.txt').toString()); // hello, zip
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to read
     *      @param password password of an encrypted entry, empty for a plain entry
     *      @return the uncompressed data of the entry
     *
     */
    readAsync(member: string, password?: string): Promise<Class_Buffer>;

    /**
     * @description Reads every entry of the archive at once
     *
     *      Walks the entries in archive order and returns, for each one, the fields of infolist
     *      plus data, the uncompressed Buffer of the entry. The password is applied to every entry,
     *      so it must be the one used to encrypt all of them — an entry that is plain or encrypted
     *      with another password fails with `data error`. Without a password the call works when no
     *      entry is encrypted.
     *
     *      Example — read name and contents of every entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.readAll().forEach((entry) => {
     *          console.log(entry.filename, entry.data.toString());
     *      });
     *      // one.txt one
     *      // dir/two.txt two
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param password password shared by all encrypted entries
     *      @return the entries with their data in archive order
     *
     */
    readAll(password?: string): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
        data: Class_Buffer;
    }[];

    readAll(password?: string, callback: (err: Error | undefined | null, retVal: {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
        data: Class_Buffer;
    }[])=>any): void;

    /**
     * @description Reads every entry of the archive at once
     *
     *      Walks the entries in archive order and returns, for each one, the fields of infolist
     *      plus data, the uncompressed Buffer of the entry. The password is applied to every entry,
     *      so it must be the one used to encrypt all of them — an entry that is plain or encrypted
     *      with another password fails with `data error`. Without a password the call works when no
     *      entry is encrypted.
     *
     *      Example — read name and contents of every entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.readAll().forEach((entry) => {
     *          console.log(entry.filename, entry.data.toString());
     *      });
     *      // one.txt one
     *      // dir/two.txt two
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param password password shared by all encrypted entries
     *      @return the entries with their data in archive order
     *
     */
    readAllSync(password?: string): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
        data: Class_Buffer;
    }[];

    /**
     * @description Reads every entry of the archive at once
     *
     *      Walks the entries in archive order and returns, for each one, the fields of infolist
     *      plus data, the uncompressed Buffer of the entry. The password is applied to every entry,
     *      so it must be the one used to encrypt all of them — an entry that is plain or encrypted
     *      with another password fails with `data error`. Without a password the call works when no
     *      entry is encrypted.
     *
     *      Example — read name and contents of every entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.readAll().forEach((entry) => {
     *          console.log(entry.filename, entry.data.toString());
     *      });
     *      // one.txt one
     *      // dir/two.txt two
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param password password shared by all encrypted entries
     *      @return the entries with their data in archive order
     *
     */
    readAllAsync(password?: string): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
        data: Class_Buffer;
    }[]>;

    /**
     * @description Decompresses one entry into a stream
     *
     *      The entry is located by name and its uncompressed bytes are written at the current
     *      position of strm; the stream is neither rewound nor closed. Use it to forward an entry
     *      into another consumer, or pass a MemoryStream to obtain its data as a Buffer. Unknown
     *      names throw `End of file.` and the password rules of read apply.
     *
     *      Example — decompress into a MemoryStream:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *
     *      const out = new io.MemoryStream();
     *      zipfile.extract('greeting.txt', out);
     *      out.rewind();
     *      console.log(out.readAll().toString()); // hello, zip
     *
     *      zipfile.close();
     *      out.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to decompress
     *      @param strm the stream that receives the entry data
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extract(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): void;

    extract(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses one entry into a stream
     *
     *      The entry is located by name and its uncompressed bytes are written at the current
     *      position of strm; the stream is neither rewound nor closed. Use it to forward an entry
     *      into another consumer, or pass a MemoryStream to obtain its data as a Buffer. Unknown
     *      names throw `End of file.` and the password rules of read apply.
     *
     *      Example — decompress into a MemoryStream:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *
     *      const out = new io.MemoryStream();
     *      zipfile.extract('greeting.txt', out);
     *      out.rewind();
     *      console.log(out.readAll().toString()); // hello, zip
     *
     *      zipfile.close();
     *      out.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to decompress
     *      @param strm the stream that receives the entry data
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extractSync(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): void;

    /**
     * @description Decompresses one entry into a stream
     *
     *      The entry is located by name and its uncompressed bytes are written at the current
     *      position of strm; the stream is neither rewound nor closed. Use it to forward an entry
     *      into another consumer, or pass a MemoryStream to obtain its data as a Buffer. Unknown
     *      names throw `End of file.` and the password rules of read apply.
     *
     *      Example — decompress into a MemoryStream:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *
     *      const out = new io.MemoryStream();
     *      zipfile.extract('greeting.txt', out);
     *      out.rewind();
     *      console.log(out.readAll().toString()); // hello, zip
     *
     *      zipfile.close();
     *      out.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to decompress
     *      @param strm the stream that receives the entry data
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extractAsync(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): Promise<void>;

    /**
     * @description Decompresses one entry to a file
     *
     *      The target file is created or truncated ("w"), and its parent directory must already
     *      exist; the entry is located by name as in getinfo, so an unknown name throws
     *      `End of file.` and the password rules of read apply.
     *
     *      Example — extract a single entry and read it back from disk:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('extracted'), 'data.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extract('data.txt', path.join(dir, 'out.txt'));
     *      zipfile.close();
     *
     *      console.log(fs.readFileSync(path.join(dir, 'out.txt'), 'utf8')); // extracted
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param member the name of the entry to decompress
     *      @param path path of the file to write
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extract(member: string, path: string, password?: string): void;

    extract(member: string, path: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses one entry to a file
     *
     *      The target file is created or truncated ("w"), and its parent directory must already
     *      exist; the entry is located by name as in getinfo, so an unknown name throws
     *      `End of file.` and the password rules of read apply.
     *
     *      Example — extract a single entry and read it back from disk:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('extracted'), 'data.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extract('data.txt', path.join(dir, 'out.txt'));
     *      zipfile.close();
     *
     *      console.log(fs.readFileSync(path.join(dir, 'out.txt'), 'utf8')); // extracted
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param member the name of the entry to decompress
     *      @param path path of the file to write
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extractSync(member: string, path: string, password?: string): void;

    /**
     * @description Decompresses one entry to a file
     *
     *      The target file is created or truncated ("w"), and its parent directory must already
     *      exist; the entry is located by name as in getinfo, so an unknown name throws
     *      `End of file.` and the password rules of read apply.
     *
     *      Example — extract a single entry and read it back from disk:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('extracted'), 'data.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extract('data.txt', path.join(dir, 'out.txt'));
     *      zipfile.close();
     *
     *      console.log(fs.readFileSync(path.join(dir, 'out.txt'), 'utf8')); // extracted
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param member the name of the entry to decompress
     *      @param path path of the file to write
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extractAsync(member: string, path: string, password?: string): Promise<void>;

    /**
     * @description Decompresses every entry below a directory
     *
     *      The directory must exist, otherwise the call reports `ZipFile: no such file or
     *      directory`; the sub-directories found in the entry names are created below it. Entry
     *      names are normalized against path but are not confined to it, so a stored name that
     *      uses ".." can make the call write outside path; only unpack archives you trust. An
     *      entry whose target file is already taken is not overwritten: a trailing "?" is appended
     *      to its name, and more "?" characters are appended until the name is free. The password
     *      is applied to every entry (see readAll).
     *
     *      Example — unpack a small archive into an existing directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'app.zip');
     *      const out = path.join(dir, 'out');
     *      fs.mkdirSync(out);
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('{"name":"fibjs"}'), 'config/app.json');
     *      zipfile.write(Buffer.from('<h1>fibjs</h1>'), 'public/index.html');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extractAll(out);
     *      zipfile.close();
     *
     *      console.log(fs.readdirSync(out).sort().join(', ')); // config, public
     *      console.log(fs.readFileSync(path.join(out, 'config', 'app.json'), 'utf8'));
     *      // {"name":"fibjs"}
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory that receives the entries
     *      @param password password shared by all encrypted entries
     *
     */
    extractAll(path: string, password?: string): void;

    extractAll(path: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Decompresses every entry below a directory
     *
     *      The directory must exist, otherwise the call reports `ZipFile: no such file or
     *      directory`; the sub-directories found in the entry names are created below it. Entry
     *      names are normalized against path but are not confined to it, so a stored name that
     *      uses ".." can make the call write outside path; only unpack archives you trust. An
     *      entry whose target file is already taken is not overwritten: a trailing "?" is appended
     *      to its name, and more "?" characters are appended until the name is free. The password
     *      is applied to every entry (see readAll).
     *
     *      Example — unpack a small archive into an existing directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'app.zip');
     *      const out = path.join(dir, 'out');
     *      fs.mkdirSync(out);
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('{"name":"fibjs"}'), 'config/app.json');
     *      zipfile.write(Buffer.from('<h1>fibjs</h1>'), 'public/index.html');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extractAll(out);
     *      zipfile.close();
     *
     *      console.log(fs.readdirSync(out).sort().join(', ')); // config, public
     *      console.log(fs.readFileSync(path.join(out, 'config', 'app.json'), 'utf8'));
     *      // {"name":"fibjs"}
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory that receives the entries
     *      @param password password shared by all encrypted entries
     *
     */
    extractAllSync(path: string, password?: string): void;

    /**
     * @description Decompresses every entry below a directory
     *
     *      The directory must exist, otherwise the call reports `ZipFile: no such file or
     *      directory`; the sub-directories found in the entry names are created below it. Entry
     *      names are normalized against path but are not confined to it, so a stored name that
     *      uses ".." can make the call write outside path; only unpack archives you trust. An
     *      entry whose target file is already taken is not overwritten: a trailing "?" is appended
     *      to its name, and more "?" characters are appended until the name is free. The password
     *      is applied to every entry (see readAll).
     *
     *      Example — unpack a small archive into an existing directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'app.zip');
     *      const out = path.join(dir, 'out');
     *      fs.mkdirSync(out);
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('{"name":"fibjs"}'), 'config/app.json');
     *      zipfile.write(Buffer.from('<h1>fibjs</h1>'), 'public/index.html');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extractAll(out);
     *      zipfile.close();
     *
     *      console.log(fs.readdirSync(out).sort().join(', ')); // config, public
     *      console.log(fs.readFileSync(path.join(out, 'config', 'app.json'), 'utf8'));
     *      // {"name":"fibjs"}
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory that receives the entries
     *      @param password password shared by all encrypted entries
     *
     */
    extractAllAsync(path: string, password?: string): Promise<void>;

    /**
     * @description Writes a Buffer as a new entry
     *
     *      The data is compressed with Deflate and stored under inZipName; a non-empty password
     *      encrypts the entry with ZipCrypto. Every entry is stamped with the current local time as
     *      its date. The archive must be open for writing ("w" or "a"), otherwise the call throws
     *      `ZipFile: file is closed.`, and the new entry becomes visible to readers at close().
     *
     *      Example — write plain and encrypted entries from memory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('written from memory'), 'memory.txt');
     *      zipfile.write(Buffer.from('encrypted'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.namelist().join(', ')); // memory.txt, secret.txt
     *      console.log(zipfile.getinfo('secret.txt').password); // true
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the data to write
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    write(data: Class_Buffer, inZipName: string, password?: string): void;

    write(data: Class_Buffer, inZipName: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Writes a Buffer as a new entry
     *
     *      The data is compressed with Deflate and stored under inZipName; a non-empty password
     *      encrypts the entry with ZipCrypto. Every entry is stamped with the current local time as
     *      its date. The archive must be open for writing ("w" or "a"), otherwise the call throws
     *      `ZipFile: file is closed.`, and the new entry becomes visible to readers at close().
     *
     *      Example — write plain and encrypted entries from memory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('written from memory'), 'memory.txt');
     *      zipfile.write(Buffer.from('encrypted'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.namelist().join(', ')); // memory.txt, secret.txt
     *      console.log(zipfile.getinfo('secret.txt').password); // true
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the data to write
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeSync(data: Class_Buffer, inZipName: string, password?: string): void;

    /**
     * @description Writes a Buffer as a new entry
     *
     *      The data is compressed with Deflate and stored under inZipName; a non-empty password
     *      encrypts the entry with ZipCrypto. Every entry is stamped with the current local time as
     *      its date. The archive must be open for writing ("w" or "a"), otherwise the call throws
     *      `ZipFile: file is closed.`, and the new entry becomes visible to readers at close().
     *
     *      Example — write plain and encrypted entries from memory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('written from memory'), 'memory.txt');
     *      zipfile.write(Buffer.from('encrypted'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.namelist().join(', ')); // memory.txt, secret.txt
     *      console.log(zipfile.getinfo('secret.txt').password); // true
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the data to write
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeAsync(data: Class_Buffer, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes a stream as a new entry
     *
     *      The stream is rewound and copied from its first byte to its end, then compressed with
     *      Deflate and stored under inZipName; the stream itself is left open. Date, password and
     *      open-mode rules are the same as for the Buffer form.
     *
     *      Example — write the contents of a MemoryStream:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const io = require('io');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const src = new io.MemoryStream();
     *      src.write(Buffer.from('streamed into the archive'));
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(src, 'stream.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.read('stream.txt').toString()); // streamed into the archive
     *      zipfile.close();
     *
     *      src.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param strm the stream whose data is written
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    write(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): void;

    write(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Writes a stream as a new entry
     *
     *      The stream is rewound and copied from its first byte to its end, then compressed with
     *      Deflate and stored under inZipName; the stream itself is left open. Date, password and
     *      open-mode rules are the same as for the Buffer form.
     *
     *      Example — write the contents of a MemoryStream:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const io = require('io');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const src = new io.MemoryStream();
     *      src.write(Buffer.from('streamed into the archive'));
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(src, 'stream.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.read('stream.txt').toString()); // streamed into the archive
     *      zipfile.close();
     *
     *      src.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param strm the stream whose data is written
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeSync(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): void;

    /**
     * @description Writes a stream as a new entry
     *
     *      The stream is rewound and copied from its first byte to its end, then compressed with
     *      Deflate and stored under inZipName; the stream itself is left open. Date, password and
     *      open-mode rules are the same as for the Buffer form.
     *
     *      Example — write the contents of a MemoryStream:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const io = require('io');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const src = new io.MemoryStream();
     *      src.write(Buffer.from('streamed into the archive'));
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(src, 'stream.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.read('stream.txt').toString()); // streamed into the archive
     *      zipfile.close();
     *
     *      src.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param strm the stream whose data is written
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeAsync(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes a local file as a new entry
     *
     *      The file is opened for reading and copied under inZipName, compressed with Deflate; a
     *      missing source reports ENOENT. The entry date is the current local time, not the
     *      modification time of the file. Password and open-mode rules are the same as for the
     *      Buffer form.
     *
     *      Example — store a report under a directory name:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *      fs.writeFileSync(path.join(dir, 'report.txt'), 'quarterly report');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(path.join(dir, 'report.txt'), 'docs/report.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.getinfo('docs/report.txt').file_size); // 16
     *      console.log(zipfile.read('docs/report.txt').toString()); // quarterly report
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename path of the file to write into the archive
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    write(filename: string, inZipName: string, password?: string): void;

    write(filename: string, inZipName: string, password?: string, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Writes a local file as a new entry
     *
     *      The file is opened for reading and copied under inZipName, compressed with Deflate; a
     *      missing source reports ENOENT. The entry date is the current local time, not the
     *      modification time of the file. Password and open-mode rules are the same as for the
     *      Buffer form.
     *
     *      Example — store a report under a directory name:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *      fs.writeFileSync(path.join(dir, 'report.txt'), 'quarterly report');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(path.join(dir, 'report.txt'), 'docs/report.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.getinfo('docs/report.txt').file_size); // 16
     *      console.log(zipfile.read('docs/report.txt').toString()); // quarterly report
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename path of the file to write into the archive
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeSync(filename: string, inZipName: string, password?: string): void;

    /**
     * @description Writes a local file as a new entry
     *
     *      The file is opened for reading and copied under inZipName, compressed with Deflate; a
     *      missing source reports ENOENT. The entry date is the current local time, not the
     *      modification time of the file. Password and open-mode rules are the same as for the
     *      Buffer form.
     *
     *      Example — store a report under a directory name:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *      fs.writeFileSync(path.join(dir, 'report.txt'), 'quarterly report');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(path.join(dir, 'report.txt'), 'docs/report.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.getinfo('docs/report.txt').file_size); // 16
     *      console.log(zipfile.read('docs/report.txt').toString()); // quarterly report
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename path of the file to write into the archive
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeAsync(filename: string, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Closes the archive and completes pending writes
     *
     *      On a writer ("w"/"a") close() appends the central directory, which is what makes the new
     *      entries visible to other readers; without it the archive cannot be read back. On a
     *      reader it releases the underlying stream. Calling close() again is harmless, but every
     *      member used after it throws `ZipFile: file is closed.`
     *
     *      Example — the second close is a no-op, further writes are rejected:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close(); // appends the central directory
     *      zipfile.close(); // harmless the second time
     *
     *      try {
     *          zipfile.write(Buffer.from('more'), 'more.txt');
     *      } catch (e) {
     *          console.log(e.message); // ZipFile: file is closed.
     *      }
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the archive and completes pending writes
     *
     *      On a writer ("w"/"a") close() appends the central directory, which is what makes the new
     *      entries visible to other readers; without it the archive cannot be read back. On a
     *      reader it releases the underlying stream. Calling close() again is harmless, but every
     *      member used after it throws `ZipFile: file is closed.`
     *
     *      Example — the second close is a no-op, further writes are rejected:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close(); // appends the central directory
     *      zipfile.close(); // harmless the second time
     *
     *      try {
     *          zipfile.write(Buffer.from('more'), 'more.txt');
     *      } catch (e) {
     *          console.log(e.message); // ZipFile: file is closed.
     *      }
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    closeSync(): void;

    /**
     * @description Closes the archive and completes pending writes
     *
     *      On a writer ("w"/"a") close() appends the central directory, which is what makes the new
     *      entries visible to other readers; without it the archive cannot be read back. On a
     *      reader it releases the underlying stream. Calling close() again is harmless, but every
     *      member used after it throws `ZipFile: file is closed.`
     *
     *      Example — the second close is a no-op, further writes are rejected:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close(); // appends the central directory
     *      zipfile.close(); // harmless the second time
     *
     *      try {
     *          zipfile.write(Buffer.from('more'), 'more.txt');
     *      } catch (e) {
     *          console.log(e.message); // ZipFile: file is closed.
     *      }
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    closeAsync(): Promise<void>;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/// <reference path="../interface/SeekableStream.d.ts" />
/**
 * The promise variant of the ZipFile class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_ZipFilePromise extends Class_object {
    /**
     * @description Lists the names of all entries in archive order
     *
     *      One name per entry, exactly as stored: names keep their `/` separators and a directory
     *      entry ends with `/`. namelist, infolist, readAll and extractAll walk the entries in the
     *      same stored order. Available only on a reader ("r" mode); on a writer and after close()
     *      the call throws `ZipFile: file is closed.`
     *
     *      Example — list the entries of an archive built in memory:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.namelist().join(', ')); // one.txt, dir/two.txt
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the entry names in archive order
     *
     */
    namelist(): Promise<string[]>;

    /**
     * @description Lists the names of all entries in archive order
     *
     *      One name per entry, exactly as stored: names keep their `/` separators and a directory
     *      entry ends with `/`. namelist, infolist, readAll and extractAll walk the entries in the
     *      same stored order. Available only on a reader ("r" mode); on a writer and after close()
     *      the call throws `ZipFile: file is closed.`
     *
     *      Example — list the entries of an archive built in memory:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.namelist().join(', ')); // one.txt, dir/two.txt
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the entry names in archive order
     *
     */
    namelistSync(): string[];

    /**
     * @description Lists the names of all entries in archive order
     *
     *      One name per entry, exactly as stored: names keep their `/` separators and a directory
     *      entry ends with `/`. namelist, infolist, readAll and extractAll walk the entries in the
     *      same stored order. Available only on a reader ("r" mode); on a writer and after close()
     *      the call throws `ZipFile: file is closed.`
     *
     *      Example — list the entries of an archive built in memory:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.namelist().join(', ')); // one.txt, dir/two.txt
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the entry names in archive order
     *
     */
    namelistAsync(): Promise<string[]>;

    /**
     * @description Lists the metadata of all entries in archive order
     *
     *      Returns one information object per entry, in the same order as namelist, with the fields
     *      filename, date, compress_type, compress_size, file_size and password; see getinfo for
     *      their meaning. Use namelist when only the names are needed and getinfo to look up a
     *      single entry by name.
     *
     *      Example — report the name, compression method and uncompressed size of each entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello'), 'hello.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.infolist().forEach((entry) => {
     *          console.log(entry.filename, entry.compress_type, entry.file_size);
     *      });
     *      // hello.txt Deflate 5
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the information objects of the entries in archive order
     *
     */
    infolist(): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }[]>;

    /**
     * @description Lists the metadata of all entries in archive order
     *
     *      Returns one information object per entry, in the same order as namelist, with the fields
     *      filename, date, compress_type, compress_size, file_size and password; see getinfo for
     *      their meaning. Use namelist when only the names are needed and getinfo to look up a
     *      single entry by name.
     *
     *      Example — report the name, compression method and uncompressed size of each entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello'), 'hello.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.infolist().forEach((entry) => {
     *          console.log(entry.filename, entry.compress_type, entry.file_size);
     *      });
     *      // hello.txt Deflate 5
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the information objects of the entries in archive order
     *
     */
    infolistSync(): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }[];

    /**
     * @description Lists the metadata of all entries in archive order
     *
     *      Returns one information object per entry, in the same order as namelist, with the fields
     *      filename, date, compress_type, compress_size, file_size and password; see getinfo for
     *      their meaning. Use namelist when only the names are needed and getinfo to look up a
     *      single entry by name.
     *
     *      Example — report the name, compression method and uncompressed size of each entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello'), 'hello.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.infolist().forEach((entry) => {
     *          console.log(entry.filename, entry.compress_type, entry.file_size);
     *      });
     *      // hello.txt Deflate 5
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @return the information objects of the entries in archive order
     *
     */
    infolistAsync(): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }[]>;

    /**
     * @description Gets the metadata of one entry
     *
     *      The object carries filename (the stored name), date (the DOS timestamp of the entry),
     *      compress_type ("Stored", "Deflate", "BZip2" or "Unknown"), compress_size (bytes stored
     *      in the archive), file_size (bytes after decompression) and password (true when the entry
     *      is encrypted). member is encoded with the codec of the archive and matched exactly; a
     *      name that is not present throws `End of file.`
     *
     *      Example — inspect one entry, including its encryption flag:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('classified'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      const info = zipfile.getinfo('secret.txt');
     *      console.log(info.filename, info.file_size, info.password); // secret.txt 10 true
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry inside the archive
     *      @return the information object of the entry
     *
     */
    getinfo(member: string): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }>;

    /**
     * @description Gets the metadata of one entry
     *
     *      The object carries filename (the stored name), date (the DOS timestamp of the entry),
     *      compress_type ("Stored", "Deflate", "BZip2" or "Unknown"), compress_size (bytes stored
     *      in the archive), file_size (bytes after decompression) and password (true when the entry
     *      is encrypted). member is encoded with the codec of the archive and matched exactly; a
     *      name that is not present throws `End of file.`
     *
     *      Example — inspect one entry, including its encryption flag:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('classified'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      const info = zipfile.getinfo('secret.txt');
     *      console.log(info.filename, info.file_size, info.password); // secret.txt 10 true
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry inside the archive
     *      @return the information object of the entry
     *
     */
    getinfoSync(member: string): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    };

    /**
     * @description Gets the metadata of one entry
     *
     *      The object carries filename (the stored name), date (the DOS timestamp of the entry),
     *      compress_type ("Stored", "Deflate", "BZip2" or "Unknown"), compress_size (bytes stored
     *      in the archive), file_size (bytes after decompression) and password (true when the entry
     *      is encrypted). member is encoded with the codec of the archive and matched exactly; a
     *      name that is not present throws `End of file.`
     *
     *      Example — inspect one entry, including its encryption flag:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('classified'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      const info = zipfile.getinfo('secret.txt');
     *      console.log(info.filename, info.file_size, info.password); // secret.txt 10 true
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry inside the archive
     *      @return the information object of the entry
     *
     */
    getinfoAsync(member: string): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
    }>;

    /**
     * @description Reads one entry and returns its uncompressed data
     *
     *      The whole entry is decompressed into a Buffer. An encrypted entry needs its password; an
     *      unencrypted entry must be read without one, because the reader decrypts whenever a
     *      password is supplied and plain data then fails with `data error`. An unknown member name
     *      throws `End of file.`; readAll reads every entry in one call.
     *
     *      Example — read an entry by name:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.read('greeting.txt').toString()); // hello, zip
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to read
     *      @param password password of an encrypted entry, empty for a plain entry
     *      @return the uncompressed data of the entry
     *
     */
    read(member: string, password?: string): Promise<Class_Buffer>;

    /**
     * @description Reads one entry and returns its uncompressed data
     *
     *      The whole entry is decompressed into a Buffer. An encrypted entry needs its password; an
     *      unencrypted entry must be read without one, because the reader decrypts whenever a
     *      password is supplied and plain data then fails with `data error`. An unknown member name
     *      throws `End of file.`; readAll reads every entry in one call.
     *
     *      Example — read an entry by name:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.read('greeting.txt').toString()); // hello, zip
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to read
     *      @param password password of an encrypted entry, empty for a plain entry
     *      @return the uncompressed data of the entry
     *
     */
    readSync(member: string, password?: string): Class_Buffer;

    /**
     * @description Reads one entry and returns its uncompressed data
     *
     *      The whole entry is decompressed into a Buffer. An encrypted entry needs its password; an
     *      unencrypted entry must be read without one, because the reader decrypts whenever a
     *      password is supplied and plain data then fails with `data error`. An unknown member name
     *      throws `End of file.`; readAll reads every entry in one call.
     *
     *      Example — read an entry by name:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      console.log(zipfile.read('greeting.txt').toString()); // hello, zip
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to read
     *      @param password password of an encrypted entry, empty for a plain entry
     *      @return the uncompressed data of the entry
     *
     */
    readAsync(member: string, password?: string): Promise<Class_Buffer>;

    /**
     * @description Reads every entry of the archive at once
     *
     *      Walks the entries in archive order and returns, for each one, the fields of infolist
     *      plus data, the uncompressed Buffer of the entry. The password is applied to every entry,
     *      so it must be the one used to encrypt all of them — an entry that is plain or encrypted
     *      with another password fails with `data error`. Without a password the call works when no
     *      entry is encrypted.
     *
     *      Example — read name and contents of every entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.readAll().forEach((entry) => {
     *          console.log(entry.filename, entry.data.toString());
     *      });
     *      // one.txt one
     *      // dir/two.txt two
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param password password shared by all encrypted entries
     *      @return the entries with their data in archive order
     *
     */
    readAll(password?: string): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
        data: Class_Buffer;
    }[]>;

    /**
     * @description Reads every entry of the archive at once
     *
     *      Walks the entries in archive order and returns, for each one, the fields of infolist
     *      plus data, the uncompressed Buffer of the entry. The password is applied to every entry,
     *      so it must be the one used to encrypt all of them — an entry that is plain or encrypted
     *      with another password fails with `data error`. Without a password the call works when no
     *      entry is encrypted.
     *
     *      Example — read name and contents of every entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.readAll().forEach((entry) => {
     *          console.log(entry.filename, entry.data.toString());
     *      });
     *      // one.txt one
     *      // dir/two.txt two
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param password password shared by all encrypted entries
     *      @return the entries with their data in archive order
     *
     */
    readAllSync(password?: string): {
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
        data: Class_Buffer;
    }[];

    /**
     * @description Reads every entry of the archive at once
     *
     *      Walks the entries in archive order and returns, for each one, the fields of infolist
     *      plus data, the uncompressed Buffer of the entry. The password is applied to every entry,
     *      so it must be the one used to encrypt all of them — an entry that is plain or encrypted
     *      with another password fails with `data error`. Without a password the call works when no
     *      entry is encrypted.
     *
     *      Example — read name and contents of every entry:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('one'), 'one.txt');
     *      zipfile.write(Buffer.from('two'), 'dir/two.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *      zipfile.readAll().forEach((entry) => {
     *          console.log(entry.filename, entry.data.toString());
     *      });
     *      // one.txt one
     *      // dir/two.txt two
     *      zipfile.close();
     *      archive.close();
     *      ```
     *      @param password password shared by all encrypted entries
     *      @return the entries with their data in archive order
     *
     */
    readAllAsync(password?: string): Promise<{
        filename: string;
        date: Date;
        compress_type: string;
        compress_size: number;
        file_size: number;
        password: boolean;
        data: Class_Buffer;
    }[]>;

    /**
     * @description Decompresses one entry into a stream
     *
     *      The entry is located by name and its uncompressed bytes are written at the current
     *      position of strm; the stream is neither rewound nor closed. Use it to forward an entry
     *      into another consumer, or pass a MemoryStream to obtain its data as a Buffer. Unknown
     *      names throw `End of file.` and the password rules of read apply.
     *
     *      Example — decompress into a MemoryStream:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *
     *      const out = new io.MemoryStream();
     *      zipfile.extract('greeting.txt', out);
     *      out.rewind();
     *      console.log(out.readAll().toString()); // hello, zip
     *
     *      zipfile.close();
     *      out.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to decompress
     *      @param strm the stream that receives the entry data
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extract(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): Promise<void>;

    /**
     * @description Decompresses one entry into a stream
     *
     *      The entry is located by name and its uncompressed bytes are written at the current
     *      position of strm; the stream is neither rewound nor closed. Use it to forward an entry
     *      into another consumer, or pass a MemoryStream to obtain its data as a Buffer. Unknown
     *      names throw `End of file.` and the password rules of read apply.
     *
     *      Example — decompress into a MemoryStream:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *
     *      const out = new io.MemoryStream();
     *      zipfile.extract('greeting.txt', out);
     *      out.rewind();
     *      console.log(out.readAll().toString()); // hello, zip
     *
     *      zipfile.close();
     *      out.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to decompress
     *      @param strm the stream that receives the entry data
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extractSync(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): void;

    /**
     * @description Decompresses one entry into a stream
     *
     *      The entry is located by name and its uncompressed bytes are written at the current
     *      position of strm; the stream is neither rewound nor closed. Use it to forward an entry
     *      into another consumer, or pass a MemoryStream to obtain its data as a Buffer. Unknown
     *      names throw `End of file.` and the password rules of read apply.
     *
     *      Example — decompress into a MemoryStream:
     *      ```JavaScript
     *      const io = require('io');
     *      const zip = require('zip');
     *
     *      const archive = new io.MemoryStream();
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('hello, zip'), 'greeting.txt');
     *      zipfile.close();
     *
     *      archive.rewind();
     *      zipfile = zip.open(archive.readAll());
     *
     *      const out = new io.MemoryStream();
     *      zipfile.extract('greeting.txt', out);
     *      out.rewind();
     *      console.log(out.readAll().toString()); // hello, zip
     *
     *      zipfile.close();
     *      out.close();
     *      archive.close();
     *      ```
     *      @param member the name of the entry to decompress
     *      @param strm the stream that receives the entry data
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extractAsync(member: string, strm: Class_SeekableStream | Class_SeekableStreamPromise, password?: string): Promise<void>;

    /**
     * @description Decompresses one entry to a file
     *
     *      The target file is created or truncated ("w"), and its parent directory must already
     *      exist; the entry is located by name as in getinfo, so an unknown name throws
     *      `End of file.` and the password rules of read apply.
     *
     *      Example — extract a single entry and read it back from disk:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('extracted'), 'data.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extract('data.txt', path.join(dir, 'out.txt'));
     *      zipfile.close();
     *
     *      console.log(fs.readFileSync(path.join(dir, 'out.txt'), 'utf8')); // extracted
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param member the name of the entry to decompress
     *      @param path path of the file to write
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extract(member: string, path: string, password?: string): Promise<void>;

    /**
     * @description Decompresses one entry to a file
     *
     *      The target file is created or truncated ("w"), and its parent directory must already
     *      exist; the entry is located by name as in getinfo, so an unknown name throws
     *      `End of file.` and the password rules of read apply.
     *
     *      Example — extract a single entry and read it back from disk:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('extracted'), 'data.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extract('data.txt', path.join(dir, 'out.txt'));
     *      zipfile.close();
     *
     *      console.log(fs.readFileSync(path.join(dir, 'out.txt'), 'utf8')); // extracted
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param member the name of the entry to decompress
     *      @param path path of the file to write
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extractSync(member: string, path: string, password?: string): void;

    /**
     * @description Decompresses one entry to a file
     *
     *      The target file is created or truncated ("w"), and its parent directory must already
     *      exist; the entry is located by name as in getinfo, so an unknown name throws
     *      `End of file.` and the password rules of read apply.
     *
     *      Example — extract a single entry and read it back from disk:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('extracted'), 'data.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extract('data.txt', path.join(dir, 'out.txt'));
     *      zipfile.close();
     *
     *      console.log(fs.readFileSync(path.join(dir, 'out.txt'), 'utf8')); // extracted
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param member the name of the entry to decompress
     *      @param path path of the file to write
     *      @param password password of an encrypted entry, empty for a plain entry
     *
     */
    extractAsync(member: string, path: string, password?: string): Promise<void>;

    /**
     * @description Decompresses every entry below a directory
     *
     *      The directory must exist, otherwise the call reports `ZipFile: no such file or
     *      directory`; the sub-directories found in the entry names are created below it. Entry
     *      names are normalized against path but are not confined to it, so a stored name that
     *      uses ".." can make the call write outside path; only unpack archives you trust. An
     *      entry whose target file is already taken is not overwritten: a trailing "?" is appended
     *      to its name, and more "?" characters are appended until the name is free. The password
     *      is applied to every entry (see readAll).
     *
     *      Example — unpack a small archive into an existing directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'app.zip');
     *      const out = path.join(dir, 'out');
     *      fs.mkdirSync(out);
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('{"name":"fibjs"}'), 'config/app.json');
     *      zipfile.write(Buffer.from('<h1>fibjs</h1>'), 'public/index.html');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extractAll(out);
     *      zipfile.close();
     *
     *      console.log(fs.readdirSync(out).sort().join(', ')); // config, public
     *      console.log(fs.readFileSync(path.join(out, 'config', 'app.json'), 'utf8'));
     *      // {"name":"fibjs"}
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory that receives the entries
     *      @param password password shared by all encrypted entries
     *
     */
    extractAll(path: string, password?: string): Promise<void>;

    /**
     * @description Decompresses every entry below a directory
     *
     *      The directory must exist, otherwise the call reports `ZipFile: no such file or
     *      directory`; the sub-directories found in the entry names are created below it. Entry
     *      names are normalized against path but are not confined to it, so a stored name that
     *      uses ".." can make the call write outside path; only unpack archives you trust. An
     *      entry whose target file is already taken is not overwritten: a trailing "?" is appended
     *      to its name, and more "?" characters are appended until the name is free. The password
     *      is applied to every entry (see readAll).
     *
     *      Example — unpack a small archive into an existing directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'app.zip');
     *      const out = path.join(dir, 'out');
     *      fs.mkdirSync(out);
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('{"name":"fibjs"}'), 'config/app.json');
     *      zipfile.write(Buffer.from('<h1>fibjs</h1>'), 'public/index.html');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extractAll(out);
     *      zipfile.close();
     *
     *      console.log(fs.readdirSync(out).sort().join(', ')); // config, public
     *      console.log(fs.readFileSync(path.join(out, 'config', 'app.json'), 'utf8'));
     *      // {"name":"fibjs"}
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory that receives the entries
     *      @param password password shared by all encrypted entries
     *
     */
    extractAllSync(path: string, password?: string): void;

    /**
     * @description Decompresses every entry below a directory
     *
     *      The directory must exist, otherwise the call reports `ZipFile: no such file or
     *      directory`; the sub-directories found in the entry names are created below it. Entry
     *      names are normalized against path but are not confined to it, so a stored name that
     *      uses ".." can make the call write outside path; only unpack archives you trust. An
     *      entry whose target file is already taken is not overwritten: a trailing "?" is appended
     *      to its name, and more "?" characters are appended until the name is free. The password
     *      is applied to every entry (see readAll).
     *
     *      Example — unpack a small archive into an existing directory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'app.zip');
     *      const out = path.join(dir, 'out');
     *      fs.mkdirSync(out);
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('{"name":"fibjs"}'), 'config/app.json');
     *      zipfile.write(Buffer.from('<h1>fibjs</h1>'), 'public/index.html');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      zipfile.extractAll(out);
     *      zipfile.close();
     *
     *      console.log(fs.readdirSync(out).sort().join(', ')); // config, public
     *      console.log(fs.readFileSync(path.join(out, 'config', 'app.json'), 'utf8'));
     *      // {"name":"fibjs"}
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param path the directory that receives the entries
     *      @param password password shared by all encrypted entries
     *
     */
    extractAllAsync(path: string, password?: string): Promise<void>;

    /**
     * @description Writes a Buffer as a new entry
     *
     *      The data is compressed with Deflate and stored under inZipName; a non-empty password
     *      encrypts the entry with ZipCrypto. Every entry is stamped with the current local time as
     *      its date. The archive must be open for writing ("w" or "a"), otherwise the call throws
     *      `ZipFile: file is closed.`, and the new entry becomes visible to readers at close().
     *
     *      Example — write plain and encrypted entries from memory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('written from memory'), 'memory.txt');
     *      zipfile.write(Buffer.from('encrypted'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.namelist().join(', ')); // memory.txt, secret.txt
     *      console.log(zipfile.getinfo('secret.txt').password); // true
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the data to write
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    write(data: Class_Buffer, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes a Buffer as a new entry
     *
     *      The data is compressed with Deflate and stored under inZipName; a non-empty password
     *      encrypts the entry with ZipCrypto. Every entry is stamped with the current local time as
     *      its date. The archive must be open for writing ("w" or "a"), otherwise the call throws
     *      `ZipFile: file is closed.`, and the new entry becomes visible to readers at close().
     *
     *      Example — write plain and encrypted entries from memory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('written from memory'), 'memory.txt');
     *      zipfile.write(Buffer.from('encrypted'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.namelist().join(', ')); // memory.txt, secret.txt
     *      console.log(zipfile.getinfo('secret.txt').password); // true
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the data to write
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeSync(data: Class_Buffer, inZipName: string, password?: string): void;

    /**
     * @description Writes a Buffer as a new entry
     *
     *      The data is compressed with Deflate and stored under inZipName; a non-empty password
     *      encrypts the entry with ZipCrypto. Every entry is stamped with the current local time as
     *      its date. The archive must be open for writing ("w" or "a"), otherwise the call throws
     *      `ZipFile: file is closed.`, and the new entry becomes visible to readers at close().
     *
     *      Example — write plain and encrypted entries from memory:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('written from memory'), 'memory.txt');
     *      zipfile.write(Buffer.from('encrypted'), 'secret.txt', 'pw');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.namelist().join(', ')); // memory.txt, secret.txt
     *      console.log(zipfile.getinfo('secret.txt').password); // true
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param data the data to write
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeAsync(data: Class_Buffer, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes a stream as a new entry
     *
     *      The stream is rewound and copied from its first byte to its end, then compressed with
     *      Deflate and stored under inZipName; the stream itself is left open. Date, password and
     *      open-mode rules are the same as for the Buffer form.
     *
     *      Example — write the contents of a MemoryStream:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const io = require('io');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const src = new io.MemoryStream();
     *      src.write(Buffer.from('streamed into the archive'));
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(src, 'stream.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.read('stream.txt').toString()); // streamed into the archive
     *      zipfile.close();
     *
     *      src.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param strm the stream whose data is written
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    write(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes a stream as a new entry
     *
     *      The stream is rewound and copied from its first byte to its end, then compressed with
     *      Deflate and stored under inZipName; the stream itself is left open. Date, password and
     *      open-mode rules are the same as for the Buffer form.
     *
     *      Example — write the contents of a MemoryStream:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const io = require('io');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const src = new io.MemoryStream();
     *      src.write(Buffer.from('streamed into the archive'));
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(src, 'stream.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.read('stream.txt').toString()); // streamed into the archive
     *      zipfile.close();
     *
     *      src.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param strm the stream whose data is written
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeSync(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): void;

    /**
     * @description Writes a stream as a new entry
     *
     *      The stream is rewound and copied from its first byte to its end, then compressed with
     *      Deflate and stored under inZipName; the stream itself is left open. Date, password and
     *      open-mode rules are the same as for the Buffer form.
     *
     *      Example — write the contents of a MemoryStream:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const io = require('io');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const src = new io.MemoryStream();
     *      src.write(Buffer.from('streamed into the archive'));
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(src, 'stream.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.read('stream.txt').toString()); // streamed into the archive
     *      zipfile.close();
     *
     *      src.close();
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param strm the stream whose data is written
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeAsync(strm: Class_SeekableStream | Class_SeekableStreamPromise, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes a local file as a new entry
     *
     *      The file is opened for reading and copied under inZipName, compressed with Deflate; a
     *      missing source reports ENOENT. The entry date is the current local time, not the
     *      modification time of the file. Password and open-mode rules are the same as for the
     *      Buffer form.
     *
     *      Example — store a report under a directory name:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *      fs.writeFileSync(path.join(dir, 'report.txt'), 'quarterly report');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(path.join(dir, 'report.txt'), 'docs/report.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.getinfo('docs/report.txt').file_size); // 16
     *      console.log(zipfile.read('docs/report.txt').toString()); // quarterly report
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename path of the file to write into the archive
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    write(filename: string, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Writes a local file as a new entry
     *
     *      The file is opened for reading and copied under inZipName, compressed with Deflate; a
     *      missing source reports ENOENT. The entry date is the current local time, not the
     *      modification time of the file. Password and open-mode rules are the same as for the
     *      Buffer form.
     *
     *      Example — store a report under a directory name:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *      fs.writeFileSync(path.join(dir, 'report.txt'), 'quarterly report');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(path.join(dir, 'report.txt'), 'docs/report.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.getinfo('docs/report.txt').file_size); // 16
     *      console.log(zipfile.read('docs/report.txt').toString()); // quarterly report
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename path of the file to write into the archive
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeSync(filename: string, inZipName: string, password?: string): void;

    /**
     * @description Writes a local file as a new entry
     *
     *      The file is opened for reading and copied under inZipName, compressed with Deflate; a
     *      missing source reports ENOENT. The entry date is the current local time, not the
     *      modification time of the file. Password and open-mode rules are the same as for the
     *      Buffer form.
     *
     *      Example — store a report under a directory name:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *      fs.writeFileSync(path.join(dir, 'report.txt'), 'quarterly report');
     *
     *      let zipfile = zip.open(archive, 'w');
     *      zipfile.write(path.join(dir, 'report.txt'), 'docs/report.txt');
     *      zipfile.close();
     *
     *      zipfile = zip.open(archive);
     *      console.log(zipfile.getinfo('docs/report.txt').file_size); // 16
     *      console.log(zipfile.read('docs/report.txt').toString()); // quarterly report
     *      zipfile.close();
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *      @param filename path of the file to write into the archive
     *      @param inZipName the name of the entry inside the archive
     *      @param password password used to encrypt the entry, empty for no encryption
     *
     */
    writeAsync(filename: string, inZipName: string, password?: string): Promise<void>;

    /**
     * @description Closes the archive and completes pending writes
     *
     *      On a writer ("w"/"a") close() appends the central directory, which is what makes the new
     *      entries visible to other readers; without it the archive cannot be read back. On a
     *      reader it releases the underlying stream. Calling close() again is harmless, but every
     *      member used after it throws `ZipFile: file is closed.`
     *
     *      Example — the second close is a no-op, further writes are rejected:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close(); // appends the central directory
     *      zipfile.close(); // harmless the second time
     *
     *      try {
     *          zipfile.write(Buffer.from('more'), 'more.txt');
     *      } catch (e) {
     *          console.log(e.message); // ZipFile: file is closed.
     *      }
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    close(): Promise<void>;

    /**
     * @description Closes the archive and completes pending writes
     *
     *      On a writer ("w"/"a") close() appends the central directory, which is what makes the new
     *      entries visible to other readers; without it the archive cannot be read back. On a
     *      reader it releases the underlying stream. Calling close() again is harmless, but every
     *      member used after it throws `ZipFile: file is closed.`
     *
     *      Example — the second close is a no-op, further writes are rejected:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close(); // appends the central directory
     *      zipfile.close(); // harmless the second time
     *
     *      try {
     *          zipfile.write(Buffer.from('more'), 'more.txt');
     *      } catch (e) {
     *          console.log(e.message); // ZipFile: file is closed.
     *      }
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    closeSync(): void;

    /**
     * @description Closes the archive and completes pending writes
     *
     *      On a writer ("w"/"a") close() appends the central directory, which is what makes the new
     *      entries visible to other readers; without it the archive cannot be read back. On a
     *      reader it releases the underlying stream. Calling close() again is harmless, but every
     *      member used after it throws `ZipFile: file is closed.`
     *
     *      Example — the second close is a no-op, further writes are rejected:
     *      ```JavaScript
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const zip = require('zip');
     *
     *      const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'fibjs-zipfile-'));
     *      const archive = path.join(dir, 'a.zip');
     *
     *      const zipfile = zip.open(archive, 'w');
     *      zipfile.write(Buffer.from('data'), 'data.txt');
     *      zipfile.close(); // appends the central directory
     *      zipfile.close(); // harmless the second time
     *
     *      try {
     *          zipfile.write(Buffer.from('more'), 'more.txt');
     *      } catch (e) {
     *          console.log(e.message); // ZipFile: file is closed.
     *      }
     *
     *      fs.rmSync(dir, { recursive: true, force: true });
     *      ```
     *
     */
    closeAsync(): Promise<void>;

}


declare namespace Class_ZipFile {
    const promises: FIBJS.GeneralObject;
}
