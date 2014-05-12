/**
 *
 */

var net = require('net');
var http = require('http');
var io = require('io');

var txt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
txt = txt + txt;
txt = txt + txt;
txt = txt + txt;

var body = new io.MemoryStream();

for (var i = 0; i < 10; i++)
	body.write(new Buffer(txt));

new net.TcpServer(8080, function(c) {
	var bs = new io.BufferedStream(c);
	bs.EOL = "\r\n";

	var body1 = body.clone();

	var req = new http.Request();
	var rep = req.response;

	while (rep.keepAlive) {
		try {
			req.readFrom(bs);
		} catch (e) {
			break;
		}

		rep.sendTo(c);
	}

	// console.log("close...");
}).run();