var net = require('net');
var coroutine = require('coroutine');

var mt = 0;
var cnt = 0;
var sum = 0;
var base_port = coroutine.vmid * 10000;

function f() {
	for (var i = 0; i < 100000; i++) {
		var t = new Date().getTime();

		var s1 = new net.Socket(net.AF_INET, net.SOCK_STREAM);
		s1.connect('127.0.0.1', 8080 + base_port);
		s1.write("GET / HTTP/1.0\r\n\r\n");
		s1.read(10240);
		s1.close();
		s1.dispose();
		t = new Date().getTime() - t;
		if (t > mt) mt = t;
		sum += t;
		cnt++;
		console.log('connect:', cnt, mt, Math.floor(sum / cnt), t);
	}
}

for (var j = 0; j < 100; j++)
	coroutine.start(f);

while (cnt < 10000000)
	coroutine.sleep(1);