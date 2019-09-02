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
     * Peeks at the next n bytes as a string but does not advance the pointer.
     * @param {number} n The number of bytes to peek at.
     * @return {string} The next n bytes as a string.
     */
    ByteStream.prototype.peekString = function (n) {
        if (n <= 0 || typeof n != typeof 1) {
            return "";
        }

        var result = "";
        for (var p = this.ptr, end = this.ptr + n; p < end; ++p) {
            result += String.fromCharCode(this.bytes[p]);
        }
        return result;
    };


    /**
     * Returns the next n bytes as an ASCII string and advances the stream pointer
     * n bytes.
     * @param {number} n The number of bytes to read.
     * @return {string} The next n bytes as a string.
     */
    ByteStream.prototype.readString = function (n) {
        var strToReturn = this.peekString(n);
        this.ptr += n;
        return strToReturn;
    };


    // Removes all characters from the first zero-byte in the string onwards.
    var readCleanString = function (bstr, numBytes) {
        var str = bstr.readString(numBytes);
        var zIndex = str.indexOf(String.fromCharCode(0));
        return zIndex != -1 ? str.substr(0, zIndex) : str;
    };

    // takes a ByteStream and parses out the local file information
    function TarLocalFile(bstream) {
        this.isValid = false;

        // Read in the header block
        this.name = readCleanString(bstream, 100);
        this.mode = readCleanString(bstream, 8);
        this.uid = readCleanString(bstream, 8);
        this.gid = readCleanString(bstream, 8);

        this.size = parseInt(readCleanString(bstream, 12), 8);
        this.mtime = readCleanString(bstream, 12);
        this.chksum = readCleanString(bstream, 8);
        this.typeflag = readCleanString(bstream, 1);
        this.linkname = readCleanString(bstream, 100);
        this.maybeMagic = readCleanString(bstream, 6);

        // 100+8+8+8+12+12+8+1+100+6 = 263 Bytes

        if (this.maybeMagic == "ustar") {
            this.version = readCleanString(bstream, 2);
            this.uname = readCleanString(bstream, 32);
            this.gname = readCleanString(bstream, 32);
            this.devmajor = readCleanString(bstream, 8);
            this.devminor = readCleanString(bstream, 8);
            this.prefix = readCleanString(bstream, 155);

            // 2+32+32+8+8+155 = 237 Bytes

            if (this.prefix.length) {
                this.name = this.prefix + this.name;
            }
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
            if (oneLocalFile && oneLocalFile.isValid) {
                localFiles.push(oneLocalFile);
                // totalUncompressedBytesInArchive += oneLocalFile.size;
            }
        }
        return localFiles;
    }
})();

exports = module.exports = untar;