const child_process = require('child_process');
const test = require('test');
const path = require('path');

while (1) {
    const ret = child_process.run(process.execPath, [path.join(__dirname, 'tls_test.js')], {
        stdio: 'inherit'
    });
    if(ret != 0) {
        throw new Error('tls_test.js failed with exit code ' + ret);
    }
}
