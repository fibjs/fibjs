const path = require('path');
const fs = require('fs');
const zlib = require('zlib');
const http = require('http');

//--------------------- untar -----------------------------
const untar = (function () {
    /*
     * bytestream.js
     *
     * Provides readers for byte streams (reading).
     *
     * Licensed under the MIT License
     *
     * Copyright(c) 2011 Google Inc.
     * Copyright(c) 2011 antimatter15
     */

    /**
     * This object allows you to peek and consume bytes as numbers and strings
     * out of an ArrayBuffer.  In this buffer, everything must be byte-aligned.
     *
     * @param {ArrayBuffer} ab The ArrayBuffer object.
     * @param {number=} opt_offset The offset into the ArrayBuffer
     * @param {number=} opt_length The length of this BitStream
     * @constructor
     */
    ByteStream = function (ab, opt_offset, opt_length) {
        var offset = opt_offset || 0;
        var length = opt_length || ab.byteLength;
        this.bytes = new Uint8Array(ab, offset, length);
        this.ptr = 0;
    };


    /**
     * Peeks at the next n bytes as an unsigned number but does not advance the
     * pointer
     * TODO: This apparently cannot read more than 4 bytes as a number?
     * @param {number} n The number of bytes to peek at.
     * @return {number} The n bytes interpreted as an unsigned number.
     */
    ByteStream.prototype.peekNumber = function (n) {
        // TODO: return error if n would go past the end of the stream?
        if (n <= 0 || typeof n != typeof 1)
            return -1;

        var result = 0;
        // read from last byte to first byte and roll them in
        var curByte = this.ptr + n - 1;
        while (curByte >= this.ptr) {
            result <<= 8;
            result |= this.bytes[curByte];
            --curByte;
        }
        return result;
    };


    /**
     * Returns the next n bytes as an unsigned number (or -1 on error)
     * and advances the stream pointer n bytes.
     * @param {number} n The number of bytes to read.
     * @return {number} The n bytes interpreted as an unsigned number.
     */
    ByteStream.prototype.readNumber = function (n) {
        var num = this.peekNumber(n);
        this.ptr += n;
        return num;
    };


    /**
     * Returns the next n bytes as a signed number but does not advance the
     * pointer.
     * @param {number} n The number of bytes to read.
     * @return {number} The bytes interpreted as a signed number.
     */
    ByteStream.prototype.peekSignedNumber = function (n) {
        var num = this.peekNumber(n);
        var HALF = Math.pow(2, (n * 8) - 1);
        var FULL = HALF * 2;

        if (num >= HALF) num -= FULL;

        return num;
    };


    /**
     * Returns the next n bytes as a signed number and advances the stream pointer.
     * @param {number} n The number of bytes to read.
     * @return {number} The bytes interpreted as a signed number.
     */
    ByteStream.prototype.readSignedNumber = function (n) {
        var num = this.peekSignedNumber(n);
        this.ptr += n;
        return num;
    };


    /**
     * This returns n bytes as a sub-array, advancing the pointer if movePointers
     * is true.
     * @param {number} n The number of bytes to read.
     * @param {boolean} movePointers Whether to move the pointers.
     * @return {Uint8Array} The subarray.
     */
    ByteStream.prototype.peekBytes = function (n, movePointers) {
        if (n <= 0 || typeof n != typeof 1) {
            return null;
        }

        var result = this.bytes.subarray(this.ptr, this.ptr + n);

        if (movePointers) {
            this.ptr += n;
        }

        return result;
    };


    /**
     * Reads the next n bytes as a sub-array.
     * @param {number} n The number of bytes to read.
     * @return {Uint8Array} The subarray.
     */
    ByteStream.prototype.readBytes = function (n) {
        return this.peekBytes(n, true);
    };


    /**
     * Returns the next n bytes as an ASCII string and advances the stream pointer
     * n bytes.
     * @param {number} n The number of bytes to read.
     * @return {string} The next n bytes as a string.
     */
    ByteStream.prototype.readString = function (n) {
        var p = this.ptr;

        for (var i = 0; i < n; i++)
            if (this.bytes[p] == 0)
                break;
            else p++;

        var binaryString = Buffer.from(this.bytes.buffer, this.ptr, p - this.ptr);
        var strToReturn = binaryString.toString('latin1');

        this.ptr += n;
        return strToReturn;
    };

    // takes a ByteStream and parses out the local file information
    function TarLocalFile(bstream) {
        this.isValid = false;

        // Read in the header block
        this.name = bstream.readString(100);
        this.mode = bstream.readString(8);
        this.uid = bstream.readString(8);
        this.gid = bstream.readString(8);

        this.size = parseInt(bstream.readString(12), 8);
        this.mtime = bstream.readString(12);
        this.chksum = bstream.readString(8);
        this.typeflag = bstream.readString(1);
        this.linkname = bstream.readString(100);
        this.maybeMagic = bstream.readString(6);

        // 100+8+8+8+12+12+8+1+100+6 = 263 Bytes

        if (this.maybeMagic == "ustar") {
            this.version = bstream.readString(2);
            this.uname = bstream.readString(32);
            this.gname = bstream.readString(32);
            this.devmajor = bstream.readString(8);
            this.devminor = bstream.readString(8);
            this.prefix = bstream.readString(155);

            // 2+32+32+8+8+155 = 237 Bytes

            if (this.prefix.length)
                if (this.prefix.endsWith('/'))
                    this.name = this.prefix + this.name;
                else
                    this.name = this.prefix + '/' + this.name;

            bstream.readBytes(12); // 512 - 263 - 237
        } else {
            bstream.readBytes(249); // 512 - 263
        }

        // Done header, now rest of blocks are the file contents.
        this.filename = this.name;
        this.fileData = null;

        // console.info("Untarring file '" + this.filename + "'");
        // console.info("  size = " + this.size);
        // console.info("  typeflag = " + this.typeflag);

        if (this.typeflag == 0) {
            // console.info("  This is a regular file.");
            var sizeInBytes = parseInt(this.size);
            this.fileData = new Uint8Array(bstream.bytes.buffer, bstream.ptr, this.size);
            if (this.name.length > 0 && this.fileData.buffer) {
                this.isValid = true;
            }
        } else if (this.typeflag == 5) {
            // console.info("  This is a directory.")
        }

        bstream.ptr += this.size;
        // Round up to 512-byte blocks.
        var remaining = 512 - bstream.ptr % 512;
        // console.log('remaining')
        if (remaining > 0 && remaining < 512) {
            bstream.readBytes(remaining)
        }
    };

    return function (arrayBuffer) {
        var bstream = new ByteStream(arrayBuffer);
        var localFiles = [];
        // While we don't encounter an empty block, keep making TarLocalFiles.
        while (bstream.peekNumber(4) != 0) {
            var oneLocalFile = new TarLocalFile(bstream);
            if (oneLocalFile && oneLocalFile.typeflag != 5)
                localFiles.push(oneLocalFile);
        }
        return localFiles;
    }
})();

//--------------------- untar -----------------------------

const packageJson = JSON.parse(fs.readFileSync(path.resolve(__dirname, 'package.json'), 'utf8'));

const matchs = {
    version: packageJson.version,
    platform: process.versions.musl ? "alpine" : process.platform,
    arch: process.arch,
    module_name: packageJson.binary.module_name
}

function $(p) {
    for (const key in matchs) {
        p = p.replace(new RegExp(`{${key}}`, 'g'), matchs[key]);
    }

    return p;
}

async function install_addon() {
    const url = `${$(packageJson.binary.host)}${$(packageJson.binary.remote_path)}/${$(packageJson.binary.package_name)}`;
    console.log("[INFO] Downloading", url);

    const res = await fetch(url);
    if (res.status !== 200) {
        throw new Error(`Failed to download ${url}`);
    }

    let tgz = res.arrayBuffer ? new Uint8Array(await res.arrayBuffer()) : res.data;

    let t;
    if (tgz[0] === 0x1f && tgz[1] === 0x8b)
        t = zlib.gunzipSync(tgz);
    else
        t = tgz;
    tgz = null;

    const untar_files = untar(t.buffer);
    t = null;

    if (untar_files.length !== 1) {
        throw new Error('Invalid tar file');
    }

    const file = untar_files[0];

    const tpath = path.join(__dirname, packageJson.binary.module_path);
    const tname = path.join(tpath, path.basename(file.filename));

    fs.mkdirSync(tpath, { recursive: true });
    fs.writeFileSync(tname, file.fileData);
    fs.chmodSync(tname, parseInt(file.mode, 8));

    console.log("[INFO] Installed", tname);
}

if (process.env.http_proxy || process.env.HTTP_PROXY) {
    http.http_proxy = process.env.http_proxy || process.env.HTTP_PROXY;
    console.log(`[install] http request using proxy: ${http.http_proxy}`);
}

if (process.env.https_proxy || process.env.HTTPS_PROXY) {
    http.https_proxy = process.env.https_proxy || process.env.HTTPS_PROXY;
    console.log(`[install] https request using proxy: ${http.https_proxy}`);
}

if (globalThis.fetch === undefined) {
    globalThis.fetch = http.getAsync;
}

install_addon();