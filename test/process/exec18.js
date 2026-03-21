process.exitCode = 18;

var conn = new WebSocket("ws://999.99.999.999/not_exists");
conn.onerror = e => {
    process.exitCode = 81;
}