var test = require("test");
test.setup();

var xml = require('xml');
var fs = require('fs');

describe('xml', function() {
	it("test suite", function() {
		for (var i = 1; i < 1143; i++) {
			var id = ("000" + i).slice(-4);
			var txt = fs.readFile("xml_files/xml/" + id + ".xml");
			var xdoc = xml.parse(txt);
			assert.equal(xdoc.toString(), fs.readFile("xml_files/out/" + id + ".xml"));
		}
	});
});

test.run(console.DEBUG);