var ws = require('ws');

process.exitCode = 19;

var conn = new ws.Socket("ws://127.0.0.1:8899/ws");

conn.onopen = () => {
    conn.send('hello');
}
conn.onmessage = function (msg) {
    console.log(1900);
    conn.close();
    httpd.stop();
};