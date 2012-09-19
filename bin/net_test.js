/**
 * 
 */

console.log('net testing....');

var assert = require('assert');
var net = require('net');
var fs = require('fs');
var os = require('os');
var coroutine = require('coroutine');

var net_config = {
	family : net.AF_INET6,
	address : '::1'
};

if (os.type == 'Windows' && os.version < "6.0")
	net_config = {
		family : net.AF_INET,
		address : '127.0.0.1'
	};

function del(f) {
	try {
		fs.unlink(f);
	} catch (e) {
	}
}

function tm() {
	while (1) {
		coroutine.sleep(1000);
		console.log("****************", os.time());
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

var s = new net.Socket(net_config.family, net.SOCK_STREAM);
s.bind(8080);
s.listen();
accept.start(s);

function conn() {
	var s1 = new net.Socket(net_config.family, net.SOCK_STREAM);
	s1.connect(net_config.address, 8080);
	console.log(s1.remoteAddress, s1.remotePort, "<-", s1.localAddress,
			s1.localPort);
	s1.send(new Buffer("GET / HTTP/1.0"));
	assert.equal("GET / HTTP/1.0", s1.recv());
	s1.close();
	s1.dispose();
}

conn();

var str = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";

for ( var i = 0; i < 8; i++)
	str = str + str;

function accept1(s) {
	while (true) {
		var c = s.accept();

		// c.write(new Buffer(str));

		fs.writeFile('net_temp_000001', str);
		var f = fs.open('net_temp_000001');
		assert.equal(f.copyTo(c), str.length);
		f.close();
		c.close();
	}
}

var s1 = new net.Socket(net_config.family, net.SOCK_STREAM);
s1.bind(8081);
s1.listen();
accept1.start(s1);

function t_conn() {
	var c1 = new net.Socket();
	c1.connect('127.0.0.1', 8081);

	var f1 = fs.open('net_temp_000002', 'w');
	assert.equal(c1.copyTo(f1), str.length);
	c1.close();
	f1.close();

	assert.equal(str, fs.readFile('net_temp_000002'));
}

for ( var i = 0; i < 100; i++)
	t_conn();

for ( var i = 0; i < 10; i++)
	str = str + str;

t_conn();

del('net_temp_000001');
del('net_temp_000002');

function accept2(s) {
	while (true) {
		var c = s.accept();

		c.write(new Buffer('a'));
		coroutine.sleep(100);
		c.write(new Buffer('a'));
		coroutine.sleep(100);
		c.write(new Buffer('b'));
		coroutine.sleep(100);
		c.write(new Buffer('c'));
		coroutine.sleep(100);
		c.write(new Buffer('d'));
		coroutine.sleep(100);

		c.close();
	}
}

var s2 = new net.Socket(net_config.family, net.SOCK_STREAM);
s2.bind(8082);
s2.listen();
accept2.start(s2);

var c1 = new net.Socket();
c1.connect('127.0.0.1', 8082);
assert.equal('a', c1.recv(100));
assert.equal('abc', c1.read(3));
assert.equal('d', c1.read(3));

console.log('Backend:', net.backend());
