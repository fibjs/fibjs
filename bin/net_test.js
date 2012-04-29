/**
 * 
 */

var assert = require('assert');
var net = require('net');
var coroutine = require('coroutine');

function tm()
{
	while(1)
	{
		coroutine.sleep(1000);
		console.log("****************", new Date());
	}
}

tm.start();

function connect(c)
{
	console.log(c.remoteAddress, c.remotePort, "->", c.localAddress, c.localPort);
	c.write(c.read());
	c.close();
	c.dispose();
}

function accept(s)
{
	while(1)
	{
		console.log('accept...');
		connect.start(s.accept());
	}
}

var s = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
s.bind(8080);
s.listen();
accept.start(s);

var s1 = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
s1.connect('::1', 8080);
s1.write(new Buffer("GET / HTTP/1.0"));
console.log(s1.read(1024));
s1.close();
s1.dispose();

console.log('Backend:', net.backend());
