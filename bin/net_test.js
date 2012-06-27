/**
 * 
 */

console.log('net testing....');

var assert = require('assert');
var net = require('net');
var io = require('io');
var os = require('os');
var coroutine = require('coroutine');

function tm() {
	while (1) {
		coroutine.sleep(1000);
		console.log("****************", new Date());
	}
}

tm.start();

function connect(c) {
	console.log(c.remoteAddress, c.remotePort, "->", c.localAddress,
			c.localPort);
	try {
		var b;

		while (b = c.recv())
			c.send(b);
	} finally {
		c.close();
		c.dispose();
	}
}

function accept(s) {
	while (1)
		connect.start(s.accept());
}

var s = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
s.bind(8080);
s.listen();
accept.start(s);

function conn() {
	var s1 = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
	s1.connect('::1', 8080);
	console.log(s1.remoteAddress, s1.remotePort, "<-", s1.localAddress,
			s1.localPort);
	s1.send(new Buffer("GET / HTTP/1.0"));
	console.log(s1.recv());
	s1.close();
	s1.dispose();
}

conn();

var str = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";

function accept1(s) {
	var c = s.accept();

	io.writeFile('net_temp_000001', str);
	var f = io.open('net_temp_000001');
	f.copyTo(c);
	f.close();
	c.close();
}

var s1 = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
s1.bind(8081);
s1.listen();
accept1.start(s1);

var c1 = new net.Socket();
c1.connect('127.0.0.1', 8081);
var f1 = io.open('net_temp_000002', 'w');
c1.copyTo(f1);
c1.close();
f1.close();

assert.equal(str, io.readFile('net_temp_000002'));

os.unlink('net_temp_000001');
os.unlink('net_temp_000002');

console.log('Backend:', net.backend());
