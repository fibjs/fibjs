//--------------------- untar -----------------------------
var untar = (function () {
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
            if (this.name.length > 0 && this.size > 0 && this.fileData && this.fileData.buffer) {
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
//--------------------- untar -----------------------------
var semvar = (function () {
    var exports = {};
    var debug;
    if (typeof process === 'object' &&
        process.env &&
        process.env.NODE_DEBUG &&
        /\bsemver\b/i.test(process.env.NODE_DEBUG))
        debug = function () {};
    else
        debug = function () {};
    exports.SEMVER_SPEC_VERSION = '2.0.0';
    var MAX_LENGTH = 256;
    var MAX_SAFE_INTEGER = Number.MAX_SAFE_INTEGER || 9007199254740991;
    var re = exports.re = [];
    var src = exports.src = [];
    var R = 0;

    var NUMERICIDENTIFIER = R++;
    src[NUMERICIDENTIFIER] = '0|[1-9]\\d*';
    var NUMERICIDENTIFIERLOOSE = R++;
    src[NUMERICIDENTIFIERLOOSE] = '[0-9]+';

    var NONNUMERICIDENTIFIER = R++;
    src[NONNUMERICIDENTIFIER] = '\\d*[a-zA-Z-][a-zA-Z0-9-]*';

    var MAINVERSION = R++;
    src[MAINVERSION] = '(' + src[NUMERICIDENTIFIER] + ')\\.' +
        '(' + src[NUMERICIDENTIFIER] + ')\\.' +
        '(' + src[NUMERICIDENTIFIER] + ')';
    var MAINVERSIONLOOSE = R++;
    src[MAINVERSIONLOOSE] = '(' + src[NUMERICIDENTIFIERLOOSE] + ')\\.' +
        '(' + src[NUMERICIDENTIFIERLOOSE] + ')\\.' +
        '(' + src[NUMERICIDENTIFIERLOOSE] + ')';

    var PRERELEASEIDENTIFIER = R++;
    src[PRERELEASEIDENTIFIER] = '(?:' + src[NUMERICIDENTIFIER] +
        '|' + src[NONNUMERICIDENTIFIER] + ')';
    var PRERELEASEIDENTIFIERLOOSE = R++;
    src[PRERELEASEIDENTIFIERLOOSE] = '(?:' + src[NUMERICIDENTIFIERLOOSE] +
        '|' + src[NONNUMERICIDENTIFIER] + ')';

    var PRERELEASE = R++;
    src[PRERELEASE] = '(?:-(' + src[PRERELEASEIDENTIFIER] +
        '(?:\\.' + src[PRERELEASEIDENTIFIER] + ')*))';
    var PRERELEASELOOSE = R++;
    src[PRERELEASELOOSE] = '(?:-?(' + src[PRERELEASEIDENTIFIERLOOSE] +
        '(?:\\.' + src[PRERELEASEIDENTIFIERLOOSE] + ')*))';

    var BUILDIDENTIFIER = R++;
    src[BUILDIDENTIFIER] = '[0-9A-Za-z-]+';

    var BUILD = R++;
    src[BUILD] = '(?:\\+(' + src[BUILDIDENTIFIER] +
        '(?:\\.' + src[BUILDIDENTIFIER] + ')*))';

    var FULL = R++;
    var FULLPLAIN = 'v?' + src[MAINVERSION] +
        src[PRERELEASE] + '?' +
        src[BUILD] + '?';
    src[FULL] = '^' + FULLPLAIN + '$';
    var LOOSEPLAIN = '[v=\\s]*' + src[MAINVERSIONLOOSE] +
        src[PRERELEASELOOSE] + '?' +
        src[BUILD] + '?';
    var LOOSE = R++;
    src[LOOSE] = '^' + LOOSEPLAIN + '$';
    var GTLT = R++;
    src[GTLT] = '((?:<|>)?=?)';
    var XRANGEIDENTIFIERLOOSE = R++;
    src[XRANGEIDENTIFIERLOOSE] = src[NUMERICIDENTIFIERLOOSE] + '|x|X|\\*';
    var XRANGEIDENTIFIER = R++;
    src[XRANGEIDENTIFIER] = src[NUMERICIDENTIFIER] + '|x|X|\\*';
    var XRANGEPLAIN = R++;
    src[XRANGEPLAIN] = '[v=\\s]*(' + src[XRANGEIDENTIFIER] + ')' +
        '(?:\\.(' + src[XRANGEIDENTIFIER] + ')' +
        '(?:\\.(' + src[XRANGEIDENTIFIER] + ')' +
        '(?:' + src[PRERELEASE] + ')?' +
        src[BUILD] + '?' +
        ')?)?';
    var XRANGEPLAINLOOSE = R++;
    src[XRANGEPLAINLOOSE] = '[v=\\s]*(' + src[XRANGEIDENTIFIERLOOSE] + ')' +
        '(?:\\.(' + src[XRANGEIDENTIFIERLOOSE] + ')' +
        '(?:\\.(' + src[XRANGEIDENTIFIERLOOSE] + ')' +
        '(?:' + src[PRERELEASELOOSE] + ')?' +
        src[BUILD] + '?' +
        ')?)?';
    var XRANGE = R++;
    src[XRANGE] = '^' + src[GTLT] + '\\s*' + src[XRANGEPLAIN] + '$';
    var XRANGELOOSE = R++;
    src[XRANGELOOSE] = '^' + src[GTLT] + '\\s*' + src[XRANGEPLAINLOOSE] + '$';
    var LONETILDE = R++;
    src[LONETILDE] = '(?:~>?)';
    var TILDETRIM = R++;
    src[TILDETRIM] = '(\\s*)' + src[LONETILDE] + '\\s+';
    re[TILDETRIM] = new RegExp(src[TILDETRIM], 'g');
    var tildeTrimReplace = '$1~';
    var TILDE = R++;
    src[TILDE] = '^' + src[LONETILDE] + src[XRANGEPLAIN] + '$';
    var TILDELOOSE = R++;
    src[TILDELOOSE] = '^' + src[LONETILDE] + src[XRANGEPLAINLOOSE] + '$';
    var LONECARET = R++;
    src[LONECARET] = '(?:\\^)';
    var CARETTRIM = R++;
    src[CARETTRIM] = '(\\s*)' + src[LONECARET] + '\\s+';
    re[CARETTRIM] = new RegExp(src[CARETTRIM], 'g');
    var caretTrimReplace = '$1^';
    var CARET = R++;
    src[CARET] = '^' + src[LONECARET] + src[XRANGEPLAIN] + '$';
    var CARETLOOSE = R++;
    src[CARETLOOSE] = '^' + src[LONECARET] + src[XRANGEPLAINLOOSE] + '$';
    var COMPARATORLOOSE = R++;
    src[COMPARATORLOOSE] = '^' + src[GTLT] + '\\s*(' + LOOSEPLAIN + ')$|^$';
    var COMPARATOR = R++;
    src[COMPARATOR] = '^' + src[GTLT] + '\\s*(' + FULLPLAIN + ')$|^$';

    var COMPARATORTRIM = R++;
    src[COMPARATORTRIM] = '(\\s*)' + src[GTLT] +
        '\\s*(' + LOOSEPLAIN + '|' + src[XRANGEPLAIN] + ')';
    re[COMPARATORTRIM] = new RegExp(src[COMPARATORTRIM], 'g');
    var comparatorTrimReplace = '$1$2$3';

    var HYPHENRANGE = R++;
    src[HYPHENRANGE] = '^\\s*(' + src[XRANGEPLAIN] + ')' +
        '\\s+-\\s+' +
        '(' + src[XRANGEPLAIN] + ')' +
        '\\s*$';
    var HYPHENRANGELOOSE = R++;
    src[HYPHENRANGELOOSE] = '^\\s*(' + src[XRANGEPLAINLOOSE] + ')' +
        '\\s+-\\s+' +
        '(' + src[XRANGEPLAINLOOSE] + ')' +
        '\\s*$';
    var STAR = R++;
    src[STAR] = '(<|>)?=?\\s*\\*';
    for (var i = 0; i < R; i++) {
        debug(i, src[i]);
        if (!re[i])
            re[i] = new RegExp(src[i]);
    }
    exports.parse = parse;

    function parse(version, loose) {
        if (version instanceof SemVer)
            return version;
        if (typeof version !== 'string')
            return null;
        if (version.length > MAX_LENGTH)
            return null;
        var r = loose ? re[LOOSE] : re[FULL];
        if (!r.test(version))
            return null;
        try {
            return new SemVer(version, loose);
        } catch (er) {
            return null;
        }
    }
    exports.valid = valid;

    function valid(version, loose) {
        var v = parse(version, loose);
        return v ? v.version : null;
    }

    exports.clean = clean;

    function clean(version, loose) {
        var s = parse(version.trim().replace(/^[=v]+/, ''), loose);
        return s ? s.version : null;
    }
    exports.SemVer = SemVer;

    function SemVer(version, loose) {
        if (version instanceof SemVer) {
            if (version.loose === loose)
                return version;
            else
                version = version.version;
        } else if (typeof version !== 'string') {
            throw new TypeError('Invalid Version: ' + version);
        }
        if (version.length > MAX_LENGTH)
            throw new TypeError('version is longer than ' + MAX_LENGTH + ' characters')
        if (!(this instanceof SemVer))
            return new SemVer(version, loose);
        debug('SemVer', version, loose);
        this.loose = loose;
        var m = version.trim().match(loose ? re[LOOSE] : re[FULL]);
        if (!m)
            throw new TypeError('Invalid Version: ' + version);
        this.raw = version;
        this.major = +m[1];
        this.minor = +m[2];
        this.patch = +m[3];
        if (this.major > MAX_SAFE_INTEGER || this.major < 0)
            throw new TypeError('Invalid major version')
        if (this.minor > MAX_SAFE_INTEGER || this.minor < 0)
            throw new TypeError('Invalid minor version')
        if (this.patch > MAX_SAFE_INTEGER || this.patch < 0)
            throw new TypeError('Invalid patch version')
        if (!m[4])
            this.prerelease = [];
        else
            this.prerelease = m[4].split('.').map(function (id) {
                if (/^[0-9]+$/.test(id)) {
                    var num = +id;
                    if (num >= 0 && num < MAX_SAFE_INTEGER)
                        return num;
                }
                return id;
            });
        this.build = m[5] ? m[5].split('.') : [];
        this.format();
    }
    SemVer.prototype.format = function () {
        this.version = this.major + '.' + this.minor + '.' + this.patch;
        if (this.prerelease.length)
            this.version += '-' + this.prerelease.join('.');
        return this.version;
    };
    SemVer.prototype.toString = function () {
        return this.version;
    };
    SemVer.prototype.compare = function (other) {
        debug('SemVer.compare', this.version, this.loose, other);
        if (!(other instanceof SemVer))
            other = new SemVer(other, this.loose);
        return this.compareMain(other) || this.comparePre(other);
    };
    SemVer.prototype.compareMain = function (other) {
        if (!(other instanceof SemVer))
            other = new SemVer(other, this.loose);
        return compareIdentifiers(this.major, other.major) ||
            compareIdentifiers(this.minor, other.minor) ||
            compareIdentifiers(this.patch, other.patch);
    };
    SemVer.prototype.comparePre = function (other) {
        if (!(other instanceof SemVer))
            other = new SemVer(other, this.loose);
        if (this.prerelease.length && !other.prerelease.length)
            return -1;
        else if (!this.prerelease.length && other.prerelease.length)
            return 1;
        else if (!this.prerelease.length && !other.prerelease.length)
            return 0;
        var i = 0;
        do {
            var a = this.prerelease[i];
            var b = other.prerelease[i];
            debug('prerelease compare', i, a, b);
            if (a === undefined && b === undefined)
                return 0;
            else if (b === undefined)
                return 1;
            else if (a === undefined)
                return -1;
            else if (a === b)
                continue;
            else
                return compareIdentifiers(a, b);
        } while (++i);
    };
    SemVer.prototype.inc = function (release, identifier) {
        switch (release) {
            case 'premajor':
                this.prerelease.length = 0;
                this.patch = 0;
                this.minor = 0;
                this.major++;
                this.inc('pre', identifier);
                break;
            case 'preminor':
                this.prerelease.length = 0;
                this.patch = 0;
                this.minor++;
                this.inc('pre', identifier);
                break;
            case 'prepatch':
                this.prerelease.length = 0;
                this.inc('patch', identifier);
                this.inc('pre', identifier);
                break;
            case 'prerelease':
                if (this.prerelease.length === 0)
                    this.inc('patch', identifier);
                this.inc('pre', identifier);
                break;
            case 'major':
                if (this.minor !== 0 || this.patch !== 0 || this.prerelease.length === 0)
                    this.major++;
                this.minor = 0;
                this.patch = 0;
                this.prerelease = [];
                break;
            case 'minor':
                if (this.patch !== 0 || this.prerelease.length === 0)
                    this.minor++;
                this.patch = 0;
                this.prerelease = [];
                break;
            case 'patch':
                if (this.prerelease.length === 0)
                    this.patch++;
                this.prerelease = [];
                break;
            case 'pre':
                if (this.prerelease.length === 0)
                    this.prerelease = [0];
                else {
                    var i = this.prerelease.length;
                    while (--i >= 0) {
                        if (typeof this.prerelease[i] === 'number') {
                            this.prerelease[i]++;
                            i = -2;
                        }
                    }
                    if (i === -1) // didn't increment anything
                        this.prerelease.push(0);
                }
                if (identifier) {
                    if (this.prerelease[0] === identifier) {
                        if (isNaN(this.prerelease[1]))
                            this.prerelease = [identifier, 0];
                    } else
                        this.prerelease = [identifier, 0];
                }
                break;
            default:
                throw new Error('invalid increment argument: ' + release);
        }
        this.format();
        this.raw = this.version;
        return this;
    };
    exports.inc = inc;

    function inc(version, release, loose, identifier) {
        if (typeof (loose) === 'string') {
            identifier = loose;
            loose = undefined;
        }
        try {
            return new SemVer(version, loose).inc(release, identifier).version;
        } catch (er) {
            return null;
        }
    }
    exports.diff = diff;

    function diff(version1, version2) {
        if (eq(version1, version2)) {
            return null;
        } else {
            var v1 = parse(version1);
            var v2 = parse(version2);
            if (v1.prerelease.length || v2.prerelease.length) {
                for (var key in v1) {
                    if (key === 'major' || key === 'minor' || key === 'patch') {
                        if (v1[key] !== v2[key]) {
                            return 'pre' + key;
                        }
                    }
                }
                return 'prerelease';
            }
            for (var key in v1) {
                if (key === 'major' || key === 'minor' || key === 'patch') {
                    if (v1[key] !== v2[key]) {
                        return key;
                    }
                }
            }
        }
    }
    exports.compareIdentifiers = compareIdentifiers;
    var numeric = /^[0-9]+$/;

    function compareIdentifiers(a, b) {
        var anum = numeric.test(a);
        var bnum = numeric.test(b);
        if (anum && bnum) {
            a = +a;
            b = +b;
        }
        return (anum && !bnum) ? -1 :
            (bnum && !anum) ? 1 :
            a < b ? -1 :
            a > b ? 1 :
            0;
    }
    exports.rcompareIdentifiers = rcompareIdentifiers;

    function rcompareIdentifiers(a, b) {
        return compareIdentifiers(b, a);
    }
    exports.major = major;

    function major(a, loose) {
        return new SemVer(a, loose).major;
    }
    exports.minor = minor;

    function minor(a, loose) {
        return new SemVer(a, loose).minor;
    }
    exports.patch = patch;

    function patch(a, loose) {
        return new SemVer(a, loose).patch;
    }
    exports.compare = compare;

    function compare(a, b, loose) {
        return new SemVer(a, loose).compare(new SemVer(b, loose));
    }
    exports.compareLoose = compareLoose;

    function compareLoose(a, b) {
        return compare(a, b, true);
    }
    exports.rcompare = rcompare;

    function rcompare(a, b, loose) {
        return compare(b, a, loose);
    }
    exports.sort = sort;

    function sort(list, loose) {
        return list.sort(function (a, b) {
            return exports.compare(a, b, loose);
        });
    }
    exports.rsort = rsort;

    function rsort(list, loose) {
        return list.sort(function (a, b) {
            return exports.rcompare(a, b, loose);
        });
    }
    exports.gt = gt;

    function gt(a, b, loose) {
        return compare(a, b, loose) > 0;
    }
    exports.lt = lt;

    function lt(a, b, loose) {
        return compare(a, b, loose) < 0;
    }
    exports.eq = eq;

    function eq(a, b, loose) {
        return compare(a, b, loose) === 0;
    }
    exports.neq = neq;

    function neq(a, b, loose) {
        return compare(a, b, loose) !== 0;
    }
    exports.gte = gte;

    function gte(a, b, loose) {
        return compare(a, b, loose) >= 0;
    }
    exports.lte = lte;

    function lte(a, b, loose) {
        return compare(a, b, loose) <= 0;
    }
    exports.cmp = cmp;

    function cmp(a, op, b, loose) {
        var ret;
        switch (op) {
            case '===':
                if (typeof a === 'object') a = a.version;
                if (typeof b === 'object') b = b.version;
                ret = a === b;
                break;
            case '!==':
                if (typeof a === 'object') a = a.version;
                if (typeof b === 'object') b = b.version;
                ret = a !== b;
                break;
            case '':
            case '=':
            case '==':
                ret = eq(a, b, loose);
                break;
            case '!=':
                ret = neq(a, b, loose);
                break;
            case '>':
                ret = gt(a, b, loose);
                break;
            case '>=':
                ret = gte(a, b, loose);
                break;
            case '<':
                ret = lt(a, b, loose);
                break;
            case '<=':
                ret = lte(a, b, loose);
                break;
            default:
                throw new TypeError('Invalid operator: ' + op);
        }
        return ret;
    }
    exports.Comparator = Comparator;

    function Comparator(comp, loose) {
        if (comp instanceof Comparator) {
            if (comp.loose === loose)
                return comp;
            else
                comp = comp.value;
        }
        if (!(this instanceof Comparator))
            return new Comparator(comp, loose);
        debug('comparator', comp, loose);
        this.loose = loose;
        this.parse(comp);
        if (this.semver === ANY)
            this.value = '';
        else
            this.value = this.operator + this.semver.version;
        debug('comp', this);
    }
    var ANY = {};
    Comparator.prototype.parse = function (comp) {
        var r = this.loose ? re[COMPARATORLOOSE] : re[COMPARATOR];
        var m = comp.match(r);
        if (!m)
            throw new TypeError('Invalid comparator: ' + comp);
        this.operator = m[1];
        if (this.operator === '=')
            this.operator = '';
        if (!m[2])
            this.semver = ANY;
        else
            this.semver = new SemVer(m[2], this.loose);
    };
    Comparator.prototype.toString = function () {
        return this.value;
    };
    Comparator.prototype.test = function (version) {
        debug('Comparator.test', version, this.loose);
        if (this.semver === ANY)
            return true;
        if (typeof version === 'string')
            version = new SemVer(version, this.loose);
        return cmp(version, this.operator, this.semver, this.loose);
    };
    Comparator.prototype.intersects = function (comp, loose) {
        if (!(comp instanceof Comparator)) {
            throw new TypeError('a Comparator is required');
        }
        var rangeTmp;
        if (this.operator === '') {
            rangeTmp = new Range(comp.value, loose);
            return satisfies(this.value, rangeTmp, loose);
        } else if (comp.operator === '') {
            rangeTmp = new Range(this.value, loose);
            return satisfies(comp.semver, rangeTmp, loose);
        }
        var sameDirectionIncreasing =
            (this.operator === '>=' || this.operator === '>') &&
            (comp.operator === '>=' || comp.operator === '>');
        var sameDirectionDecreasing =
            (this.operator === '<=' || this.operator === '<') &&
            (comp.operator === '<=' || comp.operator === '<');
        var sameSemVer = this.semver.version === comp.semver.version;
        var differentDirectionsInclusive =
            (this.operator === '>=' || this.operator === '<=') &&
            (comp.operator === '>=' || comp.operator === '<=');
        var oppositeDirectionsLessThan =
            cmp(this.semver, '<', comp.semver, loose) &&
            ((this.operator === '>=' || this.operator === '>') &&
                (comp.operator === '<=' || comp.operator === '<'));
        var oppositeDirectionsGreaterThan =
            cmp(this.semver, '>', comp.semver, loose) &&
            ((this.operator === '<=' || this.operator === '<') &&
                (comp.operator === '>=' || comp.operator === '>'));
        return sameDirectionIncreasing || sameDirectionDecreasing ||
            (sameSemVer && differentDirectionsInclusive) ||
            oppositeDirectionsLessThan || oppositeDirectionsGreaterThan;
    };

    exports.Range = Range;

    function Range(range, loose) {
        if (range instanceof Range) {
            if (range.loose === loose) {
                return range;
            } else {
                return new Range(range.raw, loose);
            }
        }
        if (range instanceof Comparator) {
            return new Range(range.value, loose);
        }
        if (!(this instanceof Range))
            return new Range(range, loose);
        this.loose = loose;
        this.raw = range;
        this.set = range.split(/\s*\|\|\s*/).map(function (range) {
            return this.parseRange(range.trim());
        }, this).filter(function (c) {
            return c.length;
        });
        if (!this.set.length) {
            throw new TypeError('Invalid SemVer Range: ' + range);
        }
        this.format();
    }
    Range.prototype.format = function () {
        this.range = this.set.map(function (comps) {
            return comps.join(' ').trim();
        }).join('||').trim();
        return this.range;
    };
    Range.prototype.toString = function () {
        return this.range;
    };
    Range.prototype.parseRange = function (range) {
        var loose = this.loose;
        range = range.trim();
        debug('range', range, loose);
        var hr = loose ? re[HYPHENRANGELOOSE] : re[HYPHENRANGE];
        range = range.replace(hr, hyphenReplace);
        debug('hyphen replace', range);
        range = range.replace(re[COMPARATORTRIM], comparatorTrimReplace);
        debug('comparator trim', range, re[COMPARATORTRIM]);
        range = range.replace(re[TILDETRIM], tildeTrimReplace);
        range = range.replace(re[CARETTRIM], caretTrimReplace);
        range = range.split(/\s+/).join(' ');

        var compRe = loose ? re[COMPARATORLOOSE] : re[COMPARATOR];
        var set = range.split(' ').map(function (comp) {
            return parseComparator(comp, loose);
        }).join(' ').split(/\s+/);
        if (this.loose) {
            set = set.filter(function (comp) {
                return !!comp.match(compRe);
            });
        }
        set = set.map(function (comp) {
            return new Comparator(comp, loose);
        });
        return set;
    };
    Range.prototype.intersects = function (range, loose) {
        if (!(range instanceof Range)) {
            throw new TypeError('a Range is required');
        }
        return this.set.some(function (thisComparators) {
            return thisComparators.every(function (thisComparator) {
                return range.set.some(function (rangeComparators) {
                    return rangeComparators.every(function (rangeComparator) {
                        return thisComparator.intersects(rangeComparator, loose);
                    });
                });
            });
        });
    };
    exports.toComparators = toComparators;

    function toComparators(range, loose) {
        return new Range(range, loose).set.map(function (comp) {
            return comp.map(function (c) {
                return c.value;
            }).join(' ').trim().split(' ');
        });
    }

    function parseComparator(comp, loose) {
        debug('comp', comp);
        comp = replaceCarets(comp, loose);
        debug('caret', comp);
        comp = replaceTildes(comp, loose);
        debug('tildes', comp);
        comp = replaceXRanges(comp, loose);
        debug('xrange', comp);
        comp = replaceStars(comp, loose);
        debug('stars', comp);
        return comp;
    }

    function isX(id) {
        return !id || id.toLowerCase() === 'x' || id === '*';
    }

    function replaceTildes(comp, loose) {
        return comp.trim().split(/\s+/).map(function (comp) {
            return replaceTilde(comp, loose);
        }).join(' ');
    }

    function replaceTilde(comp, loose) {
        var r = loose ? re[TILDELOOSE] : re[TILDE];
        return comp.replace(r, function (_, M, m, p, pr) {
            debug('tilde', comp, _, M, m, p, pr);
            var ret;
            if (isX(M))
                ret = '';
            else if (isX(m))
                ret = '>=' + M + '.0.0 <' + (+M + 1) + '.0.0';
            else if (isX(p))
                ret = '>=' + M + '.' + m + '.0 <' + M + '.' + (+m + 1) + '.0';
            else if (pr) {
                debug('replaceTilde pr', pr);
                if (pr.charAt(0) !== '-')
                    pr = '-' + pr;
                ret = '>=' + M + '.' + m + '.' + p + pr +
                    ' <' + M + '.' + (+m + 1) + '.0';
            } else
                ret = '>=' + M + '.' + m + '.' + p +
                ' <' + M + '.' + (+m + 1) + '.0';
            debug('tilde return', ret);
            return ret;
        });
    }

    function replaceCarets(comp, loose) {
        return comp.trim().split(/\s+/).map(function (comp) {
            return replaceCaret(comp, loose);
        }).join(' ');
    }

    function replaceCaret(comp, loose) {
        debug('caret', comp, loose);
        var r = loose ? re[CARETLOOSE] : re[CARET];
        return comp.replace(r, function (_, M, m, p, pr) {
            debug('caret', comp, _, M, m, p, pr);
            var ret;
            if (isX(M))
                ret = '';
            else if (isX(m))
                ret = '>=' + M + '.0.0 <' + (+M + 1) + '.0.0';
            else if (isX(p)) {
                if (M === '0')
                    ret = '>=' + M + '.' + m + '.0 <' + M + '.' + (+m + 1) + '.0';
                else
                    ret = '>=' + M + '.' + m + '.0 <' + (+M + 1) + '.0.0';
            } else if (pr) {
                debug('replaceCaret pr', pr);
                if (pr.charAt(0) !== '-')
                    pr = '-' + pr;
                if (M === '0') {
                    if (m === '0')
                        ret = '>=' + M + '.' + m + '.' + p + pr +
                        ' <' + M + '.' + m + '.' + (+p + 1);
                    else
                        ret = '>=' + M + '.' + m + '.' + p + pr +
                        ' <' + M + '.' + (+m + 1) + '.0';
                } else
                    ret = '>=' + M + '.' + m + '.' + p + pr +
                    ' <' + (+M + 1) + '.0.0';
            } else {
                debug('no pr');
                if (M === '0') {
                    if (m === '0')
                        ret = '>=' + M + '.' + m + '.' + p +
                        ' <' + M + '.' + m + '.' + (+p + 1);
                    else
                        ret = '>=' + M + '.' + m + '.' + p +
                        ' <' + M + '.' + (+m + 1) + '.0';
                } else
                    ret = '>=' + M + '.' + m + '.' + p +
                    ' <' + (+M + 1) + '.0.0';
            }
            debug('caret return', ret);
            return ret;
        });
    }

    function replaceXRanges(comp, loose) {
        debug('replaceXRanges', comp, loose);
        return comp.split(/\s+/).map(function (comp) {
            return replaceXRange(comp, loose);
        }).join(' ');
    }

    function replaceXRange(comp, loose) {
        comp = comp.trim();
        var r = loose ? re[XRANGELOOSE] : re[XRANGE];
        return comp.replace(r, function (ret, gtlt, M, m, p, pr) {
            debug('xRange', comp, ret, gtlt, M, m, p, pr);
            var xM = isX(M);
            var xm = xM || isX(m);
            var xp = xm || isX(p);
            var anyX = xp;
            if (gtlt === '=' && anyX)
                gtlt = '';
            if (xM) {
                if (gtlt === '>' || gtlt === '<') {
                    ret = '<0.0.0';
                } else {
                    ret = '*';
                }
            } else if (gtlt && anyX) {
                if (xm)
                    m = 0;
                if (xp)
                    p = 0;
                if (gtlt === '>') {
                    gtlt = '>=';
                    if (xm) {
                        M = +M + 1;
                        m = 0;
                        p = 0;
                    } else if (xp) {
                        m = +m + 1;
                        p = 0;
                    }
                } else if (gtlt === '<=') {
                    gtlt = '<';
                    if (xm)
                        M = +M + 1;
                    else
                        m = +m + 1;
                }
                ret = gtlt + M + '.' + m + '.' + p;
            } else if (xm) {
                ret = '>=' + M + '.0.0 <' + (+M + 1) + '.0.0';
            } else if (xp) {
                ret = '>=' + M + '.' + m + '.0 <' + M + '.' + (+m + 1) + '.0';
            }
            debug('xRange return', ret);
            return ret;
        });
    }

    function replaceStars(comp, loose) {
        debug('replaceStars', comp, loose);
        return comp.trim().replace(re[STAR], '');
    }

    function hyphenReplace($0,
        from, fM, fm, fp, fpr, fb,
        to, tM, tm, tp, tpr, tb) {
        if (isX(fM))
            from = '';
        else if (isX(fm))
            from = '>=' + fM + '.0.0';
        else if (isX(fp))
            from = '>=' + fM + '.' + fm + '.0';
        else
            from = '>=' + from;
        if (isX(tM))
            to = '';
        else if (isX(tm))
            to = '<' + (+tM + 1) + '.0.0';
        else if (isX(tp))
            to = '<' + tM + '.' + (+tm + 1) + '.0';
        else if (tpr)
            to = '<=' + tM + '.' + tm + '.' + tp + '-' + tpr;
        else
            to = '<=' + to;
        return (from + ' ' + to).trim();
    }

    Range.prototype.test = function (version) {
        if (!version)
            return false;
        if (typeof version === 'string')
            version = new SemVer(version, this.loose);
        for (var i = 0; i < this.set.length; i++) {
            if (testSet(this.set[i], version))
                return true;
        }
        return false;
    };

    function testSet(set, version) {
        for (var i = 0; i < set.length; i++) {
            if (!set[i].test(version))
                return false;
        }
        if (version.prerelease.length) {
            for (var i = 0; i < set.length; i++) {
                debug(set[i].semver);
                if (set[i].semver === ANY)
                    continue;
                if (set[i].semver.prerelease.length > 0) {
                    var allowed = set[i].semver;
                    if (allowed.major === version.major &&
                        allowed.minor === version.minor &&
                        allowed.patch === version.patch)
                        return true;
                }
            }
            return false;
        }
        return true;
    }
    exports.satisfies = satisfies;

    function satisfies(version, range, loose) {
        try {
            range = new Range(range, loose);
        } catch (er) {
            return false;
        }
        return range.test(version);
    }
    exports.maxSatisfying = maxSatisfying;

    function maxSatisfying(versions, range, loose) {
        var max = null;
        var maxSV = null;
        try {
            var rangeObj = new Range(range, loose);
        } catch (er) {
            return null;
        }
        versions.forEach(function (v) {
            if (rangeObj.test(v)) { // satisfies(v, range, loose)
                if (!max || maxSV.compare(v) === -1) { // compare(max, v, true)
                    max = v;
                    maxSV = new SemVer(max, loose);
                }
            }
        })
        return max;
    }
    exports.minSatisfying = minSatisfying;

    function minSatisfying(versions, range, loose) {
        var min = null;
        var minSV = null;
        try {
            var rangeObj = new Range(range, loose);
        } catch (er) {
            return null;
        }
        versions.forEach(function (v) {
            if (rangeObj.test(v)) { // satisfies(v, range, loose)
                if (!min || minSV.compare(v) === 1) { // compare(min, v, true)
                    min = v;
                    minSV = new SemVer(min, loose);
                }
            }
        })
        return min;
    }
    exports.validRange = validRange;

    function validRange(range, loose) {
        try {
            return new Range(range, loose).range || '*';
        } catch (er) {
            return null;
        }
    }
    exports.ltr = ltr;

    function ltr(version, range, loose) {
        return outside(version, range, '<', loose);
    }
    exports.gtr = gtr;

    function gtr(version, range, loose) {
        return outside(version, range, '>', loose);
    }
    exports.outside = outside;

    function outside(version, range, hilo, loose) {
        version = new SemVer(version, loose);
        range = new Range(range, loose);
        var gtfn, ltefn, ltfn, comp, ecomp;
        switch (hilo) {
            case '>':
                gtfn = gt;
                ltefn = lte;
                ltfn = lt;
                comp = '>';
                ecomp = '>=';
                break;
            case '<':
                gtfn = lt;
                ltefn = gte;
                ltfn = gt;
                comp = '<';
                ecomp = '<=';
                break;
            default:
                throw new TypeError('Must provide a hilo val of "<" or ">"');
        }
        if (satisfies(version, range, loose)) {
            return false;
        }

        for (var i = 0; i < range.set.length; ++i) {
            var comparators = range.set[i];
            var high = null;
            var low = null;
            comparators.forEach(function (comparator) {
                if (comparator.semver === ANY) {
                    comparator = new Comparator('>=0.0.0')
                }
                high = high || comparator;
                low = low || comparator;
                if (gtfn(comparator.semver, high.semver, loose)) {
                    high = comparator;
                } else if (ltfn(comparator.semver, low.semver, loose)) {
                    low = comparator;
                }
            });
            if (high.operator === comp || high.operator === ecomp) {
                return false;
            }
            if ((!low.operator || low.operator === comp) &&
                ltefn(version, low.semver)) {
                return false;
            } else if (low.operator === ecomp && ltfn(version, low.semver)) {
                return false;
            }
        }
        return true;
    }
    exports.prerelease = prerelease;

    function prerelease(version, loose) {
        var parsed = parse(version, loose);
        return (parsed && parsed.prerelease.length) ? parsed.prerelease : null;
    }
    exports.intersects = intersects;

    function intersects(r1, r2, loose) {
        r1 = new Range(r1, loose)
        r2 = new Range(r2, loose)
        return r1.intersects(r2)
    }
    return exports;
})();

//--------------------- install ---------------------------

var util = require('util');
var coroutine = require('coroutine');
var fs = require('fs');
var path = require('path');
var ssl = require('ssl');
var http = require('http');
var zlib = require('zlib');
var hash = require('hash');

function read_module(p, parent) {
    var modules = {};
    if (fs.exists(path.join(p, 'node_modules'))) {
        var files = fs.readdir(path.join(p, 'node_modules'));

        files.forEach(n => {
            var f = path.join(p, 'node_modules', n, 'package.json');
            if (fs.exists(f)) {
                var minfo = JSON.parse(fs.readTextFile(f));

                var deps = util.clone(minfo.dependencies || {});

                modules[n] = {
                    version: minfo.version,
                    deps: deps,
                    parent: parent
                };

                modules[n].node_modules = read_module(path.join(p, 'node_modules', n), modules[n]);
            }
        });
    }

    return modules;
}

var infos = {};

function get_version(m, v, parent) {
    var info = infos[m];

    if (info === undefined) {
        var url = snap.registry + m.replace(/\//g, '%2F');
        console.log('fetch metadata:', m, "=>", url);
        infos[m] = info = http.get(url).json();
    }

    var vers = [];
    Object.keys(info.versions).forEach(ver => semvar.satisfies(ver, v) ? vers.push(ver) : undefined);
    vers.sort(semvar.rcompare);

    var minfo = info.versions[vers[0]];

    var deps = util.clone(minfo.dependencies || {});

    return {
        version: minfo.version,
        deps: deps,
        node_modules: {},
        parent: parent,
        dist: minfo.dist,
        new_module: true
    }
}

function get_root() {
    var p = process.cwd();
    var info = JSON.parse(fs.readTextFile(path.join(p, 'package.json')));

    var deps = util.clone(info.dependencies || {});
    deps = util.extend(deps, info.devDependencies || {});

    var m = {
        version: info.version,
        deps: deps,
        new_module: true,
        registry: info.registry || 'https://registry.npmjs.org/'
    };
    m.node_modules = read_module(p, m);

    return m;
}

function get_deps(info) {
    if (info.new_module) {
        var deps = info.deps;

        coroutine.parallel(Object.keys(deps), d => {
            var v = deps[d];
            var m = info.node_modules[d];

            if (m === undefined || !semvar.satisfies(m.version, v))
                info.node_modules[d] = get_version(d, v);
        }, 10);

        for (var k in info.node_modules)
            get_deps(info.node_modules[k]);
    }
}

function move_up(info, parent) {
    if (info.new_module) {
        for (var k in info.node_modules)
            move_up(info.node_modules[k], info);

        if (parent !== undefined)
            for (var k in info.node_modules) {
                var m = info.node_modules[k];
                if (m.new_module) {
                    var m1 = parent.node_modules[k];
                    if (m1 === undefined || m1.version === m.version) {
                        parent.node_modules[k] = m;
                        delete info.node_modules[k];
                    }
                }
            }
    }
}

var paths = {};

function get_paths(info, p) {
    if (info.new_module) {
        for (var k in info.node_modules) {
            var m = info.node_modules[k];
            if (m.new_module) {
                var mv = k + '@' + m.version;
                var ps = paths[mv];
                var mp = path.join(p, 'node_modules', k);
                if (ps === undefined)
                    paths[mv] = ps = {
                        dist: m.dist,
                        path: [mp]
                    };
                else
                    ps.path.push(mp);
            }

            get_paths(m, mp);
        }
    }
}

function mkdir(p) {
    try {
        if (!fs.exists(p))
            fs.mkdir(p);
    } catch (e) {
        mkdir(path.dirname(p));
        try {
            fs.mkdir(p);
        } catch (e) {}
    }
}

function download_module() {
    coroutine.parallel(Object.keys(paths), m => {
        m = paths[m];
        var r = http.get(m.dist.tarball);
        if (r.statusCode !== 200) {
            console.error('doenload error::', m.dist.tarball);
            process.exit();
        }

        var tgz = r.data;

        if (hash.sha1(tgz).digest().hex() !== m.dist.shasum) {
            console.error('shasum:', m.dist.tarball);
            process.exit();
        }

        var t = tgz;
        try {
            t = zlib.gunzip(tgz);
        } catch (e) {}

        untar(t).map(function (file) {
            var ps = file.filename.split('/');
            var p = ps.slice(1).join(path.sep);

            m.path.forEach(bp => {
                p = path.join(bp, p);
                mkdir(path.dirname(p));
                fs.writeFile(p, file.fileData);
            });
        });

        console.log('extract:', m.dist.tarball);
    }, 10);
}

function dump_snap() {
    function dump_mod(base, k, m, end) {
        var has_ms = !util.isEmpty(m.node_modules);
        console.log(base + (end ? '└' : '├') + '─' + (has_ms ? '┬' : '─'), k + '@' + m.version);
        if (has_ms)
            dump_mods(base + (end ? ' ' : '│') + ' ', m.node_modules);
    }

    function dump_mods(base, ms) {
        var ks = Object.keys(ms).sort();
        ks.forEach((k, i) => {
            dump_mod(base, k, ms[k], i == ks.length - 1);
        });
    }

    dump_mods('', snap.node_modules);
}

ssl.loadRootCerts();

var snap = get_root();
get_deps(snap);
move_up(snap);
get_paths(snap, process.cwd());
download_module();

dump_snap();