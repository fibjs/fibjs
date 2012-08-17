/**
 * 
 */

var net = require('net');
var io = require('io');

var b = new Buffer(
		"HTTP/1.0 200 ok\r\nConnection: keep-alive\r\nContent-Length: 6\r\n\r\nhello.");

new net.TCPServer(8080, function(c) {
	var bs = new io.BufferedStream(c);
	bs.EOL = "\r\n";

	try {
		while (true) {
			var s;
//			console.log('-------------------------------------------');
			do {
				s = bs.readLine();
//				console.log(s);
			} while (s !== "" && s !== undefined);
			c.write(b);
		}
	} catch (e) {
	}

	c.close();
}).run();
