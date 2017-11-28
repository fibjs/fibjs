var ws = require('ws');

process.exitCode = 18;

var conn = new ws.Socket("ws://999.99.999.999/not_exists");
conn.onerror = e => {
    console.log(1800);
}