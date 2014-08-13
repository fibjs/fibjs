/**
 *
 */

var net = require('net');
var http = require('http');
var mq = require('mq');
var io = require('io');

var txt = "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
txt = txt + txt;
txt = txt + txt;
txt = txt + txt;

var body = new io.MemoryStream();

var sz = 1;

for (var i = 0; i < sz; i++)
	body.write(txt);

body.setTime(new Date());
console.log(body.clone().stat().mtime);

new net.TcpServer(8080, http.handler(function(req) {
	req.response.body = body.clone();
})).run();

//new net.TCPServer(8080, http.handler(mq.nullHandler())).run();