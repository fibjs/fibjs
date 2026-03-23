const child_process = require('child_process');
const fs = require('fs');
const path = require('path');

const logFile = path.join(__dirname, '..', 'temp', 'loop_fail.log');

var count = 0;
while (1) {
    count++;
    console.log(`\n========== Round ${count} ==========`);
    const ret = child_process.spawnSync(process.execPath, [path.join(__dirname, 'http2_test.js')], {
        stdio: 'pipe',
        timeout: 10000,
        killSignal: 'SIGINT'
    });
    if (ret.status != 0) {
        const header = `\n\n===== Round ${count} failed (status=${ret.status} error=${ret.error}) =====\n`;
        const stdout = ret.stdout ? ret.stdout.toString() : '';
        const stderr = ret.stderr ? ret.stderr.toString() : '';
        const body = '--- stdout ---\n' + stdout + '\n--- stderr ---\n' + stderr + '\n';
        fs.writeFileSync(logFile, header + body);
        console.error(`Round ${count} FAILED, log written to ${logFile}`);
        if (stderr) {
            var lines = stderr.split('\n');
            console.error(lines.slice(-30).join('\n'));
        }
        throw new Error('http2_test.js failed with exit code ' + ret.status);
    }
}
