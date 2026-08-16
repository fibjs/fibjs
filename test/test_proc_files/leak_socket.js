/*
 * leak_socket.js — scene: leaked TCP connection
 *
 * TcpServer keeps listening, the Socket stays connected.
 * Expects: watchdog fires, exit 124; report shows Socket/TcpServer class counts
 * and the test summary.
 */
var { describe, it } = require('node:test');
var net = require('net');

describe('leak socket', () => {
    it('leak a connected socket', () => {
        var server = new net.TcpServer(0, conn => { /* keep the connection */ });
        server.start();
        global.leakedServer = server;

        var s = new net.Socket();
        s.connect(server.socket.localPort, '127.0.0.1');
        global.leakedSocket = s;
    });
});
