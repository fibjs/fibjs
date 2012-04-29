/**
 * 
 */

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
	c.read();
	c.write(new Buffer("HTTP/1.0 200 ok\r\n\r\nhello."));
	c.close();
	c.dispose();
}

var s = new net.Socket(net.AF_INET6, net.SOCK_STREAM);
s.bind(8080);
s.listen(128);
while(1)
{
	console.log('wait accept..');
	var s1 = s.accept();
	console.log('accept..');
	connect.start(s1);
	console.log('start fiber..');
}
