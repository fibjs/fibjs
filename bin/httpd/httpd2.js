/**
 * 
 */

var net = require('net');

function connect(c) {
	c.on({
		read : function(d) {
//			console.log('on read');
			c.asyncWrite(new Buffer(
					"HTTP/1.0 200 ok\r\nConnection: close\r\n\r\nhello."));
		},
		write : function() {
//			console.log('on write');
			c.asyncClose();
		},
		close : function() {
//			console.log('on close');
			c.dispose();
		},
		error : function() {
//			console.log('on error');
			c.asyncClose();
		}
	});

	var b = c.asyncRead();
//	c.asyncWrite(new Buffer("HTTP/1.0 200 ok\r\nConnection: close\r\n\r\nhello."));
//	c.asyncClose();
//	c.dispose();
}

var s = new net.Socket();
s.bind(8080);
s.listen(128);
while (1) {
	// console.log('wait accept..');
	var s1 = s.accept();
	// console.log('accept..');
	connect.start(s1);
	// console.log('start fiber..');
}
