var { describe, it, before, after } = require('node:test');
var assert = require('assert');
var fs = require('fs');
var path = require('path');
var os = require('os');

var tmpDir = os.tmpdir();

function tmpFile(name) {
    return path.join(tmpDir, 'test_stream_' + name + '_' + Date.now() + '_' + Math.random().toString(36).slice(2));
}

// Collect stream data via on('data') + on('close')
// Uses 'close' instead of 'end' since fibjs currently only emits 'close'
function collectStream(stream) {
    return new Promise(function (resolve, reject) {
        var chunks = [];
        stream.on("data", function (chunk) { chunks.push(chunk); });
        stream.on("close", function () { resolve(Buffer.concat(chunks)); });
        stream.on("error", reject);
    });
}

describe("fs.ReadStream - baseline", function () {
    var content = "Hello, createReadStream!";
    var f;

    before(function () {
        f = tmpFile("rs");
        fs.writeFileSync(f, content);
    });

    after(function () {
        try { fs.unlinkSync(f); } catch (e) { }
    });

    it("read via data+close events", async function () {
        var buf = await collectStream(fs.createReadStream(f));
        assert.strictEqual(buf.toString(), content);
    });

    it("with start and end (end is inclusive)", async function () {
        var buf = await collectStream(fs.createReadStream(f, { start: 7, end: 22 }));
        assert.strictEqual(buf.toString(), "createReadStream");
    });

    it("with start only", async function () {
        var buf = await collectStream(fs.createReadStream(f, { start: 7 }));
        assert.strictEqual(buf.toString(), "createReadStream!");
    });

    it("with end only", async function () {
        var buf = await collectStream(fs.createReadStream(f, { end: 4 }));
        assert.strictEqual(buf.toString(), "Hello");
    });

    it("start=0 end=0 reads one byte", async function () {
        var buf = await collectStream(fs.createReadStream(f, { start: 0, end: 0 }));
        assert.strictEqual(buf.toString(), "H");
    });

    it("emits close event after read", async function () {
        await new Promise(function (resolve, reject) {
            var rs = fs.createReadStream(f);
            var timer = setTimeout(function () { reject(new Error("timeout")); }, 3000);
            rs.on("close", function () { clearTimeout(timer); resolve(); });
            rs.on("error", function (e) { clearTimeout(timer); reject(e); });
            rs.resume();
        });
    });

    it("fd is available", async function () {
        var rs = fs.createReadStream(f);
        rs.on("data", function () { });
        await new Promise(function (resolve) {
            if (typeof rs.fd === 'number') return resolve();
            rs.on('open', function () { resolve(); });
        });
        assert.strictEqual(typeof rs.fd, "number");
        assert.ok(rs.fd >= 0);
    });

    it("setEncoding returns string data", async function () {
        var rs = fs.createReadStream(f);
        rs.setEncoding("utf8");
        var result = await new Promise(function (resolve, reject) {
            var str = "";
            rs.on("data", function (chunk) { str += chunk; });
            rs.on("close", function () { resolve(str); });
            rs.on("error", reject);
        });
        assert.strictEqual(typeof result, "string");
        assert.strictEqual(result, content);
    });

    it("readable mode with on('readable') + read()", async function () {
        var rs = fs.createReadStream(f);
        var result = await new Promise(function (resolve, reject) {
            var chunks = [];
            rs.on("readable", function () {
                var chunk;
                while ((chunk = rs.read()) !== null) {
                    chunks.push(chunk);
                }
            });
            rs.on("close", function () { resolve(Buffer.concat(chunks).toString()); });
            rs.on("error", reject);
        });
        assert.strictEqual(result, content);
    });

    it("pause and resume", async function () {
        var rs = fs.createReadStream(f);
        var result = await new Promise(function (resolve, reject) {
            var chunks = [];
            rs.on("data", function (chunk) {
                chunks.push(chunk);
                rs.pause();
                setTimeout(function () { rs.resume(); }, 10);
            });
            rs.on("close", function () { resolve(Buffer.concat(chunks).toString()); });
            rs.on("error", reject);
        });
        assert.strictEqual(result, content);
    });
});

describe("fs.WriteStream - baseline", function () {
    it("write + end produces correct file", async function () {
        var dest = tmpFile("ws1");
        var ws = fs.createWriteStream(dest);
        ws.write("hello ");
        ws.end("world");
        // Wait for data to be flushed (no finish event yet)
        await new Promise(function (resolve) { setTimeout(resolve, 500); });
        assert.strictEqual(fs.readFileSync(dest, "utf8"), "hello world");
        fs.unlinkSync(dest);
    });

    it("end with no data", async function () {
        var dest = tmpFile("ws2");
        fs.writeFileSync(dest, "");
        var ws = fs.createWriteStream(dest);
        ws.end();
        await new Promise(function (resolve) { setTimeout(resolve, 500); });
        assert.strictEqual(fs.readFileSync(dest, "utf8"), "");
        fs.unlinkSync(dest);
    });

    it("fd is available", async function () {
        var dest = tmpFile("ws3");
        var ws = fs.createWriteStream(dest);
        await new Promise(function (resolve) {
            if (typeof ws.fd === 'number') return resolve();
            ws.on('open', function () { resolve(); });
        });
        assert.strictEqual(typeof ws.fd, "number");
        assert.ok(ws.fd >= 0);
        ws.end();
        await new Promise(function (resolve) { setTimeout(resolve, 500); });
        fs.unlinkSync(dest);
    });

    it("write with append flag", async function () {
        var dest = tmpFile("ws4");
        fs.writeFileSync(dest, "first ");
        var ws = fs.createWriteStream(dest, { flags: "a" });
        ws.end("second");
        await new Promise(function (resolve) { setTimeout(resolve, 500); });
        assert.strictEqual(fs.readFileSync(dest, "utf8"), "first second");
        fs.unlinkSync(dest);
    });

    it("multiple writes before end", async function () {
        var dest = tmpFile("ws5");
        var ws = fs.createWriteStream(dest);
        ws.write("a");
        ws.write("b");
        ws.write("c");
        ws.end("d");
        await new Promise(function (resolve) { setTimeout(resolve, 500); });
        assert.strictEqual(fs.readFileSync(dest, "utf8"), "abcd");
        fs.unlinkSync(dest);
    });
});

// --- Step 1-2 tests: end/finish/close events ---

describe("fs.ReadStream - end event", function () {
    var content = "end event test data";
    var f;

    before(function () {
        f = tmpFile("rs_end");
        fs.writeFileSync(f, content);
    });

    after(function () {
        try { fs.unlinkSync(f); } catch (e) { }
    });

    it("end event fires before close", async function () {
        var rs = fs.createReadStream(f);
        var events = [];
        rs.on("end", function () { events.push("end"); });
        rs.on("close", function () { events.push("close"); });
        rs.resume();
        await new Promise(function (resolve, reject) {
            var timer = setTimeout(function () { reject(new Error("timeout")); }, 3000);
            rs.on("close", function () { clearTimeout(timer); resolve(); });
            rs.on("error", function (e) { clearTimeout(timer); reject(e); });
        });
        assert.deepStrictEqual(events, ["end", "close"]);
    });

    it("collectStream via end event", async function () {
        var rs = fs.createReadStream(f);
        var buf = await new Promise(function (resolve, reject) {
            var chunks = [];
            rs.on("data", function (chunk) { chunks.push(chunk); });
            rs.on("end", function () { resolve(Buffer.concat(chunks)); });
            rs.on("error", reject);
        });
        assert.strictEqual(buf.toString(), content);
    });
});

describe("fs.WriteStream - finish/close events", function () {
    it("finish event fires on end()", async function () {
        var dest = tmpFile("ws_fin1");
        var ws = fs.createWriteStream(dest);
        ws.write("data");
        var finished = false;
        // Register all listeners BEFORE end() — events may fire synchronously
        var p = new Promise(function (resolve, reject) {
            var timer = setTimeout(function () { reject(new Error("timeout")); }, 3000);
            ws.on("finish", function () { finished = true; clearTimeout(timer); resolve(); });
            ws.on("error", function (e) { clearTimeout(timer); reject(e); });
        });
        ws.end();
        await p;
        assert.strictEqual(finished, true);
        assert.strictEqual(fs.readFileSync(dest, "utf8"), "data");
        fs.unlinkSync(dest);
    });

    it("close event fires on end()", async function () {
        var dest = tmpFile("ws_fin2");
        var ws = fs.createWriteStream(dest);
        ws.write("data");
        var p = new Promise(function (resolve, reject) {
            var timer = setTimeout(function () { reject(new Error("timeout")); }, 3000);
            ws.on("close", function () { clearTimeout(timer); resolve(); });
            ws.on("error", function (e) { clearTimeout(timer); reject(e); });
        });
        ws.end();
        await p;
        assert.strictEqual(fs.readFileSync(dest, "utf8"), "data");
        fs.unlinkSync(dest);
    });

    it("event order: finish before close", async function () {
        var dest = tmpFile("ws_fin3");
        var ws = fs.createWriteStream(dest);
        var events = [];
        // Register all listeners BEFORE end()
        var p = new Promise(function (resolve, reject) {
            var timer = setTimeout(function () { reject(new Error("timeout")); }, 3000);
            ws.on("finish", function () { events.push("finish"); });
            ws.on("close", function () { events.push("close"); clearTimeout(timer); resolve(); });
            ws.on("error", function (e) { clearTimeout(timer); reject(e); });
        });
        ws.write("order test");
        ws.end();
        await p;
        assert.deepStrictEqual(events, ["finish", "close"]);
        assert.strictEqual(fs.readFileSync(dest, "utf8"), "order test");
        fs.unlinkSync(dest);
    });
});

// --- Step 3 tests: open event ---

describe("fs.ReadStream - open event", function () {
    var content = "open event test data";
    var f;

    before(function () {
        f = tmpFile("rs_open");
        fs.writeFileSync(f, content);
    });

    after(function () {
        try { fs.unlinkSync(f); } catch (e) { }
    });

    it("open event fires with fd", async function () {
        var rs = fs.createReadStream(f);
        var openFd = await new Promise(function (resolve, reject) {
            var timer = setTimeout(function () { reject(new Error("timeout")); }, 3000);
            rs.on("open", function (fd) { clearTimeout(timer); resolve(fd); });
            rs.on("error", function (e) { clearTimeout(timer); reject(e); });
        });
        assert.strictEqual(typeof openFd, "number");
        assert.ok(openFd >= 0);
        rs.resume();
        await new Promise(function (resolve) {
            rs.on("close", function () { resolve(); });
        });
    });
});

describe("fs.WriteStream - open event", function () {
    it("open event fires with fd", async function () {
        var dest = tmpFile("ws_open");
        var ws = fs.createWriteStream(dest);
        var openFd = await new Promise(function (resolve, reject) {
            var timer = setTimeout(function () { reject(new Error("timeout")); }, 3000);
            ws.on("open", function (fd) { clearTimeout(timer); resolve(fd); });
            ws.on("error", function (e) { clearTimeout(timer); reject(e); });
        });
        assert.strictEqual(typeof openFd, "number");
        assert.ok(openFd >= 0);
        var p = new Promise(function (resolve) {
            ws.on("close", function () { resolve(); });
        });
        ws.end();
        await p;
        fs.unlinkSync(dest);
    });
});

// --- Step 6 tests: stream.finished() ---

var { finished } = require('stream/promises');

describe("stream.finished() with native streams", function () {
    var content = "finished test data";
    var f;

    before(function () {
        f = tmpFile("fin");
        fs.writeFileSync(f, content);
    });

    after(function () {
        try { fs.unlinkSync(f); } catch (e) { }
    });

    it("finished() resolves for ReadStream", async function () {
        var rs = fs.createReadStream(f);
        rs.resume();
        await finished(rs);
    });

    it("finished() resolves for WriteStream", async function () {
        var dest = tmpFile("fin_ws");
        var ws = fs.createWriteStream(dest);
        ws.write("data");
        var p = finished(ws);
        ws.end();
        await p;
        assert.strictEqual(fs.readFileSync(dest, "utf8"), "data");
        fs.unlinkSync(dest);
    });
});
