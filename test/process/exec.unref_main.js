// Main script for testing unref functionality
const child_process = require('child_process');
var path = require('path');

var ps = child_process.spawn(process.execPath, [path.join(__dirname, "exec.unref_test.js")], {
    stdio: 'inherit',
    detached: true
});

setTimeout(() => {
    ps.unref();
}, 3000);

process.on('exit', () => {
    console.log("main process exit");
});
