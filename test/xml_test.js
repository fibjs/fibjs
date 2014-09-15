var test = require("test");
test.setup();

var xml = require('xml');
var fs = require('fs');

function newDoc() {
	return new xml.Document();
}

var parse = xml.parse;

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
			if (v) {
				assert.equal(e.appendChild(e1), e1);
				assert.equal(e1.parentNode, e);
				assert.equal(e.lastChild, e1);
			} else
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
			assert.equal(xdoc.childNodes.length, 0);

			assert.equal(xdoc.nodeValue, null);
			xdoc.nodeValue = 'aaaaa';
			assert.equal(xdoc.nodeValue, null);

			xdoc = parse("<!DOCTYPE foo><foo/>");
			assert.equal(xdoc.doctype, xdoc.firstChild);
			xdoc.removeChild(xdoc.firstChild);
			assert.equal(xdoc.doctype, null);

			var xdoc1 = parse("<!DOCTYPE foo><foo/>");
			xdoc.appendChild(xdoc1.removeChild(xdoc1.firstChild));
		});

		it("child rule", function() {
			var xdoc = newDoc();
			test_Child(xdoc, xdoc, [0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1]);
		});

		it("Element limit", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			var e1 = xdoc.createElement("bbb");
			var c = xdoc.createComment("aaa");

			xdoc.appendChild(e);
			xdoc.appendChild(c);
			assert.throws(function() {
				xdoc.appendChild(e1);
			});

			xdoc.replaceChild(e1, e);
			assert.throws(function() {
				xdoc.replaceChild(e, c);
			});

			xdoc.removeChild(e1);
			xdoc.appendChild(e);

			assert.equal(xdoc.documentElement, e);
			var xdoc1 = newDoc();
			xdoc1.appendChild(e);
			assert.equal(e.parentNode, xdoc1);
			assert.equal(xdoc.documentElement, null);
			assert.equal(xdoc1.documentElement, e);
		});

		it("getElementsByTagName", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			xdoc.appendChild(e);
			e.appendChild(xdoc.createElement("aaa"));
			e.appendChild(xdoc.createElement("bbb"));

			var l = xdoc.getElementsByTagName("aaa");
			assert.equal(l.length, 2);
			var l1 = xdoc.getElementsByTagName("bbb");
			assert.equal(l1.length, 1);
			var l2 = xdoc.getElementsByTagName("ccc");
			assert.equal(l2.length, 0);
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
			assert.equal(e.childNodes.length, 0);

			assert.equal(e.nodeValue, null);
			e.nodeValue = 'aaaaa';
			assert.equal(e.nodeValue, null);
		});

		it("child rule", function() {
			var xdoc = newDoc();
			test_Child(xdoc, xdoc.createElement("bbb"), [0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0]);
		});

		it("getElementsByTagName", function() {
			var xdoc = newDoc();
			var root = xdoc.createElement("aaa");
			var e = xdoc.createElement("aaa");
			root.appendChild(e);
			e.appendChild(xdoc.createElement("aaa"));
			e.appendChild(xdoc.createElement("bbb"));

			var l = root.getElementsByTagName("aaa");
			assert.equal(l.length, 2);
			var l1 = root.getElementsByTagName("bbb");
			assert.equal(l1.length, 1);
			var l2 = root.getElementsByTagName("ccc");
			assert.equal(l2.length, 0);
		});
	});

	describe('Attr', function() {
		it("base", function() {
			var xdoc = newDoc();
			var e = xdoc.createAttribute("aaa");
			assert.equal(e.ownerDocument, xdoc);
			assert.equal(e.nodeType, 2);
			assert.equal(e.nodeName, 'aaa');
			assert.equal(e.childNodes.length, 0);

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
			assert.equal(e.childNodes.length, 0);

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
			assert.equal(e.childNodes.length, 0);

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

		it("normalize", function() {
			var xdoc = newDoc();
			var root = xdoc.createElement("aaa");

			xdoc.appendChild(root);
			root.appendChild(xdoc.createTextNode("aaa"));
			root.appendChild(xdoc.createTextNode("bbb"));
			xdoc.normalize();
			assert.equal(root.firstChild.nodeValue, "aaabbb");

			root.firstChild.nodeValue = "";
			root.normalize();
			assert.equal(root.firstChild, null);

			root.appendChild(xdoc.createTextNode(""));
			var next = xdoc.createElement("aaa");
			root.appendChild(next);
			next.appendChild(xdoc.createTextNode("aaa"));
			next.appendChild(xdoc.createTextNode("bbb"));

			xdoc.normalize();
			assert.equal(next.firstChild.nodeValue, "aaabbb");
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
			assert.equal(e.childNodes.length, 0);

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
			assert.equal(e.childNodes.length, 0);

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

	describe('attrs', function() {
		it("setAttribute", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			e.setAttribute("att1", "val1");
			assert.equal(e.attributes[0].nodeValue, "val1");

			e.setAttribute("att1", "val2");
			assert.equal(e.attributes[0].nodeValue, "val2");
		});

		it("getAttribute", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			e.setAttribute("att1", "val1");

			assert.equal(e.getAttribute("att1"), "val1");
			assert.equal(e.getAttribute("att2"), null);
		});

		it("removeAttribute", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			e.setAttribute("att1", "val1");

			assert.equal(e.getAttribute("att1"), "val1");
			e.removeAttribute("att1");
			assert.equal(e.getAttribute("att1"), null);
			e.removeAttribute("att1");
		});

		it("getAttributeNode", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			e.setAttribute("att1", "val1");

			assert.equal(e.getAttributeNode("att1").nodeValue, "val1");
			assert.equal(e.getAttributeNode("att2"), null);
		});

		it("setAttributeNode", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			var a = xdoc.createAttribute("attr1");
			a.nodeValue = "val1";

			assert.equal(e.setAttributeNode(a), null);
			assert.equal(e.getAttribute("attr1"), "val1");

			var a1 = xdoc.createAttribute("attr1");
			a1.nodeValue = "val2";

			assert.equal(e.setAttributeNode(a1), a);
			assert.equal(e.getAttribute("attr1"), "val2");
		});

		it("removeAttributeNode", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			e.setAttribute("att1", "val1");

			var a = e.getAttributeNode("att1");
			assert.equal(a.nodeValue, "val1");
			assert.equal(e.removeAttributeNode(a), a);
			assert.equal(e.getAttributeNode("att1"), null);
		});
	});
});

//test.run(console.DEBUG);