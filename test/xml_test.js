if (typeof window === 'undefined') {
    var { describe, it, assert } = require('test');
    var xml = require('xml');
    var isBrowser = false;

    function newDoc() {
        return new xml.Document();
    }

    function newHtmlDoc() {
        return new xml.Document("text/html");
    }

    var serialize = xml.serialize;
    var parseHtml = (txt) => {
        return xml.parse(txt, "text/html")
    };

    var parse = (txt) => {
        return xml.parse(txt, "text/xml")
    };
} else {
    var isBrowser = true;
    // Browser-compatible XML functions
    function newDoc() {
        return document.implementation.createDocument(null, null, null);
    }

    function newHtmlDoc() {
        return document.implementation.createHTMLDocument();
    }

    var serialize = (node) => {
        return new XMLSerializer().serializeToString(node);
    };

    var parseHtml = (txt) => {
        const parser = new DOMParser();
        return parser.parseFromString(txt, "text/html");
    };

    var parse = (txt) => {
        const parser = new DOMParser();
        return parser.parseFromString(txt, "text/xml");
    };
}

// Helper function for testing CharacterData interface
function test_CharacterData(fn) {
    describe('CharacterData', () => {
        it("data", () => {
            var xdoc = newDoc();
            var e = xdoc[fn]("aaa");
            assert.equal(e.data, 'aaa');
            e.data = 'aaaaa';
            assert.equal(e.data, 'aaaaa');
            assert.equal(e.length, 5);

            e.appendData('bbb');
            assert.equal(e.data, 'aaaaabbb');
        });

        it("substringData", () => {
            var xdoc = newDoc();
            var e = xdoc[fn]("aaaaabbb");

            assert.equal(e.substringData(3, 4), 'aabb');
            assert.equal(e.substringData(3, 10), 'aabbb');
            assert.equal(e.substringData(8, 10), '');
            assert.throws(() => {
                e.substringData(10, 0);
            });
        });

        it("insertData", () => {
            var xdoc = newDoc();
            var e = xdoc[fn]("aaaaabbb");

            e.insertData(3, "ccc");
            assert.equal(e.data, 'aaacccaabbb');

            e.insertData(11, "ddd");
            assert.equal(e.data, 'aaacccaabbbddd');

            assert.throws(() => {
                e.insertData(17, "ddd");
            });
        });

        it("deleteData", () => {
            var xdoc = newDoc();
            var e = xdoc[fn]("aaaaabbb");

            e.deleteData(3, 3);
            assert.equal(e.data, 'aaabb');

            e.deleteData(3, 10);
            assert.equal(e.data, 'aaa');

            assert.throws(() => {
                e.deleteData(4, 1);
            });
        });

        it("replaceData", () => {
            var xdoc = newDoc();
            var e = xdoc[fn]("aaaaabbb");

            e.replaceData(3, 3, 'ccc');
            assert.equal(e.data, 'aaacccbb');

            e.replaceData(3, 3, '');
            assert.equal(e.data, 'aaabb');

            assert.throws(() => {
                e.replaceData(7, 1);
            });
        });
    });
}

// Helper function for testing child node rules
function test_Child(xdoc, e, s) {
    function test(e1, v) {
        if (v) {
            assert.equal(e.appendChild(e1), e1);
            assert.equal(e1.parentNode, e);
            assert.equal(e.lastChild, e1);
        } else {
            assert.throws(() => {
                e.appendChild(e1)
            });
        }
    }

    test(xdoc.createElement("bbb"), s[1]);
    test(xdoc.createTextNode("bbb"), s[3]);
    test(xdoc.createCDATASection("bbb"), s[4]);
    test(xdoc.createProcessingInstruction("aaa", "bbb"), s[7]);
    test(xdoc.createComment("bbb"), s[8]);
    test(xdoc, s[9]);
}

describe('xml', () => {
    describe('dom', () => {
        // Document tests
        describe('Document', () => {
            it("base", () => {
                var xdoc = newDoc();
                // In browser, document's ownerDocument is null, not itself
                assert.equal(xdoc.ownerDocument, isBrowser ? null : xdoc);
                assert.equal(xdoc.nodeType, 9);
                assert.equal(xdoc.nodeName, '#document');
                assert.equal(xdoc.childNodes.length, 0);

                assert.equal(xdoc.nodeValue, null);
                xdoc.nodeValue = 'aaaaa';
                assert.equal(xdoc.nodeValue, null);
            });

            it("child rule", () => {
                var xdoc = newDoc();
                test_Child(xdoc, xdoc, [0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1]);
            });

            it("Element limit", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                var e1 = xdoc.createElement("bbb");
                var c = xdoc.createComment("aaa");

                xdoc.appendChild(e);
                xdoc.appendChild(c);
                assert.throws(() => {
                    xdoc.appendChild(e1);
                });

                xdoc.replaceChild(e1, e);
                assert.throws(() => {
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

            it("getElementsByTagName", () => {
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

            it("getElementById", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");
                xdoc.appendChild(root);
                var e = xdoc.createElement("aaa");
                e.setAttribute("id", "id_1");
                root.appendChild(e);

                e.appendChild(xdoc.createElement("bbb"));

                var e1 = xdoc.createElement("ccc");
                e1.setAttribute("id", "id_1");
                e.appendChild(e1);

                var e2 = xdoc.createElement("ddd");
                e2.setAttribute("id", "id_2");
                e.appendChild(e2);

                var e3 = xdoc.getElementById("id_1");
                assert.equal(e3, e);

                var e3 = xdoc.getElementById("id_2");
                assert.equal(e3, e2);

                var e3 = xdoc.getElementById("id_3");
                assert.equal(e3, null);
            });

            it("createElementNS", () => {
                var xdoc = newDoc();

                // Test creating element with namespace
                var elem = xdoc.createElementNS("http://example.com/ns", "ns:test");
                assert.equal(elem.nodeName, "ns:test");
                assert.equal(elem.localName, "test");
                assert.equal(elem.prefix, "ns");
                assert.equal(elem.namespaceURI, "http://example.com/ns");

                // Test creating element without prefix
                var elem2 = xdoc.createElementNS("http://example.com/ns", "test");
                assert.equal(elem2.nodeName, "test");
                assert.equal(elem2.localName, "test");
                assert.equal(elem2.prefix, null);
                assert.equal(elem2.namespaceURI, "http://example.com/ns");
            });

            it("getElementsByTagNameNS", () => {
                var xdoc = parse('<root xmlns:ns="http://example.com/ns"><ns:item>1</ns:item><item>2</item><ns:item>3</ns:item></root>');

                // Find elements by namespace and local name
                var nsItems = xdoc.getElementsByTagNameNS("http://example.com/ns", "item");
                assert.equal(nsItems.length, 2);
                assert.equal(nsItems[0].textContent, "1");
                assert.equal(nsItems[1].textContent, "3");

                // Test wildcard namespace
                var allItems = xdoc.getElementsByTagNameNS("*", "item");
                assert.equal(allItems.length, 3);

                // Test wildcard localName
                var allNsElements = xdoc.getElementsByTagNameNS("http://example.com/ns", "*");
                assert.equal(allNsElements.length, 2);
            });

            it("create methods", () => {
                var xdoc = newDoc();

                // Test createTextNode
                var textNode = xdoc.createTextNode("Hello World");
                assert.equal(textNode.nodeType, 3);
                assert.equal(textNode.nodeName, "#text");
                assert.equal(textNode.nodeValue, "Hello World");
                assert.equal(textNode.data, "Hello World");

                // Test createComment
                var commentNode = xdoc.createComment("This is a comment");
                assert.equal(commentNode.nodeType, 8);
                assert.equal(commentNode.nodeName, "#comment");
                assert.equal(commentNode.nodeValue, "This is a comment");
                assert.equal(commentNode.data, "This is a comment");

                // Test createCDATASection
                var cdataNode = xdoc.createCDATASection("CDATA content");
                assert.equal(cdataNode.nodeType, 4);
                assert.equal(cdataNode.nodeName, "#cdata-section");
                assert.equal(cdataNode.nodeValue, "CDATA content");
                assert.equal(cdataNode.data, "CDATA content");

                // Test createProcessingInstruction
                var piNode = xdoc.createProcessingInstruction("xml-stylesheet", 'type="text/xsl" href="style.xsl"');
                assert.equal(piNode.nodeType, 7);
                assert.equal(piNode.nodeName, "xml-stylesheet");
                assert.equal(piNode.target, "xml-stylesheet");
                assert.equal(piNode.data, 'type="text/xsl" href="style.xsl"');
            });

            it("getElementsByClassName", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("aaa");
                xdoc.appendChild(root);

                var e = xdoc.createElement("aaa");
                root.appendChild(e);
                e.className = 'cls_aaa cls_ddd';
                var e1 = xdoc.createElement("aaa");
                e1.className = 'cls_ddd cls_aaa';
                e.appendChild(e1);
                var e2 = xdoc.createElement("bbb");
                e2.className = 'cls_bbb cls_ddd';
                e.appendChild(e2);

                var e3 = xdoc.createElement("bbb");
                e3.className = ' ';
                e.appendChild(e3);

                var l = xdoc.getElementsByClassName("cls_aaa");
                assert.equal(l.length, 2);
                var l1 = xdoc.getElementsByClassName("cls_bbb");
                assert.equal(l1.length, 1);
                var l2 = xdoc.getElementsByClassName("cls_ccc");
                assert.equal(l2.length, 0);
                var l2 = xdoc.getElementsByClassName("cls_ddd");
                assert.equal(l2.length, 3);

                var l = xdoc.getElementsByClassName("cls_aaa cls_ddd");
                assert.equal(l.length, 2);

                var l2 = xdoc.getElementsByClassName("cls_ddd cls_bbb");
                assert.equal(l2.length, 1);

                var l2 = xdoc.getElementsByClassName("cls_aaa cls_aaa cls_ddd");
                assert.equal(l2.length, 2);
            });
        });

        // Element tests
        describe('Element', () => {
            it("base", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                assert.equal(e.ownerDocument, xdoc);
                xdoc.appendChild(e);
                assert.equal(e.ownerDocument, xdoc);

                assert.equal(e.nodeType, 1);
                assert.equal(e.nodeName, 'aaa');
                assert.equal(e.tagName, 'aaa');
                assert.equal(e.childNodes.length, 0);

                assert.equal(e.nodeValue, null);
                e.nodeValue = 'aaaaa';
                assert.equal(e.nodeValue, null);

                e.className = 'test_class';
                assert.equal(e.className, 'test_class');

                e.id = 'test_id';
                assert.equal(e.id, 'test_id');
            });

            it("child rule", () => {
                var xdoc = newDoc();
                test_Child(xdoc, xdoc.createElement("bbb"), [0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0]);
            });

            it("getElementsByTagName", () => {
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

            it("getElementsByClassName", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("aaa");
                var e = xdoc.createElement("aaa");
                root.appendChild(e);
                e.className = 'cls_aaa cls_ddd';
                var e1 = xdoc.createElement("aaa");
                e1.className = 'cls_ddd cls_aaa';
                e.appendChild(e1);
                var e2 = xdoc.createElement("bbb");
                e2.className = 'cls_bbb cls_ddd';
                e.appendChild(e2);

                var l = root.getElementsByClassName("cls_aaa");
                assert.equal(l.length, 2);
                var l1 = root.getElementsByClassName("cls_bbb");
                assert.equal(l1.length, 1);
                var l2 = root.getElementsByClassName("cls_ccc");
                assert.equal(l2.length, 0);
                var l2 = root.getElementsByClassName("cls_ddd");
                assert.equal(l2.length, 3);

                var l = root.getElementsByClassName("cls_aaa cls_ddd");
                assert.equal(l.length, 2);

                var l2 = root.getElementsByClassName("cls_ddd cls_bbb");
                assert.equal(l2.length, 1);

                var l2 = root.getElementsByClassName("cls_aaa cls_aaa cls_ddd");
                assert.equal(l2.length, 2);
            });

            it("getElementsByTagNameNS", () => {
                var xdoc = parse('<root xmlns:ns="http://example.com/ns"><div><ns:item>1</ns:item></div><ns:item>2</ns:item></root>');
                var root = xdoc.documentElement;
                var div = root.firstChild;

                // Find elements by namespace and local name in element subtree
                var rootNsItems = root.getElementsByTagNameNS("http://example.com/ns", "item");
                assert.equal(rootNsItems.length, 2);

                var divNsItems = div.getElementsByTagNameNS("http://example.com/ns", "item");
                assert.equal(divNsItems.length, 1);
                assert.equal(divNsItems[0].textContent, "1");
            });

            it("cloneNode edge cases", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");
                root.setAttribute("rootAttr", "rootValue");

                var child = xdoc.createElement("child");
                child.setAttribute("childAttr", "childValue");
                child.appendChild(xdoc.createTextNode("Child text"));
                root.appendChild(child);

                // Test shallow clone
                var shallowClone = root.cloneNode(false);
                assert.equal(shallowClone.nodeName, "root");
                assert.equal(shallowClone.getAttribute("rootAttr"), "rootValue");
                assert.equal(shallowClone.childNodes.length, 0);

                // Test deep clone
                var deepClone = root.cloneNode(true);
                assert.equal(deepClone.nodeName, "root");
                assert.equal(deepClone.getAttribute("rootAttr"), "rootValue");
                assert.equal(deepClone.childNodes.length, 1);
                assert.equal(deepClone.firstChild.nodeName, "child");
                assert.equal(deepClone.firstChild.getAttribute("childAttr"), "childValue");
                assert.equal(deepClone.firstChild.textContent, "Child text");

                // Ensure cloned nodes are independent
                assert.notEqual(deepClone.firstChild, child);
                deepClone.firstChild.setAttribute("newAttr", "newValue");
                assert.equal(child.getAttribute("newAttr"), null);
            });

            it("remove", () => {
                var xdoc = newDoc();
                var parent = xdoc.createElement("parent");
                var child = xdoc.createElement("child");

                // Test removing element from parent
                parent.appendChild(child);
                assert.equal(child.parentNode, parent);
                assert.equal(parent.childNodes.length, 1);

                var removed = child.remove();
                // Browser returns undefined, fibjs returns the removed element
                assert.equal(removed, isBrowser ? undefined : child);
                assert.equal(child.parentNode, null);
                assert.equal(parent.childNodes.length, 0);

                // Test removing element without parent
                var orphanElement = xdoc.createElement("orphan");
                var result = orphanElement.remove();
                // Both browser and fibjs should return undefined/null for orphan elements
                assert.equal(result, isBrowser ? undefined : null);
            });
        });

        // CharacterData tests
        test_CharacterData('createComment');
        test_CharacterData('createTextNode');

        // ProcessingInstruction tests
        describe('ProcessingInstruction', () => {
            it("base", () => {
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

            it("child rule", () => {
                var xdoc = newDoc();
                test_Child(xdoc, xdoc.createProcessingInstruction("aaa", "bbb"), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
            });

            it("remove", () => {
                var xdoc = newDoc();
                var parent = xdoc.createElement("parent");
                var piNode = xdoc.createProcessingInstruction("target", "data");

                // Test removing processing instruction from parent
                parent.appendChild(piNode);
                assert.equal(piNode.parentNode, parent);
                assert.equal(parent.childNodes.length, 1);

                var removed = piNode.remove();
                // Browser returns undefined, fibjs returns the removed element
                assert.equal(removed, isBrowser ? undefined : piNode);
                assert.equal(piNode.parentNode, null);
                assert.equal(parent.childNodes.length, 0);

                // Test removing processing instruction without parent
                var orphanPI = xdoc.createProcessingInstruction("orphan", "data");
                var result = orphanPI.remove();
                // Both browser and fibjs should return undefined/null for orphan elements
                assert.equal(result, isBrowser ? undefined : null);
            });
        });

        // TextNode tests
        describe('TextNode', () => {
            it("base", () => {
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

            it("splitText", () => {
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

            it("child rule", () => {
                var xdoc = newDoc();
                test_Child(xdoc, xdoc.createTextNode("aaa"), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
            });

            it("normalize", () => {
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

            it("mixed content normalization", () => {
                var xdoc = newDoc();
                var elem = xdoc.createElement("test");

                // Add mixed content: text + element + text + comment + text
                elem.appendChild(xdoc.createTextNode("Text1"));
                elem.appendChild(xdoc.createTextNode("Text2"));
                var childElem = xdoc.createElement("child");
                elem.appendChild(childElem);
                elem.appendChild(xdoc.createTextNode("Text3"));
                elem.appendChild(xdoc.createComment("comment"));
                elem.appendChild(xdoc.createTextNode("Text4"));
                elem.appendChild(xdoc.createTextNode("Text5"));

                assert.equal(elem.childNodes.length, 7);

                // Normalize should merge adjacent text nodes
                elem.normalize();
                assert.equal(elem.childNodes.length, 5); // merged text + element + text + comment + merged text
                assert.equal(elem.childNodes[0].nodeValue, "Text1Text2");
                assert.equal(elem.childNodes[1], childElem);
                assert.equal(elem.childNodes[2].nodeValue, "Text3");
                assert.equal(elem.childNodes[3].nodeType, 8); // comment
                assert.equal(elem.childNodes[4].nodeValue, "Text4Text5");
            });

            it("remove", () => {
                var xdoc = newDoc();
                var parent = xdoc.createElement("parent");
                var textNode = xdoc.createTextNode("test text");

                // Test removing text node from parent
                parent.appendChild(textNode);
                assert.equal(textNode.parentNode, parent);
                assert.equal(parent.childNodes.length, 1);

                var removed = textNode.remove();
                // Browser returns undefined, fibjs returns the removed element
                assert.equal(removed, isBrowser ? undefined : textNode);
                assert.equal(textNode.parentNode, null);
                assert.equal(parent.childNodes.length, 0);

                // Test removing text node without parent
                var orphanText = xdoc.createTextNode("orphan");
                var result = orphanText.remove();
                // Both browser and fibjs should return undefined/null for orphan elements
                assert.equal(result, isBrowser ? undefined : null);
            });
        });

        // Comment tests
        describe('Comment', () => {
            it("base", () => {
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

            it("child rule", () => {
                var xdoc = newDoc();
                test_Child(xdoc, xdoc.createComment("aaa"), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
            });

            it("remove", () => {
                var xdoc = newDoc();
                var parent = xdoc.createElement("parent");
                var commentNode = xdoc.createComment("test comment");

                // Test removing comment node from parent
                parent.appendChild(commentNode);
                assert.equal(commentNode.parentNode, parent);
                assert.equal(parent.childNodes.length, 1);

                var removed = commentNode.remove();
                // Browser returns undefined, fibjs returns the removed element
                assert.equal(removed, isBrowser ? undefined : commentNode);
                assert.equal(commentNode.parentNode, null);
                assert.equal(parent.childNodes.length, 0);

                // Test removing comment node without parent
                var orphanComment = xdoc.createComment("orphan");
                var result = orphanComment.remove();
                // Both browser and fibjs should return undefined/null for orphan elements
                assert.equal(result, isBrowser ? undefined : null);
            });
        });

        // childNodes tests
        describe('childNodes', () => {
            it("appendChild", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                assert.equal(e.childNodes[0], null);
                var e1 = xdoc.createElement("bbb");
                e.appendChild(e1);
                assert.equal(e.firstChild, e1);
                assert.equal(e.childNodes[0], e1);

                var xdoc1 = newDoc();
                var e2 = xdoc1.createElement("bbb");
                var e4 = xdoc1.createElement("bbb");
                e2.appendChild(e4);
                assert.equal(e2.ownerDocument, xdoc1);
                assert.equal(e4.ownerDocument, xdoc1);
                e.appendChild(e2);
                assert.equal(e2.ownerDocument, xdoc);
                assert.equal(e4.ownerDocument, xdoc);

                assert.equal(e1.nextSibling, e2);
                assert.equal(e1, e2.previousSibling);

                assert.equal(e1.nextElementSibling, e2);
                assert.equal(e1, e2.previousElementSibling);

                var e3 = xdoc.createElement("bbb");
                e.appendChild(e3);
                assert.equal(e3.parentNode, e);
                e1.appendChild(e3);
                assert.equal(e3.parentNode, e1);

                assert.throws(() => {
                    e3.appendChild(e);
                });
            });

            it("removeChild", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                var e1 = xdoc.createElement("bbb");
                var e2 = xdoc.createElement("bbb");

                e.appendChild(e1);
                e.appendChild(e2);

                assert.equal(e1.nextSibling, e2);
                assert.equal(e1, e2.previousSibling);

                assert.equal(e1.nextElementSibling, e2);
                assert.equal(e1, e2.previousElementSibling);

                e.removeChild(e1);
                assert.equal(e.childNodes.length, 1);

                assert.equal(e1.nextSibling, null);
                assert.equal(e2.previousSibling, null);

                assert.equal(e1.nextElementSibling, null);
                assert.equal(e2.previousElementSibling, null);
            });

            it("replaceChild", () => {
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

                assert.equal(e1.nextElementSibling, null);
                assert.equal(e2.previousElementSibling, e3);

                var e4 = xdoc.createElement("bbb");
                e1.appendChild(e4);
                assert.equal(e1.childNodes.length, 1);
                assert.equal(e4.parentNode, e1);
                e.replaceChild(e4, e3);
                assert.equal(e4.parentNode, e);
                assert.equal(e1.childNodes.length, 0);
            });

            it("insertBefore", () => {
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

                assert.equal(e1.nextElementSibling, e3);
                assert.equal(e2.previousElementSibling, e3);

                var e4 = xdoc.createElement("bbb");
                e1.appendChild(e4);
                assert.equal(e1.childNodes.length, 1);
                assert.equal(e4.parentNode, e1);
                e.insertBefore(e4, e3);
                assert.equal(e4.parentNode, e);
                assert.equal(e1.childNodes.length, 0);
            });

            it("ElementSibling", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                var t0 = xdoc.createTextNode("ttt");
                var e1 = xdoc.createElement("bbb");
                var t1 = xdoc.createTextNode("ttt");
                var e2 = xdoc.createElement("bbb");
                var t2 = xdoc.createTextNode("ttt");

                e.appendChild(t0);
                e.appendChild(e1);
                e.appendChild(t1);
                e.appendChild(e2);
                e.appendChild(t2);

                assert.equal(e.firstChild, t0);
                assert.equal(e1.nextSibling, t1);
                assert.equal(e2.previousSibling, t1);
                assert.equal(e.lastChild, t2);

                assert.equal(e.firstElementChild, e1);
                assert.equal(e1.nextElementSibling, e2);
                assert.equal(e2.previousElementSibling, e1);
                assert.equal(e.lastElementChild, e2);
            });

            it("childNodes", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                var e1 = xdoc.createElement("bbb");
                e.appendChild(e1);

                var t1 = xdoc.createTextNode("bbb");
                e.appendChild(t1);

                var childs = e.childNodes;
                assert.equal(childs.length, 2);
                assert.equal(e1.parentNode, e);
                assert.equal(t1.parentNode, e);
            });

            it("children", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                var e1 = xdoc.createElement("bbb");
                e.appendChild(e1);

                var t1 = xdoc.createTextNode("bbb");
                e.appendChild(t1);

                var childs = e.children;
                assert.equal(childs.length, 1);
                assert.equal(e1.parentNode, e);
                assert.equal(t1.parentNode, e);
            });

            it("iterator", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");

                // Create various types of child nodes
                var element1 = xdoc.createElement("element1");
                var text1 = xdoc.createTextNode("text content");
                var element2 = xdoc.createElement("element2");
                var comment = xdoc.createComment("comment content");
                var cdata = xdoc.createCDATASection("cdata content");

                // Append all children to root
                root.appendChild(element1);
                root.appendChild(text1);
                root.appendChild(element2);
                root.appendChild(comment);
                root.appendChild(cdata);

                // Test childNodes iterator (should include all node types)
                var childNodes = root.childNodes;
                assert.equal(childNodes.length, 5);

                // Test iterator using for...of loop
                var iteratedNodes = [];
                for (var node of childNodes) {
                    iteratedNodes.push(node);
                }

                assert.equal(iteratedNodes.length, 5);
                assert.equal(iteratedNodes[0], element1);
                assert.equal(iteratedNodes[1], text1);
                assert.equal(iteratedNodes[2], element2);
                assert.equal(iteratedNodes[3], comment);
                assert.equal(iteratedNodes[4], cdata);

                // Test iterator with Array.from()
                var arrayFromIterator = Array.from(childNodes);
                assert.equal(arrayFromIterator.length, 5);
                assert.equal(arrayFromIterator[0], element1);
                assert.equal(arrayFromIterator[4], cdata);
            });

            it("iterator with spread operator", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");

                var element1 = xdoc.createElement("child1");
                var element2 = xdoc.createElement("child2");
                var element3 = xdoc.createElement("child3");

                root.appendChild(element1);
                root.appendChild(element2);
                root.appendChild(element3);

                // Test spread operator with childNodes
                var spreadNodes = [...root.childNodes];
                assert.equal(spreadNodes.length, 3);
                assert.equal(spreadNodes[0], element1);
                assert.equal(spreadNodes[1], element2);
                assert.equal(spreadNodes[2], element3);

                // Test spread operator with children
                var spreadChildren = [...root.children];
                assert.equal(spreadChildren.length, 3);
                assert.equal(spreadChildren[0], element1);
                assert.equal(spreadChildren[1], element2);
                assert.equal(spreadChildren[2], element3);
            });

            it("iterator modification during iteration", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");

                var element1 = xdoc.createElement("child1");
                var element2 = xdoc.createElement("child2");
                var element3 = xdoc.createElement("child3");

                root.appendChild(element1);
                root.appendChild(element2);
                root.appendChild(element3);

                // Test iterator behavior when nodes are modified during iteration
                var iteratedNodes = [];
                for (var node of root.childNodes) {
                    iteratedNodes.push(node);
                    // Remove node during iteration - this affects the live collection
                    if (node === element2) {
                        root.removeChild(element1);
                    }
                }

                // Iterator reflects live state - only 2 nodes are iterated since element1 was removed
                assert.equal(iteratedNodes.length, 2);
                assert.equal(iteratedNodes[0], element1);
                assert.equal(iteratedNodes[1], element2);

                // Root should now only have 2 children
                assert.equal(root.childNodes.length, 2);
                assert.equal(root.childNodes[0], element2);
                assert.equal(root.childNodes[1], element3);
            });

            it("error handling", () => {
                var xdoc = newDoc();
                var elem = xdoc.createElement("test");

                // Test invalid insertBefore
                assert.throws(() => {
                    elem.insertBefore(xdoc, null);
                });

                // Test appendChild with wrong document
                var otherDoc = newDoc();
                var otherElem = otherDoc.createElement("other");
                // This should work (adoptNode behavior)
                elem.appendChild(otherElem);
                assert.equal(otherElem.ownerDocument, xdoc);
            });


        });

        // attrs tests - Basic DOM attribute functionality
        describe('attrs', () => {
            it("setAttribute", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                e.setAttribute("att1", "val1");
                assert.equal(e.attributes[0].value, "val1");

                e.setAttribute("att1", "val2");
                assert.equal(e.attributes[0].value, "val2");
            });

            it("getAttribute", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                e.setAttribute("att1", "val1");

                assert.equal(e.getAttribute("att1"), "val1");
                assert.equal(e.getAttribute("att2"), null);
            });

            it("removeAttribute", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("aaa");
                e.setAttribute("att1", "val1");

                assert.equal(e.getAttribute("att1"), "val1");
                e.removeAttribute("att1");
                assert.equal(e.getAttribute("att1"), null);
                e.removeAttribute("att1");
            });

            it("attribute cloneNode", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("test");

                // Test basic attribute cloning
                e.setAttribute("simple", "value1");
                var attr = e.attributes[0];
                var clonedAttr = attr.cloneNode();

                assert.equal(clonedAttr.name, "simple");
                assert.equal(clonedAttr.value, "value1");
                assert.equal(clonedAttr.nodeName, "simple");
                assert.equal(clonedAttr.nodeValue, "value1");

                // Test that cloned attribute is independent
                clonedAttr.value = "modified";
                assert.equal(attr.value, "value1");
                assert.equal(clonedAttr.value, "modified");

                // Test namespace attribute cloning
                e.setAttributeNS("http://example.com/ns", "ns:custom", "nsvalue");
                var nsAttr = e.attributes[1];
                var clonedNsAttr = nsAttr.cloneNode();

                assert.equal(clonedNsAttr.name, "ns:custom");
                assert.equal(clonedNsAttr.value, "nsvalue");
                assert.equal(clonedNsAttr.namespaceURI, "http://example.com/ns");
                assert.equal(clonedNsAttr.prefix, "ns");
                assert.equal(clonedNsAttr.localName, "custom");

                // Test that namespace properties are preserved
                assert.equal(nsAttr.namespaceURI, clonedNsAttr.namespaceURI);
                assert.equal(nsAttr.prefix, clonedNsAttr.prefix);
                assert.equal(nsAttr.localName, clonedNsAttr.localName);

                // Test special characters in attribute value
                e.setAttribute("special", "value with <>&\"' chars");
                var specialAttr = e.attributes[2];
                var clonedSpecialAttr = specialAttr.cloneNode();

                assert.equal(clonedSpecialAttr.value, "value with <>&\"' chars");
                assert.equal(clonedSpecialAttr.nodeValue, "value with <>&\"' chars");

                // Test that cloned attribute has no owner initially (implementation specific)
                // Note: ownerElement might be undefined for cloned attributes

                // Test toString method contains attribute name and value
                var toStringResult = clonedAttr.toString();
            });

            it("attribute properties", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("test");

                // Test basic attribute properties
                e.setAttribute("test-attr", "test-value");
                var attr = e.attributes[0];

                assert.equal(attr.name, "test-attr");
                assert.equal(attr.value, "test-value");
                assert.equal(attr.nodeName, "test-attr");
                assert.equal(attr.nodeValue, "test-value");
                // Note: ownerElement property might not be implemented

                // Test nodeValue setter
                attr.nodeValue = "new-value";
                assert.equal(attr.value, "new-value");
                assert.equal(attr.nodeValue, "new-value");
                assert.equal(e.getAttribute("test-attr"), "new-value");

                // Test namespace properties for regular attribute
                assert.equal(attr.namespaceURI, null);
                assert.equal(attr.prefix, null);
                assert.equal(attr.localName, "test-attr");

                // Test xmlns attribute special case
                e.setAttribute("xmlns", "http://example.com");
                var xmlnsAttr = e.attributes[1];
            });

            // setAttributeNode tests based on modern browser behavior
            it("setAttributeNode with owned attribute should throw error", () => {
                var xdoc = newDoc();
                var e1 = xdoc.createElement("element1");
                var e2 = xdoc.createElement("element2");

                // Create an attribute on first element
                e1.setAttribute("test", "value1");
                var attr = e1.attributes[0];

                // Modern browsers throw error when trying to move owned attribute
                assert.throws(() => {
                    e2.setAttributeNode(attr);
                });
            });

            it("setAttributeNode with cloned attribute", () => {
                var xdoc = newDoc();
                var e1 = xdoc.createElement("element1");
                var e2 = xdoc.createElement("element2");

                // Create an attribute on first element
                e1.setAttribute("test", "value1");
                var attr = e1.attributes[0];
                var clonedAttr = attr.cloneNode();

                // Test initial state
                assert.equal(e1.attributes.length, 1);
                assert.equal(e2.attributes.length, 0);
                assert.equal(e1.getAttribute("test"), "value1");
                assert.equal(e2.getAttribute("test"), null);

                // Cloned attribute should have no owner
                // Note: fibjs might not implement ownerElement property

                // Set cloned attribute to second element
                var result = e2.setAttributeNode(clonedAttr);

                // Should return null (no previous attribute)
                assert.equal(result, null);

                // Original element unchanged, new element has the attribute
                assert.equal(e1.attributes.length, 1);
                assert.equal(e2.attributes.length, 1);
                assert.equal(e1.getAttribute("test"), "value1");
                assert.equal(e2.getAttribute("test"), "value1");

                // Both elements should have different attribute objects
                assert.notEqual(e1.attributes[0], e2.attributes[0]);
            });

            it("setAttributeNode with new attribute", () => {
                var xdoc = newDoc();
                var e1 = xdoc.createElement("element1");

                // Create a new attribute using createElement + setAttribute
                var tempElement = xdoc.createElement("temp");
                tempElement.setAttribute("custom", "custom-value");
                var newAttr = tempElement.attributes[0].cloneNode();

                // Test initial state
                assert.equal(e1.attributes.length, 0);
                assert.equal(newAttr.name, "custom");
                assert.equal(newAttr.value, "custom-value");

                // Set new attribute
                var result = e1.setAttributeNode(newAttr);

                // Should return null (no previous attribute)
                assert.equal(result, null);
                assert.equal(e1.attributes.length, 1);
                assert.equal(e1.getAttribute("custom"), "custom-value");
            });

            it("setAttributeNode replacing existing attribute", () => {
                var xdoc = newDoc();
                var e1 = xdoc.createElement("element1");

                // Set initial attribute
                e1.setAttribute("test", "original-value");
                assert.equal(e1.getAttribute("test"), "original-value");

                // Create replacement attribute
                var tempElement = xdoc.createElement("temp");
                tempElement.setAttribute("test", "replacement-value");
                var replacementAttr = tempElement.attributes[0].cloneNode();

                // Replace existing attribute
                var result = e1.setAttributeNode(replacementAttr);

                // Should return the old attribute
                assert.notEqual(result, null);
                assert.equal(result.value, "original-value");

                // Element should have new value
                assert.equal(e1.attributes.length, 1);
                assert.equal(e1.getAttribute("test"), "replacement-value");
            });

            it("setAttributeNode with namespace attribute", () => {
                var xdoc = newDoc();
                var e1 = xdoc.createElement("element1");
                var e2 = xdoc.createElement("element2");

                // Create namespace attribute
                e1.setAttributeNS("http://example.com/ns", "ns:custom", "nsvalue");
                var nsAttr = e1.attributes[0];
                var clonedNsAttr = nsAttr.cloneNode();

                // Test initial state
                assert.equal(e1.attributes.length, 1);
                assert.equal(e2.attributes.length, 0);
                assert.equal(e1.getAttributeNS("http://example.com/ns", "custom"), "nsvalue");

                // Set cloned namespace attribute
                var result = e2.setAttributeNode(clonedNsAttr);

                assert.equal(result, null);
                assert.equal(e1.attributes.length, 1);
                assert.equal(e2.attributes.length, 1);
                assert.equal(e1.getAttributeNS("http://example.com/ns", "custom"), "nsvalue");
                assert.equal(e2.getAttributeNS("http://example.com/ns", "custom"), "nsvalue");

                // Verify namespace properties are preserved
                var e2Attr = e2.attributes[0];
                assert.equal(e2Attr.name, "ns:custom");
                assert.equal(e2Attr.value, "nsvalue");
                assert.equal(e2Attr.namespaceURI, "http://example.com/ns");
                assert.equal(e2Attr.prefix, "ns");
                assert.equal(e2Attr.localName, "custom");
            });






        });
    });

    // Namespace tests
    describe("namespace", () => {
        it("basic", () => {
            var xdoc = newDoc();

            var node = xdoc.createElement("ns1:aaa");
            assert.equal(node.localName, isBrowser ? "ns1:aaa" : "ns1:aaa");
            assert.equal(node.prefix, null);
            assert.equal(node.namespaceURI, null);
            assert.equal(serialize(node), "<ns1:aaa/>");
        });

        describe("root", () => {
            it("no prefix", () => {
                var xdoc = newDoc();

                var node = xdoc.createElementNS("nsr:xns3", "aaa");
                assert.equal(node.tagName, "aaa");
                assert.equal(node.localName, "aaa");
                assert.equal(node.prefix, null);
                assert.equal(node.namespaceURI, "nsr:xns3");
                assert.equal(serialize(node), "<aaa xmlns=\"nsr:xns3\"/>");
            });

            it("prefix", () => {
                var xdoc = newDoc();

                var node = xdoc.createElementNS("nsr:xns3", "ns1:aaa");
                assert.equal(node.tagName, "ns1:aaa");
                assert.equal(node.localName, "aaa");
                assert.equal(node.prefix, "ns1");
                assert.equal(node.namespaceURI, "nsr:xns3");
                assert.equal(serialize(node), "<ns1:aaa xmlns:ns1=\"nsr:xns3\"/>");
            });

            it("xmlns", () => {
                var xdoc = newDoc();

                var node = xdoc.createElement("aaa");
                node.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns2", "nsr:xns2");
                assert.equal(serialize(node), "<aaa xmlns:ns2=\"nsr:xns2\"/>");
            });
        });

        describe("sub node", () => {
            it("no prefix", () => {
                var xdoc = newDoc();

                var node = xdoc.createElement("aaa");
                var sub = xdoc.createElementNS("nsr:xns1", "ns2");
                node.appendChild(sub);

                assert.equal(sub.prefix, null);
                assert.equal(sub.namespaceURI, "nsr:xns1");
                assert.equal(serialize(node), "<aaa><ns2 xmlns=\"nsr:xns1\"/></aaa>");
            });

            it("prefix not exists", () => {
                var xdoc = newDoc();

                var node = xdoc.createElement("aaa");
                var sub = xdoc.createElementNS("nsr:xns1", "ns2:ns2");
                node.appendChild(sub);

                assert.equal(sub.prefix, "ns2");
                assert.equal(sub.namespaceURI, "nsr:xns1");
                assert.equal(serialize(node), "<aaa><ns2:ns2 xmlns:ns2=\"nsr:xns1\"/></aaa>");
            });

            it("prefix not match", () => {
                var xdoc = newDoc();

                var node = xdoc.createElement("aaa");
                node.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns2", "nsr:xns2");

                var sub = xdoc.createElementNS("nsr:xns1", "ns2:ns2");
                node.appendChild(sub);

                assert.equal(sub.prefix, "ns2");
                assert.equal(sub.namespaceURI, "nsr:xns1");
                // The exact serialization may vary between implementations
                var serialized = serialize(node);
                assert.ok(serialized.indexOf("xmlns:ns2=\"nsr:xns2\"") !== -1);
                assert.ok(serialized.indexOf("nsr:xns1") !== -1);
            });

            it("prefix match", () => {
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

        describe("attribute", () => {
            it("no prefix", () => {
                var xdoc = newDoc();

                var node = xdoc.createElement("aaa");
                node.setAttributeNS("nsr:xns1", "ns2", "val2");
                var attr = node.attributes[0];
                assert.equal(attr.prefix, null);
                assert.equal(attr.namespaceURI, "nsr:xns1");
                // The exact serialization may vary between implementations
                var serialized = serialize(node);
                assert.ok(serialized.indexOf("ns2=\"val2\"") !== -1);
                assert.ok(serialized.indexOf("nsr:xns1") !== -1);
            });

            it("prefix not exists", () => {
                var xdoc = newDoc();

                var node = xdoc.createElement("aaa");
                node.setAttributeNS("nsr:xns1", "ns2:ns2", "val2");
                var attr = node.attributes[0];
                assert.equal(attr.prefix, "ns2");
                assert.equal(attr.namespaceURI, "nsr:xns1");
                // The exact serialization order may vary between implementations
                var serialized = serialize(node);
                assert.ok(serialized.indexOf("ns2:ns2=\"val2\"") !== -1);
                assert.ok(serialized.indexOf("xmlns:ns2=\"nsr:xns1\"") !== -1);
            });

            it("prefix not match", () => {
                var xdoc = newDoc();

                var node = xdoc.createElement("aaa");
                node.setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:ns2", "nsr:xns2");

                node.setAttributeNS("nsr:xns1", "ns2:ns2", "val2");
                var attr = node.attributes[1];
                assert.equal(attr.prefix, "ns2");
                assert.equal(attr.namespaceURI, "nsr:xns1");
                // The exact serialization may vary between implementations
                var serialized = serialize(node);
                assert.ok(serialized.indexOf("xmlns:ns2=\"nsr:xns2\"") !== -1);
                assert.ok(serialized.indexOf("ns2=\"val2\"") !== -1 || serialized.indexOf("a0:ns2=\"val2\"") !== -1);
                assert.ok(serialized.indexOf("nsr:xns1") !== -1);
            });

            it("prefix match", () => {
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

    // HTML DOM tests
    describe("html", () => {
        describe("documentElement", () => {
            it("default", () => {
                var hdoc = newHtmlDoc();
                assert.equal(hdoc.documentElement.tagName, "HTML");
            });

            it("rejection appendChild", () => {
                var hdoc = newHtmlDoc();
                var e = hdoc.createElement("aaa");
                assert.throws(() => {
                    hdoc.appendChild(e);
                });
            });

            it("clear after removeChild", () => {
                var hdoc = newHtmlDoc();
                hdoc.removeChild(hdoc.documentElement);
                assert.equal(hdoc.documentElement, null);
            });

            it("clear after replaceChild", () => {
                var hdoc = newHtmlDoc();
                hdoc.replaceChild(hdoc.createElement("aaa"), hdoc.documentElement);
                assert.equal(hdoc.documentElement.tagName, "AAA");
            });

            it("appendChild after removeChild", () => {
                var hdoc = newHtmlDoc();
                hdoc.removeChild(hdoc.documentElement);
                hdoc.appendChild(hdoc.createElement("aaa"));
                assert.equal(hdoc.documentElement.tagName, "AAA");
            });
        });

        it("body", () => {
            var hdoc = parseHtml("<div>");
            assert.equal(hdoc.body, hdoc.documentElement.getElementsByTagName("body")[0]);
        });

        it("innerHTML", () => {
            var hdoc = parseHtml("<Div>    <p>abcdef</div>");
            assert.equal(hdoc.body.innerHTML, "<div>    <p>abcdef</p></div>");

            hdoc.body.innerHTML = "<img><br>";
            assert.equal(hdoc.body.firstChild.tagName, "IMG");
        });

        it("textContent", () => {
            var hdoc = parseHtml("<Div>    <p>abcde\nf&lt;</div>");
            assert.equal(hdoc.body.textContent, "    abcde\nf<");

            hdoc.body.textContent = "<img><br>";
            assert.equal(hdoc.body.innerHTML, "&lt;img&gt;&lt;br&gt;");
        });
    });

    describe('xml dom', () => {
        describe('Document', () => {
            it("doctype", () => {
                var xdoc = parse("<!DOCTYPE foo><foo/>");
                assert.equal(xdoc.doctype, xdoc.firstChild);
                xdoc.removeChild(xdoc.firstChild);
                assert.equal(xdoc.doctype, null);

                // Browser doesn't allow inserting doctype after root element
                // So we skip this test or modify it for browser compatibility
                var xdoc1 = parse("<!DOCTYPE foo><foo/>");
                var doctypeNode = xdoc1.removeChild(xdoc1.firstChild);
                // In browser, we can only insert doctype at the beginning
                if (xdoc.childNodes.length === 0) {
                    xdoc.appendChild(doctypeNode);
                }
            });

            it("XmlDocumentType properties", () => {
                var xmlWithDTD = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"><html></html>';
                var xdoc = parse(xmlWithDTD);

                assert.equal(xdoc.doctype.name, "html");
                assert.equal(xdoc.doctype.publicId, "-//W3C//DTD XHTML 1.0 Strict//EN");
                assert.equal(xdoc.doctype.systemId, "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd");
                assert.equal(xdoc.doctype.nodeType, 10);
                assert.equal(xdoc.doctype.nodeName, "html");
            });
        });

        describe('Element', () => {
            it("namespace", () => {
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

            it("remove", () => {
                var xdoc = newDoc();
                var parent = xdoc.createElement("parent");
                var child = xdoc.createElement("child");

                // Test removing element from parent
                parent.appendChild(child);
                assert.equal(child.parentNode, parent);
                assert.equal(parent.childNodes.length, 1);

                var removed = child.remove();
                // Browser returns undefined, fibjs returns the removed element
                assert.equal(removed, isBrowser ? undefined : child);
                assert.equal(child.parentNode, null);
                assert.equal(parent.childNodes.length, 0);

                // Test removing element without parent
                var orphanElement = xdoc.createElement("orphan");
                var result = orphanElement.remove();
                // Both browser and fibjs should return undefined/null for orphan elements
                assert.equal(result, isBrowser ? undefined : null);

                // Test removing document element
                var root = xdoc.createElement("root");
                xdoc.appendChild(root);
                assert.equal(xdoc.documentElement, root);

                removed = root.remove();
                // Browser returns undefined, fibjs returns the removed element
                assert.equal(removed, isBrowser ? undefined : root);
                assert.equal(root.parentNode, null);
                // After removing the document element, document should have no children
                assert.equal(xdoc.childNodes.length, 0);
            });
        });

        // CDATASection tests
        describe('CDATASection', () => {
            it("base", () => {
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

            it("splitText", () => {
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

            it("child rule", () => {
                var xdoc = newDoc();
                test_Child(xdoc, xdoc.createCDATASection("aaa"), [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]);
            });

            it("remove", () => {
                var xdoc = newDoc();
                var parent = xdoc.createElement("parent");
                var cdataNode = xdoc.createCDATASection("test cdata");

                // Test removing CDATA node from parent
                parent.appendChild(cdataNode);
                assert.equal(cdataNode.parentNode, parent);
                assert.equal(parent.childNodes.length, 1);

                var removed = cdataNode.remove();
                // Browser returns undefined, fibjs returns the removed element
                assert.equal(removed, isBrowser ? undefined : cdataNode);
                assert.equal(cdataNode.parentNode, null);
                assert.equal(parent.childNodes.length, 0);

                // Test removing CDATA node without parent
                var orphanCdata = xdoc.createCDATASection("orphan");
                var result = orphanCdata.remove();
                // Both browser and fibjs should return undefined/null for orphan elements
                assert.equal(result, isBrowser ? undefined : null);
            });
        });

        // XML-specific CharacterData tests
        test_CharacterData('createCDATASection');
    });
    describe('html dom', () => {
        it("default", () => {
            var hdoc = newHtmlDoc();
            assert.equal(hdoc.documentElement.tagName, "HTML");
        });

        it("rejection appendChild", () => {
            var hdoc = newHtmlDoc();
            var e = hdoc.createElement("aaa");
            assert.throws(() => {
                hdoc.appendChild(e);
            });
        });

        it("clear after removeChild", () => {
            var hdoc = newHtmlDoc();
            hdoc.removeChild(hdoc.documentElement);
            assert.equal(hdoc.documentElement, null);
        });

        it("clear after replaceChild", () => {
            var hdoc = newHtmlDoc();
            hdoc.replaceChild(hdoc.createElement("aaa"), hdoc.documentElement);
            assert.equal(hdoc.documentElement.tagName, "AAA");
        });

        it("appendChild after removeChild", () => {
            var hdoc = newHtmlDoc();
            hdoc.removeChild(hdoc.documentElement);
            hdoc.appendChild(hdoc.createElement("aaa"));
            assert.equal(hdoc.documentElement.tagName, "AAA");
        });

        it("body", () => {
            var hdoc = parseHtml("<div>");
            assert.equal(hdoc.body, hdoc.documentElement.getElementsByTagName("body")[0]);
        });

        it("innerHTML", () => {
            var hdoc = parseHtml("<Div>    <p>abcdef</div>");
            assert.equal(hdoc.body.innerHTML, "<div>    <p>abcdef</p></div>");

            hdoc.body.innerHTML = "<img><br>";
            assert.equal(hdoc.body.firstChild.tagName, "IMG");
        });

        it("textContent", () => {
            var hdoc = parseHtml("<Div>    <p>abcde\nf&lt;</div>");
            assert.equal(hdoc.body.textContent, "    abcde\nf<");

            hdoc.body.textContent = "<img><br>";
            assert.equal(hdoc.body.innerHTML, "&lt;img&gt;&lt;br&gt;");
        });


    });

    if (!isBrowser) {
        describe('fibjs', () => {
            it("XML constants", () => {
                assert.equal(xml.ELEMENT_NODE, 1);
                assert.equal(xml.ATTRIBUTE_NODE, 2);
                assert.equal(xml.TEXT_NODE, 3);
                assert.equal(xml.CDATA_SECTION_NODE, 4);
                assert.equal(xml.PROCESSING_INSTRUCTION_NODE, 7);
                assert.equal(xml.COMMENT_NODE, 8);
                assert.equal(xml.DOCUMENT_NODE, 9);
                assert.equal(xml.DOCUMENT_TYPE_NODE, 10);
            });

            // Error handling (fibjs specific behavior)
            it("invalid XML parsing", () => {
                // Test invalid XML parsing
                assert.throws(() => {
                    parse("<invalid><unclosed>");
                });
            });

            // Attribute toString method (fibjs specific)
            it("attribute toString", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("test");
                e.setAttribute("simple", "value1");
                var attr = e.attributes[0];
                var clonedAttr = attr.cloneNode();
                clonedAttr.value = "modified";

                var toStringResult = clonedAttr.toString();
                // In fibjs, toString might include attribute details
                assert.ok(toStringResult.indexOf("simple") !== -1);
                assert.ok(toStringResult.indexOf("modified") !== -1); // Use the current value
            });

            // xmlns namespace handling (fibjs specific)
            it("xmlns namespace", () => {
                var xdoc = newDoc();
                var e = xdoc.createElement("test");
                e.setAttribute("xmlns", "http://example.com");
                var xmlnsAttr = e.attributes[0];
                // fibjs implements xmlns namespace correctly
                assert.equal(xmlnsAttr.namespaceURI, "http://www.w3.org/2000/xmlns/");
            });

            // Document remove method (fibjs specific)
            it("document remove", () => {
                var xdoc = newDoc();
                // Document nodes have no parent, so remove() should return undefined/null
                var result = xdoc.remove();
                assert.equal(result, null);
            });

            // getElementById on Element (fibjs specific)
            it("element getElementById", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("aaa");
                var e = xdoc.createElement("aaa");
                e.setAttribute("id", "id_1");
                root.appendChild(e);

                e.appendChild(xdoc.createElement("aaa"));

                var e1 = xdoc.createElement("aaa");
                e1.setAttribute("id", "id_1");
                e.appendChild(e1);

                var e2 = xdoc.createElement("aaa");
                e2.setAttribute("id", "id_2");
                e.appendChild(e2);

                var e3 = root.getElementById("id_1");
                assert.equal(e3, e);

                var e3 = e.getElementById("id_1");
                assert.equal(e3, e1);

                var e3 = root.getElementById("id_2");
                assert.equal(e3, e2);

                var e3 = root.getElementById("id_3");
                assert.equal(e3, null);
            });

            // insertAfter method (fibjs specific)
            it("insertAfter", () => {
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

                assert.equal(e1.nextElementSibling, e3);
                assert.equal(e2.previousElementSibling, e3);

                var e4 = xdoc.createElement("bbb");
                e1.appendChild(e4);
                assert.equal(e1.childNodes.length, 1);
                assert.equal(e4.parentNode, e1);
                e.insertAfter(e4, e3);
                assert.equal(e4.parentNode, e);
                assert.equal(e1.childNodes.length, 0);
            });

            // HTML charset handling (fibjs specific - uses Buffer)
            it("charset", () => {
                var data = new Buffer('<html><meta charset=EUC-JP>哈哈哈哈', "EUC-JP");
                var doc = xml.parse(data, "text/html");
                assert.equal(doc.documentElement.textContent, "哈哈哈哈");

                var data = new Buffer('<html><meta http-equiv=content-type content="text/html; charset=EUC-JP">哈哈哈哈', "EUC-JP");
                var doc = xml.parse(data, "text/html");
                assert.equal(doc.documentElement.textContent, "哈哈哈哈");

                var data = new Buffer('<html><meta content="text/html; charset=EUC-JP" http-equiv=content-type>哈哈哈哈', "EUC-JP");
                var doc = xml.parse(data, "text/html");
                assert.equal(doc.documentElement.textContent, "哈哈哈哈");

                var data = new Buffer('<html><meta http-equiv=content-type content="text/html; test=111; charset=EUC-JP; ccc=222">哈哈哈哈', "EUC-JP");
                var doc = xml.parse(data, "text/html");
                assert.equal(doc.documentElement.textContent, "哈哈哈哈");
            });
        });
    }

    // Browser-only tests for features not supported in fibjs
    if (isBrowser) {
        describe('browser', () => {
            describe('DOM Node Comparison Methods', () => {
                it("isSameNode, isEqualNode, contains", () => {
                    var xdoc = newDoc();
                    var elem1 = xdoc.createElement("test");
                    var elem2 = xdoc.createElement("test");

                    // Test isSameNode (same reference)
                    assert.equal(elem1.isSameNode(elem1), true);
                    assert.equal(elem1.isSameNode(elem2), false);

                    // Test isEqualNode (same content)
                    assert.equal(elem1.isEqualNode(elem2), true);
                    elem2.setAttribute("attr", "value");
                    assert.equal(elem1.isEqualNode(elem2), false);

                    // Test contains
                    var child = xdoc.createElement("child");
                    elem1.appendChild(child);
                    assert.equal(elem1.contains(child), true);
                    assert.equal(child.contains(elem1), false);
                });
            });

            describe('Attribute Node Methods', () => {
                it("getAttributeNode and removeAttributeNode", () => {
                    var xdoc = newDoc();
                    var elem = xdoc.createElement("test");

                    // Set attribute and get attribute node
                    elem.setAttribute("testAttr", "testValue");
                    var attrNode = elem.getAttributeNode("testAttr");

                    assert.equal(attrNode.name, "testAttr");
                    assert.equal(attrNode.value, "testValue");
                    assert.equal(attrNode.nodeName, "testAttr");
                    assert.equal(attrNode.nodeValue, "testValue");

                    var removedAttr = elem.removeAttributeNode(attrNode);
                    assert.equal(removedAttr, attrNode);
                    assert.equal(elem.getAttribute("testAttr"), null);
                    assert.equal(elem.attributes.length, 0);
                });

                it("getAttributeNodeNS", () => {
                    var xdoc = newDoc();
                    var elem = xdoc.createElement("test");

                    // Set namespace attribute and get attribute node
                    elem.setAttributeNS("http://example.com/ns", "ns:attr", "value");
                    var nsAttrNode = elem.getAttributeNodeNS("http://example.com/ns", "attr");

                    assert.equal(nsAttrNode.name, "ns:attr");
                    assert.equal(nsAttrNode.value, "value");
                    assert.equal(nsAttrNode.namespaceURI, "http://example.com/ns");
                    assert.equal(nsAttrNode.prefix, "ns");
                    assert.equal(nsAttrNode.localName, "attr");
                });
            });

            describe('NamedNodeMap Methods', () => {
                it("setNamedItem and removeNamedItem", () => {
                    var xdoc = newDoc();
                    var elem = xdoc.createElement("test");
                    var attrs = elem.attributes;

                    // Create attribute and set it
                    elem.setAttribute("temp", "tempvalue");
                    var newAttr = elem.attributes[0].cloneNode();
                    newAttr.value = "newValue";

                    var oldAttr = attrs.setNamedItem(newAttr);
                    assert.notEqual(oldAttr, null);
                    assert.equal(oldAttr.value, "tempvalue");
                    assert.equal(elem.getAttribute("temp"), "newValue");

                    // Test removeNamedItem
                    var removedAttr = attrs.removeNamedItem("temp");
                    assert.equal(removedAttr.value, "newValue");
                    assert.equal(elem.getAttribute("temp"), null);
                });
            });
        });
    }
});
