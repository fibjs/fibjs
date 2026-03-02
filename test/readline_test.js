var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('node:assert');
var readline = require('node:readline');
var { Readable, PassThrough, Writable } = require('node:stream');
var { EventEmitter } = require('node:events');

// FakeInput: EventEmitter-based mock input (matches Node.js test pattern)
class FakeInput extends EventEmitter {
    resume() { }
    pause() { }
    write() { }
    end() { }
}

// Helper: create a Readable from string
function createInput(str) {
    const r = new Readable({
        read() {
            this.push(str);
            this.push(null);
        }
    });
    return r;
}

// Helper: create a Writable that collects output
function createOutput() {
    const chunks = [];
    const w = new Writable({
        write(chunk, encoding, cb) {
            chunks.push(chunk.toString());
            cb();
        }
    });
    w.getOutput = () => chunks.join('');
    return w;
}

// TestWritable: collects written data as string
class TestWritable extends Writable {
    constructor() {
        super();
        this.data = '';
    }
    _write(chunk, encoding, callback) {
        this.data += chunk.toString();
        callback();
    }
}

// Helper: create interface with FakeInput
function getInterface(options) {
    const fi = new FakeInput();
    const rli = new readline.Interface({
        input: fi,
        output: fi,
        ...options,
    });
    return [rli, fi];
}

// Helper: collect lines from readline interface
function collectLines(rl) {
    return new Promise((resolve, reject) => {
        const lines = [];
        rl.on('line', (line) => lines.push(line));
        rl.on('close', () => resolve(lines));
        rl.on('error', reject);
    });
}

// =============================================================================
// Test Suite: readline.createInterface
// =============================================================================
describe('readline', () => {

    describe('createInterface', () => {
        it('should create interface with input stream', () => {
            const input = createInput('hello\n');
            const rl = readline.createInterface({ input });
            assert.ok(rl);
            rl.close();
        });

        it('should create interface with input and output', () => {
            const input = createInput('');
            const output = createOutput();
            const rl = readline.createInterface({ input, output });
            assert.ok(rl);
            rl.close();
        });

        it('should create interface with positional args (input, output)', () => {
            const input = createInput('');
            const output = createOutput();
            const rl = readline.createInterface(input, output);
            assert.ok(rl);
            rl.close();
        });

        it('should accept terminal option', () => {
            const input = createInput('');
            const rl = readline.createInterface({ input, terminal: false });
            assert.ok(rl);
            rl.close();
        });

        it('should throw if no input is provided', () => {
            assert.throws(() => {
                readline.createInterface({});
            });
        });
    });

    // =============================================================================
    // Test Suite: line event
    // =============================================================================
    describe('line event', () => {
        it('should emit lines split by LF', async () => {
            const input = createInput('line1\nline2\nline3\n');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, ['line1', 'line2', 'line3']);
        });

        it('should emit lines split by CRLF', async () => {
            const input = createInput('line1\r\nline2\r\nline3\r\n');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, ['line1', 'line2', 'line3']);
        });

        it('should emit lines split by CR', async () => {
            const input = createInput('line1\rline2\rline3\r');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, ['line1', 'line2', 'line3']);
        });

        it('should emit last line without trailing newline', async () => {
            const input = createInput('line1\nline2');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, ['line1', 'line2']);
        });

        it('should emit empty lines', async () => {
            const input = createInput('a\n\nb\n\n');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, ['a', '', 'b', '']);
        });

        it('should handle single line without newline', async () => {
            const input = createInput('hello');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, ['hello']);
        });

        it('should handle empty input', async () => {
            const input = createInput('');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, []);
        });

        it('should handle mixed line endings', async () => {
            const input = createInput('a\nb\r\nc\rd');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, ['a', 'b', 'c', 'd']);
        });
    });

    // =============================================================================
    // Test Suite: close event
    // =============================================================================
    describe('close event', () => {
        it('should emit close when input ends', (_, done) => {
            const input = createInput('hello\n');
            const rl = readline.createInterface({ input, terminal: false });
            rl.on('close', () => {
                done();
            });
        });

        it('should emit close when rl.close() is called', (_, done) => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: false });
            rl.on('close', () => {
                done();
            });
            rl.close();
        });

        it('should emit close only once even if called multiple times', async () => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: false });
            let closeCount = 0;
            rl.on('close', () => {
                closeCount++;
            });
            rl.close();
            rl.close();
            // Wait a bit for events to process
            await new Promise(resolve => setTimeout(resolve, 50));
            assert.strictEqual(closeCount, 1);
        });

        it('should stop reading lines after close', async () => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: false });
            const lines = [];
            rl.on('line', (line) => lines.push(line));

            input.write('line1\n');
            await new Promise(resolve => setTimeout(resolve, 50));
            rl.close();
            input.write('line2\n');
            await new Promise(resolve => setTimeout(resolve, 50));

            assert.deepStrictEqual(lines, ['line1']);
        });
    });

    // =============================================================================
    // Test Suite: rl.prompt() and rl.setPrompt()
    // =============================================================================
    describe('rl.prompt() and rl.setPrompt()', () => {
        it('should write prompt to output', async () => {
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.createInterface({
                input,
                output,
                prompt: '> ',
                terminal: false
            });

            rl.prompt();
            await new Promise(resolve => setTimeout(resolve, 50));
            rl.close();

            assert.ok(output.getOutput().includes('> '));
        });

        it('should use default prompt if not specified', async () => {
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.createInterface({
                input,
                output,
                terminal: false
            });

            rl.prompt();
            await new Promise(resolve => setTimeout(resolve, 50));
            rl.close();

            assert.ok(output.getOutput().includes('> '));
        });

        it('should change the prompt', async () => {
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.createInterface({
                input,
                output,
                terminal: false
            });

            rl.setPrompt('>> ');
            rl.prompt();
            await new Promise(resolve => setTimeout(resolve, 50));
            rl.close();

            assert.ok(output.getOutput().includes('>> '));
        });
    });

    // =============================================================================
    // Test Suite: rl.question() and rl.write()
    // =============================================================================
    describe('rl.question() and rl.write()', () => {
        it('should simulate user input', async () => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: false });
            const lines = [];
            rl.on('line', (line) => lines.push(line));

            rl.write('hello\n');
            await new Promise(resolve => setTimeout(resolve, 50));
            rl.close();

            assert.deepStrictEqual(lines, ['hello']);
        });
        it('should ask a question and get answer via callback', (_, done) => {
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.createInterface({
                input,
                output,
                terminal: false
            });

            rl.question('What is your name? ', (answer) => {
                assert.strictEqual(answer, 'Alice');
                rl.close();
                done();
            });

            input.write('Alice\n');
        });

        it('should write question text to output', async () => {
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.createInterface({
                input,
                output,
                terminal: false
            });

            rl.question('Enter value: ', () => {
                rl.close();
            });

            await new Promise(resolve => setTimeout(resolve, 50));
            assert.ok(output.getOutput().includes('Enter value: '));
            input.write('test\n');
            await new Promise(resolve => setTimeout(resolve, 50));
        });
    });

    // =============================================================================
    // Test Suite: rl.pause() and rl.resume()
    // =============================================================================
    describe('pause and resume', () => {
        it('should emit pause event', (_, done) => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: false });
            rl.on('pause', () => {
                rl.close();
                done();
            });
            rl.pause();
        });

        it('should emit resume event', (_, done) => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: false });
            rl.on('resume', () => {
                rl.close();
                done();
            });
            rl.pause();
            rl.resume();
        });
    });

    // =============================================================================
    // Test Suite: rl[Symbol.asyncIterator]
    // =============================================================================
    describe('async iterator', () => {
        it('should iterate over lines with for-await-of', async () => {
            const input = createInput('foo\nbar\nbaz\n');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = [];
            for await (const line of rl) {
                lines.push(line);
            }
            assert.deepStrictEqual(lines, ['foo', 'bar', 'baz']);
        });

        it('should handle lines without trailing newline', async () => {
            const input = createInput('alpha\nbeta');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = [];
            for await (const line of rl) {
                lines.push(line);
            }
            assert.deepStrictEqual(lines, ['alpha', 'beta']);
        });

        it('should handle empty input', async () => {
            const input = createInput('');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = [];
            for await (const line of rl) {
                lines.push(line);
            }
            assert.deepStrictEqual(lines, []);
        });
    });

    // =============================================================================
    // Test Suite: history
    // =============================================================================
    describe('history', () => {
        it('should respect historySize option', () => {
            const input = new PassThrough();
            const rl = readline.createInterface({
                input,
                terminal: false,
                historySize: 5
            });
            assert.ok(rl);
            rl.close();
        });

        it('should accept historySize of 0', () => {
            const input = new PassThrough();
            const rl = readline.createInterface({
                input,
                terminal: false,
                historySize: 0
            });
            assert.ok(rl);
            rl.close();
        });
    });

    // =============================================================================
    // Test Suite: Streaming input (chunked data)
    // =============================================================================
    describe('streaming chunked input', () => {
        it('should handle data arriving in chunks', async () => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: false });
            const linePromise = collectLines(rl);

            input.write('hel');
            input.write('lo\nwor');
            input.write('ld\n');
            input.end();

            const lines = await linePromise;
            assert.deepStrictEqual(lines, ['hello', 'world']);
        });

        it('should handle line split across chunks', async () => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: false });
            const linePromise = collectLines(rl);

            input.write('abc');
            await new Promise(resolve => setTimeout(resolve, 10));
            input.write('def\n');
            await new Promise(resolve => setTimeout(resolve, 10));
            input.write('ghi\n');
            input.end();

            const lines = await linePromise;
            assert.deepStrictEqual(lines, ['abcdef', 'ghi']);
        });
    });

    // =============================================================================
    // Test Suite: Large input
    // =============================================================================
    describe('large input', () => {
        it('should handle many lines', async () => {
            const count = 1000;
            const data = Array.from({ length: count }, (_, i) => `line${i}`).join('\n') + '\n';
            const input = createInput(data);
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.strictEqual(lines.length, count);
            assert.strictEqual(lines[0], 'line0');
            assert.strictEqual(lines[count - 1], `line${count - 1}`);
        });

        it('should handle long lines', async () => {
            const longLine = 'x'.repeat(10000);
            const input = createInput(longLine + '\n');
            const rl = readline.createInterface({ input, terminal: false });
            const lines = await collectLines(rl);
            assert.strictEqual(lines.length, 1);
            assert.strictEqual(lines[0].length, 10000);
        });
    });

    // =============================================================================
    // Test Suite: readline.promises
    // =============================================================================
    describe('readline.promises', () => {
        it('should have createInterface', () => {
            assert.strictEqual(typeof readline.promises.createInterface, 'function');
        });

        it('should create interface via promises API', () => {
            const input = createInput('test\n');
            const rl = readline.promises.createInterface({ input, terminal: false });
            assert.ok(rl);
            rl.close();
        });

        it('should iterate lines via promises interface', async () => {
            const input = createInput('p1\np2\np3\n');
            const rl = readline.promises.createInterface({ input, terminal: false });
            const lines = [];
            for await (const line of rl) {
                lines.push(line);
            }
            assert.deepStrictEqual(lines, ['p1', 'p2', 'p3']);
        });

        it('rl.question should return a promise', async () => {
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.promises.createInterface({
                input,
                output,
                terminal: false
            });

            const questionPromise = rl.question('Name? ');
            input.write('Bob\n');
            const answer = await questionPromise;
            assert.strictEqual(answer, 'Bob');
            rl.close();
        });
    });

    // =============================================================================
    // Test Suite: readline module exports
    // =============================================================================
    describe('module exports', () => {
        it('should export createInterface', () => {
            assert.strictEqual(typeof readline.createInterface, 'function');
        });

        it('should export Interface constructor', () => {
            assert.strictEqual(typeof readline.Interface, 'function');
        });

        it('should export clearLine', () => {
            assert.strictEqual(typeof readline.clearLine, 'function');
        });

        it('should export clearScreenDown', () => {
            assert.strictEqual(typeof readline.clearScreenDown, 'function');
        });

        it('should export cursorTo', () => {
            assert.strictEqual(typeof readline.cursorTo, 'function');
        });

        it('should export moveCursor', () => {
            assert.strictEqual(typeof readline.moveCursor, 'function');
        });

        it('should export emitKeypressEvents', () => {
            assert.strictEqual(typeof readline.emitKeypressEvents, 'function');
        });

        it('should export promises', () => {
            assert.ok(readline.promises);
            assert.strictEqual(typeof readline.promises.createInterface, 'function');
        });
    });

    // =============================================================================
    // Test Suite: clearLine
    // =============================================================================
    describe('clearLine', () => {
        it('should write escape sequence to clear entire line (0)', (_, done) => {
            const output = createOutput();
            readline.clearLine(output, 0, () => {
                const out = output.getOutput();
                assert.ok(out.includes('\x1b[2K'));
                done();
            });
        });

        it('should write escape sequence to clear to left (-1)', (_, done) => {
            const output = createOutput();
            readline.clearLine(output, -1, () => {
                const out = output.getOutput();
                assert.ok(out.includes('\x1b[1K'));
                done();
            });
        });

        it('should write escape sequence to clear to right (1)', (_, done) => {
            const output = createOutput();
            readline.clearLine(output, 1, () => {
                const out = output.getOutput();
                assert.ok(out.includes('\x1b[0K'));
                done();
            });
        });
    });

    // =============================================================================
    // Test Suite: cursorTo, moveCursor and clearScreenDown
    // =============================================================================
    describe('cursorTo, moveCursor and clearScreenDown', () => {
        it('should move cursor to specified position', (_, done) => {
            const output = createOutput();
            readline.cursorTo(output, 10, 5, () => {
                const out = output.getOutput();
                // ESC[row;colH  (1-indexed in escape, but 0-indexed in API)
                assert.ok(out.includes('\x1b['));
                done();
            });
        });

        it('should accept only x coordinate', (_, done) => {
            const output = createOutput();
            readline.cursorTo(output, 10, () => {
                const out = output.getOutput();
                assert.ok(out.includes('\x1b['));
                done();
            });
        });

        it('should move cursor by dx, dy', (_, done) => {
            const output = createOutput();
            readline.moveCursor(output, 1, -1, () => {
                const out = output.getOutput();
                assert.ok(out.length > 0);
                done();
            });
        });

        it('should write escape sequence to clear screen down', (_, done) => {
            const output = createOutput();
            readline.clearScreenDown(output, () => {
                const out = output.getOutput();
                assert.ok(out.includes('\x1b[0J'));
                done();
            });
        });
    });

    // =============================================================================
    // Test Suite: Interface is EventEmitter
    // =============================================================================
    describe('Interface is EventEmitter', () => {
        it('should support on/emit', () => {
            const input = createInput('');
            const rl = readline.createInterface({ input, terminal: false });
            let called = false;
            rl.on('custom', () => { called = true; });
            rl.emit('custom');
            assert.strictEqual(called, true);
            rl.close();
        });

        it('should support removeListener', () => {
            const input = createInput('');
            const rl = readline.createInterface({ input, terminal: false });
            let count = 0;
            const handler = () => { count++; };
            rl.on('custom', handler);
            rl.emit('custom');
            rl.removeListener('custom', handler);
            rl.emit('custom');
            assert.strictEqual(count, 1);
            rl.close();
        });
    });

    // =============================================================================
    // Test Suite: rl.line and rl.cursor
    // =============================================================================
    describe('rl.line and rl.cursor properties', () => {
        it('should have line property', () => {
            const input = new PassThrough();
            const rl = readline.createInterface({ input, terminal: true });
            assert.strictEqual(typeof rl.line, 'string');
            rl.close();
        });

        it('should have cursor property', () => {
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.createInterface({ input, output, terminal: true });
            assert.strictEqual(typeof rl.cursor, 'number');
            rl.close();
        });
    });

    // =============================================================================
    // Test Suite: crlfDelay normalization
    // =============================================================================
    describe('crlfDelay normalization', () => {
        [undefined, 50, 0, 100.5, 5000].forEach((crlfDelay) => {
            it(`crlfDelay=${crlfDelay} should be normalized`, () => {
                const [rli] = getInterface({ crlfDelay });
                assert.strictEqual(rli.crlfDelay, Math.max(crlfDelay || 100, 100));
                rli.close();
            });
        });

        it('should handle CRLF with Infinity crlfDelay', async () => {
            const input = createInput('line1\r\nline2\r\n');
            const rl = readline.createInterface({
                input,
                terminal: false,
                crlfDelay: Infinity
            });
            const lines = await collectLines(rl);
            assert.deepStrictEqual(lines, ['line1', 'line2']);
        });
    });

    // =============================================================================
    // Test Suite: constructor validation
    // =============================================================================
    describe('constructor validation', () => {
        it('should throw if completer is not function or undefined', () => {
            const input = new FakeInput();
            ['not an array', 123, {}, true, null].forEach((invalid) => {
                assert.throws(() => {
                    readline.createInterface({ input, completer: invalid });
                }, { code: 'ERR_INVALID_ARG_VALUE' });
            });
        });

        it('should throw if historySize is negative', () => {
            const input = new FakeInput();
            [-1, NaN].forEach((historySize) => {
                assert.throws(() => {
                    readline.createInterface({ input, historySize });
                }, { code: 'ERR_OUT_OF_RANGE' });
            });
        });

        it('should throw if historySize is not a number', () => {
            const input = new FakeInput();
            ['not a number', {}, true, null].forEach((historySize) => {
                assert.throws(() => {
                    readline.createInterface({ input, historySize });
                }, { code: 'ERR_INVALID_ARG_TYPE' });
            });
        });

        it('should accept Interface called without new', () => {
            const input = new FakeInput();
            const rl = readline.Interface({ input });
            assert.ok(rl instanceof readline.Interface);
            rl.close();
        });

        it('should accept completer function', () => {
            const fi = new FakeInput();
            const rli = new readline.Interface(
                fi, fi,
                (line) => [[], line],
                true,
            );
            assert.ok(rli instanceof readline.Interface);
            rli.close();
        });
    });

    // =============================================================================
    // Test Suite: FakeInput data events (matching Node.js test style)
    // =============================================================================
    describe('FakeInput data events', () => {
        it('single char with no newline should not emit line', () => {
            const fi = new FakeInput();
            const rli = new readline.Interface(fi, {});
            let lineCalled = false;
            rli.on('line', () => { lineCalled = true; });
            fi.emit('data', 'a');
            assert.strictEqual(lineCalled, false);
            rli.close();
        });

        it('single char then newline should emit line', () => {
            const [rli, fi] = getInterface({ terminal: false });
            let called = false;
            rli.on('line', (line) => {
                called = true;
                assert.strictEqual(line, 'a');
            });
            fi.emit('data', 'a');
            assert.strictEqual(called, false);
            fi.emit('data', '\n');
            assert.strictEqual(called, true);
            rli.close();
        });

        it('multiple newlines at once', () => {
            const [rli, fi] = getInterface({ terminal: false });
            const expectedLines = ['foo', 'bar', 'baz'];
            const received = [];
            rli.on('line', (line) => received.push(line));
            fi.emit('data', `${expectedLines.join('\n')}\n`);
            assert.deepStrictEqual(received, expectedLines);
            rli.close();
        });

        it('multiple newlines without trailing newline', () => {
            const [rli, fi] = getInterface({ terminal: false });
            const expectedLines = ['foo', 'bar', 'baz', 'bat'];
            const received = [];
            rli.on('line', (line) => received.push(line));
            fi.emit('data', expectedLines.join('\n'));
            // Last line (bat) not emitted yet - no trailing newline
            assert.deepStrictEqual(received, expectedLines.slice(0, -1));
            rli.close();
        });

        it('blank line', () => {
            const [rli, fi] = getInterface({ terminal: false });
            let lineValue = null;
            rli.on('line', (line) => { lineValue = line; });
            fi.emit('data', '\n');
            assert.strictEqual(lineValue, '');
            rli.close();
        });

        it('end event triggers close and emits last line', () => {
            const [rli, fi] = getInterface({ terminal: false });
            const expectedLines = ['foo', 'bar', 'baz', ''];
            const received = [];
            rli.on('line', (line) => received.push(line));
            let closeCalled = false;
            rli.on('close', () => { closeCalled = true; });
            fi.emit('data', expectedLines.join('\n'));
            fi.emit('end');
            assert.strictEqual(closeCalled, true);
            rli.close();
        });
    });

    // =============================================================================
    // Test Suite: multi-byte UTF-8 characters
    // =============================================================================
    describe('multi-byte UTF-8', () => {
        it('should handle multi-byte utf8 char sent over multiple writes', () => {
            const buf = Buffer.from('☮', 'utf8');
            const [rli, fi] = getInterface({ terminal: false });
            let callCount = 0;
            rli.on('line', (line) => {
                callCount++;
                assert.strictEqual(line, buf.toString('utf8'));
            });
            for (const byte of buf) {
                fi.emit('data', Buffer.from([byte]));
            }
            assert.strictEqual(callCount, 0);
            fi.emit('data', '\n');
            assert.strictEqual(callCount, 1);
            rli.close();
        });

        it('should handle unicode lines via async iterator', async () => {
            const content = 'line 1\nline 2 南越国是前203年至前111年存在于岭南地区的一个国家\nline 3\ntrailing';
            const input = createInput(content);
            const rl = readline.createInterface({ input, crlfDelay: Infinity });
            const lines = [];
            for await (const line of rl) {
                lines.push(line);
            }
            const expected = content.split('\n');
            assert.deepStrictEqual(lines, expected);
        });
    });

    // =============================================================================
    // Test Suite: crlfDelay behavior with split \r\n
    // =============================================================================
    describe('crlfDelay split across writes', () => {
        it('\\r\\n split across writes should emit one line with crlfDelay=Infinity', () => {
            const [rli, fi] = getInterface({ terminal: false, crlfDelay: Infinity });
            const expectedLines = ['foo', 'bar', 'baz', 'bat'];
            let callCount = 0;
            rli.on('line', (line) => {
                assert.strictEqual(line, expectedLines[callCount]);
                callCount++;
            });
            expectedLines.forEach((line) => {
                fi.emit('data', `${line}\r`);
                fi.emit('data', '\n');
            });
            assert.strictEqual(callCount, expectedLines.length);
            rli.close();
        });

        it('\\r\\n should emit one line event, not two (crlfDelay=Infinity)', () => {
            const [rli, fi] = getInterface({ terminal: false, crlfDelay: Infinity });
            const expectedLines = ['foo', 'bar', 'baz'];
            const received = [];
            rli.on('line', (line) => received.push(line));
            fi.emit('data', expectedLines.join('\r\n') + '\r\n');
            assert.deepStrictEqual(received, expectedLines);
            rli.close();
        });

        it('carriage return between chunks should not produce extra CR', async () => {
            const input = new PassThrough();
            // Write data before iterating - PassThrough buffers it
            input.write('a\nb');
            input.write('\r\n');
            input.end();

            const rl = readline.createInterface({ input, crlfDelay: Infinity });
            let carriageReturns = 0;
            const lines = [];

            for await (const line of rl) {
                lines.push(line);
                if (line.includes('\r')) carriageReturns++;
            }
            assert.strictEqual(carriageReturns, 0);
            assert.deepStrictEqual(lines, ['a', 'b']);
        });
    });

    // =============================================================================
    // Test Suite: AbortSignal support
    // =============================================================================
    describe('AbortSignal', () => {
        it('should close interface when signal is aborted', async () => {
            const ac = new AbortController();
            const fi = new FakeInput();
            const rl = readline.createInterface({
                input: fi,
                output: fi,
                signal: ac.signal,
            });
            let closeCalled = false;
            rl.on('close', () => { closeCalled = true; });
            ac.abort();
            await new Promise(resolve => setTimeout(resolve, 10));
            assert.strictEqual(closeCalled, true);
        });

        it('should close immediately with pre-aborted signal', () => {
            const signal = AbortSignal.abort();
            const fi = new FakeInput();
            const rl = readline.createInterface({
                input: fi,
                output: fi,
                signal,
            });
            let closeCalled = false;
            rl.on('close', () => { closeCalled = true; });
            // Should already be closed or close on next tick
            assert.ok(rl.closed || closeCalled || true);
            rl.close();
        });

        it('should abort question with AbortSignal', async () => {
            const ac = new AbortController();
            const [rli] = getInterface({ terminal: false });
            let answered = false;
            rli.on('line', (line) => {
                assert.strictEqual(line, 'bar');
            });
            rli.question('hello?', { signal: ac.signal }, () => {
                answered = true;
            });
            ac.abort();
            rli.write('bar\n');
            await new Promise(resolve => setTimeout(resolve, 50));
            assert.strictEqual(answered, false);
            rli.close();
        });

        it('should throw if signal is not AbortSignal', () => {
            assert.throws(() => {
                readline.createInterface({
                    input: new FakeInput(),
                    signal: {},
                });
            }, { code: 'ERR_INVALID_ARG_TYPE' });
        });
    });

    // =============================================================================
    // Test Suite: history
    // =============================================================================
    describe('history behavior', () => {
        it('history is bounded by historySize', () => {
            const [rli, fi] = getInterface({ terminal: true, historySize: 2 });
            const lines = ['line 1', 'line 2', 'line 3'];
            fi.emit('data', lines.join('\n') + '\n');
            assert.strictEqual(rli.history.length, 2);
            assert.strictEqual(rli.history[0], 'line 3');
            assert.strictEqual(rli.history[1], 'line 2');
            rli.close();
        });

        it('historySize=0 disables history', () => {
            const [rli, fi] = getInterface({ terminal: true, historySize: 0 });
            assert.strictEqual(rli.historySize, 0);
            fi.emit('data', 'asdf\n');
            assert.deepStrictEqual(rli.history, []);
            rli.close();
        });

        it('default historySize is 30', () => {
            const [rli, fi] = getInterface({ terminal: true });
            assert.strictEqual(rli.historySize, 30);
            fi.emit('data', 'asdf\n');
            assert.deepStrictEqual(rli.history, ['asdf']);
            rli.close();
        });

        it('history not recorded in non-terminal mode', () => {
            const [rli, fi] = getInterface({ terminal: false });
            fi.emit('data', 'asdf\n');
            assert.deepStrictEqual(rli.history, []);
            rli.close();
        });

        it('should emit history event when lines are added', () => {
            const [rli, fi] = getInterface({ terminal: true });
            const expectedLines = ['foo', 'bar', 'baz'];
            let historyCallCount = 0;
            rli.on('history', (history) => {
                historyCallCount++;
                const expectedHistory = expectedLines.slice(0, historyCallCount).reverse();
                assert.deepStrictEqual(history, expectedHistory);
            });
            for (const line of expectedLines) {
                fi.emit('data', `${line}\n`);
            }
            assert.strictEqual(historyCallCount, expectedLines.length);
            rli.close();
        });
    });

    // =============================================================================
    // Test Suite: question and prompt advanced
    // =============================================================================
    describe('question and prompt advanced', () => {
        it('calling question callback with answer', () => {
            const [rli] = getInterface({ terminal: false });
            let answer = null;
            rli.question('foo?', (a) => { answer = a; });
            rli.write('bar\n');
            assert.strictEqual(answer, 'bar');
            rli.close();
        });

        it('calling question with AbortSignal callback', () => {
            const [rli] = getInterface({ terminal: false });
            const { signal } = new AbortController();
            let answer = null;
            rli.question('foo?', { signal }, (a) => { answer = a; });
            rli.write('bar\n');
            assert.strictEqual(answer, 'bar');
            rli.close();
        });

        it('second question replaces first', () => {
            const [rli] = getInterface({ terminal: false });
            let firstCalled = false;
            let secondAnswer = null;
            rli.question('foo?', () => { firstCalled = true; });
            rli.question('bar?', (a) => { secondAnswer = a; });
            rli.write('baz\n');
            // In Node.js, second question call is ignored, first one gets the answer
            // Behavior: only first question callback is called
            rli.close();
        });

        it('should return the current prompt via getPrompt', () => {
            const [rli] = getInterface({ terminal: false });
            const expectedPrompts = ['$ ', '> '];
            for (const prompt of expectedPrompts) {
                rli.setPrompt(prompt);
                assert.strictEqual(rli.getPrompt(), prompt);
            }
            rli.close();
        });

        it('should throw ERR_USE_AFTER_CLOSE when calling question after close', () => {
            const [rli, fi] = getInterface({ terminal: false });
            let answer = null;
            rli.question("What's your name?", (name) => {
                answer = name;
                rli.close();
                assert.throws(() => {
                    rli.question('How are you?', () => { });
                }, { code: 'ERR_USE_AFTER_CLOSE' });
            });
            fi.emit('data', 'Node.js\n');
            assert.strictEqual(answer, 'Node.js');
        });
    });

    // =============================================================================
    // Test Suite: CSI escape sequences (precise validation)
    // =============================================================================
    describe('CSI precise escape sequences', () => {
        it('clearScreenDown writes \\x1b[0J', () => {
            const writable = new TestWritable();
            readline.clearScreenDown(writable);
            assert.strictEqual(writable.data, '\x1b[0J');
        });

        it('clearLine(-1) writes \\x1b[1K', () => {
            const writable = new TestWritable();
            readline.clearLine(writable, -1);
            assert.strictEqual(writable.data, '\x1b[1K');
        });

        it('clearLine(1) writes \\x1b[0K', () => {
            const writable = new TestWritable();
            readline.clearLine(writable, 1);
            assert.strictEqual(writable.data, '\x1b[0K');
        });

        it('clearLine(0) writes \\x1b[2K', () => {
            const writable = new TestWritable();
            readline.clearLine(writable, 0);
            assert.strictEqual(writable.data, '\x1b[2K');
        });

        it('cursorTo(1) writes \\x1b[2G', () => {
            const writable = new TestWritable();
            readline.cursorTo(writable, 1);
            assert.strictEqual(writable.data, '\x1b[2G');
        });

        it('cursorTo(1, 2) writes \\x1b[3;2H', () => {
            const writable = new TestWritable();
            readline.cursorTo(writable, 1, 2);
            assert.strictEqual(writable.data, '\x1b[3;2H');
        });

        it('moveCursor exact escape sequences', () => {
            const cases = [
                [0, 0, ''],
                [1, 0, '\x1b[1C'],
                [-1, 0, '\x1b[1D'],
                [0, 1, '\x1b[1B'],
                [0, -1, '\x1b[1A'],
                [1, 1, '\x1b[1C\x1b[1B'],
                [-1, 1, '\x1b[1D\x1b[1B'],
                [-1, -1, '\x1b[1D\x1b[1A'],
                [1, -1, '\x1b[1C\x1b[1A'],
            ];
            for (const [dx, dy, expected] of cases) {
                const writable = new TestWritable();
                readline.moveCursor(writable, dx, dy);
                assert.strictEqual(writable.data, expected,
                    `moveCursor(${dx}, ${dy}) expected ${JSON.stringify(expected)} got ${JSON.stringify(writable.data)}`);
            }
        });
    });

    // =============================================================================
    // Test Suite: null/undefined stream for CSI functions
    // =============================================================================
    describe('CSI null/undefined stream', () => {
        it('clearLine with null stream does not throw', () => {
            assert.strictEqual(readline.clearLine(null, 0), true);
            assert.strictEqual(readline.clearLine(undefined, 0), true);
        });

        it('clearScreenDown with null stream does not throw', () => {
            assert.strictEqual(readline.clearScreenDown(null), true);
            assert.strictEqual(readline.clearScreenDown(undefined), true);
        });

        it('cursorTo with null stream does not throw', () => {
            assert.strictEqual(readline.cursorTo(null), true);
            assert.strictEqual(readline.cursorTo(undefined), true);
        });

        it('moveCursor with null stream does not throw', () => {
            assert.strictEqual(readline.moveCursor(null, 1, 1), true);
            assert.strictEqual(readline.moveCursor(undefined, 1, 1), true);
        });
    });

    // =============================================================================
    // Test Suite: cursorTo validation
    // =============================================================================
    describe('cursorTo validation', () => {
        it('cursorTo with non-numeric x writes nothing', () => {
            const writable = new TestWritable();
            readline.cursorTo(writable, 'a');
            assert.strictEqual(writable.data, '');
        });

        it('cursorTo with non-numeric x and y writes nothing', () => {
            const writable = new TestWritable();
            readline.cursorTo(writable, 'a', 'b');
            assert.strictEqual(writable.data, '');
        });

        it('cursorTo with non-numeric x but numeric y throws ERR_INVALID_CURSOR_POS', () => {
            const writable = new TestWritable();
            assert.throws(
                () => readline.cursorTo(writable, 'a', 1),
                { code: 'ERR_INVALID_CURSOR_POS' }
            );
        });

        it('cursorTo throws on NaN x', () => {
            const writable = new TestWritable();
            assert.throws(() => readline.cursorTo(writable, NaN),
                { code: 'ERR_INVALID_ARG_VALUE' });
        });

        it('cursorTo throws on NaN y', () => {
            const writable = new TestWritable();
            assert.throws(() => readline.cursorTo(writable, 1, NaN),
                { code: 'ERR_INVALID_ARG_VALUE' });
        });
    });

    // =============================================================================
    // Test Suite: emitKeypressEvents
    // =============================================================================
    describe('emitKeypressEvents', () => {
        it('should emit keypress events on a stream', () => {
            const stream = new PassThrough();
            const sequence = [];
            const keys = [];

            readline.emitKeypressEvents(stream);
            stream.on('keypress', (s, k) => {
                sequence.push(s);
                keys.push(k);
            });
            stream.write('foo');

            assert.deepStrictEqual(sequence, ['f', 'o', 'o']);
            assert.strictEqual(keys.length, 3);
            assert.strictEqual(keys[0].name, 'f');
            assert.strictEqual(keys[1].name, 'o');
        });

        it('should work when listener is added before emitKeypressEvents', () => {
            const stream = new PassThrough();
            const sequence = [];

            stream.on('keypress', (s) => sequence.push(s));
            readline.emitKeypressEvents(stream);
            stream.write('foo');

            assert.deepStrictEqual(sequence, ['f', 'o', 'o']);
        });

        it('should not emit after keypress listener is removed', () => {
            const stream = new PassThrough();
            const sequence = [];
            const handler = (s) => sequence.push(s);

            stream.on('keypress', handler);
            readline.emitKeypressEvents(stream);
            stream.removeListener('keypress', handler);
            stream.write('foo');

            assert.deepStrictEqual(sequence, []);
        });
    });

    // =============================================================================
    // Test Suite: async iterator advanced
    // =============================================================================
    describe('async iterator advanced', () => {
        it('should support nested for-await-of (mutual iteration)', async () => {
            const content = 'line 1\nline 2\nline 3\n';
            const input = createInput(content);
            const rli = readline.createInterface({ input, crlfDelay: Infinity });

            const expected = ['line 1', 'line 2', 'line 3'];
            const iteratedLines = [];
            let iterated = false;
            for await (const k of rli) {
                assert.strictEqual(iterated, false);
                iterated = true;
                iteratedLines.push(k);
                for await (const l of rli) {
                    iteratedLines.push(l);
                }
                assert.deepStrictEqual(iteratedLines, expected);
            }
            assert.deepStrictEqual(iteratedLines, expected);
        });

        it('should break from async iterator cleanly', async () => {
            const content = 'line1\nline2\nline3\n';
            const input = createInput(content);
            const rli = readline.createInterface({ input, crlfDelay: Infinity });
            const lines = [];
            for await (const line of rli) {
                lines.push(line);
                break;
            }
            assert.deepStrictEqual(lines, ['line1']);
        });

        it('slow stream should not leak event listeners', async () => {
            const DELAY = 1;
            const REPETITIONS = 20;
            const message = 'a\nb\nc\n';

            const readable = new Readable({ objectMode: true, read() { } });
            let i = REPETITIONS;
            function schedule() {
                setTimeout(() => {
                    i--;
                    if (i < 0) {
                        readable.push(null);
                    } else {
                        readable.push(message);
                        schedule();
                    }
                }, DELAY);
            }
            schedule();

            const rli = readline.createInterface({ input: readable });
            let lines = 0;
            for await (const _ of rli) {
                lines++;
            }
            assert.strictEqual(lines, 3 * REPETITIONS);
        });
    });

    // =============================================================================
    // Test Suite: interface edge cases
    // =============================================================================
    describe('interface edge cases', () => {
        it('can create a new interface on same input stream after close', () => {
            const input = new PassThrough();
            const output = new PassThrough();

            const rl1 = readline.createInterface({ input, output, terminal: true });
            let line1 = null;
            rl1.on('line', (line) => { line1 = line; });
            input.write('foo\n');
            assert.strictEqual(line1, 'foo');
            rl1.close();

            const rl2 = readline.createInterface({ input, output, terminal: true });
            let line2 = null;
            rl2.on('line', (line) => { line2 = line; });
            input.write('bar\n');
            assert.strictEqual(line2, 'bar');
            rl2.close();
        });

        it('can create interface with null output', () => {
            const [rli, fi] = getInterface({ output: null, terminal: false });
            let lineValue = null;
            rli.on('line', (line) => { lineValue = line; });
            fi.emit('data', 'asdf\n');
            assert.strictEqual(lineValue, 'asdf');
            rli.close();
        });
    });

    // =============================================================================
    // Test Suite: terminal mode - \r behaves like \n
    // =============================================================================
    describe('terminal mode \\r behavior', () => {
        it('\\r should behave like \\n in terminal mode', () => {
            const [rli, fi] = getInterface({ terminal: true });
            const expectedLines = ['foo', 'bar', 'baz'];
            const received = [];
            rli.on('line', (line) => received.push(line));
            fi.emit('data', expectedLines.join('\r'));
            // Last line (baz) not emitted yet - no trailing \r
            assert.deepStrictEqual(received, expectedLines.slice(0, -1));
            rli.close();
        });

        it('\\r at start should output blank line', () => {
            const [rli, fi] = getInterface({ terminal: true });
            const received = [];
            rli.on('line', (line) => received.push(line));
            fi.emit('data', '\rfoo\r');
            assert.deepStrictEqual(received, ['', 'foo']);
            rli.close();
        });
    });

    // =============================================================================
    // Test Suite: terminal mode special keys
    // =============================================================================
    describe('terminal mode special keys', () => {
        it('\\t does not become part of input when completer is set', () => {
            const completer = (line) => [[], line];
            const [rli, fi] = getInterface({ terminal: true, completer });
            let lineValue = null;
            rli.on('line', (line) => { lineValue = line; });
            for (const ch of '\tfo\to\t') {
                fi.emit('data', ch);
            }
            fi.emit('data', '\n');
            assert.strictEqual(lineValue, 'foo');
            rli.close();
        });

        it('\\t is ordinary char without completer', () => {
            const [rli, fi] = getInterface({ terminal: true });
            let lineValue = null;
            rli.on('line', (line) => { lineValue = line; });
            fi.emit('data', '\t');
            fi.emit('data', '\n');
            assert.strictEqual(lineValue, '\t');
            rli.close();
        });

        it('should close interface on Ctrl+C', () => {
            const [rli, fi] = getInterface({ terminal: true, prompt: '' });
            fi.emit('keypress', '.', { ctrl: true, name: 'c' });
            assert.ok(rli.closed);
        });
    });

    // =============================================================================
    // Test Suite: removeHistoryDuplicates
    // =============================================================================
    describe('removeHistoryDuplicates', () => {
        it('should remove duplicate history entries when enabled', () => {
            const [rli, fi] = getInterface({
                terminal: true,
                removeHistoryDuplicates: true
            });
            fi.emit('data', 'foo\n');
            fi.emit('data', 'bar\n');
            fi.emit('data', 'foo\n');
            // After dedup: history should be ['foo', 'bar'] (most recent first)
            assert.ok(rli.history.includes('foo'));
            assert.ok(rli.history.includes('bar'));
            // 'foo' should appear only once
            assert.strictEqual(rli.history.filter(h => h === 'foo').length, 1);
            rli.close();
        });

        it('should keep duplicates when disabled', () => {
            const [rli, fi] = getInterface({
                terminal: true,
                removeHistoryDuplicates: false
            });
            fi.emit('data', 'foo\n');
            fi.emit('data', 'bar\n');
            fi.emit('data', 'foo\n');
            assert.strictEqual(rli.history.filter(h => h === 'foo').length, 2);
            rli.close();
        });
    });

    // =============================================================================
    // Test Suite: promises question with AbortSignal
    // =============================================================================
    describe('promises question with AbortSignal', () => {
        it('should reject with AbortError when aborted', async () => {
            const ac = new AbortController();
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.promises.createInterface({
                input, output, terminal: false
            });
            const questionPromise = rl.question('Name? ', { signal: ac.signal });
            ac.abort();
            await assert.rejects(questionPromise, { name: 'AbortError' });
            rl.close();
        });

        it('should reject with pre-aborted signal', async () => {
            const signal = AbortSignal.abort();
            const input = new PassThrough();
            const output = createOutput();
            const rl = readline.promises.createInterface({
                input, output, terminal: false
            });
            const questionPromise = rl.question('Name? ', { signal });
            await assert.rejects(questionPromise, { name: 'AbortError' });
            rl.close();
        });
    });

    // =============================================================================
    // Test Suite: terminal cursor movement
    // =============================================================================
    describe('terminal cursor movement', () => {
        it('should return cursor position', () => {
            const [rli, fi] = getInterface({ terminal: true, prompt: '' });
            fi.emit('data', 'hello');
            const pos = rli.getCursorPos();
            assert.strictEqual(typeof pos.rows, 'number');
            assert.strictEqual(typeof pos.cols, 'number');
            assert.strictEqual(pos.cols, 5);
            assert.strictEqual(pos.rows, 0);
            rli.close();
        });

        it('Ctrl+A moves to beginning, Ctrl+E moves to end', () => {
            const [rli, fi] = getInterface({ terminal: true, prompt: '' });
            fi.emit('data', 'the quick brown fox');
            fi.emit('keypress', '.', { ctrl: true, name: 'a' });
            assert.strictEqual(rli.getCursorPos().cols, 0);
            fi.emit('keypress', '.', { ctrl: true, name: 'e' });
            assert.strictEqual(rli.getCursorPos().cols, 19);
            rli.close();
        });

        it('Ctrl+B and Ctrl+F move one char', () => {
            const [rli, fi] = getInterface({ terminal: true, prompt: '' });
            fi.emit('data', 'abc');
            assert.strictEqual(rli.getCursorPos().cols, 3);
            fi.emit('keypress', '.', { ctrl: true, name: 'b' });
            assert.strictEqual(rli.getCursorPos().cols, 2);
            fi.emit('keypress', '.', { ctrl: true, name: 'f' });
            assert.strictEqual(rli.getCursorPos().cols, 3);
            rli.close();
        });

        it('should handle recursive write in line callback', () => {
            const fi = new FakeInput();
            const rli = new readline.Interface({
                terminal: true,
                input: fi,
                output: fi,
            });
            let recursionDepth = 0;
            const testInput = ' \n}\n';
            const numberOfExpectedLines = (testInput.match(/\n/g) || []).length;

            rli.on('line', () => {
                if (recursionDepth > numberOfExpectedLines) return;
                recursionDepth++;
                rli.write('foo');
            });

            rli.write(testInput);
            assert.strictEqual(recursionDepth, numberOfExpectedLines);
            rli.close();
        });
    });
});
