/**
 * 
 */

var net = require('net');

function connect(c) {
	c.on({
		read : function(d) {
//			var b = new Buffer("HTTP/1.0 200 ok\r\nConnection: keep-alive\r\nContent-Length: 6\r\n\r\nhello.");
//			console.log('on read');
			if(d)
				c.asyncWrite(new Buffer("HTTP/1.0 200 ok\r\nConnection: keep-alive\r\nContent-Length: 6\r\n\r\nhello."));
			else
				c.asyncClose();
		},
		write : function() {
//			console.log('on write');
			c.asyncRead();
		},
		error : function() {
//			console.log('on error');
			c.asyncClose();
		}
	});

	c.asyncRead();
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
