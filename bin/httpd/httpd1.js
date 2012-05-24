/**
 * 
 */

var net = require('net');

function connect(c)
{
	var b = c.read();
	if(b)
		c.write(new Buffer("HTTP/1.0 200 ok\r\nConnection: close\r\n\r\nhello."));
	c.close();
	c.dispose();
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
