// run this file with node/fibjs

const path = require('path');

var io;
try {
    io = require('io')
} catch (error) { }

const { spawn } = require('child_process');

const printer = spawn(process.execPath, [
    path.resolve(__dirname, './exec.print_kvs.js')
], {
    env: {
        // would not be overridden
        PATH: undefined,
        // should be overridden
        WINDIR: 'abc',
        // should be overridden
        USERPROFILE: '123',
        // should be set
        _CUSTOM: 'abc123'
    }
});

if (io) {
    new io.BufferedStream(printer.stdout).readLines().map(line => console.log(line));
    new io.BufferedStream(printer.stderr).readLines().map(line => console.error(line));
} else {
    printer.stdout.on('data', (data) => {
        console.log(data.toString());
    });

    printer.stderr.on('data', (data) => {
        console.error(data.toString());
    });

    printer.on('exit', (code) => {
        console.log(`Child exited with code ${code}`);
    });
}