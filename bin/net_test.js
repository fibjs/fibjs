/**
 * 
 */

console.log('net testing....');

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
	try
	{
		var b;
		
		while(b = c.recv())
			c.send(b);
	}finally
	{
		c.close();
		c.dispose();
	}
}

function accept(s)
{
	while(1)
		connect.start(s.accept());
}

var s = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
s.bind(8080);
s.listen();
accept.start(s);

function conn()
{
	var s1 = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
	s1.connect('::1', 8080);
	console.log(s1.remoteAddress, s1.remotePort, "<-", s1.localAddress, s1.localPort);
	s1.send(new Buffer("GET / HTTP/1.0"));
	console.log(s1.recv(1024));
	s1.close();
	s1.dispose();
}


//for(var i = 0; i < 100; i ++)
	conn();

console.log('Backend:', net.backend());
