/**
 * 
 */

var net = require('net');
var http = require('http');
var mq = require('mq');
var rpc = require('rpc');

var hdlr = http.handler(mq.routing({
	'^(/.*)$' : http.fileHandler('httpd/www/'),
	'^/docs(/.*)$' : http.fileHandler('../fibjs/docs/'),
	'^/xhr(/.*)$' : rpc.json({
		test : {
			test : {
				fun : function(request, d) {
					console.log(d);
					return "ok"
				}
			}
		}
	}),
	'^/test$': function(v) {
		v.response.body.write(new Buffer("define('" + new Date() + "');"));
		console.log('requested.', new Date());
	}
}));
hdlr.crossDomain = true;

new net.TCPServer(8080, hdlr).run();
