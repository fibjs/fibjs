var test = require("test");
test.setup();

var fs = require("fs");
var ssl = require("ssl");

describe('ssl', function() {
	it("x509 cert", function() {
		var fl = fs.readdir('cert_files/');
		fl.forEach(function(s) {
			if (!s.isDirectory() && s.name.match(/\.crt/))
				ssl.loadCert(fs.readFile('cert_files/' + s.name));
		});
	});

	it("x509 crl", function() {
		var fl = fs.readdir('crl_files/');
		fl.forEach(function(s) {
			if (!s.isDirectory() && s.name.match(/\.pem/))
				ssl.loadCrl(fs.readFile('crl_files/' + s.name));
		});
	});
});

//test.run(console.DEBUG);