/* NodeJS sever demo with cluster
 */

var cluster = require('cluster'),
	http = require('http'),
	url = require('url'),
	numCPUs = require('os').cpus().length;

if (cluster.isMaster) {
	console.log("master start...");

	// Fork workers.
	for (var i = 0; i < numCPUs; i++) {
		cluster.fork();
	}

	cluster.on('listening', function(worker, address) {
		console.log('listening: worker ' + worker.process.pid + ', Address: ' + address.address + ":" + address.port);
	});

	cluster.on('exit', function(worker, code, signal) {
		console.log('worker ' + worker.process.pid + ' died');
	});
} else {
	http.createServer(function(req, res) {
		var pathname = url.parse(req.url).pathname;
		if ("/node" === pathname) {
			res.writeHead(200);
			res.end("Hello World!\n");
		}
	}).listen(8080);
}