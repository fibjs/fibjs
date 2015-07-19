var test = require("test");
test.setup();

var xml = require('xml');
var fs = require('fs');

var props = [
	[],
	['tagName', 'namespaceURI', 'prefix', 'localName'],
	[],
	['data', 'length'],
	['data', 'length'],
	[],
	[],
	['target', 'data'],
	['data', 'length'],
	[],
	['name', 'systemId']
];
var attr_props = ['name', 'value', 'namespaceURI', 'prefix', 'localName'];

function dump_dom(dom) {
	var type = dom.nodeType;
	var o = {
		'nodeType': type,
		'nodeName': dom.nodeName,
		'nodeValue': dom.nodeValue,
		'childNodes': []
	};

	props[type].forEach(function(n) {
		o[n] = dom[n];
	});

	if (type == 1) {
		o.attributes = [];
		var attrs = dom.attributes;
		var sz = attrs.length;
		for (var i = 0; i < sz; i++) {
			var o1 = {};
			var attr = attrs[i];

			attr_props.forEach(function(n) {
				o1[n] = attr[n];
			});
			o.attributes.push(o1);
		}
	}

	var childs = dom.childNodes;
	if (childs) {
		var sz = childs.length;

		for (var i = 0; i < sz; i++)
			o.childNodes.push(dump_dom(childs[i]));
	}

	return o;
}

describe('xml', function() {
	function _test(i) {
		var id = ("000" + i).slice(-4);
		it("xml_files/xml/" + id + ".xml", function() {
			var txt = fs.readFile("xml_files/xml/" + id + ".xml");
			var json = fs.readFile("xml_files/json/" + id + ".json");
			var out = fs.readFile("xml_files/out/" + id + ".xml");

			var xdoc = xml.parse(txt);
			assert.equal(JSON.stringify(dump_dom(xdoc)), json);
			assert.equal(xdoc.toString(), out);

			var xdoc1 = xdoc.cloneNode();
			assert.notEqual(xdoc, xdoc1);
			assert.equal(JSON.stringify(dump_dom(xdoc1)), json);
			assert.equal(xdoc1.toString(), out);

			xdoc1.normalize();
			assert.equal(xdoc1.toString(), out);
			assert.equal(JSON.stringify(dump_dom(xdoc1)), json);
		});
	}

	for (var i = 1; i <= 1180; i++)
		_test(i);
});

test.run(console.DEBUG);