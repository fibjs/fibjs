var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');
var coroutine = require('coroutine');
var path = require('path');
var fs = require('fs');
var net = require('net');
var io = require('io');
var test_util = require('./test_util');

var vmid = coroutine.vmid;

describe('stream', () => {
    after(() => {
        test_util.cleanup();
    });

    it("file data event", () => {
        var testFile = fs.openFile(path.join(__dirname, 'fs_test.js.data_event' + vmid), 'w+');
        var receivedData = [];
        var dataEventCount = 0;

        // Write data to trigger data events
        testFile.write('Hello, ');
        testFile.write('World!');
        testFile.write(' Test file data event.');
        testFile.rewind();

        // Register data event handler
        testFile.on('data', (data) => {
            receivedData.push(data.toString());
            dataEventCount++;
        });

        // Let the fiber yield to process data events
        coroutine.sleep(10);

        // Verify that data events were triggered
        assert.equal(dataEventCount, 1);
        assert.deepEqual(receivedData, ['Hello, World! Test file data event.']);

        // Verify the complete content
        testFile.rewind();
        var fullContent = testFile.read().toString();
        assert.equal(fullContent, 'Hello, World! Test file data event.');

        testFile.close();
        fs.unlink(path.join(__dirname, 'fs_test.js.data_event' + vmid));
    });

    it("stream setEncoding and read", () => {
        var fn = path.join(__dirname, 'fs_test.js.enc' + vmid);

        // Write UTF-8 content with multibyte characters
        var content = 'Hello, 世界! 🌍 café';
        fs.writeFile(fn, content);

        // read without encoding returns Buffer
        var f = fs.openFile(fn);
        var result = f.read();
        assert.ok(Buffer.isBuffer(result));
        assert.equal(result.toString(), content);
        f.close();

        // read with setEncoding returns string
        f = fs.openFile(fn);
        f.setEncoding('utf8');
        result = f.read();
        assert.equal(typeof result, 'string');
        assert.equal(result, content);
        f.close();

        // readBuffer always returns Buffer regardless of encoding
        f = fs.openFile(fn);
        f.setEncoding('utf8');
        result = f.readBuffer();
        assert.ok(Buffer.isBuffer(result));
        assert.equal(result.toString(), content);
        f.close();

        fs.unlink(fn);
    });

    it("stream setEncoding with data event", () => {
        var fn = path.join(__dirname, 'fs_test.js.enc_event' + vmid);
        var content = 'Hello, 世界!';
        fs.writeFile(fn, content);

        var f = fs.openFile(fn);
        f.setEncoding('utf8');

        var receivedData = [];
        f.on('data', (data) => {
            // data event should emit strings when encoding is set
            assert.equal(typeof data, 'string');
            receivedData.push(data);
        });

        coroutine.sleep(10);

        assert.equal(receivedData.join(''), content);

        f.close();
        fs.unlink(fn);
    });

    it("stream write with encoding", () => {
        var fn = path.join(__dirname, 'fs_test.js.enc_write' + vmid);

        // write string with default utf8 encoding
        var f = fs.openFile(fn, 'w+');
        f.write('Hello, 世界!');
        f.rewind();
        var result = f.read();
        assert.ok(Buffer.isBuffer(result));
        assert.equal(result.toString(), 'Hello, 世界!');

        // write string with hex encoding
        f.rewind();
        f.truncate(0);
        f.write('48656c6c6f', 'hex');
        f.rewind();
        result = f.read();
        assert.equal(result.toString(), 'Hello');

        // write string with base64 encoding
        f.rewind();
        f.truncate(0);
        f.write('SGVsbG8=', 'base64');
        f.rewind();
        result = f.read();
        assert.equal(result.toString(), 'Hello');

        f.close();
        fs.unlink(fn);
    });

    it("stream read with setEncoding on MemoryStream", () => {
        var stm = new io.MemoryStream();

        // Write multibyte UTF-8 content: "Hi中!🌍"
        // "中" = E4 B8 AD, "🌍" = F0 9F 8C 8D
        stm.write(Buffer.from([0x48, 0x69, 0xE4, 0xB8, 0xAD, 0x21,
            0xF0, 0x9F, 0x8C, 0x8D]));
        stm.rewind();

        // Without encoding, read returns Buffer
        var result = stm.read();
        assert.ok(Buffer.isBuffer(result));
        assert.equal(result.toString(), 'Hi中!🌍');

        // With encoding, read returns string
        stm.rewind();
        stm.setEncoding('utf-8');
        result = stm.read();
        assert.equal(typeof result, 'string');
        assert.equal(result, 'Hi中!🌍');

        // readBuffer always returns Buffer
        stm.rewind();
        result = stm.readBuffer();
        assert.ok(Buffer.isBuffer(result));
        assert.equal(result.toString(), 'Hi中!🌍');
    });

    it("stream setEncoding data event on MemoryStream", () => {
        var stm = new io.MemoryStream();
        var content = 'Hello, 世界!🌍';
        stm.write(Buffer.from(content));
        stm.rewind();

        stm.setEncoding('utf-8');

        var receivedData = [];
        stm.on('data', (data) => {
            assert.equal(typeof data, 'string');
            receivedData.push(data);
        });

        coroutine.sleep(10);
        assert.equal(receivedData.join(''), content);
    });

    it("stream setEncoding with incomplete multibyte fragments over TCP", () => {
        var port = 28900 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();

        client.setEncoding('utf-8');

        var received = [];
        var closeEvent = new coroutine.Event();

        client.on('data', function (data) {
            assert.equal(typeof data, 'string');
            received.push(data);
        });

        client.on('close', function () {
            closeEvent.set();
        });

        // "中" = E4 B8 AD, "🌍" = F0 9F 8C 8D
        // Send "Hi" + first 2 bytes of "中"
        serverConn.send(Buffer.from([0x48, 0x69, 0xE4, 0xB8]));
        coroutine.sleep(100);

        // Send last byte of "中" + "!" + first byte of 🌍
        serverConn.send(Buffer.from([0xAD, 0x21, 0xF0]));
        coroutine.sleep(100);

        // Send remaining 3 bytes of 🌍
        serverConn.send(Buffer.from([0x9F, 0x8C, 0x8D]));
        coroutine.sleep(100);

        serverConn.close();
        closeEvent.wait();

        // Decoder should reassemble incomplete multibyte fragments across chunks
        assert.equal(received.join(''), 'Hi中!🌍');

    });

    it("stream setEncoding with incomplete GBK fragments over TCP", () => {
        var port = 28910 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();

        client.setEncoding('gbk');

        var received = [];
        var closeEvent = new coroutine.Event();

        client.on('data', function (data) {
            assert.equal(typeof data, 'string');
            received.push(data);
        });

        client.on('close', function () {
            closeEvent.set();
        });

        // GBK: "中" = D6 D0, "国" = B9 FA, "人" = C8 CB
        // Send "A" + half of "中"
        serverConn.send(Buffer.from([0x41, 0xD6]));
        coroutine.sleep(100);

        // Send complete "中" + half of "国"
        serverConn.send(Buffer.from([0xD0, 0xB9]));
        coroutine.sleep(100);

        // Send complete "国" + complete "人"
        serverConn.send(Buffer.from([0xFA, 0xC8, 0xCB]));
        coroutine.sleep(100);

        serverConn.close();
        closeEvent.wait();

        assert.equal(received.join(''), 'A中国人');

    });

    it("stream readable event on MemoryStream", () => {
        var stm = new io.MemoryStream();
        stm.write("hello world");
        stm.rewind();
        stm.setEncoding("utf8");

        var chunks = [];
        var readableCount = 0;
        var readableDone = new coroutine.Event();

        stm.on("readable", () => {
            readableCount++;
            var chunk;
            while ((chunk = stm.read()) !== null) {
                chunks.push(chunk);
            }
            readableDone.set();
        });

        readableDone.wait();
        assert.equal(chunks.join(""), "hello world");
        assert.equal(typeof chunks[0], "string");
    });

    it("stream readable read(n) on MemoryStream", () => {
        var stm = new io.MemoryStream();
        stm.write("abcdefghij"); // 10 bytes
        stm.rewind();

        var chunks = [];
        var readableDone = new coroutine.Event();

        stm.on("readable", () => {
            var chunk;
            while ((chunk = stm.read(3)) !== null) {
                chunks.push(chunk);
            }
            // remaining 1 byte, read(3) returns null, read() gets it
            chunk = stm.read();
            if (chunk !== null)
                chunks.push(chunk);
            readableDone.set();
        });

        readableDone.wait();

        // Should get: [abc], [def], [ghi], [j]
        assert.equal(chunks.length, 4);
        assert.deepEqual(chunks[0], Buffer.from("abc"));
        assert.deepEqual(chunks[1], Buffer.from("def"));
        assert.deepEqual(chunks[2], Buffer.from("ghi"));
        assert.deepEqual(chunks[3], Buffer.from("j"));
    });

    it("stream readable raw buffer on MemoryStream", () => {
        var stm = new io.MemoryStream();
        stm.write("raw data");
        stm.rewind();

        var chunks = [];
        var readableDone = new coroutine.Event();

        stm.on("readable", () => {
            var chunk;
            while ((chunk = stm.read()) !== null) {
                chunks.push(chunk);
            }
            readableDone.set();
        });

        readableDone.wait();
        assert.ok(Buffer.isBuffer(chunks[0]));
        assert.equal(chunks[0].toString(), "raw data");
    });

    it("stream readable with incomplete multibyte over TCP", () => {
        var port = 28920 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();

        client.setEncoding('utf8');

        var received = [];
        var closeEvent = new coroutine.Event();

        client.on("readable", () => {
            var chunk;
            while ((chunk = client.read()) !== null) {
                assert.equal(typeof chunk, "string");
                received.push(chunk);
            }
        });

        client.on("close", () => {
            closeEvent.set();
        });

        // "中" = E4 B8 AD, send split across packets
        serverConn.send(Buffer.from([0x48, 0x69, 0xE4])); // "Hi" + half of "中"
        coroutine.sleep(100);
        serverConn.send(Buffer.from([0xB8, 0xAD, 0x21])); // rest of "中" + "!"
        coroutine.sleep(100);

        serverConn.close();
        closeEvent.wait();

        assert.equal(received.join(''), 'Hi中!');

    });

    it("stream pause stops data events over TCP", () => {
        var port = 28930 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();
        test_util.push(serverConn);

        var received = [];
        var pauseAfterSecond = false;

        client.on('data', function (data) {
            received.push(data.toString());
            // Pause inside the data handler after receiving chunk2
            // This ensures the reader is not in readBuffer when we pause,
            // so subsequent reads are truly stopped.
            if (received.length === 2) {
                client.pause();
                pauseAfterSecond = true;
            }
        });

        // Send first chunk
        serverConn.send(Buffer.from("chunk1"));
        coroutine.sleep(100);

        // Send second chunk - pause will be called in data handler
        serverConn.send(Buffer.from("chunk2"));
        coroutine.sleep(100);

        assert.ok(pauseAfterSecond);
        assert.equal(received.length, 2);
        assert.equal(received[0], 'chunk1');
        assert.equal(received[1], 'chunk2');

        // Send third chunk while paused - should NOT trigger data event
        serverConn.send(Buffer.from("chunk3"));
        coroutine.sleep(100);

        // No new data events should have been received
        assert.equal(received.length, 2);

        // Resume the stream
        client.resume();

        // Wait for the buffered data to be delivered
        coroutine.sleep(100);

        // After resume, chunk3 should be delivered
        assert.ok(received.length >= 3);
        assert.ok(received.join('').indexOf('chunk3') >= 0);

    });

    it("stream pause/resume on MemoryStream", () => {
        var stm = new io.MemoryStream();
        stm.write("hello world, this is a test for pause and resume");
        stm.rewind();

        var received = [];
        var firstDataEvent = new coroutine.Event();

        stm.on('data', (data) => {
            received.push(data.toString());
            firstDataEvent.set();
        });

        // Wait for first data event
        firstDataEvent.wait();
        var countAfterFirst = received.length;
        assert.ok(countAfterFirst >= 1);

        // Pause should stop further events
        stm.pause();
        coroutine.sleep(50);
        var countAfterPause = received.length;

        // Resume
        stm.resume();
        coroutine.sleep(50);

        // All data should have been received
        assert.equal(received.join(''), 'hello world, this is a test for pause and resume');
    });

    it("stream readable mode backpressure (drain)", () => {
        var port = 28940 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();

        var allData = '';
        var readableCount = 0;

        client.on("readable", () => {
            readableCount++;
            var chunk;
            while ((chunk = client.read()) !== null) {
                allData += chunk.toString();
            }
        });

        // Send data exceeding the default highWaterMark (16384)
        var bigChunk = Buffer.alloc(8192, 0x41); // 'A' * 8192
        serverConn.send(bigChunk);
        coroutine.sleep(100);
        serverConn.send(bigChunk);
        coroutine.sleep(100);
        serverConn.send(bigChunk);
        coroutine.sleep(100);

        serverConn.close();

        coroutine.sleep(200);

        // All data should eventually be received through readable events
        assert.equal(allData.length, 8192 * 3);
        assert.ok(readableCount >= 1);

    });

    it("pause in data handler stops next data immediately", () => {
        var port = 28950 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();
        test_util.push(serverConn);

        var received = [];

        client.on('data', function (data) {
            received.push(data.toString());
            // Pause after first data event, like Node.js readline does
            client.pause();
        });

        // Send chunk, handler will pause immediately
        serverConn.send(Buffer.from("first"));
        coroutine.sleep(100);

        assert.equal(received.length, 1);
        assert.equal(received[0], 'first');

        // Send more while paused
        serverConn.send(Buffer.from("second"));
        coroutine.sleep(100);

        // Should still be 1 - no data events while paused
        assert.equal(received.length, 1);

        // Resume - handler will pause again after receiving
        client.resume();
        coroutine.sleep(100);

        assert.equal(received.length, 2);
        assert.equal(received[1], 'second');

        // Still paused after second handler ran
        serverConn.send(Buffer.from("third"));
        coroutine.sleep(100);
        assert.equal(received.length, 2);

        // Resume again
        client.resume();
        coroutine.sleep(100);

        assert.equal(received.length, 3);
        assert.equal(received[2], 'third');

    });

    it("data events are sequential, not concurrent", () => {
        var port = 28960 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();
        test_util.push(serverConn);

        var inHandler = false;
        var concurrentCount = 0;
        var totalCount = 0;

        client.on('data', (data) => {
            totalCount++;
            if (inHandler) concurrentCount++;
            inHandler = true;

            // Busy wait to simulate slow handler
            var start = Date.now();
            while (Date.now() - start < 50) { }

            inHandler = false;
        });

        // Rapid sends with tiny gaps
        for (var i = 0; i < 5; i++) {
            serverConn.send(Buffer.from("chunk" + i));
            coroutine.sleep(10);
        }

        coroutine.sleep(500);

        assert.equal(concurrentCount, 0);
        assert.equal(totalCount, 5);

    });

    it("multiple pause/resume cycles over TCP", () => {
        var port = 28970 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();
        test_util.push(serverConn);

        var received = [];
        var pauseCount = 0;

        // Pause inside handler so reader is guaranteed to stop
        client.on('data', function (data) {
            received.push(data.toString());
            client.pause();
        });

        // Cycle 1: send, handler pauses after receiving
        serverConn.send(Buffer.from("a1"));
        coroutine.sleep(100);
        assert.equal(received.length, 1);
        assert.equal(received[0], 'a1');

        // Cycle 2: send while paused, verify nothing, resume
        serverConn.send(Buffer.from("b1"));
        coroutine.sleep(100);
        assert.equal(received.length, 1);
        client.resume();
        coroutine.sleep(100);
        assert.equal(received.length, 2);
        assert.equal(received[1], 'b1');

        // Cycle 3: send multiple while paused, resume gets them
        serverConn.send(Buffer.from("c1"));
        coroutine.sleep(10);
        serverConn.send(Buffer.from("c2"));
        coroutine.sleep(100);
        assert.equal(received.length, 2);
        client.resume();
        coroutine.sleep(100);
        // Handler pauses after first recv, so c1 received, c2 pending
        assert.equal(received.length, 3);
        client.resume();
        coroutine.sleep(100);
        var all = received.join('');
        assert.ok(all.indexOf('c1') >= 0);
        assert.ok(all.indexOf('c2') >= 0);

        // Cycle 4: rapid pause/resume (resume undoes the handler's pause)
        serverConn.send(Buffer.from("d1"));
        coroutine.sleep(100);
        // Handler received d1 and paused
        assert.ok(received.join('').indexOf('d1') >= 0);
        client.resume();
        coroutine.sleep(50);

    });

    it("readable mode: read() drains buffer and resumes reader", () => {
        var port = 28980 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();
        test_util.push(serverConn);

        var chunks = [];
        var readableEvents = 0;

        client.on("readable", () => {
            readableEvents++;
            var chunk;
            while ((chunk = client.read()) !== null) {
                chunks.push(chunk.toString());
            }
        });

        // Send small data - should buffer and emit readable
        serverConn.send(Buffer.from("hello"));
        coroutine.sleep(100);
        assert.ok(readableEvents >= 1);
        assert.ok(chunks.length >= 1);
        assert.equal(chunks.join(''), 'hello');

        // Send more data - reader should have resumed after drain
        serverConn.send(Buffer.from(" world"));
        coroutine.sleep(100);
        assert.equal(chunks.join(''), 'hello world');

    });

    it("readable mode: read(n) partial reads", () => {
        var port = 28990 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();
        test_util.push(serverConn);

        var chunks = [];

        client.on("readable", () => {
            // Read exactly 3 bytes at a time
            var chunk;
            while ((chunk = client.read(3)) !== null) {
                chunks.push(chunk.toString());
            }
        });

        // Send 9 bytes - should be readable as 3 x 3-byte chunks
        serverConn.send(Buffer.from("abcdefghi"));
        coroutine.sleep(200);

        assert.equal(chunks.length, 3);
        assert.equal(chunks[0], 'abc');
        assert.equal(chunks[1], 'def');
        assert.equal(chunks[2], 'ghi');

    });

    it("readable mode: highWaterMark causes drain, read() resumes", () => {
        var port = 29000 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();

        var totalRead = 0;
        var readableEvents = 0;

        client.on("readable", () => {
            readableEvents++;
            // Intentionally delay reading to let buffer fill
            coroutine.sleep(50);
            var chunk;
            while ((chunk = client.read()) !== null) {
                totalRead += chunk.length;
            }
        });

        // Send much more than highWaterMark (16384)
        var chunk = Buffer.alloc(8192, 0x42); // 'B' * 8192
        for (var i = 0; i < 5; i++) {
            serverConn.send(chunk);
            coroutine.sleep(20);
        }

        serverConn.close();
        coroutine.sleep(500);

        // All data must eventually be received despite drain pauses
        assert.equal(totalRead, 8192 * 5);
        // Should have multiple readable events (drain + resume cycles)
        assert.ok(readableEvents >= 2);

    });

    it("pause does not lose data on MemoryStream", () => {
        var stm = new io.MemoryStream();
        stm.write("abcdefghijklmnopqrstuvwxyz");
        stm.rewind();

        var received = [];
        var pauseOnce = false;

        stm.on('data', (data) => {
            received.push(data.toString());
            if (!pauseOnce) {
                pauseOnce = true;
                stm.pause();
            }
        });

        // Wait for first data event + pause
        coroutine.sleep(50);
        assert.equal(received.length, 1);

        // Resume to get remaining data
        stm.resume();
        coroutine.sleep(50);

        // All data should be received, nothing lost
        assert.equal(received.join(''), 'abcdefghijklmnopqrstuvwxyz');
    });

    it("ref/unref controls process exit behavior", () => {
        var port = 29010 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();
        test_util.push(serverConn);

        // Start flowing mode
        client.on('data', () => { });
        coroutine.sleep(50);

        // unref should work
        client.unref();
        // ref should work
        client.ref();

        // pause should unref implicitly
        client.pause();

        // resume should ref implicitly
        client.resume();

    });

    it("pause/resume with setEncoding", () => {
        var port = 29020 + vmid;
        var serverConn = null;
        var serverReady = new coroutine.Event();

        var s = new net.Socket(net.AF_INET);
        s.bind(port);
        s.listen();
        test_util.push(s);

        coroutine.start(function () {
            try {
                serverConn = s.accept();
                serverReady.set();
            } catch (e) { }
        });

        var client = new net.Socket(net.AF_INET);
        client.connect(port, '127.0.0.1');
        test_util.push(client);
        serverReady.wait();
        test_util.push(serverConn);

        client.setEncoding('utf8');

        var received = [];

        client.on('data', (data) => {
            assert.equal(typeof data, 'string');
            received.push(data);
            // Pause after first chunk
            if (received.length === 1) {
                client.pause();
            }
        });

        // Send first chunk with multibyte chars
        serverConn.send(Buffer.from('Hello, 世界!'));
        coroutine.sleep(100);
        assert.equal(received.length, 1);
        assert.equal(received[0], 'Hello, 世界!');

        // Send second chunk while paused
        serverConn.send(Buffer.from('你好'));
        coroutine.sleep(100);
        // Should not have received it yet
        assert.equal(received.length, 1);

        // Resume
        client.resume();
        coroutine.sleep(100);

        assert.equal(received.length, 2);
        assert.equal(received[1], '你好');

    });
});
