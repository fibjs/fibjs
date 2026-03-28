var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var fs = require('fs');
var io = require('io');
var coroutine = require('coroutine');

var ms = new io.MemoryStream();
var cms;

describe('ms', () => {
    it("new", () => {
        ms = new io.MemoryStream();
        assert.equal(ms.stat().size, ms.size());
    });

    it("write", () => {
        ms.write('abcdefghijklmnopqrstuvwxyz');
        assert.equal(ms.stat().size, ms.size());
    });

    it("size", () => {
        assert.equal(26, ms.size());
    });

    it("tell", () => {
        assert.equal(26, ms.tell());
    });

    it("seek", () => {
        ms.seek(-10, fs.SEEK_END);
        assert.equal(16, ms.tell());
        ms.seek(ms.size() + 10, fs.SEEK_SET);
        assert.equal(26, ms.tell());
    });

    it("rewind", () => {
        ms.rewind();
        assert.equal(0, ms.tell());
        assert.equal('abcdefghijklmnopqrstuvwxyz', ms.read().toString());
    });

    it("seek & read", () => {
        ms.seek(10, fs.SEEK_SET);
        assert.equal('klmnopqrstuvwxyz', ms.read().toString());

        ms.seek(10, fs.SEEK_SET);
        assert.equal('klmnopqrstuvwxyz', ms.read(ms.size()).toString());

        ms.seek(10, fs.SEEK_SET);
        ms.seek(10, fs.SEEK_CUR);
        assert.equal('uvwxyz', ms.read(ms.size()).toString());

        ms.seek(-10, fs.SEEK_END);
        assert.equal('qrstuvwxyz', ms.read(ms.size()).toString());
    });

    it("clone", () => {
        cms = ms.clone();
        assert.deepEqual(cms.stat().mtime, ms.stat().mtime);

        assert.equal('abcdefghijklmnopqrstuvwxyz', cms.read().toString());

        cms.seek(10, fs.SEEK_SET);
        assert.equal(cms.tell(), 10);
        assert.equal('klmnopqrstuvwxyz', cms.read().toString());

        cms.seek(10, fs.SEEK_SET);
        assert.equal(cms.tell(), 10);
        assert.equal('klmnopqrstuvwxyz', cms.read(cms.size()).toString());

        cms.seek(10, fs.SEEK_SET);
        cms.seek(10, fs.SEEK_CUR);
        assert.equal(cms.tell(), 20);
        assert.equal('uvwxyz', cms.read(cms.size()).toString());

        cms.seek(-10, fs.SEEK_END);
        assert.equal(cms.tell(), 16);
        assert.equal('qrstuvwxyz', cms.read(cms.size()).toString());
    });

    it("seek & write", () => {
        ms.seek(10, fs.SEEK_SET);
        ms.write('abcdefghijklmnopqrstuvwxyz');
        assert.equal(36, ms.size());
        assert.equal(36, ms.tell());

        ms.rewind();
        assert.equal('abcdefghijabcdefghijklmnopqrstuvwxyz', ms.read().toString());
    });

    it("data event", () => {
        var testMs = new io.MemoryStream();
        var receivedData = [];
        var dataEventCount = 0;

        // Write data to trigger data events
        testMs.write('Hello, ');
        testMs.write('World!');
        testMs.write(' Test data event.');
        testMs.rewind();

        // Register data event handler
        testMs.on('data', (data) => {
            receivedData.push(data.toString());
            dataEventCount++;
        });

        // Let the fiber yield to process data events
        coroutine.sleep(10);

        // Verify that data events were triggered
        assert.equal(dataEventCount, 1);
        assert.deepEqual(receivedData, ['Hello, World! Test data event.']);

        // Verify the complete content
        testMs.rewind();
        var fullContent = testMs.read().toString();
        assert.equal(fullContent, 'Hello, World! Test data event.');
    });

    it("write return value validation", () => {
        var testMs = new io.MemoryStream();

        // Test write return value with string (write now returns Boolean)
        var testData = 'Hello, World!';
        var bytesWritten = testMs.write(testData);
        assert.equal(bytesWritten, true);

        // Test write return value with Buffer
        var bufferData = new Buffer('Buffer test data');
        bytesWritten = testMs.write(bufferData);
        assert.equal(bytesWritten, true);

        // Test write return value with empty string
        bytesWritten = testMs.write('');
        assert.equal(bytesWritten, true);

        // Verify total size
        var expectedSize = testData.length + bufferData.length + 0;
        assert.equal(testMs.size(), expectedSize);
    });

    it("write with encoding", () => {
        var testMs = new io.MemoryStream();

        // Test write with utf8 encoding (default) - write now returns Boolean
        var bytesWritten = testMs.write('Hello', 'utf8');
        assert.equal(bytesWritten, true);

        // Test write with utf-8 encoding
        bytesWritten = testMs.write(' World', 'utf-8');
        assert.equal(bytesWritten, true);

        // Verify content
        testMs.rewind();
        assert.equal(testMs.read().toString(), 'Hello World');

        // Test write Chinese characters with utf8
        testMs = new io.MemoryStream();
        bytesWritten = testMs.write('中文', 'utf8');
        assert.equal(bytesWritten, true);
        testMs.rewind();
        assert.equal(testMs.read().toString(), '中文');

        // Test write with hex encoding
        testMs = new io.MemoryStream();
        bytesWritten = testMs.write('48656c6c6f', 'hex');
        assert.equal(bytesWritten, true);
        testMs.rewind();
        assert.equal(testMs.read().toString(), 'Hello');

        // Test write with base64 encoding
        testMs = new io.MemoryStream();
        bytesWritten = testMs.write('SGVsbG8=', 'base64');
        assert.equal(bytesWritten, true);
        testMs.rewind();
        assert.equal(testMs.read().toString(), 'Hello');

        // Test write with latin1 encoding
        testMs = new io.MemoryStream();
        bytesWritten = testMs.write('Hello', 'latin1');
        assert.equal(bytesWritten, true);
        testMs.rewind();
        assert.equal(testMs.read().toString(), 'Hello');

        // Test write with binary encoding (alias for latin1)
        testMs = new io.MemoryStream();
        bytesWritten = testMs.write('Hello', 'binary');
        assert.equal(bytesWritten, true);
        testMs.rewind();
        assert.equal(testMs.read().toString(), 'Hello');
    });

    describe("stream close event", () => {
        it("EOF triggers end event", () => {
            var testMs = new io.MemoryStream();
            testMs.write('hello world');
            testMs.rewind();

            var events = [];
            var done = new coroutine.Event();
            testMs.on('data', () => { events.push('data'); });
            testMs.on('end', () => { events.push('end'); done.set(); });
            done.wait();
            assert.ok(events.indexOf('data') >= 0);
            assert.ok(events.indexOf('end') >= 0);
        });

        it("destroy() triggers close", () => {
            var testMs = new io.MemoryStream();
            var done = new coroutine.Event();
            var closed = false;
            testMs.on('close', () => { closed = true; done.set(); });
            testMs.destroy();
            done.wait();
            assert.strictEqual(closed, true);
        });

        it("destroy(err) emits error then close", () => {
            var testMs = new io.MemoryStream();
            var events = [];
            var done = new coroutine.Event();
            testMs.on('error', () => { events.push('error'); });
            testMs.on('close', () => { events.push('close'); done.set(); });
            testMs.destroy(new Error('test'));
            done.wait();
            assert.deepStrictEqual(events, ['error', 'close']);
        });
    });
});

