/**
 *
 */

var net = require('net');
var http = require('http');
var mq = require('mq');
var rpc = require('rpc');

var hdlr = new http.Handler(new mq.Routing({
	'^(/.*)$': http.fileHandler('httpd/www/'),
	'^/docs(/.*)$': http.fileHandler('../fibjs/docs/'),
	'^/xhr(/.*)$': rpc.json({
		test: {
			test: {
				fun: function(request, d) {
					console.log(d);
					return "ok"
				}
			}
		}
	}),
	'^/test$': function(v) {
		v.response.body.write("define('" + new Date() + "');");
		console.log('requested.', new Date());
	}
}));
hdlr.crossDomain = true;

new net.TcpServer(8080, hdlr).run();