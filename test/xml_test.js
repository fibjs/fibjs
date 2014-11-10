var test = require("test");
test.setup();

var xml = require('xml');
var fs = require('fs');

function newDoc() {
	return new xml.Document();
}

var serialize = xml.serialize;
var parse = xml.parse;
var parseHtml = function(txt) {
	return xml.parse(txt, "text/html")
};

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

		it("namespace", function() {
			var xdoc = parse("<foo xmlns:ns1=\"nsr:xns1\"><ns1:tag1 ns1:attr=\"val\" /></foo>");
			var root = xdoc.firstChild;

			assert.equal(root.nodeName, "foo");
			assert.equal(root.localName, "foo");
			assert.equal(root.prefix, null);
			assert.equal(root.namespaceURI, null);

			var node = root.firstChild;

			assert.equal(node.nodeName, "ns1:tag1");
			assert.equal(node.localName, "tag1");
			assert.equal(node.prefix, "ns1");
			assert.equal(node.namespaceURI, "nsr:xns1");

			assert.equal(node.lookupNamespaceURI("ns1"), "nsr:xns1");
			assert.equal(node.lookupPrefix("nsr:xns1"), "ns1");
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

			e.nodeValue = "aaabbb";
			var root = xdoc.createElement("aaa");
			root.appendChild(e);
			assert.equal(e.parentNode, root);
			e1 = e.splitText(3);
			assert.equal(e1.parentNode, root);
			assert.equal(root.firstChild, e);
			assert.equal(root.lastChild, e1);
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

			e.nodeValue = "aaabbb";
			var root = xdoc.createElement("aaa");
			root.appendChild(e);
			assert.equal(e.parentNode, root);
			e1 = e.splitText(3);
			assert.equal(e1.parentNode, root);
			assert.equal(root.firstChild, e);
			assert.equal(root.lastChild, e1);
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

		it("insertAfter", function() {
			var xdoc = newDoc();
			var e = xdoc.createElement("aaa");
			var e1 = xdoc.createElement("bbb");
			var e2 = xdoc.createElement("bbb");

			e.appendChild(e1);
			e.appendChild(e2);

			var e3 = xdoc.createElement("bbb");
			e.insertAfter(e3, e1);

			assert.equal(e1.nextSibling, e3);
			assert.equal(e2.previousSibling, e3);

			var e4 = xdoc.createElement("bbb");
			e1.appendChild(e4);
			assert.equal(e1.childNodes.length, 1);
			assert.equal(e4.parentNode, e1);
			e.insertAfter(e4, e3);
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
			assert.equal(e.attributes[0].value, "val1");

			e.setAttribute("att1", "val2");
			assert.equal(e.attributes[0].value, "val2");
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
	});

	describe("namespace", function() {
		it("basic", function() {
			var xdoc = newDoc();

			var node = xdoc.createElement("ns1:aaa");
			assert.equal(node.localName, "ns1:aaa");
			assert.equal(node.prefix, null);
			assert.equal(node.namespaceURI, null);
			assert.equal(serialize(node), "<ns1:aaa/>");
		});

		describe("root", function() {
			it("no prefix", function() {
				var xdoc = newDoc();

				var node = xdoc.createElementNS("nsr:xns3", "aaa");
				assert.equal(node.tagName, "aaa");
				assert.equal(node.localName, "aaa");
				assert.equal(node.prefix, null);
				assert.equal(node.namespaceURI, "nsr:xns3");
				assert.equal(serialize(node), "<aaa xmlns=\"nsr:xns3\"/>");
			});

			it("prefix", function() {
				var xdoc = newDoc();

				var node = xdoc.createElementNS("nsr:xns3", "ns1:aaa");
				assert.equal(node.tagName, "ns1:aaa");
				assert.equal(node.localName, "aaa");
				assert.equal(node.prefix, "ns1");
				assert.equal(node.namespaceURI, "nsr:xns3");
				assert.equal(serialize(node), "<ns1:aaa xmlns:ns1=\"nsr:xns3\"/>");
			});

			it("xmlns", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				node.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns2", "nsr:xns2");
				assert.equal(serialize(node), "<aaa xmlns:ns2=\"nsr:xns2\"/>");
			});
		});

		describe("sub node", function() {
			it("no prefix", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				var sub = xdoc.createElementNS("nsr:xns1", "ns2");
				node.appendChild(sub);

				assert.equal(sub.prefix, null);
				assert.equal(sub.namespaceURI, "nsr:xns1");
				assert.equal(serialize(node), "<aaa><ns2 xmlns=\"nsr:xns1\"/></aaa>");
			});

			it("prefix not exists", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				var sub = xdoc.createElementNS("nsr:xns1", "ns2:ns2");
				node.appendChild(sub);

				assert.equal(sub.prefix, "ns2");
				assert.equal(sub.namespaceURI, "nsr:xns1");
				assert.equal(serialize(node), "<aaa><ns2:ns2 xmlns:ns2=\"nsr:xns1\"/></aaa>");
			});

			it("prefix not match", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				node.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns2", "nsr:xns2");

				var sub = xdoc.createElementNS("nsr:xns1", "ns2:ns2");
				node.appendChild(sub);

				assert.equal(sub.prefix, "ns2");
				assert.equal(sub.namespaceURI, "nsr:xns1");
				assert.equal(serialize(node), "<aaa xmlns:ns2=\"nsr:xns2\"><a0:ns2 xmlns:a0=\"nsr:xns1\"/></aaa>");
			});

			it("prefix match", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				node.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns2", "nsr:xns2");

				var sub = xdoc.createElementNS("nsr:xns2", "ns2:ns2");
				node.appendChild(sub);

				assert.equal(sub.prefix, "ns2");
				assert.equal(sub.namespaceURI, "nsr:xns2");
				assert.equal(serialize(node), "<aaa xmlns:ns2=\"nsr:xns2\"><ns2:ns2/></aaa>");
			});
		});

		describe("attribute", function() {
			it("no prefix", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				node.setAttributeNS("nsr:xns1", "ns2", "val2");
				var attr = node.attributes[0];
				assert.equal(attr.prefix, null);
				assert.equal(attr.namespaceURI, "nsr:xns1");
				assert.equal(serialize(node), "<aaa a0:ns2=\"val2\" xmlns:a0=\"nsr:xns1\"/>");
			});

			it("prefix not exists", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				node.setAttributeNS("nsr:xns1", "ns2:ns2", "val2");
				var attr = node.attributes[0];
				assert.equal(attr.prefix, "ns2");
				assert.equal(attr.namespaceURI, "nsr:xns1");
				assert.equal(serialize(node), "<aaa ns2:ns2=\"val2\" xmlns:ns2=\"nsr:xns1\"/>");
			});

			it("prefix not match", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				node.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns2", "nsr:xns2");

				node.setAttributeNS("nsr:xns1", "ns2:ns2", "val2");
				var attr = node.attributes[1];
				assert.equal(attr.prefix, "ns2");
				assert.equal(attr.namespaceURI, "nsr:xns1");
				assert.equal(serialize(node), "<aaa xmlns:ns2=\"nsr:xns2\" a0:ns2=\"val2\" xmlns:a0=\"nsr:xns1\"/>");
			});

			it("prefix match", function() {
				var xdoc = newDoc();

				var node = xdoc.createElement("aaa");
				node.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns2", "nsr:xns2");
				var attr = node.attributes[0];
				assert.equal(attr.prefix, "xmlns");
				assert.equal(attr.namespaceURI, "http://www.w3.org/2000/xmlns/");

				node.setAttributeNS("nsr:xns2", "ns2:ns2", "val2");
				var attr = node.attributes[1];
				assert.equal(attr.prefix, "ns2");
				assert.equal(attr.namespaceURI, "nsr:xns2");
				assert.equal(serialize(node), "<aaa xmlns:ns2=\"nsr:xns2\" ns2:ns2=\"val2\"/>");
			});
		});
	});

	describe("html", function() {
		it("body", function() {
			var hdoc = parseHtml("<div>");
			assert.equal(hdoc.body, hdoc.documentElement.getElementsByTagName("body")[0]);
		});

		it("innerHTML", function() {
			var hdoc = parseHtml("<Div>    <p>abcdef</div>");
			assert.equal(hdoc.body.innerHTML, "<div>    <p>abcdef</p></div>");

			hdoc.body.innerHTML = "<img><br>";
			assert.equal(hdoc.body.firstChild.tagName, "IMG");
		});

		it("textContent", function() {
			var hdoc = parseHtml("<Div>    <p>abcde\nf&lt;</div>");
			assert.equal(hdoc.body.textContent, "    abcde\nf<");

			hdoc.body.textContent = "<img><br>";
			assert.equal(hdoc.body.innerHTML, "&lt;img&gt;&lt;br&gt;");
		});

		it("charset", function() {
			var data = new Buffer('<html><meta charset=gb2312>哈哈哈哈', "gb2312");
			var doc = xml.parse(data, "text/html");
			assert.equal(doc.documentElement.textContent, "哈哈哈哈");

			var data = new Buffer('<html><meta http-equiv=content-type content="text/html; charset=gb2312">哈哈哈哈', "gb2312");
			var doc = xml.parse(data, "text/html");
			assert.equal(doc.documentElement.textContent, "哈哈哈哈");

			var data = new Buffer('<html><meta content="text/html; charset=gb2312" http-equiv=content-type>哈哈哈哈', "gb2312");
			var doc = xml.parse(data, "text/html");
			assert.equal(doc.documentElement.textContent, "哈哈哈哈");

			var data = new Buffer('<html><meta http-equiv=content-type content="text/html; test=111; charset=gb2312; ccc=222">哈哈哈哈', "gb2312");
			var doc = xml.parse(data, "text/html");
			assert.equal(doc.documentElement.textContent, "哈哈哈哈");
		});
	});
});

//test.run(console.DEBUG);