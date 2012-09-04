/**
 * 
 */

var net = require('net');
var http = require('http');
var mq = require('mq');

var hdlr = http.handler(mq.routing({
	'^/docs(/.*)$' : http
			.fileHandler('/Users/lion/Documents/projects/fibjs/docs/')
}));
hdlr.crossDomain = true;

new net.TCPServer(8080, hdlr).run();
