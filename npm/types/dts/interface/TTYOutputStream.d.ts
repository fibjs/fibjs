/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description The writable side of a terminal: window size, resize events and ANSI cursor control
 *
 *  TTYOutputStream wraps a terminal descriptor. In a process attached to a terminal, process.stdout
 *  and process.stderr are instances of this class, and any terminal descriptor or FileHandle can be
 *  wrapped explicitly with `new tty.WriteStream(fd[, opts])`. The class extends Stream, so write,
 *  flush, close and the event interface behave as documented there; the output side adds the terminal
 *  dimensions (columns, rows, getWindowSize), the control members (clearLine, clearScreenDown,
 *  cursorTo, moveCursor) and the `'resize'` event.
 *
 *  Concepts:
 *
 *  - **Window size and resize**: the terminal has a number of columns and rows, read through columns,
 *    rows or getWindowSize; the values are re-read from the device on every access. When the user
 *    resizes the window the process receives SIGWINCH and the stream emits `'resize'` with no
 *    arguments, after which the properties report the new size.
 *  - **Control sequences**: clearLine, clearScreenDown, cursorTo and moveCursor write ANSI CSI
 *    sequences to the terminal, which interprets them instead of displaying them. fibjs emits the
 *    same sequences as Node.js; the members return nothing and write straight to the terminal
 *    device, bypassing the JavaScript write override and the stream back pressure.
 *  - **Positions**: columns and the x arguments are 0-based; cursorTo(x) moves to column x of the
 *    current row, cursorTo(x, y) moves to the 0-based row y, and moveCursor(dx, dy) is relative.
 *    Each non-zero component is written as its own sequence.
 *  - **Color depth**: Node.js hasColors/getColorDepth are not implemented; use util.colors.hasColors
 *    to test whether the terminal supports color (see the colors module).
 *
 *  Obtained from:
 *  - `process.stdout` and `process.stderr` — when the matching standard stream is a terminal;
 *  - `new tty.WriteStream(fd[, opts])` — wrap any terminal descriptor or FileHandle;
 *  - the standard output of a child started with `stdio: 'pty'`.
 *
 *  Example 1 — the window size of a pseudo terminal:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *  const io = require('io');
 *
 *  // The pty is created with the size given by the cols/rows options (80x24 by
 *  // default); the child reads it from its own terminal stream, while the
 *  // parent side is a plain Stream without the terminal members.
 *  const code = 'process.stdout.write(process.stdout.getWindowSize().join("x") + "\\n");';
 *  const bs = child_process.spawn(process.execPath, ['-e', code],
 *      { stdio: 'pty', cols: 100, rows: 40 });
 *
 *  console.log(new io.BufferedStream(bs.stdout).readLine()); // 100x40
 *  console.log(bs.stdout.isTTY); // undefined
 *  bs.join();
 *  ```
 *
 *  Example 2 — escape sequences produced by the control members:
 *  ```JavaScript
 *  const child_process = require('child_process');
 *  const io = require('io');
 *
 *  // The child clears the line, writes x, moves the cursor to column 5 and
 *  // writes y; the parent captures the raw bytes that travel through the pty.
 *  const code = [
 *      'process.stdout.clearLine(0);',
 *      'process.stdout.write("x");',
 *      'process.stdout.cursorTo(4);',
 *      'process.stdout.write("y");',
 *      'process.stdout.write("\\n");'
 *  ].join('');
 *
 *  const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
 *  const line = new io.BufferedStream(bs.stdout).readLine();
 *  console.log(JSON.stringify(line)); // "\u001b[2Kx\u001b[5Gy"
 *  console.log(line.includes('\x1b[2K')); // true
 *  bs.join();
 *  ```
 *
 *  Example 3 — overwrite a status line in place:
 *  ```JavaScript
 *  // On a terminal the same line is overwritten; when the output is piped the
 *  // branches print one step per line instead.
 *  if (process.stdout.isTTY) {
 *      process.stdout.clearLine(0);
 *      process.stdout.cursorTo(0);
 *      process.stdout.write('step 1/2');
 *      process.stdout.clearLine(0);
 *      process.stdout.cursorTo(0);
 *      process.stdout.write('step 2/2\n');
 *  } else {
 *      console.log('step 1/2');
 *      console.log('step 2/2');
 *  }
 *  ```
 *
 */
declare class Class_TTYOutputStream extends Class_Stream {
    /**
     * @description Creates a TTYOutputStream wrapping a terminal descriptor
     *
     *      The descriptor must already be a terminal: any other descriptor (a pipe, a file, a closed
     *      FileHandle) throws `TypeError: fd N is not a TTY.` ([20004]), and a negative descriptor
     *      throws [20009]. A FileHandle, a numeric string and a fractional number are accepted (the
     *      last two are coerced to an integer descriptor), and the direction of the descriptor is not
     *      checked, so a terminal descriptor opened for input can be wrapped as well.
     *
     *      opts is accepted for interface compatibility and ignored: it is not forwarded to the Stream
     *      base. Node.js has no options parameter on tty.WriteStream at all.
     *
     *      Example — construct a second write stream for the terminal of a pty child:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'const tty = require("tty");',
     *          'const out = new tty.WriteStream(1);',
     *          'process.stdout.write("isTTY: " + out.isTTY + "\\n");',
     *          'out.clearLine(0);',
     *          'process.stdout.write("cleared\\n");'
     *      ].join('\n');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      const reader = new io.BufferedStream(bs.stdout);
     *      console.log(reader.readLine()); // isTTY: true
     *      console.log(JSON.stringify(reader.readLine())); // "\u001b[2Kcleared"
     *      bs.join();
     *      ```
     *
     *      @param fd the file descriptor; an integer descriptor or a FileHandle object
     *      @param opts options object, accepted for compatibility and ignored
     *
     */
    constructor(fd: number | Class_FileHandle | Class_FileHandlePromise, opts?: FIBJS.GeneralObject);

    /**
     * @description Always true: a TTYOutputStream is only created for a terminal descriptor
     *
     *      The property is fixed to true by the class; the real check happens when the stream is created.
     *      When stdout is a pipe or a file, process.stdout is a plain Stream instead and its isTTY is
     *      undefined. Use tty.isatty(fd) to test an arbitrary descriptor.
     *
     */
    readonly isTTY: boolean;

    /**
     * @description The current number of columns of the terminal
     *
     *      The value is read from the terminal device on every access, so it follows a resize; it is the
     *      first element of getWindowSize and matches Node.js. A pty created with `stdio: 'pty'` starts
     *      at the cols option of child_process (80 by default).
     *
     */
    readonly columns: number;

    /**
     * @description The current number of rows of the terminal
     *
     *      The value is read from the terminal device on every access, so it follows a resize; it is the
     *      second element of getWindowSize and matches Node.js. A pty created with `stdio: 'pty'` starts
     *      at the rows option of child_process (24 by default).
     *
     */
    readonly rows: number;

    /**
     * @description Clears the current line in the direction given by dir
     *
     *      dir selects the part of the line to clear: -1 clears from the cursor to the beginning of the
     *      line, 0 the whole line and 1 from the cursor to the end of the line (the default is 0). Each
     *      direction writes the corresponding CSI sequence: `\x1b[1K`, `\x1b[2K` or `\x1b[0K`. Any other
     *      value throws [20004] ("clearLine: invalid direction"), where Node.js writes `\x1b[0K` instead.
     *
     *      Example — the three directions produce three different sequences:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.clearLine(-1);',
     *          'process.stdout.clearLine(0);',
     *          'process.stdout.clearLine(1);',
     *          'process.stdout.write("end\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      const line = new io.BufferedStream(bs.stdout).readLine();
     *      console.log(JSON.stringify(line)); // "\u001b[1K\u001b[2K\u001b[0Kend"
     *      bs.join();
     *      ```
     *
     *      @param dir clearing direction: -1, 0 (default) or 1
     *
     */
    clearLine(dir?: number): void;

    /**
     * @description Clears the terminal from the cursor down to the end of the screen
     *
     *      Writes the CSI sequence `\x1b[0J`; the cursor itself does not move. Node.js emits the same
     *      sequence from its synchronous form.
     *
     */
    clearScreenDown(): void;

    /**
     * @description Moves the cursor to an absolute position
     *
     *      x is the 0-based column to move to. When y is omitted or negative the row is left unchanged
     *      and the member writes `\x1b[(x+1)G`; otherwise y is the 0-based row and the member writes
     *      `\x1b[(y+1);(x+1)H`. A negative x throws [20004] ("cursorTo: x must be non-negative") where
     *      Node.js writes `\x1b[0G`, and a negative y means "do not change the row" where Node.js emits
     *      a negative row number.
     *
     *      The member is declared async, so it also has a callback form and the cursorToSync/cursorToAsync
     *      aliases; all of them write the sequence when they run. Node.js emits the same bytes from its
     *      synchronous form.
     *
     *      Example — absolute row/column addressing:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.cursorTo(5);',
     *          'process.stdout.cursorTo(5, 3);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[6G\u001b[4;6H"
     *      bs.join();
     *      ```
     *
     *      @param x the column number, 0-based
     *      @param y the row number, 0-based; omitted or negative leaves the row unchanged
     *
     */
    cursorTo(x: number, y?: number): void;

    cursorTo(x: number, y?: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Moves the cursor to an absolute position
     *
     *      x is the 0-based column to move to. When y is omitted or negative the row is left unchanged
     *      and the member writes `\x1b[(x+1)G`; otherwise y is the 0-based row and the member writes
     *      `\x1b[(y+1);(x+1)H`. A negative x throws [20004] ("cursorTo: x must be non-negative") where
     *      Node.js writes `\x1b[0G`, and a negative y means "do not change the row" where Node.js emits
     *      a negative row number.
     *
     *      The member is declared async, so it also has a callback form and the cursorToSync/cursorToAsync
     *      aliases; all of them write the sequence when they run. Node.js emits the same bytes from its
     *      synchronous form.
     *
     *      Example — absolute row/column addressing:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.cursorTo(5);',
     *          'process.stdout.cursorTo(5, 3);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[6G\u001b[4;6H"
     *      bs.join();
     *      ```
     *
     *      @param x the column number, 0-based
     *      @param y the row number, 0-based; omitted or negative leaves the row unchanged
     *
     */
    cursorToSync(x: number, y?: number): void;

    /**
     * @description Moves the cursor to an absolute position
     *
     *      x is the 0-based column to move to. When y is omitted or negative the row is left unchanged
     *      and the member writes `\x1b[(x+1)G`; otherwise y is the 0-based row and the member writes
     *      `\x1b[(y+1);(x+1)H`. A negative x throws [20004] ("cursorTo: x must be non-negative") where
     *      Node.js writes `\x1b[0G`, and a negative y means "do not change the row" where Node.js emits
     *      a negative row number.
     *
     *      The member is declared async, so it also has a callback form and the cursorToSync/cursorToAsync
     *      aliases; all of them write the sequence when they run. Node.js emits the same bytes from its
     *      synchronous form.
     *
     *      Example — absolute row/column addressing:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.cursorTo(5);',
     *          'process.stdout.cursorTo(5, 3);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[6G\u001b[4;6H"
     *      bs.join();
     *      ```
     *
     *      @param x the column number, 0-based
     *      @param y the row number, 0-based; omitted or negative leaves the row unchanged
     *
     */
    cursorToAsync(x: number, y?: number): Promise<void>;

    /**
     * @description Moves the cursor by a relative offset
     *
     *      Each non-zero component is written as its own CSI sequence: dx > 0 emits `\x1b[dxC` (right)
     *      and dx < 0 emits `\x1b[|dx|D` (left); dy > 0 emits `\x1b[dyB` (down) and dy < 0 emits
     *      `\x1b[|dy|A` (up). moveCursor(0, 0) writes nothing. Node.js emits the same sequences from its
     *      synchronous form; the explicit moveCursorSync/moveCursorAsync aliases are fibjs extensions.
     *
     *      Example — relative moves in all four directions:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.moveCursor(1, 1);',
     *          'process.stdout.moveCursor(-3, 2);',
     *          'process.stdout.moveCursor(0, -4);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[1C\u001b[1B\u001b[3D\u001b[2B\u001b[4A"
     *      bs.join();
     *      ```
     *
     *      @param dx the column offset; positive moves right, negative moves left
     *      @param dy the row offset; positive moves down, negative moves up
     *
     */
    moveCursor(dx: number, dy: number): void;

    moveCursor(dx: number, dy: number, callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Moves the cursor by a relative offset
     *
     *      Each non-zero component is written as its own CSI sequence: dx > 0 emits `\x1b[dxC` (right)
     *      and dx < 0 emits `\x1b[|dx|D` (left); dy > 0 emits `\x1b[dyB` (down) and dy < 0 emits
     *      `\x1b[|dy|A` (up). moveCursor(0, 0) writes nothing. Node.js emits the same sequences from its
     *      synchronous form; the explicit moveCursorSync/moveCursorAsync aliases are fibjs extensions.
     *
     *      Example — relative moves in all four directions:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.moveCursor(1, 1);',
     *          'process.stdout.moveCursor(-3, 2);',
     *          'process.stdout.moveCursor(0, -4);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[1C\u001b[1B\u001b[3D\u001b[2B\u001b[4A"
     *      bs.join();
     *      ```
     *
     *      @param dx the column offset; positive moves right, negative moves left
     *      @param dy the row offset; positive moves down, negative moves up
     *
     */
    moveCursorSync(dx: number, dy: number): void;

    /**
     * @description Moves the cursor by a relative offset
     *
     *      Each non-zero component is written as its own CSI sequence: dx > 0 emits `\x1b[dxC` (right)
     *      and dx < 0 emits `\x1b[|dx|D` (left); dy > 0 emits `\x1b[dyB` (down) and dy < 0 emits
     *      `\x1b[|dy|A` (up). moveCursor(0, 0) writes nothing. Node.js emits the same sequences from its
     *      synchronous form; the explicit moveCursorSync/moveCursorAsync aliases are fibjs extensions.
     *
     *      Example — relative moves in all four directions:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.moveCursor(1, 1);',
     *          'process.stdout.moveCursor(-3, 2);',
     *          'process.stdout.moveCursor(0, -4);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[1C\u001b[1B\u001b[3D\u001b[2B\u001b[4A"
     *      bs.join();
     *      ```
     *
     *      @param dx the column offset; positive moves right, negative moves left
     *      @param dy the row offset; positive moves down, negative moves up
     *
     */
    moveCursorAsync(dx: number, dy: number): Promise<void>;

    /**
     * @description Returns the size of the terminal as [numColumns, numRows]
     *
     *      The pair is read from the terminal device on every call, so it follows a resize; it is
     *      equivalent to reading columns and rows and has the same Number[] shape as Node.js (the members
     *      are numbers, not integers).
     *
     *      @return returns the array [numColumns, numRows] of the terminal
     *
     */
    getWindowSize(): number[];

    /**
     * @description Emitted when the terminal size changes
     *
     *      The event carries no arguments; read columns, rows or getWindowSize inside the listener to get
     *      the new size. It is emitted when the process receives SIGWINCH, which the terminal sends when
     *      its size changes (on a pseudo terminal, when stty or an ioctl changes the size). Node.js emits
     *      the same event under the same name.
     *
     *      Example — watch the size of a pty child while stty resizes it (Linux only):
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      if (process.platform !== 'linux') {
     *          console.log('the resize demo uses stty and runs on Linux only');
     *      } else {
     *          const code = [
     *              'const child_process = require("child_process");',
     *              'process.stdout.on("resize", () => {',
     *              '    process.stdout.write("resized: " +',
     *              '        process.stdout.getWindowSize().join("x") + "\\n");',
     *              '});',
     *              'process.stdout.write("before: " + process.stdout.getWindowSize().join("x") + "\\n");',
     *              'child_process.exec("stty cols 100 rows 40 < /dev/tty");',
     *              'setTimeout(() => process.exit(0), 500);'
     *          ].join('\n');
     *
     *          const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *          const reader = new io.BufferedStream(bs.stdout);
     *          console.log(reader.readLine()); // before: 80x24
     *          console.log(reader.readLine()); // resized: 100x40
     *          bs.join();
     *      }
     *      ```
     *
     */
    on(event: "resize", listener: ()=>void): this;

    once(event: "resize", listener: ()=>void): this;

    off(event: "resize", listener: ()=>void): this;

    addListener(event: "resize", listener: ()=>void): this;

    removeListener(event: "resize", listener: ()=>void): this;

    addEventListener(event: "resize", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "resize", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "resize", listener: ()=>void): this;

    prependOnceListener(event: "resize", listener: ()=>void): this;

    /**
     * @description Emitted when the terminal size changes
     *
     *      The event carries no arguments; read columns, rows or getWindowSize inside the listener to get
     *      the new size. It is emitted when the process receives SIGWINCH, which the terminal sends when
     *      its size changes (on a pseudo terminal, when stty or an ioctl changes the size). Node.js emits
     *      the same event under the same name.
     *
     *      Example — watch the size of a pty child while stty resizes it (Linux only):
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      if (process.platform !== 'linux') {
     *          console.log('the resize demo uses stty and runs on Linux only');
     *      } else {
     *          const code = [
     *              'const child_process = require("child_process");',
     *              'process.stdout.on("resize", () => {',
     *              '    process.stdout.write("resized: " +',
     *              '        process.stdout.getWindowSize().join("x") + "\\n");',
     *              '});',
     *              'process.stdout.write("before: " + process.stdout.getWindowSize().join("x") + "\\n");',
     *              'child_process.exec("stty cols 100 rows 40 < /dev/tty");',
     *              'setTimeout(() => process.exit(0), 500);'
     *          ].join('\n');
     *
     *          const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *          const reader = new io.BufferedStream(bs.stdout);
     *          console.log(reader.readLine()); // before: 80x24
     *          console.log(reader.readLine()); // resized: 100x40
     *          bs.join();
     *      }
     *      ```
     *
     */
    onresize: (()=>void) | null;

    on(event: "data", listener: (data: Class_Buffer)=>void): this;

    on(event: "close", listener: ()=>void): this;

    on(event: "error", listener: (code: number)=>void): this;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(event: "data", listener: (data: Class_Buffer)=>void): this;

    once(event: "close", listener: ()=>void): this;

    once(event: "error", listener: (code: number)=>void): this;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(event: "data", listener: (data: Class_Buffer)=>void): this;

    off(event: "close", listener: ()=>void): this;

    off(event: "error", listener: (code: number)=>void): this;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    addListener(event: "error", listener: (code: number)=>void): this;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    removeListener(event: "error", listener: (code: number)=>void): this;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(event: "data", listener: (data: Class_Buffer)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "error", listener: (code: number)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependListener(event: "error", listener: (code: number)=>void): this;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(event: "data", listener: (data: Class_Buffer)=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "error", listener: (code: number)=>void): this;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}


/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/Stream.d.ts" />
/// <reference path="../interface/FileHandle.d.ts" />
/**
 * The promise variant of the TTYOutputStream class: async methods return a Promise as their primary form, and no callback overload is bound (the promise prototype rejects a callback with 20001).
 */
declare class Class_TTYOutputStreamPromise extends Class_StreamPromise {
    /**
     * @description Creates a TTYOutputStream wrapping a terminal descriptor
     *
     *      The descriptor must already be a terminal: any other descriptor (a pipe, a file, a closed
     *      FileHandle) throws `TypeError: fd N is not a TTY.` ([20004]), and a negative descriptor
     *      throws [20009]. A FileHandle, a numeric string and a fractional number are accepted (the
     *      last two are coerced to an integer descriptor), and the direction of the descriptor is not
     *      checked, so a terminal descriptor opened for input can be wrapped as well.
     *
     *      opts is accepted for interface compatibility and ignored: it is not forwarded to the Stream
     *      base. Node.js has no options parameter on tty.WriteStream at all.
     *
     *      Example — construct a second write stream for the terminal of a pty child:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'const tty = require("tty");',
     *          'const out = new tty.WriteStream(1);',
     *          'process.stdout.write("isTTY: " + out.isTTY + "\\n");',
     *          'out.clearLine(0);',
     *          'process.stdout.write("cleared\\n");'
     *      ].join('\n');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      const reader = new io.BufferedStream(bs.stdout);
     *      console.log(reader.readLine()); // isTTY: true
     *      console.log(JSON.stringify(reader.readLine())); // "\u001b[2Kcleared"
     *      bs.join();
     *      ```
     *
     *      @param fd the file descriptor; an integer descriptor or a FileHandle object
     *      @param opts options object, accepted for compatibility and ignored
     *
     */
    constructor(fd: number | Class_FileHandle | Class_FileHandlePromise, opts?: FIBJS.GeneralObject);

    /**
     * @description Always true: a TTYOutputStream is only created for a terminal descriptor
     *
     *      The property is fixed to true by the class; the real check happens when the stream is created.
     *      When stdout is a pipe or a file, process.stdout is a plain Stream instead and its isTTY is
     *      undefined. Use tty.isatty(fd) to test an arbitrary descriptor.
     *
     */
    readonly isTTY: boolean;

    /**
     * @description The current number of columns of the terminal
     *
     *      The value is read from the terminal device on every access, so it follows a resize; it is the
     *      first element of getWindowSize and matches Node.js. A pty created with `stdio: 'pty'` starts
     *      at the cols option of child_process (80 by default).
     *
     */
    readonly columns: number;

    /**
     * @description The current number of rows of the terminal
     *
     *      The value is read from the terminal device on every access, so it follows a resize; it is the
     *      second element of getWindowSize and matches Node.js. A pty created with `stdio: 'pty'` starts
     *      at the rows option of child_process (24 by default).
     *
     */
    readonly rows: number;

    /**
     * @description Clears the current line in the direction given by dir
     *
     *      dir selects the part of the line to clear: -1 clears from the cursor to the beginning of the
     *      line, 0 the whole line and 1 from the cursor to the end of the line (the default is 0). Each
     *      direction writes the corresponding CSI sequence: `\x1b[1K`, `\x1b[2K` or `\x1b[0K`. Any other
     *      value throws [20004] ("clearLine: invalid direction"), where Node.js writes `\x1b[0K` instead.
     *
     *      Example — the three directions produce three different sequences:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.clearLine(-1);',
     *          'process.stdout.clearLine(0);',
     *          'process.stdout.clearLine(1);',
     *          'process.stdout.write("end\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      const line = new io.BufferedStream(bs.stdout).readLine();
     *      console.log(JSON.stringify(line)); // "\u001b[1K\u001b[2K\u001b[0Kend"
     *      bs.join();
     *      ```
     *
     *      @param dir clearing direction: -1, 0 (default) or 1
     *
     */
    clearLine(dir?: number): void;

    /**
     * @description Clears the terminal from the cursor down to the end of the screen
     *
     *      Writes the CSI sequence `\x1b[0J`; the cursor itself does not move. Node.js emits the same
     *      sequence from its synchronous form.
     *
     */
    clearScreenDown(): void;

    /**
     * @description Moves the cursor to an absolute position
     *
     *      x is the 0-based column to move to. When y is omitted or negative the row is left unchanged
     *      and the member writes `\x1b[(x+1)G`; otherwise y is the 0-based row and the member writes
     *      `\x1b[(y+1);(x+1)H`. A negative x throws [20004] ("cursorTo: x must be non-negative") where
     *      Node.js writes `\x1b[0G`, and a negative y means "do not change the row" where Node.js emits
     *      a negative row number.
     *
     *      The member is declared async, so it also has a callback form and the cursorToSync/cursorToAsync
     *      aliases; all of them write the sequence when they run. Node.js emits the same bytes from its
     *      synchronous form.
     *
     *      Example — absolute row/column addressing:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.cursorTo(5);',
     *          'process.stdout.cursorTo(5, 3);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[6G\u001b[4;6H"
     *      bs.join();
     *      ```
     *
     *      @param x the column number, 0-based
     *      @param y the row number, 0-based; omitted or negative leaves the row unchanged
     *
     */
    cursorTo(x: number, y?: number): Promise<void>;

    /**
     * @description Moves the cursor to an absolute position
     *
     *      x is the 0-based column to move to. When y is omitted or negative the row is left unchanged
     *      and the member writes `\x1b[(x+1)G`; otherwise y is the 0-based row and the member writes
     *      `\x1b[(y+1);(x+1)H`. A negative x throws [20004] ("cursorTo: x must be non-negative") where
     *      Node.js writes `\x1b[0G`, and a negative y means "do not change the row" where Node.js emits
     *      a negative row number.
     *
     *      The member is declared async, so it also has a callback form and the cursorToSync/cursorToAsync
     *      aliases; all of them write the sequence when they run. Node.js emits the same bytes from its
     *      synchronous form.
     *
     *      Example — absolute row/column addressing:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.cursorTo(5);',
     *          'process.stdout.cursorTo(5, 3);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[6G\u001b[4;6H"
     *      bs.join();
     *      ```
     *
     *      @param x the column number, 0-based
     *      @param y the row number, 0-based; omitted or negative leaves the row unchanged
     *
     */
    cursorToSync(x: number, y?: number): void;

    /**
     * @description Moves the cursor to an absolute position
     *
     *      x is the 0-based column to move to. When y is omitted or negative the row is left unchanged
     *      and the member writes `\x1b[(x+1)G`; otherwise y is the 0-based row and the member writes
     *      `\x1b[(y+1);(x+1)H`. A negative x throws [20004] ("cursorTo: x must be non-negative") where
     *      Node.js writes `\x1b[0G`, and a negative y means "do not change the row" where Node.js emits
     *      a negative row number.
     *
     *      The member is declared async, so it also has a callback form and the cursorToSync/cursorToAsync
     *      aliases; all of them write the sequence when they run. Node.js emits the same bytes from its
     *      synchronous form.
     *
     *      Example — absolute row/column addressing:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.cursorTo(5);',
     *          'process.stdout.cursorTo(5, 3);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[6G\u001b[4;6H"
     *      bs.join();
     *      ```
     *
     *      @param x the column number, 0-based
     *      @param y the row number, 0-based; omitted or negative leaves the row unchanged
     *
     */
    cursorToAsync(x: number, y?: number): Promise<void>;

    /**
     * @description Moves the cursor by a relative offset
     *
     *      Each non-zero component is written as its own CSI sequence: dx > 0 emits `\x1b[dxC` (right)
     *      and dx < 0 emits `\x1b[|dx|D` (left); dy > 0 emits `\x1b[dyB` (down) and dy < 0 emits
     *      `\x1b[|dy|A` (up). moveCursor(0, 0) writes nothing. Node.js emits the same sequences from its
     *      synchronous form; the explicit moveCursorSync/moveCursorAsync aliases are fibjs extensions.
     *
     *      Example — relative moves in all four directions:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.moveCursor(1, 1);',
     *          'process.stdout.moveCursor(-3, 2);',
     *          'process.stdout.moveCursor(0, -4);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[1C\u001b[1B\u001b[3D\u001b[2B\u001b[4A"
     *      bs.join();
     *      ```
     *
     *      @param dx the column offset; positive moves right, negative moves left
     *      @param dy the row offset; positive moves down, negative moves up
     *
     */
    moveCursor(dx: number, dy: number): Promise<void>;

    /**
     * @description Moves the cursor by a relative offset
     *
     *      Each non-zero component is written as its own CSI sequence: dx > 0 emits `\x1b[dxC` (right)
     *      and dx < 0 emits `\x1b[|dx|D` (left); dy > 0 emits `\x1b[dyB` (down) and dy < 0 emits
     *      `\x1b[|dy|A` (up). moveCursor(0, 0) writes nothing. Node.js emits the same sequences from its
     *      synchronous form; the explicit moveCursorSync/moveCursorAsync aliases are fibjs extensions.
     *
     *      Example — relative moves in all four directions:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.moveCursor(1, 1);',
     *          'process.stdout.moveCursor(-3, 2);',
     *          'process.stdout.moveCursor(0, -4);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[1C\u001b[1B\u001b[3D\u001b[2B\u001b[4A"
     *      bs.join();
     *      ```
     *
     *      @param dx the column offset; positive moves right, negative moves left
     *      @param dy the row offset; positive moves down, negative moves up
     *
     */
    moveCursorSync(dx: number, dy: number): void;

    /**
     * @description Moves the cursor by a relative offset
     *
     *      Each non-zero component is written as its own CSI sequence: dx > 0 emits `\x1b[dxC` (right)
     *      and dx < 0 emits `\x1b[|dx|D` (left); dy > 0 emits `\x1b[dyB` (down) and dy < 0 emits
     *      `\x1b[|dy|A` (up). moveCursor(0, 0) writes nothing. Node.js emits the same sequences from its
     *      synchronous form; the explicit moveCursorSync/moveCursorAsync aliases are fibjs extensions.
     *
     *      Example — relative moves in all four directions:
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      const code = [
     *          'process.stdout.moveCursor(1, 1);',
     *          'process.stdout.moveCursor(-3, 2);',
     *          'process.stdout.moveCursor(0, -4);',
     *          'process.stdout.write("\\n");'
     *      ].join('');
     *
     *      const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *      console.log(JSON.stringify(new io.BufferedStream(bs.stdout).readLine()));
     *      // "\u001b[1C\u001b[1B\u001b[3D\u001b[2B\u001b[4A"
     *      bs.join();
     *      ```
     *
     *      @param dx the column offset; positive moves right, negative moves left
     *      @param dy the row offset; positive moves down, negative moves up
     *
     */
    moveCursorAsync(dx: number, dy: number): Promise<void>;

    /**
     * @description Returns the size of the terminal as [numColumns, numRows]
     *
     *      The pair is read from the terminal device on every call, so it follows a resize; it is
     *      equivalent to reading columns and rows and has the same Number[] shape as Node.js (the members
     *      are numbers, not integers).
     *
     *      @return returns the array [numColumns, numRows] of the terminal
     *
     */
    getWindowSize(): number[];

    /**
     * @description Emitted when the terminal size changes
     *
     *      The event carries no arguments; read columns, rows or getWindowSize inside the listener to get
     *      the new size. It is emitted when the process receives SIGWINCH, which the terminal sends when
     *      its size changes (on a pseudo terminal, when stty or an ioctl changes the size). Node.js emits
     *      the same event under the same name.
     *
     *      Example — watch the size of a pty child while stty resizes it (Linux only):
     *      ```JavaScript
     *      const child_process = require('child_process');
     *      const io = require('io');
     *
     *      if (process.platform !== 'linux') {
     *          console.log('the resize demo uses stty and runs on Linux only');
     *      } else {
     *          const code = [
     *              'const child_process = require("child_process");',
     *              'process.stdout.on("resize", () => {',
     *              '    process.stdout.write("resized: " +',
     *              '        process.stdout.getWindowSize().join("x") + "\\n");',
     *              '});',
     *              'process.stdout.write("before: " + process.stdout.getWindowSize().join("x") + "\\n");',
     *              'child_process.exec("stty cols 100 rows 40 < /dev/tty");',
     *              'setTimeout(() => process.exit(0), 500);'
     *          ].join('\n');
     *
     *          const bs = child_process.spawn(process.execPath, ['-e', code], { stdio: 'pty' });
     *          const reader = new io.BufferedStream(bs.stdout);
     *          console.log(reader.readLine()); // before: 80x24
     *          console.log(reader.readLine()); // resized: 100x40
     *          bs.join();
     *      }
     *      ```
     *
     */
    onresize: (()=>void) | null;

}


declare namespace Class_TTYOutputStream {
    const promises: FIBJS.GeneralObject;
}
