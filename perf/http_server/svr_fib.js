var http = require('http'),
	net = require('net'),
	coroutine = require('coroutine');

var interval = 1000;

var hdlr = new http.Handler(function(req) {
	if (req.address == "/fibjs") {
		req.response.write('Hello, World!');
	}
})

var demon = function() {
	while (true) {
		console.error("connections:", svr.stats.connections,
			"\trequest:", hdlr.stats.request,
			"\tresponse:", hdlr.stats.response);
		hdlr.stats.reset();
		svr.stats.reset();
		coroutine.sleep(interval);
	}
}

var svr = new net.TcpServer(8080, hdlr);

coroutine.start(demon);
svr.run();