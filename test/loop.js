const child_process = require('child_process');
const fs = require('fs');
const path = require('path');
const coroutine = require('coroutine');

const logFile = path.join(__dirname, '..', 'temp', 'loop_fail.log');

function runOnce(count) {
    var output = [];
    var ev = new coroutine.Event();
    var exited = false;
    var stdoutClosed = false;
    var stderrClosed = false;
    var child = child_process.spawn(process.execPath, [path.join(__dirname, 'net_test.js')]);

    function checkDone() {
        if (exited && stdoutClosed && stderrClosed)
            ev.set();
    }

    // Kill child if no output received for 30 consecutive seconds
    var idle = 30000;
    var timer = setTimeout(() => {
        child.kill('SIGINT');
    }, idle);

    function resetTimer() {
        clearTimeout(timer);
        timer = setTimeout(() => {
            child.kill('SIGINT');
        }, idle);
    }

    child.stdout.on('data', d => { output.push(d.toString()); resetTimer(); });
    child.stderr.on('data', d => { output.push(d.toString()); resetTimer(); });
    child.stdout.on('close', () => { stdoutClosed = true; checkDone(); });
    child.stderr.on('close', () => { stderrClosed = true; checkDone(); });
    child.on('exit', () => { exited = true; checkDone(); });

    ev.wait();
    clearTimeout(timer);

    return { status: child.exitCode, output: output.join('') };
}

var count = 0;
while (1) {
    count++;
    console.log(`\n========== Round ${count} ==========`);
    const ret = runOnce(count);
    var crashed = ret.output.indexOf('SIGSEGV') >= 0 || ret.output.indexOf('SIGABRT') >= 0;
    if (ret.status != 0 || crashed) {
        const header = `\n===== Round ${count} failed (status=${ret.status}) =====\n`;
        fs.writeFileSync(logFile, header + ret.output);
        console.error(`Round ${count} FAILED, log written to ${logFile}`);
        throw new Error('net_test.js failed with exit code ' + ret.status);
    }
}
