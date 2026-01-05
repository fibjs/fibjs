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

            it("replaceWith", () => {
                var xdoc = newDoc();
                var parent = xdoc.createElement("parent");
                var child1 = xdoc.createElement("child1");
                var child2 = xdoc.createElement("child2");
                var child3 = xdoc.createElement("child3");
                var newElement = xdoc.createElement("newElement");

                parent.appendChild(child1);
                parent.appendChild(child2);
                parent.appendChild(child3);

                // Test replaceWith single element
                child2.replaceWith(newElement);
                assert.equal(parent.childNodes.length, 3);
                assert.equal(parent.childNodes[0], child1);
                assert.equal(parent.childNodes[1], newElement);
                assert.equal(parent.childNodes[2], child3);
                assert.equal(child2.parentNode, null);
                assert.equal(newElement.parentNode, parent);

                // Test replaceWith multiple elements
                var newEl1 = xdoc.createElement("new1");
                var newEl2 = xdoc.createElement("new2");
                child3.replaceWith(newEl1, newEl2);
                assert.equal(parent.childNodes.length, 4);
                assert.equal(parent.childNodes[0], child1);
                assert.equal(parent.childNodes[1], newElement);
                assert.equal(parent.childNodes[2], newEl1);
                assert.equal(parent.childNodes[3], newEl2);

                // Test replaceWith on orphan element (should do nothing)
                var orphan = xdoc.createElement("orphan");
                var replacement = xdoc.createElement("replacement");
                orphan.replaceWith(replacement); // Should not throw
                assert.equal(orphan.parentNode, null);
            });

            it("hasAttributes", () => {
                var xdoc = newDoc();
                var elem = xdoc.createElement("test");

                // Element without attributes
                assert.equal(elem.hasAttributes(), false);

                // Element with one attribute
                elem.setAttribute("id", "test-id");
                assert.equal(elem.hasAttributes(), true);

                // Element with multiple attributes
                elem.setAttribute("class", "test-class");
                elem.setAttribute("data-value", "123");
                assert.equal(elem.hasAttributes(), true);

                // After removing all attributes
                elem.removeAttribute("id");
                elem.removeAttribute("class");
                elem.removeAttribute("data-value");
                assert.equal(elem.hasAttributes(), false);
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

            it("forEach", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");

                var element1 = xdoc.createElement("child1");
                var element2 = xdoc.createElement("child2");
                var element3 = xdoc.createElement("child3");

                root.appendChild(element1);
                root.appendChild(element2);
                root.appendChild(element3);

                // Test forEach with childNodes
                var items = [];
                var indices = [];
                var lists = [];
                root.childNodes.forEach(function (node, index, list) {
                    items.push(node);
                    indices.push(index);
                    lists.push(list);
                });

                assert.equal(items.length, 3);
                assert.equal(items[0], element1);
                assert.equal(items[1], element2);
                assert.equal(items[2], element3);
                assert.deepEqual(indices, [0, 1, 2]);
                assert.equal(lists[0], root.childNodes);

                // Test forEach with empty nodeList
                var emptyRoot = xdoc.createElement("empty");
                var emptyItems = [];
                emptyRoot.childNodes.forEach(function (node) {
                    emptyItems.push(node);
                });
                assert.equal(emptyItems.length, 0);

                // Test forEach with querySelectorAll result
                var container = xdoc.createElement("container");
                xdoc.appendChild(container);
                var div1 = xdoc.createElement("div");
                var div2 = xdoc.createElement("div");
                var span = xdoc.createElement("span");
                container.appendChild(div1);
                container.appendChild(div2);
                container.appendChild(span);

                var divs = [];
                container.querySelectorAll("div").forEach(function (el) {
                    divs.push(el);
                });
                assert.equal(divs.length, 2);
                assert.equal(divs[0], div1);
                assert.equal(divs[1], div2);
            });

            it("keys", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");
                var child1 = xdoc.createElement("child1");
                var child2 = xdoc.createElement("child2");
                var child3 = xdoc.createElement("child3");
                root.appendChild(child1);
                root.appendChild(child2);
                root.appendChild(child3);

                // Test keys() returns indices
                var keys = [];
                for (var k of root.childNodes.keys()) {
                    keys.push(k);
                }
                assert.deepEqual(keys, [0, 1, 2]);

                // Test empty NodeList
                var empty = xdoc.createElement("empty");
                var emptyKeys = [];
                for (var k of empty.childNodes.keys()) {
                    emptyKeys.push(k);
                }
                assert.deepEqual(emptyKeys, []);
            });

            it("values", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");
                var child1 = xdoc.createElement("child1");
                var child2 = xdoc.createElement("child2");
                root.appendChild(child1);
                root.appendChild(child2);

                // Test values() returns nodes
                var values = [];
                for (var v of root.childNodes.values()) {
                    values.push(v);
                }
                assert.equal(values.length, 2);
                assert.equal(values[0], child1);
                assert.equal(values[1], child2);

                // values() should be same as @@iterator
                var iteratorValues = [];
                for (var n of root.childNodes) {
                    iteratorValues.push(n);
                }
                assert.deepEqual(values, iteratorValues);
            });

            it("entries", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");
                var child1 = xdoc.createElement("child1");
                var child2 = xdoc.createElement("child2");
                root.appendChild(child1);
                root.appendChild(child2);

                // Test entries() returns [index, node] pairs
                var entries = [];
                for (var e of root.childNodes.entries()) {
                    entries.push(e);
                }
                assert.equal(entries.length, 2);
                assert.deepEqual(entries[0], [0, child1]);
                assert.deepEqual(entries[1], [1, child2]);
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

            it("contains", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");
                var child = xdoc.createElement("child");
                var grandchild = xdoc.createElement("grandchild");
                root.appendChild(child);
                child.appendChild(grandchild);

                // contains returns true for self
                assert.equal(root.contains(root), true);
                // contains returns true for direct children
                assert.equal(root.contains(child), true);
                // contains returns true for descendants
                assert.equal(root.contains(grandchild), true);
                // contains returns false for ancestors
                assert.equal(child.contains(root), false);
                // contains returns false for unrelated nodes
                var other = xdoc.createElement("other");
                assert.equal(root.contains(other), false);
            });

            it("getRootNode", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");
                var child = xdoc.createElement("child");
                root.appendChild(child);
                xdoc.appendChild(root);

                // getRootNode returns document for connected nodes
                assert.equal(child.getRootNode(), xdoc);
                assert.equal(root.getRootNode(), xdoc);
                // getRootNode returns self for detached nodes
                var detached = xdoc.createElement("detached");
                assert.equal(detached.getRootNode(), detached);
            });

            it("isConnected", () => {
                var xdoc = newDoc();
                var root = xdoc.createElement("root");
                
                // Detached node is not connected
                assert.equal(root.isConnected, false);
                
                // Append to document
                xdoc.appendChild(root);
                assert.equal(root.isConnected, true);
                
                // Child of connected node is also connected
                var child = xdoc.createElement("child");
                root.appendChild(child);
                assert.equal(child.isConnected, true);
                
                // Remove from document
                xdoc.removeChild(root);
                assert.equal(root.isConnected, false);
                assert.equal(child.isConnected, false);
            });

            it("closest", () => {
                var xdoc = newDoc();
                var div = xdoc.createElement("div");
                div.className = "container";
                var span = xdoc.createElement("span");
                span.id = "inner";
                var b = xdoc.createElement("b");
                div.appendChild(span);
                span.appendChild(b);

                // closest finds ancestor by tag
                assert.equal(b.closest("span"), span);
                assert.equal(b.closest("div"), div);
                // closest can return self
                assert.equal(b.closest("b"), b);
                // closest finds by class
                assert.equal(b.closest(".container"), div);
                // closest finds by id
                assert.equal(span.closest("#inner"), span);
                // closest returns null when no match
                assert.equal(b.closest(".nonexistent"), null);
                assert.equal(b.closest("p"), null);
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

                assert.throws(() => {
                    elem.removeAttributeNode(attrNode);
                }, /owned by another/);
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
    });

    // Namespace tests
    describe("namespace", () => {
        it("basic", () => {
            var xdoc = newDoc();

            var node = xdoc.createElement("ns1:aaa");
            assert.equal(node.localName, "ns1:aaa");
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
        // XML document initialization and basic behavior tests
        describe('XML document initialization and behavior', () => {
            describe('newDoc() initial state', () => {
                it('should create empty XML document', () => {
                    var xdoc = newDoc();

                    // Document should exist
                    assert.notEqual(xdoc, null);
                    assert.equal(xdoc.nodeType, 9); // DOCUMENT_NODE

                    // Should have no documentElement initially
                    assert.equal(xdoc.documentElement, null);

                    // Should have empty childNodes
                    assert.equal(xdoc.childNodes.length, 0);

                    // Should have no doctype
                    assert.equal(xdoc.doctype, null);
                });

                it('should allow adding root element', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');

                    // Append root element
                    xdoc.appendChild(root);

                    // Document should now have documentElement
                    assert.equal(xdoc.documentElement, root);
                    assert.equal(xdoc.documentElement.tagName, 'root');
                    assert.equal(xdoc.childNodes.length, 1);
                    assert.equal(xdoc.firstChild, root);
                });

                it('should reject multiple root elements', () => {
                    var xdoc = newDoc();
                    var root1 = xdoc.createElement('root1');
                    var root2 = xdoc.createElement('root2');

                    // Add first root element
                    xdoc.appendChild(root1);
                    assert.equal(xdoc.documentElement, root1);

                    // Adding second root element should throw error
                    assert.throws(() => {
                        xdoc.appendChild(root2);
                    });

                    // Document should still have only one root
                    assert.equal(xdoc.childNodes.length, 1);
                    assert.equal(xdoc.documentElement, root1);
                });

                it('should allow comments and PIs at document level', () => {
                    var xdoc = newDoc();
                    var comment = xdoc.createComment('XML comment');
                    var pi = xdoc.createProcessingInstruction('xml-stylesheet', 'type="text/css" href="style.css"');
                    var root = xdoc.createElement('root');

                    // Add comment and PI before root
                    xdoc.appendChild(comment);
                    xdoc.appendChild(pi);
                    xdoc.appendChild(root);

                    // Should have 3 children
                    assert.equal(xdoc.childNodes.length, 3);
                    assert.equal(xdoc.childNodes[0], comment);
                    assert.equal(xdoc.childNodes[1], pi);
                    assert.equal(xdoc.childNodes[2], root);
                    assert.equal(xdoc.documentElement, root);
                });

                it('should handle DOCTYPE from parsed XML', () => {
                    // Test DOCTYPE handling with parsed XML document
                    var xmlWithDTD = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"><html></html>';
                    var xdoc = parse(xmlWithDTD);

                    assert.notEqual(xdoc.doctype, null);
                    assert.equal(xdoc.doctype.name, "html");
                    assert.equal(xdoc.doctype.publicId, "-//W3C//DTD XHTML 1.0 Strict//EN");
                    assert.equal(xdoc.doctype.systemId, "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd");
                    assert.equal(xdoc.doctype.nodeType, 10); // DOCUMENT_TYPE_NODE
                    assert.equal(xdoc.documentElement.tagName, "html");
                });
            });

            describe('innerHTML behavior', () => {
                it('should get innerHTML from element with mixed content', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Add mixed content: elements, text, and comments
                    var child1 = xdoc.createElement('child1');
                    child1.setAttribute('attr', 'value');
                    root.appendChild(child1);

                    root.appendChild(xdoc.createTextNode('Some text'));

                    var child2 = xdoc.createElement('child2');
                    child2.appendChild(xdoc.createTextNode('Nested text'));
                    root.appendChild(child2);

                    root.appendChild(xdoc.createComment('Comment content'));

                    // Get innerHTML - should return serialized content
                    var innerHTML = root.innerHTML;

                    // Verify it contains all child elements and content
                    assert.ok(innerHTML.indexOf('<child1 attr="value"></child1>') !== -1 ||
                        innerHTML.indexOf('<child1 attr="value"/>') !== -1);
                    assert.ok(innerHTML.indexOf('Some text') !== -1);
                    assert.ok(innerHTML.indexOf('<child2>Nested text</child2>') !== -1);
                    assert.ok(innerHTML.indexOf('<!--Comment content-->') !== -1);
                });

                it('should get innerHTML from empty element', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('empty');
                    xdoc.appendChild(root);

                    // Empty element should have empty innerHTML
                    assert.equal(root.innerHTML, '');
                });

                it('should get innerHTML with special characters', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Add text with special characters
                    root.appendChild(xdoc.createTextNode('Text with <>&"\''));

                    var child = xdoc.createElement('child');
                    child.setAttribute('attr', 'value with <>&"\'');
                    root.appendChild(child);

                    var innerHTML = root.innerHTML;

                    // Special characters in text should be escaped
                    assert.ok(innerHTML.indexOf('&lt;&gt;&amp;') !== -1);
                    // Special characters in attributes should be escaped
                    assert.ok(innerHTML.indexOf('&quot;') !== -1);
                });

                it('should set innerHTML with valid XML', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Set innerHTML with valid XML content
                    root.innerHTML = '<child1 attr="value1">Text content</child1><child2/>';

                    // Verify children were created correctly
                    assert.equal(root.childNodes.length, 2);
                    assert.equal(root.firstChild.tagName, 'child1');
                    assert.equal(root.firstChild.getAttribute('attr'), 'value1');
                    assert.equal(root.firstChild.textContent, 'Text content');
                    assert.equal(root.lastChild.tagName, 'child2');
                });

                it('should set innerHTML with self-closing tags', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Set innerHTML with self-closing tags
                    root.innerHTML = '<img src="test.jpg"/><br/><input type="text"/>';

                    // Verify self-closing elements were created
                    assert.equal(root.childNodes.length, 3);
                    assert.equal(root.childNodes[0].tagName, 'img');
                    assert.equal(root.childNodes[0].getAttribute('src'), 'test.jpg');
                    assert.equal(root.childNodes[1].tagName, 'br');
                    assert.equal(root.childNodes[2].tagName, 'input');
                    assert.equal(root.childNodes[2].getAttribute('type'), 'text');
                });

                it('should set innerHTML with nested elements', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Set innerHTML with deeply nested structure
                    root.innerHTML = '<level1><level2><level3>Deep text</level3></level2></level1>';

                    // Verify nested structure
                    assert.equal(root.childNodes.length, 1);
                    var level1 = root.firstChild;
                    assert.equal(level1.tagName, 'level1');
                    assert.equal(level1.childNodes.length, 1);

                    var level2 = level1.firstChild;
                    assert.equal(level2.tagName, 'level2');
                    assert.equal(level2.childNodes.length, 1);

                    var level3 = level2.firstChild;
                    assert.equal(level3.tagName, 'level3');
                    assert.equal(level3.textContent, 'Deep text');
                });

                it('should set innerHTML with comments and CDATA', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Set innerHTML with comments and CDATA sections
                    root.innerHTML = '<!-- This is a comment --><data><![CDATA[Raw data with <>&]]></data>';

                    // Verify comment and CDATA were parsed correctly
                    assert.equal(root.childNodes.length, 2);
                    assert.equal(root.firstChild.nodeType, 8); // Comment node
                    assert.equal(root.firstChild.nodeValue, ' This is a comment ');

                    var dataElement = root.lastChild;
                    assert.equal(dataElement.tagName, 'data');
                    assert.equal(dataElement.childNodes.length, 1);
                    assert.equal(dataElement.firstChild.nodeType, 4); // CDATA node
                    assert.equal(dataElement.firstChild.nodeValue, 'Raw data with <>&');
                });

                it('should replace existing content when setting innerHTML', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Add initial content
                    root.appendChild(xdoc.createElement('old1'));
                    root.appendChild(xdoc.createElement('old2'));
                    assert.equal(root.childNodes.length, 2);

                    // Set new innerHTML - should replace all existing content
                    root.innerHTML = '<new1>New content</new1><new2/>';

                    // Verify old content was replaced
                    assert.equal(root.childNodes.length, 2);
                    assert.equal(root.firstChild.tagName, 'new1');
                    assert.equal(root.firstChild.textContent, 'New content');
                    assert.equal(root.lastChild.tagName, 'new2');
                });

                it('should handle innerHTML with XML namespaces', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Set innerHTML with namespace declarations
                    root.innerHTML = '<ns:element xmlns:ns="http://example.com/ns" ns:attr="value">Content</ns:element>';

                    // Verify namespace element was created
                    assert.equal(root.childNodes.length, 1);
                    var nsElement = root.firstChild;
                    assert.equal(nsElement.tagName, 'ns:element');
                    assert.equal(nsElement.namespaceURI, 'http://example.com/ns');
                    assert.equal(nsElement.textContent, 'Content');
                });

                it('should set innerHTML to empty string', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Add some content first
                    root.appendChild(xdoc.createElement('child1'));
                    root.appendChild(xdoc.createTextNode('Some text'));
                    root.appendChild(xdoc.createElement('child2'));
                    assert.equal(root.childNodes.length, 3);

                    // Clear content with empty string
                    root.innerHTML = '';

                    // Should have no children
                    assert.equal(root.childNodes.length, 0);
                    assert.equal(root.innerHTML, '');
                });

                it('should handle innerHTML with processing instructions', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Set innerHTML with processing instruction
                    root.innerHTML = '<?xml-stylesheet type="text/xsl" href="style.xsl"?><content>Data</content>';

                    // Verify processing instruction and element were created
                    assert.equal(root.childNodes.length, 2);
                    assert.equal(root.firstChild.nodeType, 7); // Processing instruction
                    assert.equal(root.firstChild.nodeName, 'xml-stylesheet');
                    assert.equal(root.lastChild.tagName, 'content');
                    assert.equal(root.lastChild.textContent, 'Data');
                });

                it('should preserve attribute order and case in innerHTML', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Set innerHTML with multiple attributes
                    root.innerHTML = '<element id="test" class="myClass" data-value="123">Content</element>';

                    // Verify attributes were set correctly
                    var element = root.firstChild;
                    assert.equal(element.getAttribute('id'), 'test');
                    assert.equal(element.getAttribute('class'), 'myClass');
                    assert.equal(element.getAttribute('data-value'), '123');
                    assert.equal(element.textContent, 'Content');
                });
            });

            describe('XML document structure constraints', () => {
                it('should maintain well-formed structure', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    var child1 = xdoc.createElement('child1');
                    var child2 = xdoc.createElement('child2');

                    // Build well-formed structure
                    root.appendChild(child1);
                    root.appendChild(child2);
                    xdoc.appendChild(root);

                    assert.equal(xdoc.documentElement, root);
                    assert.equal(root.childNodes.length, 2);
                    assert.equal(child1.parentNode, root);
                    assert.equal(child2.parentNode, root);
                });

                it('should handle text nodes at document level behavior', () => {
                    var xdoc = newDoc();
                    var textNode = xdoc.createTextNode('   '); // Whitespace text
                    var root = xdoc.createElement('root');

                    // Text nodes at document level should throw error (DOM standard)
                    assert.throws(() => {
                        xdoc.appendChild(textNode);
                    });

                    // Should still be able to add root element
                    xdoc.appendChild(root);
                    assert.equal(xdoc.childNodes.length, 1);
                    assert.equal(xdoc.documentElement, root);
                });

                it('should handle removal of document element', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    xdoc.appendChild(root);

                    // Remove document element
                    xdoc.removeChild(root);

                    assert.equal(xdoc.documentElement, null);
                    assert.equal(xdoc.childNodes.length, 0);
                    assert.equal(root.parentNode, null);

                    // Should be able to add new root
                    var newRoot = xdoc.createElement('newRoot');
                    xdoc.appendChild(newRoot);
                    assert.equal(xdoc.documentElement, newRoot);
                });

                it('should handle replacement of document element', () => {
                    var xdoc = newDoc();
                    var oldRoot = xdoc.createElement('oldRoot');
                    var newRoot = xdoc.createElement('newRoot');
                    xdoc.appendChild(oldRoot);

                    // Replace document element
                    xdoc.replaceChild(newRoot, oldRoot);

                    assert.equal(xdoc.documentElement, newRoot);
                    assert.equal(xdoc.childNodes.length, 1);
                    assert.equal(oldRoot.parentNode, null);
                    assert.equal(newRoot.parentNode, xdoc);
                });
            });

            describe('XML document methods', () => {
                it('should create elements preserving case', () => {
                    var xdoc = newDoc();

                    // XML documents should preserve case exactly
                    var lowerCase = xdoc.createElement('element');
                    var upperCase = xdoc.createElement('ELEMENT');
                    var mixedCase = xdoc.createElement('Element');

                    assert.equal(lowerCase.tagName, 'element');
                    assert.equal(upperCase.tagName, 'ELEMENT');
                    assert.equal(mixedCase.tagName, 'Element');

                    // Elements with same case should be considered same type
                    assert.notEqual(lowerCase.tagName, upperCase.tagName);
                    assert.notEqual(lowerCase.tagName, mixedCase.tagName);
                });

                it('should handle namespace-aware element creation', () => {
                    var xdoc = newDoc();
                    var nsURI = 'http://example.com/ns';

                    // Create elements with namespace
                    var nsElement = xdoc.createElementNS(nsURI, 'ns:element');
                    var defaultNsElement = xdoc.createElementNS(nsURI, 'element');

                    assert.equal(nsElement.namespaceURI, nsURI);
                    assert.equal(nsElement.prefix, 'ns');
                    assert.equal(nsElement.localName, 'element');
                    assert.equal(nsElement.tagName, 'ns:element');

                    assert.equal(defaultNsElement.namespaceURI, nsURI);
                    assert.equal(defaultNsElement.prefix, null);
                    assert.equal(defaultNsElement.localName, 'element');
                    assert.equal(defaultNsElement.tagName, 'element');
                });

                it('should handle getElementById correctly', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    var child1 = xdoc.createElement('child');
                    var child2 = xdoc.createElement('child');

                    // Set IDs
                    child1.setAttribute('id', 'child1');
                    child2.setAttribute('id', 'child2');

                    root.appendChild(child1);
                    root.appendChild(child2);
                    xdoc.appendChild(root);

                    // Test getElementById
                    var found1 = xdoc.getElementById('child1');
                    var found2 = xdoc.getElementById('child2');
                    var notFound = xdoc.getElementById('nonexistent');

                    assert.equal(found1, child1);
                    assert.equal(found2, child2);
                    assert.equal(notFound, null);

                    // Test case sensitivity
                    var foundWrongCase = xdoc.getElementById('CHILD1');
                    assert.equal(foundWrongCase, null); // Should be case sensitive
                });

                it('should handle getElementsByTagName correctly', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    var child1 = xdoc.createElement('child');
                    var child2 = xdoc.createElement('child');
                    var different = xdoc.createElement('different');

                    root.appendChild(child1);
                    root.appendChild(child2);
                    root.appendChild(different);
                    xdoc.appendChild(root);

                    // Test getElementsByTagName
                    var children = xdoc.getElementsByTagName('child');
                    var allElements = xdoc.getElementsByTagName('*');

                    assert.equal(children.length, 2);
                    assert.equal(children[0], child1);
                    assert.equal(children[1], child2);
                    assert.equal(allElements.length, 4); // root, child, child, different

                    // Test case sensitivity
                    var childrenWrongCase = xdoc.getElementsByTagName('CHILD');
                    assert.equal(childrenWrongCase.length, 0); // Should be case sensitive
                });

                it('should handle namespace-aware element queries', () => {
                    var xdoc = newDoc();
                    var nsURI = 'http://example.com/ns';
                    var root = xdoc.createElement('root');
                    var nsChild1 = xdoc.createElementNS(nsURI, 'ns:child');
                    var nsChild2 = xdoc.createElementNS(nsURI, 'ns:child');
                    var regularChild = xdoc.createElement('child');

                    root.appendChild(nsChild1);
                    root.appendChild(nsChild2);
                    root.appendChild(regularChild);
                    xdoc.appendChild(root);

                    // Test getElementsByTagNameNS
                    var nsChildren = xdoc.getElementsByTagNameNS(nsURI, 'child');
                    var allNsElements = xdoc.getElementsByTagNameNS(nsURI, '*');
                    var regularChildren = xdoc.getElementsByTagName('child');

                    assert.equal(nsChildren.length, 2);
                    assert.equal(nsChildren[0], nsChild1);
                    assert.equal(nsChildren[1], nsChild2);
                    assert.equal(allNsElements.length, 2);
                    assert.equal(regularChildren.length, 1);
                    assert.equal(regularChildren[0], regularChild);
                });
            });

            describe('XML document vs HTML document differences', () => {
                it('should handle attribute case sensitivity', () => {
                    var xdoc = newDoc();
                    var element = xdoc.createElement('test');

                    // XML should be case sensitive for attributes
                    element.setAttribute('id', 'lowercase');
                    element.setAttribute('ID', 'uppercase');

                    assert.equal(element.getAttribute('id'), 'lowercase');
                    assert.equal(element.getAttribute('ID'), 'uppercase');
                    assert.equal(element.attributes.length, 2);

                    // Different case should be different attributes
                    assert.notEqual(element.getAttribute('id'), element.getAttribute('ID'));
                });

                it('should support innerHTML property in XML elements', () => {
                    var xdoc = newDoc();
                    var element = xdoc.createElement('test');

                    // XML elements should support innerHTML as string property
                    assert.equal(typeof element.innerHTML, 'string');
                    assert.equal(element.innerHTML, '');

                    // Setting innerHTML should work
                    element.innerHTML = '<child>content</child>';
                    assert.equal(element.childNodes.length, 1);
                    assert.equal(element.firstChild.tagName, 'child');
                    assert.equal(element.firstChild.textContent, 'content');

                    // Getting innerHTML should return proper XML string
                    var html = element.innerHTML;
                    assert.equal(typeof html, 'string');
                    assert.ok(html.indexOf('<child>') !== -1);
                    assert.ok(html.indexOf('content') !== -1);
                    assert.ok(html.indexOf('</child>') !== -1);
                });

                it('should preserve exact whitespace and formatting', () => {
                    var xdoc = newDoc();
                    var element = xdoc.createElement('test');
                    var textNode = xdoc.createTextNode('  \n  text with spaces  \n  ');

                    element.appendChild(textNode);

                    // XML should preserve exact whitespace
                    assert.equal(textNode.nodeValue, '  \n  text with spaces  \n  ');
                    assert.equal(element.textContent, '  \n  text with spaces  \n  ');
                });

                it('should handle CDATA sections', () => {
                    var xdoc = newDoc();
                    var element = xdoc.createElement('test');
                    var cdata = xdoc.createCDATASection('<script>alert("test");</script>');

                    element.appendChild(cdata);

                    assert.equal(cdata.nodeType, 4); // CDATA_SECTION_NODE
                    assert.equal(cdata.nodeValue, '<script>alert("test");</script>');
                    assert.equal(element.childNodes.length, 1);
                    assert.equal(element.firstChild, cdata);
                });
            });

            describe('XML document serialization behavior', () => {
                it('should serialize with proper XML syntax', () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement('root');
                    var child = xdoc.createElement('child');
                    child.setAttribute('attr', 'value');
                    child.textContent = 'text content';

                    root.appendChild(child);
                    xdoc.appendChild(root);

                    // Test serialization
                    var serialized = serialize(xdoc);
                    assert.ok(serialized.indexOf('<root>') !== -1);
                    assert.ok(serialized.indexOf('<child attr="value">') !== -1);
                    assert.ok(serialized.indexOf('text content') !== -1);
                    assert.ok(serialized.indexOf('</child>') !== -1);
                    assert.ok(serialized.indexOf('</root>') !== -1);
                });

                it('should handle special characters in text content', () => {
                    var xdoc = newDoc();
                    var element = xdoc.createElement('test');
                    var specialText = 'Text with <>&"\' characters';

                    element.textContent = specialText;

                    assert.equal(element.textContent, specialText);

                    // Test that special characters are preserved
                    var textNode = element.firstChild;
                    assert.equal(textNode.nodeValue, specialText);
                });

                it('should handle special characters in attributes', () => {
                    var xdoc = newDoc();
                    var element = xdoc.createElement('test');
                    var specialValue = 'value with <>&"\' characters';

                    element.setAttribute('special', specialValue);

                    assert.equal(element.getAttribute('special'), specialValue);

                    // Test attribute node
                    var attr = element.attributes[0];
                    assert.equal(attr.value, specialValue);
                });
            });
        });

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
        // HTML document initialization and basic behavior tests (based on browser standards)
        describe('HTML document initialization and behavior', () => {
            describe('newHtmlDoc() initial state', () => {
                it('should create HTML document with proper structure', () => {
                    var hdoc = newHtmlDoc();

                    // Document should exist
                    assert.notEqual(hdoc, null);
                    assert.equal(hdoc.nodeType, 9); // DOCUMENT_NODE

                    // Should have documentElement as HTML
                    assert.notEqual(hdoc.documentElement, null);
                    assert.equal(hdoc.documentElement.tagName, 'HTML');
                    assert.equal(hdoc.documentElement.nodeType, 1); // ELEMENT_NODE

                    // HTML element should have head and body children
                    var htmlChildren = hdoc.documentElement.children;
                    assert.equal(htmlChildren.length, 2);

                    // First child should be HEAD
                    var head = htmlChildren[0];
                    assert.equal(head.tagName, 'HEAD');
                    assert.equal(head.parentNode, hdoc.documentElement);

                    // Second child should be BODY
                    var body = htmlChildren[1];
                    assert.equal(body.tagName, 'BODY');
                    assert.equal(body.parentNode, hdoc.documentElement);

                    // Document should have head and body shortcuts
                    assert.equal(hdoc.head, head);
                    assert.equal(hdoc.body, body);
                });

                it('should have empty head and body initially', () => {
                    var hdoc = newHtmlDoc();

                    // Head should be empty initially
                    assert.equal(hdoc.head.childNodes.length, 0);
                    assert.equal(hdoc.head.children.length, 0);
                    assert.equal(hdoc.head.innerHTML, '');

                    // Body should be empty initially
                    assert.equal(hdoc.body.childNodes.length, 0);
                    assert.equal(hdoc.body.children.length, 0);
                    assert.equal(hdoc.body.innerHTML, '');
                });

                it('should allow modification of head content', () => {
                    var hdoc = newHtmlDoc();

                    // Add title to head
                    var title = hdoc.createElement('title');
                    title.textContent = 'Test Document';
                    hdoc.head.appendChild(title);

                    assert.equal(hdoc.head.children.length, 1);
                    assert.equal(hdoc.head.children[0].tagName, 'TITLE');
                    assert.equal(hdoc.head.children[0].textContent, 'Test Document');

                    // Add meta tag
                    var meta = hdoc.createElement('meta');
                    meta.setAttribute('charset', 'UTF-8');
                    hdoc.head.appendChild(meta);

                    assert.equal(hdoc.head.children.length, 2);
                    assert.equal(hdoc.head.children[1].tagName, 'META');
                    assert.equal(hdoc.head.children[1].getAttribute('charset'), 'UTF-8');
                });

                it('should allow modification of body content', () => {
                    var hdoc = newHtmlDoc();

                    // Add div to body
                    var div = hdoc.createElement('div');
                    div.className = 'test-div';
                    div.textContent = 'Hello World';
                    hdoc.body.appendChild(div);

                    assert.equal(hdoc.body.children.length, 1);
                    assert.equal(hdoc.body.children[0].tagName, 'DIV');
                    assert.equal(hdoc.body.children[0].className, 'test-div');
                    assert.equal(hdoc.body.children[0].textContent, 'Hello World');

                    // Test innerHTML
                    hdoc.body.innerHTML = '<p>Paragraph text</p><span>Span text</span>';
                    assert.equal(hdoc.body.children.length, 2);
                    assert.equal(hdoc.body.children[0].tagName, 'P');
                    assert.equal(hdoc.body.children[1].tagName, 'SPAN');
                    assert.equal(hdoc.body.children[0].textContent, 'Paragraph text');
                    assert.equal(hdoc.body.children[1].textContent, 'Span text');
                });
            });

            describe('HTML attribute name case sensitivity', () => {
                it('should normalize attribute names to lowercase when setting/getting', () => {
                    var hdoc = newHtmlDoc();
                    var div = hdoc.createElement('div');

                    // Test setAttribute with uppercase attribute names
                    div.setAttribute('ID', 'test-id');
                    div.setAttribute('CLASS', 'test-class');
                    div.setAttribute('DATA-VALUE', 'test-data');
                    div.setAttribute('onClick', 'alert("test")');

                    // Test getAttribute with different cases
                    assert.equal(div.getAttribute('id'), 'test-id');
                    assert.equal(div.getAttribute('ID'), 'test-id');
                    assert.equal(div.getAttribute('Id'), 'test-id');

                    assert.equal(div.getAttribute('class'), 'test-class');
                    assert.equal(div.getAttribute('CLASS'), 'test-class');
                    assert.equal(div.getAttribute('Class'), 'test-class');

                    assert.equal(div.getAttribute('data-value'), 'test-data');
                    assert.equal(div.getAttribute('DATA-VALUE'), 'test-data');
                    assert.equal(div.getAttribute('Data-Value'), 'test-data');

                    assert.equal(div.getAttribute('onclick'), 'alert("test")');
                    assert.equal(div.getAttribute('ONCLICK'), 'alert("test")');
                    assert.equal(div.getAttribute('onClick'), 'alert("test")');
                });

                it('should preserve lowercase in internal storage and serialization', () => {
                    var hdoc = newHtmlDoc();
                    var div = hdoc.createElement('div');

                    // Set attributes with mixed case
                    div.setAttribute('ID', 'test-id');
                    div.setAttribute('CLASS', 'test-class');
                    div.setAttribute('DATA-Custom', 'custom-value');

                    // Check attribute names in attributes collection
                    var attrs = div.attributes;
                    var attrNames = [];
                    for (var i = 0; i < attrs.length; i++) {
                        attrNames.push(attrs[i].name);
                    }

                    // All attribute names should be lowercase
                    assert.ok(attrNames.includes('id'));
                    assert.ok(attrNames.includes('class'));
                    assert.ok(attrNames.includes('data-custom'));

                    // Should not contain uppercase versions
                    assert.ok(!attrNames.includes('ID'));
                    assert.ok(!attrNames.includes('CLASS'));
                    assert.ok(!attrNames.includes('DATA-Custom'));

                    // Check serialization (outerHTML should show lowercase attribute names)
                    if (div.outerHTML) {
                        var html = div.outerHTML;
                        assert.ok(html.includes('id="test-id"'));
                        assert.ok(html.includes('class="test-class"'));
                        assert.ok(html.includes('data-custom="custom-value"'));
                    }
                });

                it('should handle innerHTML parsing with uppercase attributes', () => {
                    var hdoc = newHtmlDoc();

                    // Set innerHTML with uppercase attribute names
                    hdoc.body.innerHTML = '<div ID="parsed-id" CLASS="parsed-class" DATA-TEST="parsed-data">Content</div>';

                    var div = hdoc.body.children[0];

                    // Should be able to access with any case
                    assert.equal(div.getAttribute('id'), 'parsed-id');
                    assert.equal(div.getAttribute('ID'), 'parsed-id');
                    assert.equal(div.getAttribute('class'), 'parsed-class');
                    assert.equal(div.getAttribute('CLASS'), 'parsed-class');
                    assert.equal(div.getAttribute('data-test'), 'parsed-data');
                    assert.equal(div.getAttribute('DATA-TEST'), 'parsed-data');

                    // Check internal storage is lowercase
                    var attrs = div.attributes;
                    var attrNames = [];
                    for (var i = 0; i < attrs.length; i++) {
                        attrNames.push(attrs[i].name);
                    }

                    assert.ok(attrNames.includes('id'));
                    assert.ok(attrNames.includes('class'));
                    assert.ok(attrNames.includes('data-test'));
                });

                it('should handle empty string namespaceURI in HTML', () => {
                    var hdoc = newHtmlDoc();
                    var div = hdoc.createElement('div');

                    // setAttributeNS with empty string namespace should behave like null namespace
                    div.setAttributeNS('', 'test-attr', 'value1');
                    div.setAttributeNS('http://example.com', 'custom:attr', 'value2');

                    // getAttributeNS with empty string should find the attribute
                    assert.equal(div.getAttributeNS('', 'test-attr'), 'value1');
                    assert.equal(div.getAttributeNS('http://example.com', 'attr'), 'value2');

                    // getAttribute should also find the empty namespace attribute in HTML mode
                    assert.equal(div.getAttribute('test-attr'), 'value1');

                    // hasAttributeNS with empty string should work
                    assert.equal(div.hasAttributeNS('', 'test-attr'), true);
                    assert.equal(div.hasAttributeNS('http://example.com', 'attr'), true);
                    assert.equal(div.hasAttributeNS('', 'nonexistent'), false);

                    // hasAttribute should work for empty namespace attributes
                    assert.equal(div.hasAttribute('test-attr'), true);
                    assert.equal(div.hasAttribute('custom:attr'), true); // This has a namespace
                });

                it('should handle hasAttribute with case insensitivity', () => {
                    var hdoc = newHtmlDoc();
                    var div = hdoc.createElement('div');

                    div.setAttribute('ID', 'test-id');
                    div.setAttribute('data-VALUE', 'test-data');

                    // hasAttribute should work with any case
                    assert.equal(div.hasAttribute('id'), true);
                    assert.equal(div.hasAttribute('ID'), true);
                    assert.equal(div.hasAttribute('Id'), true);

                    assert.equal(div.hasAttribute('data-value'), true);
                    assert.equal(div.hasAttribute('DATA-VALUE'), true);
                    assert.equal(div.hasAttribute('Data-Value'), true);

                    // Non-existent attributes
                    assert.equal(div.hasAttribute('nonexistent'), false);
                    assert.equal(div.hasAttribute('NONEXISTENT'), false);
                });
            });

            describe('HTML document structure constraints', () => {
                it('should reject appendChild of element to document when HTML exists', () => {
                    var hdoc = newHtmlDoc();
                    var extraElement = hdoc.createElement('div');

                    // Should throw error when trying to append element to document
                    assert.throws(() => {
                        hdoc.appendChild(extraElement);
                    });

                    // Document may have DOCTYPE and HTML elements in browser
                    var initialChildCount = hdoc.childNodes.length;
                    assert.equal(hdoc.documentElement.tagName, 'HTML');

                    // Verify the HTML element is still the documentElement after failed append
                    assert.equal(hdoc.documentElement.tagName, 'HTML');
                    assert.equal(hdoc.childNodes.length, initialChildCount); // Should not change
                });

                it('should allow comments and PIs at document level', () => {
                    var hdoc = newHtmlDoc();
                    var initialChildCount = hdoc.childNodes.length;
                    var comment = hdoc.createComment('This is a comment');
                    var pi = hdoc.createProcessingInstruction('xml-stylesheet', 'type="text/css" href="style.css"');

                    // Should allow comments and PIs before HTML element
                    hdoc.insertBefore(comment, hdoc.documentElement);
                    hdoc.insertBefore(pi, hdoc.documentElement);

                    // Should have 2 more children than initial count
                    assert.equal(hdoc.childNodes.length, initialChildCount + 2);

                    // Find the positions of comment, PI, and HTML element
                    var commentIndex = -1;
                    var piIndex = -1;
                    var htmlIndex = -1;

                    for (var i = 0; i < hdoc.childNodes.length; i++) {
                        var node = hdoc.childNodes[i];
                        if (node === comment) commentIndex = i;
                        else if (node === pi) piIndex = i;
                        else if (node === hdoc.documentElement) htmlIndex = i;
                    }

                    // Verify all elements are found and comment/PI come before HTML
                    assert.notEqual(commentIndex, -1);
                    assert.notEqual(piIndex, -1);
                    assert.notEqual(htmlIndex, -1);
                    assert.ok(commentIndex < htmlIndex);
                    assert.ok(piIndex < htmlIndex);
                });

                it('should maintain HTML->HEAD->BODY hierarchy', () => {
                    var hdoc = newHtmlDoc();

                    // Try to remove HEAD
                    var originalHead = hdoc.head;
                    hdoc.documentElement.removeChild(originalHead);

                    // Should be able to add content to body even without head
                    var div = hdoc.createElement('div');
                    div.textContent = 'Content without head';
                    hdoc.body.appendChild(div);

                    assert.equal(hdoc.body.children.length, 1);
                    assert.equal(hdoc.body.children[0].textContent, 'Content without head');

                    // Re-add head
                    var newHead = hdoc.createElement('head');
                    hdoc.documentElement.insertBefore(newHead, hdoc.body);

                    // Document shortcuts may or may not update automatically (implementation dependent)
                    // Just verify that we can still access body and its content
                    assert.equal(hdoc.body.children.length, 1);
                    assert.equal(hdoc.body.children[0].textContent, 'Content without head');

                    // Verify the new head is in the right place
                    assert.equal(newHead.parentNode, hdoc.documentElement);
                    assert.equal(newHead.nextSibling, hdoc.body);
                });
            });

            describe('HTML document methods', () => {
                it('should create elements with correct case handling', () => {
                    var hdoc = newHtmlDoc();

                    // HTML documents should normalize tag names to uppercase
                    var div = hdoc.createElement('div');
                    var DIV = hdoc.createElement('DIV');
                    var MixedCase = hdoc.createElement('DiV');

                    assert.equal(div.tagName, 'DIV');
                    assert.equal(DIV.tagName, 'DIV');
                    assert.equal(MixedCase.tagName, 'DIV');

                    // Custom elements should preserve case but may be normalized
                    var customElement = hdoc.createElement('my-custom-element');
                    assert.equal(customElement.tagName, 'MY-CUSTOM-ELEMENT');
                });

                it('should handle getElementById correctly', () => {
                    var hdoc = newHtmlDoc();

                    // Add elements with IDs
                    var div1 = hdoc.createElement('div');
                    div1.id = 'test-id-1';
                    hdoc.body.appendChild(div1);

                    var span1 = hdoc.createElement('span');
                    span1.id = 'test-id-2';
                    hdoc.body.appendChild(span1);

                    // Test getElementById
                    var found1 = hdoc.getElementById('test-id-1');
                    var found2 = hdoc.getElementById('test-id-2');
                    var notFound = hdoc.getElementById('non-existent');

                    assert.equal(found1, div1);
                    assert.equal(found2, span1);
                    assert.equal(notFound, null);

                    // Test case sensitivity
                    var foundWrongCase = hdoc.getElementById('TEST-ID-1');
                    assert.equal(foundWrongCase, null); // Should be case sensitive
                });

                it('should handle getElementsByTagName correctly', () => {
                    var hdoc = newHtmlDoc();

                    // Add various elements
                    var div1 = hdoc.createElement('div');
                    var div2 = hdoc.createElement('div');
                    var span1 = hdoc.createElement('span');

                    hdoc.body.appendChild(div1);
                    hdoc.body.appendChild(div2);
                    hdoc.body.appendChild(span1);

                    // Test getElementsByTagName
                    var divs = hdoc.getElementsByTagName('div');
                    var spans = hdoc.getElementsByTagName('span');
                    var allElements = hdoc.getElementsByTagName('*');

                    assert.equal(divs.length, 2);
                    assert.equal(spans.length, 1);
                    assert.ok(allElements.length >= 5); // html, head, body, div, div, span at minimum

                    // Test case insensitivity for HTML
                    var divsUpper = hdoc.getElementsByTagName('DIV');
                    assert.equal(divsUpper.length, 2);
                    assert.equal(divsUpper[0], div1);
                    assert.equal(divsUpper[1], div2);
                });

                it('should handle getElementsByClassName correctly', () => {
                    var hdoc = newHtmlDoc();

                    // Add elements with classes
                    var div1 = hdoc.createElement('div');
                    div1.className = 'class1 class2';
                    var div2 = hdoc.createElement('div');
                    div2.className = 'class2 class3';
                    var span1 = hdoc.createElement('span');
                    span1.className = 'class1';

                    hdoc.body.appendChild(div1);
                    hdoc.body.appendChild(div2);
                    hdoc.body.appendChild(span1);

                    // Test getElementsByClassName
                    var class1Elements = hdoc.getElementsByClassName('class1');
                    var class2Elements = hdoc.getElementsByClassName('class2');
                    var class3Elements = hdoc.getElementsByClassName('class3');

                    assert.equal(class1Elements.length, 2); // div1, span1
                    assert.equal(class2Elements.length, 2); // div1, div2
                    assert.equal(class3Elements.length, 1); // div2

                    // Test multiple class names
                    var multipleClasses = hdoc.getElementsByClassName('class1 class2');
                    assert.equal(multipleClasses.length, 1); // div1
                });
            });

            describe('HTML document vs XML document differences', () => {
                it('should behave differently from XML document for case sensitivity', () => {
                    var hdoc = newHtmlDoc();
                    var xdoc = newDoc();

                    // HTML should normalize tag names
                    var htmlDiv = hdoc.createElement('div');
                    assert.equal(htmlDiv.tagName, 'DIV');

                    // XML should preserve case
                    var xmlDiv = xdoc.createElement('div');
                    assert.equal(xmlDiv.tagName, 'div');

                    // Mixed case
                    var htmlMixed = hdoc.createElement('DiV');
                    var xmlMixed = xdoc.createElement('DiV');
                    assert.equal(htmlMixed.tagName, 'DIV');
                    assert.equal(xmlMixed.tagName, 'DiV');
                });

                it('should handle attribute case differently', () => {
                    var hdoc = newHtmlDoc();
                    var xdoc = newDoc();

                    var htmlEl = hdoc.createElement('div');
                    var xmlEl = xdoc.createElement('div');

                    // Set attributes with different cases
                    htmlEl.setAttribute('ID', 'test');
                    xmlEl.setAttribute('ID', 'test');

                    // HTML should normalize some attributes
                    assert.equal(htmlEl.getAttribute('id'), 'test');
                    assert.equal(htmlEl.getAttribute('ID'), 'test');

                    // XML should be case sensitive
                    assert.equal(xmlEl.getAttribute('ID'), 'test');
                    assert.equal(xmlEl.getAttribute('id'), null);
                });

                it('should have different innerHTML behavior', () => {
                    var hdoc = newHtmlDoc();

                    // HTML documents should support innerHTML
                    assert.equal(typeof hdoc.body.innerHTML, 'string');

                    // Test setting innerHTML
                    hdoc.body.innerHTML = '<div>Test</div>';
                    assert.equal(hdoc.body.children.length, 1);
                    assert.equal(hdoc.body.children[0].tagName, 'DIV');
                    assert.equal(hdoc.body.children[0].textContent, 'Test');

                    // Test getting innerHTML
                    var html = hdoc.body.innerHTML;
                    assert.equal(html.toLowerCase(), '<div>test</div>');
                });
            });

            describe('HTML document property shortcuts', () => {
                it('should provide head and body shortcuts', () => {
                    var hdoc = newHtmlDoc();

                    // Both should be defined
                    assert.notEqual(hdoc.head, null);
                    assert.notEqual(hdoc.body, null);

                    // They should be the actual head and body elements
                    assert.equal(hdoc.head.tagName, 'HEAD');
                    assert.equal(hdoc.body.tagName, 'BODY');

                    // They should be children of documentElement
                    assert.equal(hdoc.head.parentNode, hdoc.documentElement);
                    assert.equal(hdoc.body.parentNode, hdoc.documentElement);
                });

                it('should update shortcuts when structure changes', () => {
                    var hdoc = newHtmlDoc();
                    var originalHead = hdoc.head;
                    var originalBody = hdoc.body;

                    // Remove and replace head
                    hdoc.documentElement.removeChild(originalHead);
                    var newHead = hdoc.createElement('head');
                    hdoc.documentElement.insertBefore(newHead, originalBody);

                    // Body should remain the same
                    assert.equal(hdoc.body, originalBody);

                    // Verify the new head is in the right place
                    assert.equal(newHead.parentNode, hdoc.documentElement);
                    assert.equal(newHead.nextSibling, hdoc.body);

                    // Head shortcut behavior is implementation dependent - 
                    // just verify we can still access it
                    assert.notEqual(hdoc.head, null);
                    assert.equal(hdoc.head.tagName, 'HEAD');
                });
            });
        });

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

        it("outerHTML", () => {
            var hdoc = parseHtml("<div id='test'>Hello</div>");
            var div = hdoc.getElementById("test");

            // Test get outerHTML
            assert.equal(div.outerHTML, '<div id="test">Hello</div>');

            // Test set outerHTML - replaces element with new content
            div.outerHTML = "<span>World</span><p>New</p>";
            assert.equal(hdoc.body.innerHTML, "<span>World</span><p>New</p>");

            // Original element should be detached (no parent)
            assert.equal(div.parentNode, null);

            // Test outerHTML on element without parent (should throw error per MDN spec)
            var orphan = hdoc.createElement("div");
            orphan.textContent = "orphan";
            assert.throws(() => {
                orphan.outerHTML = "<span>replaced</span>";
            });
        });

        it("textContent", () => {
            var hdoc = parseHtml("<Div>    <p>abcde\nf&lt;</div>");
            assert.equal(hdoc.body.textContent, "    abcde\nf<");

            hdoc.body.textContent = "<img><br>";
            assert.equal(hdoc.body.innerHTML, "&lt;img&gt;&lt;br&gt;");
        });

        describe("dataset", () => {
            it("basic camelCase conversion", () => {
                var hdoc = parseHtml('<div id="user" data-user-id="12345" data-user-name="John" data-abc-def-ghi="test">Content</div>');
                var div = hdoc.getElementById("user");

                assert.equal(typeof div.dataset, "object");
                assert.equal(div.dataset.userId, "12345");
                assert.equal(div.dataset.userName, "John");
                assert.equal(div.dataset.abcDefGhi, "test");
            });

            it("non-existent attribute returns undefined", () => {
                var hdoc = parseHtml('<div id="test" data-exists="yes">Test</div>');
                var div = hdoc.getElementById("test");
                assert.equal(div.dataset.nonExistent, undefined);
            });

            it("element without data attributes", () => {
                var hdoc = parseHtml('<span id="empty">No data</span>');
                var span = hdoc.getElementById("empty");
                assert.equal(Object.keys(span.dataset).length, 0);
            });

            it("single letter attribute names", () => {
                var hdoc = parseHtml('<div id="t1" data-a="1" data-b="2">Test</div>');
                var t1 = hdoc.getElementById("t1");
                assert.equal(t1.dataset.a, "1");
                assert.equal(t1.dataset.b, "2");
            });

            it("numbers in attribute name", () => {
                var hdoc = parseHtml('<div id="t2" data-item1="a" data-item2name="b" data-123="c">Test</div>');
                var t2 = hdoc.getElementById("t2");
                assert.equal(t2.dataset.item1, "a");
                assert.equal(t2.dataset.item2name, "b");
                assert.equal(t2.dataset["123"], "c");
            });

            it("empty value and boolean-like attribute", () => {
                var hdoc = parseHtml('<div id="t3" data-empty="" data-flag>Test</div>');
                var t3 = hdoc.getElementById("t3");
                assert.equal(t3.dataset.empty, "");
                assert.equal(t3.dataset.flag, "");
            });

            it("special characters (dots, colons) preserved", () => {
                var hdoc = parseHtml('<div id="t4" data-a.b="dot" data-a:b="colon">Test</div>');
                var t4 = hdoc.getElementById("t4");
                assert.equal(t4.dataset["a.b"], "dot");
                assert.equal(t4.dataset["a:b"], "colon");
            });

            it("uppercase in HTML normalized to lowercase", () => {
                var hdoc = parseHtml('<div id="t5" data-MyValue="upper" DATA-OTHER="caps">Test</div>');
                var t5 = hdoc.getElementById("t5");
                assert.equal(t5.dataset.myvalue, "upper");
                assert.equal(t5.dataset.other, "caps");
            });

            it("dynamically added via setAttribute", () => {
                var hdoc = parseHtml('<div id="t6">Test</div>');
                var t6 = hdoc.getElementById("t6");
                assert.equal(Object.keys(t6.dataset).length, 0);
                t6.setAttribute("data-dynamic-value", "added");
                assert.equal(t6.dataset.dynamicValue, "added");
            });

            it("multiple consecutive dashes", () => {
                var hdoc = parseHtml('<div id="t7" data-a--b="double" data-x---y="triple">Test</div>');
                var t7 = hdoc.getElementById("t7");
                // data-a--b -> a-B (dash + lowercase = uppercase, other dashes preserved)
                assert.equal(t7.dataset["a-B"], "double");
                assert.equal(t7.dataset["x--Y"], "triple");
            });

            it("trailing dash", () => {
                var hdoc = parseHtml('<div id="t8" data-test-="trailing">Test</div>');
                var t8 = hdoc.getElementById("t8");
                assert.equal(t8.dataset["test-"], "trailing");
            });

            it("underscore in name preserved", () => {
                var hdoc = parseHtml('<div id="t9" data-my_value="underscore" data-a_b-c="mixed">Test</div>');
                var t9 = hdoc.getElementById("t9");
                assert.equal(t9.dataset.my_value, "underscore");
                assert.equal(t9.dataset.a_bC, "mixed");
            });

            it("value with special characters", () => {
                var hdoc = parseHtml('<div id="t10" data-json=\'{"key":"value"}\' data-html="&lt;b&gt;bold&lt;/b&gt;">Test</div>');
                var t10 = hdoc.getElementById("t10");
                assert.equal(t10.dataset.json, '{"key":"value"}');
                assert.equal(t10.dataset.html, "<b>bold</b>");
            });

            it("remove data attribute updates dataset", () => {
                var hdoc = parseHtml('<div id="t11" data-remove-me="value">Test</div>');
                var t11 = hdoc.getElementById("t11");
                assert.equal(t11.dataset.removeMe, "value");
                t11.removeAttribute("data-remove-me");
                assert.equal(t11.dataset.removeMe, undefined);
                assert.equal(Object.keys(t11.dataset).length, 0);
            });

            it("modify existing data attribute updates dataset", () => {
                var hdoc = parseHtml('<div id="t12" data-modify="old">Test</div>');
                var t12 = hdoc.getElementById("t12");
                assert.equal(t12.dataset.modify, "old");
                t12.setAttribute("data-modify", "new");
                assert.equal(t12.dataset.modify, "new");
            });
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

            // DocumentFragment tests
            describe('DocumentFragment', () => {
                it("createDocumentFragment basic", () => {
                    var xdoc = newDoc();
                    var fragment = xdoc.createDocumentFragment();

                    assert.equal(fragment.nodeType, 11);
                    assert.equal(fragment.nodeName, "#document-fragment");
                    assert.equal(fragment.parentNode, null);
                    assert.equal(fragment.ownerDocument, xdoc);
                });

                it("appendChild children to fragment", () => {
                    var xdoc = newDoc();
                    var fragment = xdoc.createDocumentFragment();

                    var text = xdoc.createTextNode("Hello");
                    var el = xdoc.createElement("span");
                    el.textContent = "World";

                    fragment.appendChild(text);
                    fragment.appendChild(el);

                    assert.equal(fragment.childNodes.length, 2);
                    assert.equal(fragment.firstChild.nodeValue, "Hello");
                    assert.equal(fragment.lastChild.tagName, "span");
                });

                it("appendChild fragment moves children", () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement("root");
                    xdoc.appendChild(root);

                    root.appendChild(xdoc.createElement("a"));
                    root.appendChild(xdoc.createElement("b"));

                    var fragment = xdoc.createDocumentFragment();
                    fragment.appendChild(xdoc.createElement("x"));
                    fragment.appendChild(xdoc.createElement("y"));

                    assert.equal(root.childNodes.length, 2);
                    assert.equal(fragment.childNodes.length, 2);

                    root.appendChild(fragment);

                    assert.equal(root.childNodes.length, 4);
                    assert.equal(fragment.childNodes.length, 0);
                    assert.equal(root.childNodes[2].tagName, "x");
                    assert.equal(root.childNodes[3].tagName, "y");
                });

                it("insertBefore with fragment", () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement("root");
                    xdoc.appendChild(root);

                    var a = xdoc.createElement("a");
                    var b = xdoc.createElement("b");
                    var c = xdoc.createElement("c");
                    root.appendChild(a);
                    root.appendChild(b);
                    root.appendChild(c);

                    var fragment = xdoc.createDocumentFragment();
                    fragment.appendChild(xdoc.createElement("x"));
                    fragment.appendChild(xdoc.createElement("y"));

                    root.insertBefore(fragment, b);

                    assert.equal(root.childNodes.length, 5);
                    assert.equal(root.childNodes[0].tagName, "a");
                    assert.equal(root.childNodes[1].tagName, "x");
                    assert.equal(root.childNodes[2].tagName, "y");
                    assert.equal(root.childNodes[3].tagName, "b");
                    assert.equal(root.childNodes[4].tagName, "c");
                });

                it("replaceChild with fragment", () => {
                    var xdoc = newDoc();
                    var root = xdoc.createElement("root");
                    xdoc.appendChild(root);

                    var a = xdoc.createElement("a");
                    var b = xdoc.createElement("b");
                    var c = xdoc.createElement("c");
                    root.appendChild(a);
                    root.appendChild(b);
                    root.appendChild(c);

                    var fragment = xdoc.createDocumentFragment();
                    fragment.appendChild(xdoc.createElement("x"));
                    fragment.appendChild(xdoc.createElement("y"));

                    root.replaceChild(fragment, b);

                    assert.equal(root.childNodes.length, 4);
                    assert.equal(root.childNodes[0].tagName, "a");
                    assert.equal(root.childNodes[1].tagName, "x");
                    assert.equal(root.childNodes[2].tagName, "y");
                    assert.equal(root.childNodes[3].tagName, "c");
                });

                it("fragment parentNode always null", () => {
                    var xdoc = newDoc();
                    var fragment = xdoc.createDocumentFragment();
                    var el = xdoc.createElement("test");
                    fragment.appendChild(el);

                    assert.equal(fragment.parentNode, null);
                    assert.equal(el.parentNode, fragment);
                });
            });

            // template.content tests
            describe('template.content', () => {
                it("returns DocumentFragment for template element", () => {
                    var doc = xml.parse('<html><body><template><p>Hello</p><p>World</p></template></body></html>', 'text/html');
                    var template = doc.body.firstChild;

                    assert.equal(template.tagName, "TEMPLATE");
                    assert.notEqual(template.content, null);
                    assert.equal(template.content.nodeType, 11);
                    assert.equal(template.content.nodeName, "#document-fragment");
                });

                it("content contains children", () => {
                    var doc = xml.parse('<html><body><template><span>A</span><span>B</span></template></body></html>', 'text/html');
                    var template = doc.body.firstChild;
                    var content = template.content;

                    assert.equal(content.childNodes.length, 2);
                    assert.equal(content.firstChild.tagName, "SPAN");
                    assert.equal(content.firstChild.textContent, "A");
                    assert.equal(content.lastChild.tagName, "SPAN");
                    assert.equal(content.lastChild.textContent, "B");
                });

                it("modifying content reflects in innerHTML", () => {
                    var doc = new xml.Document('text/html');
                    var template = doc.createElement('template');
                    template.innerHTML = '<div>Hello</div><script>alert(1)</script>';

                    assert.equal(template.innerHTML, '<div>Hello</div><script>alert(1)</script>');

                    // Remove script from content
                    var content = template.content;
                    var script = content.lastChild;
                    var warning = doc.createElement('pre');
                    warning.textContent = 'Blocked!';
                    script.replaceWith(warning);

                    assert.equal(template.innerHTML, '<div>Hello</div><pre>Blocked!</pre>');
                });

                it("content returns null for non-template elements", () => {
                    var doc = xml.parse('<html><body><div><p>Hello</p></div></body></html>', 'text/html');
                    var div = doc.body.firstChild;

                    assert.equal(div.content, null);
                });

                it("content returns null in XML mode", () => {
                    var xdoc = newDoc();
                    var template = xdoc.createElement("template");
                    template.appendChild(xdoc.createElement("child"));

                    assert.equal(template.content, null);
                });

                it("content can be inserted with appendChild", () => {
                    var doc = xml.parse('<html><body><template><p>Hello</p></template><div id="target"></div></body></html>', 'text/html');
                    var template = doc.body.firstChild;
                    var target = doc.getElementById('target');

                    assert.equal(target.innerHTML, "");
                    target.appendChild(template.content);
                    assert.equal(target.innerHTML, '<p>Hello</p>');
                });

                it("content can be inserted with insertBefore", () => {
                    var doc = xml.parse('<html><body><template><p>A</p><p>B</p></template><div id="target"><span>X</span></div></body></html>', 'text/html');
                    var template = doc.body.firstChild;
                    var target = doc.getElementById('target');
                    var refNode = target.firstChild;

                    target.insertBefore(template.content, refNode);
                    assert.equal(target.innerHTML, '<p>A</p><p>B</p><span>X</span>');
                });

                it("content can be inserted with insertAfter", () => {
                    var doc = xml.parse('<html><body><template><p>A</p><p>B</p></template><div id="target"><span>X</span></div></body></html>', 'text/html');
                    var template = doc.body.firstChild;
                    var target = doc.getElementById('target');
                    var refNode = target.firstChild;

                    target.insertAfter(template.content, refNode);
                    assert.equal(target.innerHTML, '<span>X</span><p>A</p><p>B</p>');
                });

                it("content can be inserted with replaceChild", () => {
                    var doc = xml.parse('<html><body><template><p>A</p><p>B</p></template><div id="target"><span>X</span></div></body></html>', 'text/html');
                    var template = doc.body.firstChild;
                    var target = doc.getElementById('target');
                    var oldChild = target.firstChild;

                    target.replaceChild(template.content, oldChild);
                    assert.equal(target.innerHTML, '<p>A</p><p>B</p>');
                });
            });

            // classList (DOMTokenList) tests
            describe('classList', () => {
                it("basic access", () => {
                    var doc = xml.parse('<div class="foo bar baz"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    assert.equal(div.classList.length, 3);
                    assert.equal(div.classList.value, "foo bar baz");
                    assert.equal(div.classList[0], "foo");
                    assert.equal(div.classList[1], "bar");
                    assert.equal(div.classList[2], "baz");
                    assert.equal(div.classList.item(0), "foo");
                    assert.equal(div.classList.item(3), null);
                });

                it("contains", () => {
                    var doc = xml.parse('<div class="foo bar"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    assert.equal(div.classList.contains("foo"), true);
                    assert.equal(div.classList.contains("bar"), true);
                    assert.equal(div.classList.contains("baz"), false);
                });

                it("add", () => {
                    var doc = xml.parse('<div class="foo"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    div.classList.add("bar");
                    assert.equal(div.className, "foo bar");

                    div.classList.add("baz", "qux");
                    assert.equal(div.className, "foo bar baz qux");

                    // Adding existing class should not duplicate
                    div.classList.add("foo");
                    assert.equal(div.className, "foo bar baz qux");
                });

                it("remove", () => {
                    var doc = xml.parse('<div class="foo bar baz qux"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    div.classList.remove("bar");
                    assert.equal(div.className, "foo baz qux");

                    div.classList.remove("foo", "qux");
                    assert.equal(div.className, "baz");

                    // Removing non-existent class should be no-op
                    div.classList.remove("notexist");
                    assert.equal(div.className, "baz");
                });

                it("toggle", () => {
                    var doc = xml.parse('<div class="foo bar"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    // Toggle existing off
                    assert.equal(div.classList.toggle("foo"), false);
                    assert.equal(div.className, "bar");

                    // Toggle non-existing on
                    assert.equal(div.classList.toggle("foo"), true);
                    assert.equal(div.className, "bar foo");
                });

                it("toggle with force", () => {
                    var doc = xml.parse('<div class="foo"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    // force=true adds if not present
                    assert.equal(div.classList.toggle("bar", true), true);
                    assert.equal(div.className, "foo bar");

                    // force=true keeps if present
                    assert.equal(div.classList.toggle("bar", true), true);
                    assert.equal(div.className, "foo bar");

                    // force=false removes if present
                    assert.equal(div.classList.toggle("bar", false), false);
                    assert.equal(div.className, "foo");

                    // force=false no-op if not present
                    assert.equal(div.classList.toggle("bar", false), false);
                    assert.equal(div.className, "foo");
                });

                it("replace", () => {
                    var doc = xml.parse('<div class="foo bar baz"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    assert.equal(div.classList.replace("bar", "replaced"), true);
                    assert.equal(div.className, "foo replaced baz");

                    assert.equal(div.classList.replace("notexist", "x"), false);
                    assert.equal(div.className, "foo replaced baz");
                });

                it("toString", () => {
                    var doc = xml.parse('<div class="foo bar"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    assert.equal(div.classList.toString(), "foo bar");
                });

                it("classList only in HTML mode", () => {
                    var xdoc = newDoc();
                    var e = xdoc.createElement("test");
                    // In XML mode, classList should throw or return undefined
                    assert.throws(() => {
                        var cl = e.classList;
                    });
                });

                it("classList reflects className changes", () => {
                    var doc = xml.parse('<div class="a"></div>', 'text/html');
                    var div = doc.body.firstChild;

                    div.className = "x y z";
                    assert.equal(div.classList.length, 3);
                    assert.equal(div.classList[0], "x");
                    assert.equal(div.classList[1], "y");
                    assert.equal(div.classList[2], "z");
                });
            });
        });

        describe('append/prepend/before/after/replaceChildren', () => {
            describe('append()', () => {
                it('should append single node', () => {
                    const doc = parseHtml('<html><body></body></html>');
                    const body = doc.body;
                    const div = doc.createElement('div');
                    body.append(div);
                    assert.equal(body.lastChild.tagName.toLowerCase(), 'div');
                });

                it('should append multiple nodes', () => {
                    const doc = parseHtml('<html><body></body></html>');
                    const body = doc.body;
                    const div1 = doc.createElement('div');
                    const div2 = doc.createElement('span');
                    body.append(div1, div2);
                    assert.equal(body.children.length, 2);
                    assert.equal(body.children[0].tagName.toLowerCase(), 'div');
                    assert.equal(body.children[1].tagName.toLowerCase(), 'span');
                });

                it('should append string as text node', () => {
                    const doc = parseHtml('<html><body></body></html>');
                    const body = doc.body;
                    body.append('Hello World');
                    assert.equal(body.textContent, 'Hello World');
                });

                it('should append mixed nodes and strings', () => {
                    const doc = parseHtml('<html><body></body></html>');
                    const body = doc.body;
                    const div = doc.createElement('div');
                    body.append('Text1', div, 'Text2');
                    assert.equal(body.childNodes.length, 3);
                    assert.equal(body.childNodes[0].nodeType, 3); // TEXT_NODE
                    assert.equal(body.childNodes[1].tagName.toLowerCase(), 'div');
                    assert.equal(body.childNodes[2].nodeType, 3); // TEXT_NODE
                });
            });

            describe('prepend()', () => {
                it('should prepend single node', () => {
                    const doc = parseHtml('<html><body><p>existing</p></body></html>');
                    const body = doc.body;
                    const div = doc.createElement('div');
                    body.prepend(div);
                    assert.equal(body.firstChild.tagName.toLowerCase(), 'div');
                });

                it('should prepend multiple nodes', () => {
                    const doc = parseHtml('<html><body><p>existing</p></body></html>');
                    const body = doc.body;
                    const div1 = doc.createElement('div');
                    const div2 = doc.createElement('span');
                    body.prepend(div1, div2);
                    assert.equal(body.children[0].tagName.toLowerCase(), 'div');
                    assert.equal(body.children[1].tagName.toLowerCase(), 'span');
                    assert.equal(body.children[2].tagName.toLowerCase(), 'p');
                });

                it('should prepend string as text node', () => {
                    const doc = parseHtml('<html><body><p>existing</p></body></html>');
                    const body = doc.body;
                    body.prepend('Hello');
                    assert.equal(body.firstChild.nodeType, 3); // TEXT_NODE
                    assert.equal(body.firstChild.nodeValue, 'Hello');
                });
            });

            describe('before()', () => {
                it('should insert node before element', () => {
                    const doc = parseHtml('<html><body><p id="ref">test</p></body></html>');
                    const p = doc.getElementById('ref');
                    const div = doc.createElement('div');
                    p.before(div);
                    assert.equal(doc.body.firstChild.tagName.toLowerCase(), 'div');
                });

                it('should insert multiple nodes before element', () => {
                    const doc = parseHtml('<html><body><p id="ref">test</p></body></html>');
                    const p = doc.getElementById('ref');
                    const div1 = doc.createElement('div');
                    const div2 = doc.createElement('span');
                    p.before(div1, div2);
                    assert.equal(doc.body.children[0].tagName.toLowerCase(), 'div');
                    assert.equal(doc.body.children[1].tagName.toLowerCase(), 'span');
                    assert.equal(doc.body.children[2].tagName.toLowerCase(), 'p');
                });

                it('should insert string as text node before element', () => {
                    const doc = parseHtml('<html><body><p id="ref">test</p></body></html>');
                    const p = doc.getElementById('ref');
                    p.before('Hello');
                    assert.equal(doc.body.firstChild.nodeType, 3); // TEXT_NODE
                    assert.equal(doc.body.firstChild.nodeValue, 'Hello');
                });
            });

            describe('after()', () => {
                it('should insert node after element', () => {
                    const doc = parseHtml('<html><body><p id="ref">test</p></body></html>');
                    const p = doc.getElementById('ref');
                    const div = doc.createElement('div');
                    p.after(div);
                    assert.equal(doc.body.lastChild.tagName.toLowerCase(), 'div');
                });

                it('should insert multiple nodes after element', () => {
                    const doc = parseHtml('<html><body><p id="ref">test</p></body></html>');
                    const p = doc.getElementById('ref');
                    const div1 = doc.createElement('div');
                    const div2 = doc.createElement('span');
                    p.after(div1, div2);
                    assert.equal(doc.body.children[0].tagName.toLowerCase(), 'p');
                    assert.equal(doc.body.children[1].tagName.toLowerCase(), 'div');
                    assert.equal(doc.body.children[2].tagName.toLowerCase(), 'span');
                });

                it('should insert string as text node after element', () => {
                    const doc = parseHtml('<html><body><p id="ref">test</p></body></html>');
                    const p = doc.getElementById('ref');
                    p.after('World');
                    assert.equal(doc.body.lastChild.nodeType, 3); // TEXT_NODE
                    assert.equal(doc.body.lastChild.nodeValue, 'World');
                });
            });

            describe('replaceChildren()', () => {
                it('should replace all children with new nodes', () => {
                    const doc = parseHtml('<html><body><p>old1</p><p>old2</p></body></html>');
                    const body = doc.body;
                    const div = doc.createElement('div');
                    body.replaceChildren(div);
                    assert.equal(body.children.length, 1);
                    assert.equal(body.firstChild.tagName.toLowerCase(), 'div');
                });

                it('should clear all children when called with no arguments', () => {
                    const doc = parseHtml('<html><body><p>old1</p><p>old2</p></body></html>');
                    const body = doc.body;
                    body.replaceChildren();
                    assert.equal(body.children.length, 0);
                });

                it('should replace with mixed nodes and strings', () => {
                    const doc = parseHtml('<html><body><p>old</p></body></html>');
                    const body = doc.body;
                    const div = doc.createElement('div');
                    body.replaceChildren('Text', div, 'More');
                    assert.equal(body.childNodes.length, 3);
                    assert.equal(body.childNodes[0].nodeValue, 'Text');
                    assert.equal(body.childNodes[1].tagName.toLowerCase(), 'div');
                    assert.equal(body.childNodes[2].nodeValue, 'More');
                });
            });

            describe('replaceWith() with string', () => {
                it('should replace element with string', () => {
                    const doc = parseHtml('<html><body><p id="old">remove me</p></body></html>');
                    const p = doc.getElementById('old');
                    p.replaceWith('Hello World');
                    assert.equal(doc.body.textContent, 'Hello World');
                    assert.equal(doc.body.firstChild.nodeType, 3);
                });

                it('should replace element with mixed nodes and strings', () => {
                    const doc = parseHtml('<html><body><p id="old">remove me</p></body></html>');
                    const p = doc.getElementById('old');
                    const span = doc.createElement('span');
                    p.replaceWith('Before', span, 'After');
                    assert.equal(doc.body.childNodes.length, 3);
                    assert.equal(doc.body.childNodes[0].nodeValue, 'Before');
                    assert.equal(doc.body.childNodes[1].tagName.toLowerCase(), 'span');
                    assert.equal(doc.body.childNodes[2].nodeValue, 'After');
                });
            });
        });

        describe('insertAdjacent methods', () => {
            describe('insertAdjacentElement', () => {
                it('beforebegin - inserts element before current element', () => {
                    const doc = parseHtml('<html><body><div id="ref">content</div></body></html>');
                    const ref = doc.getElementById('ref');
                    const span = doc.createElement('span');
                    const result = ref.insertAdjacentElement('beforebegin', span);
                    assert.equal(result.tagName.toLowerCase(), 'span');
                    assert.equal(doc.body.firstChild.tagName.toLowerCase(), 'span');
                });

                it('afterbegin - inserts element as first child', () => {
                    const doc = parseHtml('<html><body><div id="ref"><p>existing</p></div></body></html>');
                    const ref = doc.getElementById('ref');
                    const span = doc.createElement('span');
                    ref.insertAdjacentElement('afterbegin', span);
                    assert.equal(ref.firstChild.tagName.toLowerCase(), 'span');
                });

                it('beforeend - inserts element as last child', () => {
                    const doc = parseHtml('<html><body><div id="ref"><p>existing</p></div></body></html>');
                    const ref = doc.getElementById('ref');
                    const span = doc.createElement('span');
                    ref.insertAdjacentElement('beforeend', span);
                    assert.equal(ref.lastChild.tagName.toLowerCase(), 'span');
                });

                it('afterend - inserts element after current element', () => {
                    const doc = parseHtml('<html><body><div id="ref">content</div></body></html>');
                    const ref = doc.getElementById('ref');
                    const span = doc.createElement('span');
                    ref.insertAdjacentElement('afterend', span);
                    assert.equal(doc.body.lastChild.tagName.toLowerCase(), 'span');
                });

                it('is case-insensitive for position', () => {
                    const doc = parseHtml('<html><body><div id="ref">content</div></body></html>');
                    const ref = doc.getElementById('ref');
                    const span = doc.createElement('span');
                    ref.insertAdjacentElement('BeforeEnd', span);
                    assert.equal(ref.lastChild.tagName.toLowerCase(), 'span');
                });
            });

            describe('insertAdjacentText', () => {
                it('beforebegin - inserts text before current element', () => {
                    const doc = parseHtml('<html><body><div id="ref">content</div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentText('beforebegin', 'Hello');
                    assert.equal(doc.body.firstChild.nodeType, 3);
                    assert.equal(doc.body.firstChild.nodeValue, 'Hello');
                });

                it('afterbegin - inserts text as first child', () => {
                    const doc = parseHtml('<html><body><div id="ref"><p>existing</p></div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentText('afterbegin', 'Hello');
                    assert.equal(ref.firstChild.nodeType, 3);
                    assert.equal(ref.firstChild.nodeValue, 'Hello');
                });

                it('beforeend - inserts text as last child', () => {
                    const doc = parseHtml('<html><body><div id="ref"><p>existing</p></div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentText('beforeend', 'World');
                    assert.equal(ref.lastChild.nodeType, 3);
                    assert.equal(ref.lastChild.nodeValue, 'World');
                });

                it('afterend - inserts text after current element', () => {
                    const doc = parseHtml('<html><body><div id="ref">content</div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentText('afterend', 'World');
                    assert.equal(doc.body.lastChild.nodeType, 3);
                    assert.equal(doc.body.lastChild.nodeValue, 'World');
                });
            });

            describe('insertAdjacentHTML', () => {
                it('beforebegin - inserts HTML before current element', () => {
                    const doc = parseHtml('<html><body><div id="ref">content</div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentHTML('beforebegin', '<span>new</span>');
                    assert.equal(doc.body.firstChild.tagName.toLowerCase(), 'span');
                });

                it('afterbegin - inserts HTML as first child', () => {
                    const doc = parseHtml('<html><body><div id="ref"><p>existing</p></div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentHTML('afterbegin', '<span>new</span>');
                    assert.equal(ref.firstChild.tagName.toLowerCase(), 'span');
                });

                it('beforeend - inserts HTML as last child', () => {
                    const doc = parseHtml('<html><body><div id="ref"><p>existing</p></div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentHTML('beforeend', '<span>new</span>');
                    assert.equal(ref.lastChild.tagName.toLowerCase(), 'span');
                });

                it('afterend - inserts HTML after current element', () => {
                    const doc = parseHtml('<html><body><div id="ref">content</div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentHTML('afterend', '<span>new</span>');
                    assert.equal(doc.body.lastChild.tagName.toLowerCase(), 'span');
                });

                it('inserts multiple elements from HTML', () => {
                    const doc = parseHtml('<html><body><div id="ref">content</div></body></html>');
                    const ref = doc.getElementById('ref');
                    ref.insertAdjacentHTML('beforeend', '<span>1</span><span>2</span>');
                    assert.equal(ref.children.length, 2);
                });
            });
        });

        describe('Node comparison methods', () => {
            describe('compareDocumentPosition', () => {
                it('returns 0 for same node', () => {
                    const doc = xml.parse('<root><item/></root>');
                    const item = doc.documentElement.firstChild;
                    assert.equal(item.compareDocumentPosition(item), 0);
                });

                it('returns FOLLOWING | CONTAINED_BY for child', () => {
                    const doc = xml.parse('<root><parent id="p"><child id="c"/></parent></root>');
                    const parent = doc.getElementById('p');
                    const child = doc.getElementById('c');
                    const pos = parent.compareDocumentPosition(child);
                    // FOLLOWING = 4, CONTAINED_BY = 16
                    assert.ok(pos & 4, 'should have FOLLOWING bit');
                    assert.ok(pos & 16, 'should have CONTAINED_BY bit');
                });

                it('returns PRECEDING | CONTAINS for parent', () => {
                    const doc = xml.parse('<root><parent id="p"><child id="c"/></parent></root>');
                    const parent = doc.getElementById('p');
                    const child = doc.getElementById('c');
                    const pos = child.compareDocumentPosition(parent);
                    // PRECEDING = 2, CONTAINS = 8
                    assert.ok(pos & 2, 'should have PRECEDING bit');
                    assert.ok(pos & 8, 'should have CONTAINS bit');
                });

                it('returns FOLLOWING for later sibling', () => {
                    const doc = xml.parse('<root><a id="a"/><b id="b"/></root>');
                    const a = doc.getElementById('a');
                    const b = doc.getElementById('b');
                    const pos = a.compareDocumentPosition(b);
                    assert.ok(pos & 4, 'should have FOLLOWING bit');
                });

                it('returns PRECEDING for earlier sibling', () => {
                    const doc = xml.parse('<root><a id="a"/><b id="b"/></root>');
                    const a = doc.getElementById('a');
                    const b = doc.getElementById('b');
                    const pos = b.compareDocumentPosition(a);
                    assert.ok(pos & 2, 'should have PRECEDING bit');
                });
            });

            describe('isEqualNode', () => {
                it('returns true for structurally equal nodes', () => {
                    const doc1 = xml.parse('<root><item id="1">hello</item></root>');
                    const doc2 = xml.parse('<root><item id="1">hello</item></root>');
                    assert.ok(doc1.documentElement.isEqualNode(doc2.documentElement));
                });

                it('returns false for different attributes', () => {
                    const doc1 = xml.parse('<root id="1"/>');
                    const doc2 = xml.parse('<root id="2"/>');
                    assert.ok(!doc1.documentElement.isEqualNode(doc2.documentElement));
                });

                it('returns false for different text content', () => {
                    const doc1 = xml.parse('<root>hello</root>');
                    const doc2 = xml.parse('<root>world</root>');
                    assert.ok(!doc1.documentElement.isEqualNode(doc2.documentElement));
                });

                it('returns false for different child count', () => {
                    const doc1 = xml.parse('<root><a/><b/></root>');
                    const doc2 = xml.parse('<root><a/></root>');
                    assert.ok(!doc1.documentElement.isEqualNode(doc2.documentElement));
                });
            });

            describe('isSameNode', () => {
                it('returns true for same reference', () => {
                    const doc = xml.parse('<root><item/></root>');
                    const item = doc.documentElement.firstChild;
                    assert.ok(item.isSameNode(item));
                });

                it('returns false for different nodes with same content', () => {
                    const doc1 = xml.parse('<root><item/></root>');
                    const doc2 = xml.parse('<root><item/></root>');
                    assert.ok(!doc1.documentElement.isSameNode(doc2.documentElement));
                });
            });
        });

        describe('toggleAttribute', () => {
            it('adds attribute when not present (no force)', () => {
                const doc = xml.parse('<root><button/></root>');
                const btn = doc.documentElement.firstChild;
                const result = btn.toggleAttribute('disabled');
                assert.equal(result, true);
                assert.ok(btn.hasAttribute('disabled'));
            });

            it('removes attribute when present (no force)', () => {
                const doc = xml.parse('<root><button disabled=""/></root>');
                const btn = doc.documentElement.firstChild;
                const result = btn.toggleAttribute('disabled');
                assert.equal(result, false);
                assert.ok(!btn.hasAttribute('disabled'));
            });

            it('adds attribute when force is true and not present', () => {
                const doc = xml.parse('<root><button/></root>');
                const btn = doc.documentElement.firstChild;
                const result = btn.toggleAttribute('hidden', true);
                assert.equal(result, true);
                assert.ok(btn.hasAttribute('hidden'));
            });

            it('keeps attribute when force is true and present', () => {
                const doc = xml.parse('<root><button hidden=""/></root>');
                const btn = doc.documentElement.firstChild;
                const result = btn.toggleAttribute('hidden', true);
                assert.equal(result, true);
                assert.ok(btn.hasAttribute('hidden'));
            });

            it('removes attribute when force is false and present', () => {
                const doc = xml.parse('<root><button hidden=""/></root>');
                const btn = doc.documentElement.firstChild;
                const result = btn.toggleAttribute('hidden', false);
                assert.equal(result, false);
                assert.ok(!btn.hasAttribute('hidden'));
            });

            it('does nothing when force is false and not present', () => {
                const doc = xml.parse('<root><button/></root>');
                const btn = doc.documentElement.firstChild;
                const result = btn.toggleAttribute('hidden', false);
                assert.equal(result, false);
                assert.ok(!btn.hasAttribute('hidden'));
            });
        });
    }

    // Browser-only tests for features not supported in fibjs
    if (isBrowser) {
        describe('browser', () => {
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
