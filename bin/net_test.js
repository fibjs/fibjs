/**
 * 
 */

var assert = require('assert');
var net = require('net');
var coroutine = require('coroutine');

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

for(var i = 0; i < 1000; i ++)
{
	var s1 = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
	s1.connect('::1', 8080);
	s1.write(new Buffer("GET / HTTP/1.0"));
	console.log(s1.read());
	s1.dispose();
}
