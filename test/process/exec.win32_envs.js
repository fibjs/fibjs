const ssl = require('ssl');
const http = require('http');
const net = require('net');
const coroutine = require("coroutine");

ssl.loadRootCerts();

// use socket :start
http.get('https://stackoverflow.com');
http.get('https://github.com');
// http.get('https://fibjs.org');
const base_port = coroutine.vmid * 10000;

svr = new net.TcpServer(8881 + base_port, {});

svr.start();
setTimeout(() => {
    svr.stop();
}, 1000);
// use socket :end
