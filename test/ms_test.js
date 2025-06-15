var test = require("test");
test.setup();

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
        
        // Test write return value with string
        var testData = 'Hello, World!';
        var bytesWritten = testMs.write(testData);
        assert.equal(bytesWritten, testData.length);
        
        // Test write return value with Buffer
        var bufferData = new Buffer('Buffer test data');
        bytesWritten = testMs.write(bufferData);
        assert.equal(bytesWritten, bufferData.length);
        
        // Test write return value with empty string
        bytesWritten = testMs.write('');
        assert.equal(bytesWritten, 0);
        
        // Verify total size
        var expectedSize = testData.length + bufferData.length + 0;
        assert.equal(testMs.size(), expectedSize);
    });
});

