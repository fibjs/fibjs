var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

const io = require('io');
const path = require('path');
const tty = require('tty');
const child_process = require('child_process');

const cmd = process.execPath;

describe('tty', () => {
  it("isatty", () => {
    assert.isTrue(tty.isatty(0));
    assert.isTrue(tty.isatty(1));
    assert.isTrue(tty.isatty(2));
    assert.isFalse(tty.isatty(3));
    assert.throws(() => tty.isatty(-1));
    assert.throws(() => tty.isatty(9999999999999999));
  });

  describe('TTYInputStream: process.stdin', () => {
    after(() => {
      process.stdin.setRawMode(false)
    });

    it("isTTY", () => {
      assert.isTrue(process.stdin.isTTY);
    });

    it("::setRawMode()", () => {
      process.stdin.setRawMode(true)
      process.stdin.setRawMode(false)
    })

    it("'isRaw is true' by default", () => {
      assert.isFalse(process.stdin.isRaw);
    });

    it("setRawMode: `process.stdin.isRaw` is `undefined` in non-tty mode", () => {
      var bs = child_process.spawn(cmd, [path.join(__dirname, 'tty_test', 'raw_mode1.js')]);

      var stdout = new io.BufferedStream(bs.stdout);
      var stderr = new io.BufferedStream(bs.stderr);

      assert.equal(stdout.readLine(), "by default, process.stdin.isRaw is undefined");
      assert.equal(stdout.readLine(), null);

      assert.ok(
        stderr.readLines().find(line => line.includes('TypeError: process.stdin.setRawMode is not a function'))
      );
    });
  });

  describe('TTYOutputStream: process.stdout', () => {
    it("isTTY", () => {
      assert.isTrue(process.stdout.isTTY);
    });

    it("TTYOutputStream::clearLine", () => {
      var bs = child_process.spawn(cmd, [path.join(__dirname, 'tty_test', 'clearLine.js')]);
      var stdout = new io.BufferedStream(bs.stdout);
    });
  });

  describe('TTYOutputStream: process.stderr', () => {
    it("isTTY", () => {
      assert.isTrue(process.stderr.isTTY);
    });
  });

  it("stdin data event in pty mode", () => {
    var bs = child_process.spawn(cmd, [path.join(__dirname, 'tty_test', 'pty_stdin_data_test.js')], {
      stdio: 'pty'
    });

    var stdout = new io.BufferedStream(bs.stdout);

    // Function to strip ANSI and carriage return
    function cleanLine(str) {
      return str.replace(/\x1b\[[0-9;?]*[a-zA-Z]/g, '').replace(/\r$/, '');
    }

    // Wait for process to be ready
    var line = cleanLine(stdout.readLine());
    assert.equal(line, "PTY_STDIN_TEST_START");

    line = cleanLine(stdout.readLine());
    assert.equal(line, "PTY_STDIN_LISTENER_READY");

    // Send input to stdin
    bs.stdin.write("HELLO\n");

    // PTY echoes input first, then our output
    line = cleanLine(stdout.readLine());
    // Skip echo if present
    if (line === "HELLO") {
      line = cleanLine(stdout.readLine());
    }
    assert.equal(line, "RECEIVED:HELLO");

    // Send another input
    bs.stdin.write("WORLD\n");
    line = cleanLine(stdout.readLine());
    if (line === "WORLD") {
      line = cleanLine(stdout.readLine());
    }
    assert.equal(line, "RECEIVED:WORLD");

    // Send exit command
    bs.stdin.write("EXIT\n");
    line = cleanLine(stdout.readLine());
    if (line === "EXIT") {
      line = cleanLine(stdout.readLine());
    }
    assert.equal(line, "RECEIVED:EXIT");

    line = cleanLine(stdout.readLine());
    assert.equal(line, "PTY_STDIN_TEST_EXIT");

    bs.join();
    assert.equal(bs.exitCode, 0);
  });
});


