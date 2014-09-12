var test = require("test");
test.setup();

var xml = require('xml');

function newDoc() {
	return new xml.Document();
}

describe('xml', function() {
	function test_CharacterData(fn) {
		describe('CharacterData', function() {
			it("data", function() {
				var xdoc = newDoc();
				var e = xdoc[fn]("aaa");
				assert.equal(e.data, 'aaa');
				e.data = 'aaaaa';
				assert.equal(e.data, 'aaaaa');
				assert.equal(e.length, 5);

				e.appendData('bbb');
				assert.equal(e.data, 'aaaaabbb');
			});

			it("substringData", function() {
				var xdoc = newDoc();
				var e = xdoc[fn]("aaaaabbb");

				assert.equal(e.substringData(3, 4), 'aabb');
				assert.equal(e.substringData(3, 10), 'aabbb');
				assert.equal(e.substringData(8, 10), '');
				assert.throws(function() {
					e.substringData(10, 0);
				});
			});

			it("insertData", function() {
				var xdoc = newDoc();
				var e = xdoc[fn]("aaaaabbb");

				e.insertData(3, "ccc");
				assert.equal(e.data, 'aaacccaabbb');

				e.insertData(11, "ddd");
				assert.equal(e.data, 'aaacccaabbbddd');

				assert.throws(function() {
					e.insertData(17, "ddd");
				});
			});

			it("deleteData", function() {
				var xdoc = newDoc();
				var e = xdoc[fn]("aaaaabbb");

				e.deleteData(3, 3);
				assert.equal(e.data, 'aaabb');

				e.deleteData(3, 10);
				assert.equal(e.data, 'aaa');

				assert.throws(function() {
					e.deleteData(4, 1);
				});
			});

			it("replaceData", function() {
				var xdoc = newDoc();
				var e = xdoc[fn]("aaaaabbb");

				e.replaceData(3, 3, 'ccc');
				assert.equal(e.data, 'aaacccbb');

				e.replaceData(3, 3, '');
				assert.equal(e.data, 'aaabb');

				assert.throws(function() {
					e.replaceData(7, 1);
				});
			});
		});
	}

	function test_Child(xdoc, e, s) {
		function test(e1, v) {
			if (v)
				assert.equal(e.appendChild(e1), e1);
			else
				assert.throws(function() {
					e.appendChild(e1)
				});
		}

		test(xdoc.createElement("bbb"), s[1]);
		test(xdoc.createAttribute("bbb"), s[2]);
		test(xdoc.createTextNode("bbb"), s[3]);
		test(xdoc.createCDATASection("bbb"), s[4]);
		test(xdoc.createProcessingInstruction("aaa", "bbb"), s[7]);
		test(xdoc.createComment("bbb"), s[8]);
		test(xdoc, s[9]);
	}

	describe('Document', function() {
		it("base", function() {
			var xdoc = newDoc();
			assert.equal(xdoc.ownerDocument, null);
			assert.equal(xdoc.nodeType, 9);
			assert.equal(xdoc.nodeName, '#document');

			assert.equal(xdoc.nodeValue, null);
			xdoc.nodeValue = 'aaaaa';
			assert.equal(xdoc.nodeValue, null);
		});
	});

	describe('Element', function() {
		it("base", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			assert.equal(e.ownerDocument, xdoc);
			assert.equal(e.nodeType, 1);
			assert.equal(e.nodeName, 'aaa');
			assert.equal(e.tagName, 'aaa');

			assert.equal(e.nodeValue, null);
			e.nodeValue = 'aaaaa';
			assert.equal(e.nodeValue, null);
		});

		it("child rule", function() {
			var xdoc = newDoc();
			test_Child(xdoc, xdoc.createElement("bbb"), [0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0]);
		});
	});

	describe('Attr', function() {
		it("base", function() {
			var xdoc = newDoc();
			var e = xdoc.createAttribute("aaa");
			assert.equal(e.ownerDocument, xdoc);
			assert.equal(e.nodeType, 2);
			assert.equal(e.nodeName, 'aaa');

			assert.equal(e.nodeValue, '');
			e.nodeValue = 'aaaaa';
			assert.equal(e.nodeValue, 'aaaaa');

			assert.equal(e.name, 'aaa');
			assert.equal(e.value, 'aaaaa');
			e.value = 'bbbbb';
			assert.equal(e.value, 'bbbbb');

			assert.equal(e.specified, true);
		});

		it("child rule", function() {
			var xdoc = newDoc();
			test_Child(xdoc, xdoc.createAttribute("bbb"), [0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0]);
		});
	});

	describe('ProcessingInstruction', function() {
		it("base", function() {
			var xdoc = newDoc();
			var e = xdoc.createProcessingInstruction("aaa", "bbb");
			assert.equal(e.ownerDocument, xdoc);
			assert.equal(e.nodeType, 7);
			assert.equal(e.nodeName, 'aaa');

			assert.equal(e.nodeValue, 'bbb');
			e.nodeValue = 'aaaaa';
			assert.equal(e.nodeValue, 'aaaaa');

			assert.equal(e.target, 'aaa');
			assert.equal(e.data, 'aaaaa');
			e.data = 'bbbbb';
			assert.equal(e.data, 'bbbbb');
		});

		it("child rule", function() {
			var xdoc = newDoc();
			test_Child(xdoc, xdoc.createProcessingInstruction("aaa", "bbb"), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
		});
	});

	describe('TextNode', function() {
		it("base", function() {
			var xdoc = newDoc();
			var e = xdoc.createTextNode("aaa");
			assert.equal(e.ownerDocument, xdoc);
			assert.equal(e.nodeType, 3);
			assert.equal(e.nodeName, '#text');

			assert.equal(e.nodeValue, 'aaa');
			e.nodeValue = 'aaaaa';
			assert.equal(e.nodeValue, 'aaaaa');
		});

		it("splitText", function() {
			var xdoc = newDoc();
			var e = xdoc.createTextNode("aaabbb");
			var e1 = e.splitText(3);

			assert.equal(e.nodeValue, 'aaa');
			assert.equal(e1.nodeValue, 'bbb');
		});

		it("child rule", function() {
			var xdoc = newDoc();
			test_Child(xdoc, xdoc.createTextNode("aaa"), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
		});

		test_CharacterData("createTextNode");
	});

	describe('CDATASection', function() {
		it("base", function() {
			var xdoc = newDoc();
			var e = xdoc.createCDATASection("aaa");
			assert.equal(e.ownerDocument, xdoc);
			assert.equal(e.nodeType, 4);
			assert.equal(e.nodeName, '#cdata-section');

			assert.equal(e.nodeValue, 'aaa');
			e.nodeValue = 'aaaaa';
			assert.equal(e.nodeValue, 'aaaaa');
		});

		it("splitText", function() {
			var xdoc = newDoc();
			var e = xdoc.createCDATASection("aaabbb");
			var e1 = e.splitText(3);

			assert.equal(e.nodeValue, 'aaa');
			assert.equal(e1.nodeValue, 'bbb');
		});

		it("child rule", function() {
			var xdoc = newDoc();
			test_Child(xdoc, xdoc.createCDATASection("aaa"), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
		});

		test_CharacterData("createCDATASection");
	});

	describe('Comment', function() {
		it("base", function() {
			var xdoc = newDoc();
			var e = xdoc.createComment("aaa");
			assert.equal(e.ownerDocument, xdoc);
			assert.equal(e.nodeType, 8);
			assert.equal(e.nodeName, '#comment');

			assert.equal(e.nodeValue, 'aaa');
			e.nodeValue = 'aaaaa';
			assert.equal(e.nodeValue, 'aaaaa');
		});

		it("child rule", function() {
			var xdoc = newDoc();
			test_Child(xdoc, xdoc.createComment("aaa"), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
		});

		test_CharacterData("createComment");
	});

	describe('childNodes', function() {
		it("apendChild", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			assert.equal(e.childNodes[0], null);
			var e1 = xdoc.createElement("bbb");
			e.appendChild(e1);
			assert.equal(e.firstChild, e1);
			assert.equal(e.childNodes[0], e1);

			var xdoc1 = newDoc();
			var e2 = xdoc1.createElement("bbb");
			assert.equal(e2.ownerDocument, xdoc1)
			e.appendChild(e2);
			assert.equal(e2.ownerDocument, xdoc)

			assert.equal(e1.nextSibling, e2);
			assert.equal(e1, e2.previousSibling);

			var e3 = xdoc.createElement("bbb");
			e.appendChild(e3);
			assert.equal(e3.parentNode, e);
			e1.appendChild(e3);
			assert.equal(e3.parentNode, e1);

			assert.throws(function() {
				e3.appendChild(e);
			})
		});

		it("removeChild", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			var e1 = xdoc.createElement("bbb");
			var e2 = xdoc.createElement("bbb");

			e.appendChild(e1);
			e.appendChild(e2);

			assert.equal(e1.nextSibling, e2);
			assert.equal(e1, e2.previousSibling);

			e.removeChild(e1);
			assert.equal(e.childNodes.length, 1);

			assert.equal(e1.nextSibling, null);
			assert.equal(e2.previousSibling, null);
		});

		it("replaceChild", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			var e1 = xdoc.createElement("bbb");
			var e2 = xdoc.createElement("bbb");

			e.appendChild(e1);
			e.appendChild(e2);

			var e3 = xdoc.createElement("bbb");
			e.replaceChild(e3, e1);

			assert.equal(e1.nextSibling, null);
			assert.equal(e2.previousSibling, e3);

			var e4 = xdoc.createElement("bbb");
			e1.appendChild(e4);
			assert.equal(e1.childNodes.length, 1);
			assert.equal(e4.parentNode, e1);
			e.replaceChild(e4, e3);
			assert.equal(e4.parentNode, e);
			assert.equal(e1.childNodes.length, 0);
		});

		it("insertBefore", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			var e1 = xdoc.createElement("bbb");
			var e2 = xdoc.createElement("bbb");

			e.appendChild(e1);
			e.appendChild(e2);

			var e3 = xdoc.createElement("bbb");
			e.insertBefore(e3, e2);

			assert.equal(e1.nextSibling, e3);
			assert.equal(e2.previousSibling, e3);

			var e4 = xdoc.createElement("bbb");
			e1.appendChild(e4);
			assert.equal(e1.childNodes.length, 1);
			assert.equal(e4.parentNode, e1);
			e.insertBefore(e4, e3);
			assert.equal(e4.parentNode, e);
			assert.equal(e1.childNodes.length, 0);
		});

		it("childNodes", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			var e1 = xdoc.createElement("bbb");
			e.appendChild(e1);

			var childs = e.childNodes;
			assert.equal(childs.length, 1);
			assert.equal(e1.parentNode, e);
			e.dispose();
			assert.equal(childs.length, 0);
			assert.equal(e1.parentNode, null);
		});
	});
});

//test.run(console.DEBUG);