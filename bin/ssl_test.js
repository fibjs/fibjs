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

	it("cert clear/export", function() {
		var s = ssl.exportCert();
		ssl.clearCert();
		assert.deepEqual(ssl.exportCert(), []);

		s.forEach(function(c) {
			ssl.loadCert(c);
		});

		assert.deepEqual(ssl.exportCert(), s);
	});

	it("load x509 cert file", function() {
		var s = ssl.exportCert();
		ssl.clearCert();

		var fl = fs.readdir('cert_files/');
		fl.forEach(function(s) {
			if (!s.isDirectory() && s.name.match(/\.crt/))
				ssl.loadCertFile('cert_files/' + s.name);
		});

		assert.deepEqual(ssl.exportCert(), s);
	});

	it("certdata.txt", function() {
		ssl.clearCert();
		assert.deepEqual(ssl.exportCert(), []);

		ssl.loadCert(fs.readFile('cert_files/certdata.txt'));
		var s = ssl.exportCert();
		assert.notDeepEqual(s, []);

		ssl.clearCert();
		assert.deepEqual(ssl.exportCert(), []);

		ssl.loadCert(fs.readFile('cert_files/certdata.txt'));
		assert.deepEqual(ssl.exportCert(), s);
	});

	it("unknown format", function() {
		assert.throws(function() {
			ssl.loadCert('cert_files/certdata.txt');
		});
	});

	it("x509 crl", function() {
		var fl = fs.readdir('crl_files/');
		fl.forEach(function(s) {
			if (!s.isDirectory() && s.name.match(/\.pem/))
				ssl.loadCrl(fs.readFile('crl_files/' + s.name));
		});
	});

	it("crl clear/export", function() {
		var s = ssl.exportCrl();
		ssl.clearCrl();
		assert.deepEqual(ssl.exportCrl(), []);

		s.forEach(function(c) {
			ssl.loadCrl(c);
		});

		assert.deepEqual(ssl.exportCrl(), s);
	});

	it("load x509 crl file", function() {
		var s = ssl.exportCrl();
		ssl.clearCrl();

		var fl = fs.readdir('crl_files/');
		fl.forEach(function(s) {
			if (!s.isDirectory() && s.name.match(/\.pem/))
				ssl.loadCrlFile('crl_files/' + s.name);
		});

		assert.deepEqual(ssl.exportCrl(), s);
	});

});

//test.run(console.DEBUG);