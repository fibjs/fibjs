/**
 *
 */

var net = require('net');
var http = require('http');
var mq = require('mq');

var hdlr = new http.Handler(new mq.Routing({
	'^/docs(/.*)$': http
		.fileHandler('/Users/lion/Documents/projects/fibjs/docs/')
}));
hdlr.crossDomain = true;

new net.TcpServer(8080, hdlr).run();