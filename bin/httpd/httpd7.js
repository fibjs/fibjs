/**
 * 
 */

var net = require('net');
var http = require('http');
var mq = require('mq');
var rpc = require('rpc');

var hdlr = http.handler(mq.routing({
	'^(/.*)$' : http.fileHandler('httpd/www/'),
	'^/xhr(/.*)$' : rpc.json({
		test : {
			test : {
				fun : function(v, n, a, b) {
					console.log(n, a, b);
					return n + a + b;
				}
			}
		}
	})
}));
hdlr.crossDomain = true;

new net.TCPServer(8080, hdlr).run();
