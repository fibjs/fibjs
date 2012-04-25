/**
 * 
 */

var assert = require('assert');
var net = require('net');

function accept(s)
{
	while(1)
	{
		var c = s.accept();
		console.log(c.remoteAddress, c.remotePort, "->", c.localAddress, c.localPort);
		c.write(c.read());
		c.close();
		c.dispose();
	}
}

var s = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
s.bind(8080);
s.listen();
accept.start(s);

for(var i = 0; i < 100; i ++)
{
	var s1 = new net.Socket(net.AF_INET, net.SOCK_STREAM);
	s1.connect('192.168.65.52', 8080);
	s1.write(new Buffer("GET / HTTP/1.0\n\n"));
	console.log(s1.read());
}
