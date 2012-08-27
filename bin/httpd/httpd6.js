/**
 * 
 */

var net = require('net');
var http = require('http');
var mq = require('mq');

new net.TCPServer(8080, http.handler(new mq.Routing({
	'^/docs(/.*)$' : http.fileHandler('/Documents/projects/fibjs/docs/')
}))).run();
