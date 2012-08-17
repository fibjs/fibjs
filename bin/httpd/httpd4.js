/**
 * 
 */

var net = require('net');
var http = require('http');
var io = require('io');

new net.TCPServer(8080, function(c) {
	var bs = new io.BufferedStream(c);
	bs.EOL = "\r\n";

//	console.log("accept...");
	var req = new http.Request();
	var rep = new http.Response();

	while (rep.keepAlive) {
		try {
			req.clear();
			req.read(bs);

			rep.clear();
			rep.protocol = req.protocol;
			rep.keepAlive = req.keepAlive;
			
			rep.contentType = "text/html";

			rep.body.write(new Buffer("0123456789"));
			rep.body.write(new Buffer("0123456789"));
			rep.body.write(new Buffer("0123456789"));
			
			rep.send(c);
		} catch (e) {
			break;
		}
	}

	c.close();
	c.dispose();
//	console.log("close...");
}).run();
