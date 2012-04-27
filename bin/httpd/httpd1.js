/**
 * 
 */

var net = require('net');

function connect(c)
{
	c.read();
	c.write(new Buffer("HTTP/1.0 200 ok\r\n\r\nhello."));
	c.close();
	c.dispose();
}

var s = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
s.bind(8080);
s.listen(128);
while(1)
	connect.start(s.accept());
