const child_process = require('child_process');
const path = require('path');

process.exitCode = child_process.run(process.execPath, [path.join(__dirname, "exec25.js")]);
