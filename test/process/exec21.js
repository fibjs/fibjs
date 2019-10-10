var net = require('net');

var s = new net.TcpServer(28080, c => {
    console.log(700);
    process.exitCode = 21;
    s.stop();
});
s.start();