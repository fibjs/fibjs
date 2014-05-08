var test = require("test");
test.setup();

var fs = require('fs');
var io = require('io');
var net = require('net');
var mq = require('mq');

describe("buffered stream", function() {
	var s;

	before(function() {
		s = '0123456789\r\n';

		for (var i = 0; i < 13; i++)
			s = s + s;

		var f = fs.open("test0000", 'w');
		f.write(new Buffer(s));
		f.close();
	});

	after(function() {
		fs.unlink("test0000");
	});

	function t_read(f, sz) {
		var r = new io.BufferedStream(f);
		var p = 0;
		var d;

		while (p < s.length) {
			d = r.read(sz);
			if (!d)
				break;
			assert.equal(d.toString(), s.substring(p, p + sz));
			p += sz;
		}
		f.close();
	}

	it("block size", function() {
		for (var i = 3; i < 100000; i *= 3)
			t_read(fs.open("test0000"), i);
	});

	it("buffered tcp stream", function() {
		function accept1(s) {
			while (true) {
				var c = s.accept();
				var f = fs.open('test0000');
				f.copyTo(c);
				f.close();
				c.close();
			}
		}

		var s1 = new net.Socket();
		s1.bind(8182);
		s1.listen();
		accept1.start(s1);

		for (var i = 3; i < 100000; i *= 3) {
			var conn = new net.Socket();
			conn.connect('127.0.0.1', 8182);
			t_read(conn, i);
		}
	});

	it("readline", function() {
		f = fs.open("test0000");
		var r = new io.BufferedStream(f);
		r.EOL = '\r\n';

		var n = 0;

		while ((s1 = r.readLine()) !== null) {
			assert.equal('0123456789', s1);
			n++;
		}
		assert.equal(8192, n);
		f.close();

		f = fs.open("test0000");
		var r = new io.BufferedStream(f);
		r.EOL = '\r\n';

		assert.equal(r.readLine(10), '0123456789');
		assert.throws(function() {
			r.readLine(9);
		});

		f.close();
	});

	it('packet read&write', function() {
		f = fs.open("test0000", 'w+');
		var r = new io.BufferedStream(f);

		for (var i = 0; i < 1000; i++)
			r.writePacket(new Buffer(s.substring(0, i)));

		f.rewind();
		for (var i = 0; i < 1000; i++)
			assert.equal(r.readPacket().toString(), s.substring(0, i));
	});

	it("PacketMessage sendTo", function() {
		f.rewind();
		f.truncate(0);

		var m = new mq.PacketMessage();

		for (var i = 0; i < 1000; i++) {
			m.body.write(new Buffer(s.substring(0, i)));
			m.sendTo(f);
			m.clear();
		}

		f.rewind();
		var r = new io.BufferedStream(f);

		for (var i = 0; i < 1000; i++) {
			assert.equal(r.readPacket().toString(), s.substring(0, i));
		}
	});

	it("PacketMessage readFrom", function() {
		f.rewind();
		f.truncate(0);

		var r = new io.BufferedStream(f);

		for (var i = 1; i < 1000; i++)
			r.writePacket(new Buffer(s.substring(0, i)));

		var m = new mq.PacketMessage();

		f.rewind();
		for (var i = 1; i < 1000; i++) {
			m.readFrom(r);
			assert.equal(m.body.readAll().toString(), s.substring(0, i));
			m.clear();
		}
	});

	it('readPacket return null at the end of file', function() {
		f.rewind();
		f.truncate(0);

		var r = new io.BufferedStream(f);
		assert.isNull(r.readPacket());
	});

	it('readPacket limit', function() {
		var r = new io.BufferedStream(f);
		f.rewind();
		r.writePacket(new Buffer(s.substring(0, 65567)));

		f.rewind();
		assert.equal(r.readPacket(65567).toString(), s.substring(0, 65567));

		f.rewind();
		r = new io.BufferedStream(f);
		assert.throws(function() {
			r.readPacket(65566);
		});

		f.close();
	});
});

//test.run(console.DEBUG);