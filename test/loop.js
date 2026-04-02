const child_process = require('child_process');
const fs = require('fs');
const path = require('path');
const coroutine = require('coroutine');

const logFile = path.join(__dirname, '..', 'temp', 'loop_fail.log');

function runOnce(count) {
    var output = [];
    var ev = new coroutine.Event();
    var child = child_process.spawn(process.execPath, [path.join(__dirname, 'http2_test.js')]);

    child.stdout.on('data', d => output.push(d.toString()));
    child.stderr.on('data', d => output.push(d.toString()));
    child.on('exit', () => ev.set());

    var timer = setTimeout(() => {
        child.kill('SIGINT');
    }, 10000);

    ev.wait();
    clearTimeout(timer);

    return { status: child.exitCode, output: output.join('') };
}

var count = 0;
while (1) {
    count++;
    console.log(`\n========== Round ${count} ==========`);
    const ret = runOnce(count);
    if (ret.status != 0) {
        const header = `\n===== Round ${count} failed (status=${ret.status}) =====\n`;
        fs.writeFileSync(logFile, header + ret.output);
        console.error(`Round ${count} FAILED, log written to ${logFile}`);
        throw new Error('http2_test.js failed with exit code ' + ret.status);
    }
}
