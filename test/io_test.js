var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

const io = require("io");
const fs = require("fs");
const path = require("path");
const coroutine = require("coroutine");

const {
    generateFakeMp4
} = require('./http_files/range_test/fake');
const {
    assert_error_msg
} = require('./_helpers/error');

const LF = `\n`
const CRLF = `\r\n`
let EOL = null

describe('io', () => {
    before(() => {
        const ms = new io.MemoryStream()
        const bs = new io.BufferedStream(ms)
        EOL = bs.EOL
    });

    describe('io.BufferedStream', () => {
        it('readLines', () => {
            const ms = new io.MemoryStream()
            const bs = new io.BufferedStream(ms)

            ms.write(`line1${bs.EOL}`)
            ms.write(`line2${bs.EOL}`)
            ms.rewind()

            assert.deepEqual(
                bs.readLines(),
                [
                    `line1`,
                    `line2`,
                ]
            )
        });

        describe('readUntil', () => {
            ;
            [
                ['system EOL', EOL],
                ['LF', LF],
                ['CRLF', CRLF],
                ['\\0', '\0'],
                ['\\r', '\r'],
            ].forEach(([
                desc,
                eol,
            ]) => {
                it(desc, () => {
                    const ms = new io.MemoryStream()
                    const bs = new io.BufferedStream(ms)

                    assert.deepEqual(bs.readUntil(eol), null)

                    ms.write(`line1${eol}`)
                    ms.write(`line2${eol}`)
                    ms.rewind()

                    assert.deepEqual(bs.readUntil(eol), `line1`)
                    assert.deepEqual(bs.readUntil(eol), `line2`)
                    assert.deepEqual(bs.readUntil(eol), null)
                })
            })
        })
    });

    describe('io.RangeStream', () => {
        var filePath;
        var file;
        var fsize;

        before(() => {
            filePath = generateFakeMp4().target;
            file = fs.openFile(filePath);
            fsize = Number(file.size());
        });

        describe('invalid constructor params', () => {
            it('NOT ALLOWED: negative begin', () => {
                assert_error_msg(() => {
                    new io.RangeStream(fs.openFile(filePath), -100, 0);
                }, `'begin' must be non-negative integer!`)
            });

            it('NOT ALLOWED: infinite begin', () => {
                assert.throws(() => {
                    new io.RangeStream(file, Infinity, file.size());
                })
            });

            it('NOT ALLOWED: infinite end', () => {
                assert.throws(() => {
                    new io.RangeStream(file, 0, Infinity);
                })
            });

            it('NOT ALLOWED: over file size when use range string', () => {
                assert.throws(() => {
                    new io.RangeStream(file, `0-${fsize}`);
                });

                assert_error_msg(() => {
                    new io.RangeStream(file, `0-${fsize}`);
                }, `Parse Range-String Error, invalid range string '0-${fsize}' which is out of file size range!`)
            });

            it('NOT ALLOWED: bad range format', () => {
                assert_error_msg(() => {
                    new io.RangeStream(file, ``);
                }, `Parse Range-String Error, expect next token '-'!`)
            });
        });

        describe('valid constructor params', () => {
            it("accept range string", () => {
                var stm = new io.RangeStream(fs.openFile(filePath), '2-10');

                assert.equal(stm.begin, 2);
                assert.equal(stm.end, 11);

                file.seek(stm.begin, fs.SEEK_SET);
                stm.rewind();
                assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
            });

            it("http 206 range string(but weired for RangeStream)", () => {
                var stm = new io.RangeStream(fs.openFile(filePath), 'bytes=2-10');

                assert.equal(stm.begin, 0);
                assert.equal(stm.end, 11);

                file.seek(stm.begin, fs.SEEK_SET);
                stm.rewind();
                assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
            });

            it("accept numberic begin_pos, end_pos", () => {
                var stm = new io.RangeStream(fs.openFile(filePath), 0, 10);

                assert.equal(stm.begin, 0);
                assert.equal(stm.end, 10);

                file.seek(stm.begin, fs.SEEK_SET);
                stm.rewind();
                assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
            });

            it("readAll immediately after construct with begin > 0", () => {
                // Regression: RangeStream constructed with begin > 0 should be
                // readable immediately without calling rewind() first.
                // Previously real_pos was initialized to get_c_pos() (underlying
                // stream position, typically 0), causing readBuffer to return null
                // because the check (b_pos > real_pos) was true.
                var stm = new io.RangeStream(fs.openFile(filePath), 5, 15);

                assert.equal(stm.tell(), 0);
                assert.equal(stm.size(), 10);

                // This must return data, not null
                var data = stm.readAll();
                assert.ok(data !== null, "readAll() returned null for RangeStream with begin > 0");
                assert.equal(data.length, 10);

                // Verify the data matches the expected range from the file
                file.seek(5, fs.SEEK_SET);
                var expected = file.read(10);
                assert.equal(0, expected.compare(data));
            });

            it("readBuffer immediately after construct with begin > 0", () => {
                var stm = new io.RangeStream(fs.openFile(filePath), 10, 20);

                // Reading a few bytes should work without rewind
                var data = stm.read(5);
                assert.ok(data !== null, "read() returned null for RangeStream with begin > 0");
                assert.equal(data.length, 5);
                assert.equal(stm.tell(), 5);

                // Read remaining
                var rest = stm.readAll();
                assert.ok(rest !== null);
                assert.equal(rest.length, 5);
            });
        });

        describe('robust case', () => {
            var file;
            const ranges = [
                [1, 10],
                [5, 15],
                [10, 20],
            ];

            before(() => {
                file = fs.openFile(filePath)
            });

            it("actions after close RangeStream", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(file, 0, file.size());

                // seekable
                stm.seek(0, fs.SEEK_SET);
                // readable
                stm.read(1);
                // query current position
                assert.equal(stm.tell(), 1);
                // get size of range
                assert.equal(stm.size(), file.size());
                // get the real stream's fd
                assert.isDefined(stm.fd);

                // but after close it
                stm.close();

                // you can call `.seek()` still.
                stm.seek(0, fs.SEEK_SET);

                // but you cannot read from it.
                assert_error_msg(() => {
                    stm.read(1);
                }, `[20027] Object closed.`)

                // nor query current positoin
                assert_error_msg(() => {
                    stm.tell();
                }, `[20027] Object closed.`)

                // not allowed to get size of it.
                assert_error_msg(() => {
                    stm.size();
                }, `[20027] Object closed.`)

                // not allowed to get fd of it.
                assert_error_msg(() => {
                    stm.fd;
                }, `[20027] Object closed.`)
            });

            describe("ALLOW: multiple range upon the same file in order", () => {
                ranges.forEach(([begin, end]) => {
                    it(`[${begin}, ${end}]`, () => {
                        var stm = new io.RangeStream(file, begin, end);

                        assert.equal(stm.begin, begin);
                        assert.equal(stm.end, end);

                        file.seek(stm.begin, fs.SEEK_SET);
                        stm.rewind();
                        assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
                    });
                });
            });

            it.skip('BAD CASE: parallel read upon SAME FILE INVALID!!', () => {
                coroutine.parallel(ranges, ([begin, end]) => {
                    var stm = new io.RangeStream(file, begin, end);

                    assert.equal(stm.begin, begin);
                    assert.equal(stm.end, end);

                    file.seek(stm.begin, fs.SEEK_SET);
                    stm.rewind();
                    assert.equal(0, file.read(stm.end - stm.begin).compare(stm.readAll()));
                });
            });

            it("ALLOW: io.RangeStream::begin > filesize", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(file, 0, file.size());

                assert.equal(stm.end, file.size());
            });

            it("ALLOW: io.RangeStream::end = filesize", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(file, 0, file.size());

                stm.seek(0, fs.SEEK_END);

                assert.equal(null, stm.readAll());
            });

            it("range[begin, end] never change", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                assert.equal(begin, begin);
                assert.equal(stm.end, sz * 2);

                stm.seek(-sz, fs.SEEK_END);
                assert.equal(begin, begin);
                assert.equal(stm.end, sz * 2);

                stm.seek(-sz * 2 + begin, fs.SEEK_END);
                assert.equal(begin, begin);
                assert.equal(stm.end, sz * 2);

                stm.rewind();
                assert.equal(begin, begin);
                assert.equal(stm.end, sz * 2);
            });

            it("range intersect with filesize", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var stm = new io.RangeStream(file, sz / 2, sz * 2);

                stm.seek(0, fs.SEEK_END);

                assert.equal(null, stm.readAll());
            });

            it("allow stream.end > stream.size()", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                assert.equal(begin, stm.begin);
                assert.equal(sz * 2, stm.end);

                assert.ok(stm.end - stm.begin > Number(stm.size()));
                assert.equal(sz - begin, Number(stm.size()));
            });

            it("seek relative to begin as SEEK_SET", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                stm.seek(0, fs.SEEK_SET);

                assert.equal(sz - begin, Number(stm.size()));
                assert.equal(sz - begin, stm.readAll().length);
            });

            it("seek relative to end as SEEK_END, but never out range", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                assert.equal(stm.end, sz * 2);
                stm.seek(-sz, fs.SEEK_END);

                assert.equal(stm.tell(), sz / 2);
                assert.equal(file.tell(), 0);

                stm.seek(-sz * 2 + begin, fs.SEEK_END);
                assert.throws(() => {
                    stm.seek(-sz * 2 + begin - 1, fs.SEEK_END);
                })
            });

            it("::stat", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var begin = Math.floor(sz / 2);
                var stm = new io.RangeStream(file, begin, sz * 2);

                assert.ok(stm.stat().size < sz);

                var stm = new io.RangeStream(file, 0, sz * 2);
                assert.ok(stm.stat().size === sz);

                assert.isDefined(stm.stat().name);
                assert.isDefined(stm.stat().size);
                assert.isDefined(stm.stat().ctimeMs);
                assert.isDefined(stm.stat().atimeMs);
                assert.isDefined(stm.stat().mtimeMs);

                assert.strictEqual(stm.stat().size, file.stat().size);
                assert.strictEqual(stm.stat().ctimeMs, file.stat().ctimeMs);
                assert.strictEqual(stm.stat().atimeMs, file.stat().atimeMs);
                assert.strictEqual(stm.stat().mtimeMs, file.stat().mtimeMs);
                assert.strictEqual(stm.stat().isFile(), file.stat().isFile());
                assert.strictEqual(stm.stat().isFile(), true);
            });
        });

        describe('RangeStream(Stream, end) constructor', () => {
            it("SeekableStream passed as Stream uses seekable mode", () => {
                var file = fs.openFile(filePath);
                var sz = Number(file.size());
                var end = Math.min(100, sz);
                var stm = new io.RangeStream(file, end);

                // should be treated as RangeStream(file, 0, end)
                assert.equal(stm.begin, 0);
                assert.equal(stm.end, end);

                // seekable operations should work
                stm.seek(0, fs.SEEK_SET);
                stm.rewind();

                // read data and verify it matches direct file read
                file.seek(0, fs.SEEK_SET);
                var expected = file.read(end);
                stm.rewind();
                var actual = stm.readAll();
                assert.equal(0, expected.compare(actual));

                // stat should work
                assert.isDefined(stm.stat());
            });

            it("SeekableStream: size equals end", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(file, 200);

                assert.equal(stm.size(), 200);
            });

            it("SeekableStream: tell works after read", () => {
                var file = fs.openFile(filePath);
                var stm = new io.RangeStream(file, 200);

                stm.rewind();
                stm.read(50);
                assert.equal(stm.tell(), 50);
            });

            it("non-seekable stream: basic read", () => {
                var data = Buffer.from("hello world, this is a test of RangeStream with non-seekable stream");
                var ms = new io.MemoryStream();
                ms.write(data);
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 5);

                assert.equal(stm.begin, 0);
                assert.equal(stm.end, 5);

                var buf = stm.readAll();
                assert.equal(buf.length, 5);
                assert.equal(buf.toString(), "hello");
            });

            it("non-seekable stream: read with limit", () => {
                var data = Buffer.from("abcdefghijklmnopqrstuvwxyz");
                var ms = new io.MemoryStream();
                ms.write(data);
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 10);

                // read 3 bytes at a time
                var b1 = stm.read(3);
                assert.equal(b1.toString(), "abc");
                assert.equal(stm.tell(), 3);

                var b2 = stm.read(3);
                assert.equal(b2.toString(), "def");
                assert.equal(stm.tell(), 6);

                var b3 = stm.read(3);
                assert.equal(b3.toString(), "ghi");
                assert.equal(stm.tell(), 9);

                // only 1 byte left
                var b4 = stm.read(3);
                assert.equal(b4.toString(), "j");
                assert.equal(stm.tell(), 10);

                // EOF
                assert.equal(stm.read(1), null);
            });

            it("non-seekable stream: readAll respects end limit", () => {
                var data = Buffer.from("0123456789ABCDEF");
                var ms = new io.MemoryStream();
                ms.write(data);
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 8);
                var buf = stm.readAll();
                assert.equal(buf.length, 8);
                assert.equal(buf.toString(), "01234567");
            });

            it("non-seekable stream: size returns end", () => {
                var ms = new io.MemoryStream();
                ms.write(Buffer.from("test data"));
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 100);
                assert.equal(stm.size(), 100);
            });

            it("non-seekable stream: tell tracks position", () => {
                var ms = new io.MemoryStream();
                ms.write(Buffer.from("abcdefghij"));
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 10);
                assert.equal(stm.tell(), 0);

                stm.read(4);
                assert.equal(stm.tell(), 4);

                stm.read(3);
                assert.equal(stm.tell(), 7);
            });

            it("non-seekable stream: eof detection", () => {
                var ms = new io.MemoryStream();
                ms.write(Buffer.from("abc"));
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 3);
                assert.equal(stm.eof(), false);

                stm.readAll();
                assert.equal(stm.eof(), true);
            });

            it("non-seekable stream: seek/rewind/stat not allowed", () => {
                var ms = new io.MemoryStream();
                ms.write(Buffer.from("test"));
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 4);

                assert.throws(() => {
                    stm.seek(0, fs.SEEK_SET);
                });

                assert.throws(() => {
                    stm.rewind();
                });

                assert.throws(() => {
                    stm.stat();
                });
            });

            it("non-seekable stream: close then read fails", () => {
                var ms = new io.MemoryStream();
                ms.write(Buffer.from("test data"));
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 5);
                stm.read(2);
                stm.close();

                assert_error_msg(() => {
                    stm.read(1);
                }, `[20027] Object closed.`);

                assert_error_msg(() => {
                    stm.tell();
                }, `[20027] Object closed.`);

                assert_error_msg(() => {
                    stm.size();
                }, `[20027] Object closed.`);
            });

            it("non-seekable stream: end=0 reads nothing", () => {
                var ms = new io.MemoryStream();
                ms.write(Buffer.from("data"));
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 0);
                assert.equal(stm.readAll(), null);
                assert.equal(stm.tell(), 0);
                assert.equal(stm.eof(), true);
            });

            it("invalid: negative end", () => {
                var ms = new io.MemoryStream();
                var bs = new io.BufferedStream(ms);

                assert_error_msg(() => {
                    new io.RangeStream(bs, -1);
                }, `'end' must be non-negative integer!`);
            });

            it("non-seekable stream: end larger than actual data", () => {
                var ms = new io.MemoryStream();
                ms.write(Buffer.from("short"));
                ms.rewind();
                var bs = new io.BufferedStream(ms);

                var stm = new io.RangeStream(bs, 1000);
                // should read only available data (5 bytes), not hang
                var buf = stm.readAll();
                assert.equal(buf.length, 5);
                assert.equal(buf.toString(), "short");
            });

            it("MemoryStream passed directly uses seekable mode", () => {
                var ms = new io.MemoryStream();
                ms.write(Buffer.from("hello world"));
                ms.rewind();

                // MemoryStream is SeekableStream, should auto-detect
                var stm = new io.RangeStream(ms, 5);
                assert.equal(stm.begin, 0);
                assert.equal(stm.end, 5);

                // seekable operations should work
                stm.rewind();
                var buf = stm.readAll();
                assert.equal(buf.toString(), "hello");

                // seek should work since it's seekable mode
                stm.seek(0, fs.SEEK_SET);
                var buf2 = stm.read(3);
                assert.equal(buf2.toString(), "hel");
            });
        });
    });
});

