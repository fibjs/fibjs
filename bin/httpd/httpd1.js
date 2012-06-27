/**
 * 
 */

var net = require('net');
var b = new Buffer("HTTP/1.0 200 ok\r\nConnection: keep-alive\r\nContent-Length: 6\r\n\r\nhello.");

function connect(c)
{
	while(c.read())
		c.write(b);
	c.close();
}

var s = new net.Socket();
s.bind(8080);
s.listen(128);
while(1)
{
//	console.log('wait accept..');
	var s1 = s.accept();
//	console.log('accept..');
	connect.start(s1);
//	console.log('start fiber..');
}
