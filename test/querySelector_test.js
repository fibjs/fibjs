if (typeof window === 'undefined') {
    var { describe, it, beforeEach, assert } = require('test');
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

describe('querySelector', () => {
    let testDoc;
    let rootElement;

    beforeEach(() => {
        // Create test document structure for each test - use HTML document for consistent behavior
        testDoc = newHtmlDoc();
        if (isBrowser) {
            rootElement = testDoc.body;
            rootElement.innerHTML = '';
        } else {
            // For fibjs HTML documents, we need to create body element if it doesn't exist
            if (!testDoc.body) {
                // Create basic HTML structure
                if (!testDoc.documentElement) {
                    const html = testDoc.createElement('html');
                    testDoc.appendChild(html);
                }
                if (!testDoc.body) {
                    const body = testDoc.createElement('body');
                    testDoc.documentElement.appendChild(body);
                }
            }
            rootElement = testDoc.body;
            // Clear any existing content
            while (rootElement.firstChild) {
                rootElement.removeChild(rootElement.firstChild);
            }
        }
    });

    // Helper function to create test DOM structure
    function createTestStructure() {
        // Create basic structure:
        // <root>
        //   <header id="main-header" class="site-header">
        //     <nav class="navigation">
        //       <ul class="nav-list">
        //         <li class="nav-item active"><a href="#home">Home</a></li>
        //         <li class="nav-item"><a href="#about">About</a></li>
        //       </ul>
        //     </nav>
        //   </header>
        //   <main id="content" class="main-content">
        //     <article class="post featured" data-category="tech">
        //       <h1>First Post</h1>
        //       <p class="summary">Summary text</p>
        //       <div class="content">Content here</div>
        //     </article>
        //     <article class="post" data-category="news">
        //       <h2>Second Post</h2>
        //       <p>Regular text</p>
        //     </article>
        //   </main>
        //   <footer class="site-footer">
        //     <p>Footer content</p>
        //   </footer>
        // </root>

        const header = testDoc.createElement('header');
        header.setAttribute('id', 'main-header');
        header.setAttribute('class', 'site-header');

        const nav = testDoc.createElement('nav');
        nav.setAttribute('class', 'navigation');

        const ul = testDoc.createElement('ul');
        ul.setAttribute('class', 'nav-list');

        const li1 = testDoc.createElement('li');
        li1.setAttribute('class', 'nav-item active');
        const a1 = testDoc.createElement('a');
        a1.setAttribute('href', '#home');
        a1.appendChild(testDoc.createTextNode('Home'));
        li1.appendChild(a1);

        const li2 = testDoc.createElement('li');
        li2.setAttribute('class', 'nav-item');
        const a2 = testDoc.createElement('a');
        a2.setAttribute('href', '#about');
        a2.appendChild(testDoc.createTextNode('About'));
        li2.appendChild(a2);

        ul.appendChild(li1);
        ul.appendChild(li2);
        nav.appendChild(ul);
        header.appendChild(nav);

        const main = testDoc.createElement('main');
        main.setAttribute('id', 'content');
        main.setAttribute('class', 'main-content');

        const article1 = testDoc.createElement('article');
        article1.setAttribute('class', 'post featured');
        article1.setAttribute('data-category', 'tech');

        const h1 = testDoc.createElement('h1');
        h1.appendChild(testDoc.createTextNode('First Post'));
        const p1 = testDoc.createElement('p');
        p1.setAttribute('class', 'summary');
        p1.appendChild(testDoc.createTextNode('Summary text'));
        const div1 = testDoc.createElement('div');
        div1.setAttribute('class', 'content');
        div1.appendChild(testDoc.createTextNode('Content here'));

        article1.appendChild(h1);
        article1.appendChild(p1);
        article1.appendChild(div1);

        const article2 = testDoc.createElement('article');
        article2.setAttribute('class', 'post');
        article2.setAttribute('data-category', 'news');

        const h2 = testDoc.createElement('h2');
        h2.appendChild(testDoc.createTextNode('Second Post'));
        const p2 = testDoc.createElement('p');
        p2.appendChild(testDoc.createTextNode('Regular text'));

        article2.appendChild(h2);
        article2.appendChild(p2);

        main.appendChild(article1);
        main.appendChild(article2);

        const footer = testDoc.createElement('footer');
        footer.setAttribute('class', 'site-footer');
        const pFooter = testDoc.createElement('p');
        pFooter.appendChild(testDoc.createTextNode('Footer content'));
        footer.appendChild(pFooter);

        rootElement.appendChild(header);
        rootElement.appendChild(main);
        rootElement.appendChild(footer);
    }

    describe('Basic selectors', () => {
        it("querySelector should return first matching element", () => {
            createTestStructure();

            // Test element selector
            const header = rootElement.querySelector('header');
            assert.notEqual(header, null);
            assert.equal(header.tagName, 'HEADER');

            // Test that it returns first match
            const firstP = rootElement.querySelector('p');
            assert.notEqual(firstP, null);
            assert.equal(firstP.className, 'summary');
        });

        it("querySelector should return null when no match", () => {
            createTestStructure();

            const nonExistent = rootElement.querySelector('video');
            assert.equal(nonExistent, null);
        });

        it("querySelectorAll should return all matching elements", () => {
            createTestStructure();

            // Test element selector
            const articles = rootElement.querySelectorAll('article');
            assert.equal(articles.length, 2);
            assert.equal(articles[0].className, 'post featured');
            assert.equal(articles[1].className, 'post');

            // Test paragraph elements
            const paragraphs = rootElement.querySelectorAll('p');
            assert.equal(paragraphs.length, 3); // summary, regular text, footer
        });

        it("querySelectorAll should return empty NodeList when no match", () => {
            createTestStructure();

            const nonExistent = rootElement.querySelectorAll('video');
            assert.equal(nonExistent.length, 0);
        });
    });

    describe('ID and class selectors', () => {
        it("should select by ID", () => {
            createTestStructure();

            const header = rootElement.querySelector('#main-header');
            assert.notEqual(header, null);
            assert.equal(header.tagName, 'HEADER');
            assert.equal(header.id, 'main-header');

            const content = rootElement.querySelector('#content');
            assert.notEqual(content, null);
            assert.equal(content.tagName, 'MAIN');
        });

        it("should select by class", () => {
            createTestStructure();

            const navItem = rootElement.querySelector('.nav-item');
            assert.notEqual(navItem, null);
            assert.equal(navItem.tagName, 'LI');

            const allNavItems = rootElement.querySelectorAll('.nav-item');
            assert.equal(allNavItems.length, 2);
        });

        it("should select by multiple classes", () => {
            createTestStructure();

            const featured = rootElement.querySelector('.post.featured');
            assert.notEqual(featured, null);
            assert.equal(featured.getAttribute('data-category'), 'tech');

            const activeNavItem = rootElement.querySelector('.nav-item.active');
            assert.notEqual(activeNavItem, null);
            assert.equal(activeNavItem.querySelector('a').getAttribute('href'), '#home');
        });
    });

    describe('Descendant and child selectors', () => {
        it("should select descendants", () => {
            createTestStructure();

            // Test descendant selector
            const navLinks = rootElement.querySelectorAll('nav a');
            assert.equal(navLinks.length, 2);
            assert.equal(navLinks[0].textContent, 'Home');
            assert.equal(navLinks[1].textContent, 'About');

            // Test deeper nesting
            const headerParagraphs = rootElement.querySelectorAll('header p');
            assert.equal(headerParagraphs.length, 0); // No paragraphs in header
        });

        it("should select direct children", () => {
            createTestStructure();

            // Test direct child selector
            const mainChildren = rootElement.querySelectorAll('main > article');
            assert.equal(mainChildren.length, 2);

            // Test that it doesn't select deeper descendants
            const directLi = rootElement.querySelectorAll('nav > li');
            assert.equal(directLi.length, 0); // li are children of ul, not nav

            const ulLi = rootElement.querySelectorAll('ul > li');
            assert.equal(ulLi.length, 2);
        });

        it("should select adjacent siblings", () => {
            createTestStructure();

            // Test adjacent sibling
            const headerNext = rootElement.querySelector('header + main');
            assert.notEqual(headerNext, null);
            assert.equal(headerNext.tagName, 'MAIN');

            // Test that non-adjacent doesn't match
            const headerFooter = rootElement.querySelector('header + footer');
            assert.equal(headerFooter, null);
        });

        it("should select general siblings", () => {
            createTestStructure();

            // Test general sibling
            const headerSiblings = rootElement.querySelectorAll('header ~ *');
            assert.equal(headerSiblings.length, 2); // main and footer

            const mainSiblings = rootElement.querySelectorAll('main ~ footer');
            assert.equal(mainSiblings.length, 1);
        });
    });

    describe('Attribute selectors', () => {
        it("should select by attribute existence", () => {
            createTestStructure();

            // Test attribute existence
            const withHref = rootElement.querySelectorAll('[href]');
            assert.equal(withHref.length, 2);

            const withDataCategory = rootElement.querySelectorAll('[data-category]');
            assert.equal(withDataCategory.length, 2);
        });

        it("should select by exact attribute value", () => {
            createTestStructure();

            // Test exact value
            const homeLink = rootElement.querySelector('[href="#home"]');
            assert.notEqual(homeLink, null);
            assert.equal(homeLink.textContent, 'Home');

            const techArticle = rootElement.querySelector('[data-category="tech"]');
            assert.notEqual(techArticle, null);
            assert.equal(techArticle.querySelector('h1').textContent, 'First Post');
        });

        it("should select by attribute value prefix", () => {
            createTestStructure();

            // Test value starts with
            const hashLinks = rootElement.querySelectorAll('[href^="#"]');
            assert.equal(hashLinks.length, 2);
        });

        it("should select by attribute value containing substring", () => {
            createTestStructure();

            // Test value contains
            const postElements = rootElement.querySelectorAll('[class*="post"]');
            assert.equal(postElements.length, 2);

            const contentClasses = rootElement.querySelectorAll('[class*="content"]');
            assert.equal(contentClasses.length, 2); // main-content and content div
        });

        it("should select by attribute value ending", () => {
            createTestStructure();

            // Test value ends with
            const aboutLink = rootElement.querySelectorAll('[href$="about"]');
            assert.equal(aboutLink.length, 1);
        });

        it("should handle advanced attribute operators", () => {
            const container = testDoc.createElement('div');

            // Create elements for testing ~= (word match) selector
            const elem1 = testDoc.createElement('div');
            elem1.setAttribute('class', 'nav item active selected');

            const elem2 = testDoc.createElement('div');
            elem2.setAttribute('class', 'navigation menu');

            // Create elements for testing |= (language match) selector  
            const elem3 = testDoc.createElement('div');
            elem3.setAttribute('lang', 'en-US');

            const elem4 = testDoc.createElement('div');
            elem4.setAttribute('lang', 'en');

            const elem5 = testDoc.createElement('div');
            elem5.setAttribute('lang', 'fr-FR');

            container.appendChild(elem1);
            container.appendChild(elem2);
            container.appendChild(elem3);
            container.appendChild(elem4);
            container.appendChild(elem5);
            rootElement.appendChild(container);

            // Test ~= selector (word match)
            const wordMatches = rootElement.querySelectorAll('[class~="item"]');
            assert.equal(wordMatches.length, 1);
            assert.equal(wordMatches[0], elem1);

            const activeMatches = rootElement.querySelectorAll('[class~="selected"]');
            assert.equal(activeMatches.length, 1);
            assert.equal(activeMatches[0], elem1);

            // Test |= selector (language/prefix match)
            const langMatches = rootElement.querySelectorAll('[lang|="en"]');
            assert.equal(langMatches.length, 2); // both en-US and en

            const frenchMatches = rootElement.querySelectorAll('[lang|="fr"]');
            assert.equal(frenchMatches.length, 1);
            assert.equal(frenchMatches[0], elem5);
        });
    });

    describe('Pseudo-class selectors', () => {
        it("should select first child", () => {
            createTestStructure();

            // Test :first-child - should find the first li element in nav
            const firstNavItem = rootElement.querySelector('li:first-child');
            assert.notEqual(firstNavItem, null);
            assert.equal(firstNavItem.className, 'nav-item active');
        });

        it("should select last child", () => {
            createTestStructure();

            // Test :last-child - select the last article in main
            const lastArticle = rootElement.querySelector('main > article:last-child');
            assert.notEqual(lastArticle, null);
            assert.equal(lastArticle.tagName, 'ARTICLE');
            assert.equal(lastArticle.getAttribute('data-category'), 'news');

            // Test :last-child for any element - should find last child of first article
            const anyLastChild = rootElement.querySelector('main article:first-child :last-child');
            assert.notEqual(anyLastChild, null);
            // This should be the div.content element (last child of first article)
            assert.equal(anyLastChild.className, 'content');
        });

        it("should select nth-child", () => {
            createTestStructure();

            // Test :nth-child - should find the second li element
            const secondNavItem = rootElement.querySelector('li:nth-child(2)');
            assert.notEqual(secondNavItem, null);
            assert.equal(secondNavItem.querySelector('a').textContent, 'About');
        });

        it("should handle advanced nth-child selectors", () => {
            // Create test structure specifically for nth-child testing
            const container = testDoc.createElement('div');
            container.innerHTML = `
                <div class="n0">0</div>
                <b class="n1">1</b>
                <span class="n2">2</span>
                <div class="n3">3</div>
                <b class="n4">4</b>
                <span class="n5">5</span>
                <div class="n6">6</div>
                <b class="n7">7</b>
                <span class="n8">8</span>
                <div class="n9">9</div>
                <i class="n10">10</i>
            `;
            rootElement.appendChild(container);

            // Test :nth-child(n+8) - elements from 8th position onwards
            const fromEighth = rootElement.querySelectorAll(':nth-child(n+8)');
            assert.ok(fromEighth.length >= 3); // at least elements from position 8 onwards

            // Test :nth-child(2n) - even positioned elements
            const evenPositions = rootElement.querySelectorAll(':nth-child(2n)');
            assert.ok(evenPositions.length >= 5); // even positions

            // Test :nth-child(-n + 3) - first 3 elements
            const firstThree = rootElement.querySelectorAll(':nth-child(-n + 3)');
            assert.ok(firstThree.length >= 3);

            // Test :nth-child(3n+1) - positions 1, 4, 7, 10...
            const threeNPlusOne = rootElement.querySelectorAll(':nth-child(3n+1)');
            assert.ok(threeNPlusOne.length >= 3);

            // Test :nth-child(odd) and :nth-child(even)
            const oddChildren = rootElement.querySelectorAll(':nth-child(odd)');
            const evenChildren = rootElement.querySelectorAll(':nth-child(even)');
            assert.ok(oddChildren.length > 0);
            assert.ok(evenChildren.length > 0);
        });

        it("should handle :only-child and type-based pseudo-classes", () => {
            const container = testDoc.createElement('div');
            container.innerHTML = `
                <section>
                    <h1>Only heading</h1>
                </section>
                <section>
                    <div>First div</div>
                    <div>Second div</div>
                    <span>First span</span>
                    <span>Second span</span>
                </section>
            `;
            rootElement.appendChild(container);

            // Test :only-child
            const onlyChild = rootElement.querySelectorAll(':only-child');
            assert.ok(onlyChild.length >= 1); // h1 is only child

            // Test :first-of-type and :last-of-type
            const firstOfType = rootElement.querySelectorAll(':first-of-type');
            assert.ok(firstOfType.length >= 4); // first section, h1, first div, first span

            const lastOfType = rootElement.querySelectorAll(':last-of-type');
            assert.ok(lastOfType.length >= 4); // last section, h1, last div, last span

            // Test :nth-of-type
            const secondOfType = rootElement.querySelectorAll(':nth-of-type(2)');
            assert.ok(secondOfType.length >= 2); // second div, second span
        });
    });

    describe('Complex selectors', () => {
        it("should handle complex combinations", () => {
            createTestStructure();

            // Complex selector combining multiple types
            const complexQuery = 'main article.post[data-category="tech"] h1';
            const result = rootElement.querySelector(complexQuery);
            assert.notEqual(result, null);
            assert.equal(result.textContent, 'First Post');

            // Multiple class and attribute selector
            const navActiveLink = rootElement.querySelector('.nav-item.active a[href^="#"]');
            assert.notEqual(navActiveLink, null);
            assert.equal(navActiveLink.textContent, 'Home');
        });

        it("should handle multiple selectors", () => {
            createTestStructure();

            // Test comma-separated selectors
            const headersAndFooters = rootElement.querySelectorAll('header, footer');
            assert.equal(headersAndFooters.length, 2);

            const headingsAndLinks = rootElement.querySelectorAll('h1, h2, a');
            assert.equal(headingsAndLinks.length, 4); // 1 h1, 1 h2, 2 a elements
        });

        it("should handle deeply nested selectors", () => {
            createTestStructure();

            // Deep nesting
            const deepQuery = 'header nav ul li a';
            const deepResults = rootElement.querySelectorAll(deepQuery);
            assert.equal(deepResults.length, 2);

            // Very specific selector
            const specificQuery = 'main > article.featured > div.content';
            const specificResult = rootElement.querySelector(specificQuery);
            assert.notEqual(specificResult, null);
            assert.equal(specificResult.textContent, 'Content here');
        });
    });

    describe('Edge cases and error handling', () => {
        it("should handle invalid selectors gracefully", () => {
            createTestStructure();

            // Test selectors that should definitely throw errors in most implementations
            const selectorsThatShouldThrow = [
                '',            // empty selector - universally throws
            ];

            selectorsThatShouldThrow.forEach(selector => {
                assert.throws(() => {
                    rootElement.querySelector(selector);
                }, /Failed to execute|invalid selector|Uncaught|SyntaxError/i, `querySelector should throw for: ${selector}`);

                assert.throws(() => {
                    rootElement.querySelectorAll(selector);
                }, /Failed to execute|invalid selector|Uncaught|SyntaxError/i, `querySelectorAll should throw for: ${selector}`);
            });

            // Test selectors that might throw or return null/empty depending on implementation
            const problematicSelectors = [
                '[attr',       // unclosed bracket
                'attr]',       // unmatched bracket
                'div > > p',   // double combinator
                'div + + p',   // double combinator
                'div ~ ~ p',   // double combinator
                '1',           // starts with digit
                '.1',          // class starts with digit  
                '#1',          // id starts with digit
                'a.',          // trailing dot
                'a#',          // trailing hash
                '..class',     // double dots
                '##id'         // double hashes
            ];

            problematicSelectors.forEach(selector => {
                try {
                    const result = rootElement.querySelector(selector);
                    // If it doesn't throw, it should return null (no match)
                    assert.equal(result, null, `querySelector should return null for problematic selector: ${selector}`);
                } catch (e) {
                    // If it throws, that's also acceptable
                    assert.ok(true, `querySelector correctly threw error for: ${selector}`);
                }

                try {
                    const results = rootElement.querySelectorAll(selector);
                    // If it doesn't throw, it should return empty NodeList
                    assert.equal(results.length, 0, `querySelectorAll should return empty for problematic selector: ${selector}`);
                } catch (e) {
                    // If it throws, that's also acceptable
                    assert.ok(true, `querySelectorAll correctly threw error for: ${selector}`);
                }
            });
        });

        it("should work with elements without parent", () => {
            const orphanElement = testDoc.createElement('div');
            const p = testDoc.createElement('p');
            p.appendChild(testDoc.createTextNode('Test'));
            orphanElement.appendChild(p);

            const result = orphanElement.querySelector('p');
            assert.notEqual(result, null);
            assert.equal(result.textContent, 'Test');

            const results = orphanElement.querySelectorAll('p');
            assert.equal(results.length, 1);
        });

        it("should handle special characters in attributes", () => {
            const container = testDoc.createElement('div');
            const elem = testDoc.createElement('span');
            elem.setAttribute('data-test', 'value with spaces');
            elem.setAttribute('data-special', 'value-with-dashes');
            elem.setAttribute('data-colon', 'a:b');
            container.appendChild(elem);
            rootElement.appendChild(container);

            const bySpaces = rootElement.querySelector('[data-test="value with spaces"]');
            assert.notEqual(bySpaces, null);

            const byDashes = rootElement.querySelector('[data-special="value-with-dashes"]');
            assert.notEqual(byDashes, null);

            // Test colon in attribute value (both escaped and unescaped)
            const byColon1 = rootElement.querySelector('[data-colon="a\\:b"]');
            assert.notEqual(byColon1, null);

            const byColon2 = rootElement.querySelector('[data-colon="a:b"]');
            assert.notEqual(byColon2, null);
        });

        it("should be case sensitive for XML, case insensitive for HTML", () => {
            const container = testDoc.createElement('div');
            const elem = testDoc.createElement('SPAN');
            elem.setAttribute('ID', 'TestID');
            elem.setAttribute('CLASS', 'TestClass');
            container.appendChild(elem);
            rootElement.appendChild(container);

            // In HTML mode, tag names should be case insensitive
            const byTag = rootElement.querySelector('span');
            assert.notEqual(byTag, null);

            // ID should work (case handling depends on implementation)
            const byId = rootElement.querySelector('#TestID');
            assert.notEqual(byId, null);

            // Class should work
            const byClass = rootElement.querySelector('.TestClass');
            assert.notEqual(byClass, null);
        });

        it("should handle whitespace normalization in selectors", () => {
            createTestStructure();

            // Test various whitespace patterns
            const whitespaceSelectors = [
                'header nav ul li',
                'header  nav  ul  li',
                'header\tnav\tul\tli',
                'header \t\n nav \t\n ul \t\n li'
            ];

            whitespaceSelectors.forEach(selector => {
                const results = rootElement.querySelectorAll(selector);
                assert.equal(results.length, 2, `Failed for selector with whitespace: "${selector}"`);
            });

            // Test child combinator with various whitespace
            const childSelectors = [
                'main>article',
                'main >article',
                'main> article',
                'main > article',
                'main\t>\tarticle'
            ];

            childSelectors.forEach(selector => {
                const results = rootElement.querySelectorAll(selector);
                assert.equal(results.length, 2, `Failed for child selector: "${selector}"`);
            });
        });

        it("should handle pseudo-elements correctly", () => {
            createTestStructure();

            // Pseudo-elements should return null/empty results
            const beforeElements = rootElement.querySelectorAll('::before');
            assert.equal(beforeElements.length, 0);

            const afterElements = rootElement.querySelectorAll('::after');
            assert.equal(afterElements.length, 0);

            const webkitElements = rootElement.querySelectorAll('::-webkit-inner-spin-button');
            assert.equal(webkitElements.length, 0);

            // Test with querySelector
            const beforeElement = rootElement.querySelector('::before');
            assert.equal(beforeElement, null);

            const afterElement = rootElement.querySelector('::after');
            assert.equal(afterElement, null);
        });

        it("should maintain correct document order in results", () => {
            const container = testDoc.createElement('div');

            // Create elements in specific order
            for (let i = 1; i <= 5; i++) {
                const elem = testDoc.createElement(i % 2 === 0 ? 'span' : 'div');
                elem.className = 'ordered-test';
                elem.setAttribute('data-order', i.toString());
                elem.textContent = `Element ${i}`;
                container.appendChild(elem);
            }
            rootElement.appendChild(container);

            // Test that querySelectorAll maintains document order
            const orderedResults = rootElement.querySelectorAll('.ordered-test');
            assert.equal(orderedResults.length, 5);

            for (let i = 0; i < orderedResults.length; i++) {
                assert.equal(orderedResults[i].getAttribute('data-order'), (i + 1).toString());
            }

            // Test with reverse order selector - should still return in document order
            const reverseOrderSelector = rootElement.querySelectorAll('span.ordered-test, div.ordered-test');
            assert.equal(reverseOrderSelector.length, 5);

            // Results should be in document order, not selector order
            for (let i = 0; i < reverseOrderSelector.length; i++) {
                assert.equal(reverseOrderSelector[i].getAttribute('data-order'), (i + 1).toString());
            }
        });

        it("should handle very long selector strings", () => {
            // Create a deep nested structure
            let current = rootElement;
            for (let i = 0; i < 10; i++) {
                const div = testDoc.createElement('div');
                div.className = `level-${i}`;
                current.appendChild(div);
                current = div;
            }

            // Add target element
            const target = testDoc.createElement('span');
            target.className = 'deep-target';
            current.appendChild(target);

            // Create very long selector
            const longSelector = Array.from({ length: 10 }, (_, i) => `.level-${i}`).join(' ') + ' .deep-target';

            const result = rootElement.querySelector(longSelector);
            assert.notEqual(result, null);
            assert.equal(result.className, 'deep-target');
        });
    });

    describe('Advanced edge cases and special scenarios', () => {
        it("should handle extra whitespace in selectors", () => {
            createTestStructure();

            // Test with extra spaces
            const withSpaces = rootElement.querySelectorAll('header     nav     ul     li');
            assert.equal(withSpaces.length, 2);

            const moreSpaces = rootElement.querySelector('   main   >   article   ');
            assert.notEqual(moreSpaces, null);
            assert.equal(moreSpaces.tagName, 'ARTICLE');
        });

        it("should handle attribute selector with special characters", () => {
            const container = testDoc.createElement('div');
            const elem1 = testDoc.createElement('div');
            elem1.setAttribute('data-test', 'Hello, World');
            const elem2 = testDoc.createElement('div');
            elem2.setAttribute('data-test', 'Hello "quoted" value');
            const elem3 = testDoc.createElement('div');
            elem3.setAttribute('data-brackets', '[ bracket content ]');

            container.appendChild(elem1);
            container.appendChild(elem2);
            container.appendChild(elem3);
            rootElement.appendChild(container);

            // Test comma in attribute value
            const withComma = rootElement.querySelector('[data-test="Hello, World"]');
            assert.notEqual(withComma, null);

            // Test quotes in attribute value (escaped)
            const withQuotes = rootElement.querySelector('[data-test="Hello \\"quoted\\" value"]');
            assert.notEqual(withQuotes, null);

            // Test brackets in attribute value
            const withBrackets = rootElement.querySelector('[data-brackets="[ bracket content ]"]');
            assert.notEqual(withBrackets, null);
        });

        it("should handle attribute selector spacing", () => {
            const container = testDoc.createElement('div');
            const elem = testDoc.createElement('span');
            elem.setAttribute('data-test', 'test-value');
            container.appendChild(elem);
            rootElement.appendChild(container);

            // Test attribute selector with spaces around operators
            const result1 = rootElement.querySelector('[ data-test = "test-value" ]');
            assert.notEqual(result1, null);
            assert.equal(result1.tagName, 'SPAN');

            // Test attribute selector with extra spaces
            const result2 = rootElement.querySelector('[  data-test  =  "test-value"  ]');
            assert.notEqual(result2, null);
            assert.equal(result2.tagName, 'SPAN');
        });

        it("should handle complex selector combinations", () => {
            createTestStructure();

            // Test multiple comma-separated selectors with varying complexity
            const multipleSelectors = rootElement.querySelectorAll('header nav, footer p, main > article');
            assert.equal(multipleSelectors.length, 4); // 1 nav + 1 footer p + 2 articles

            // Test more specific selector combination that returns exactly 3 elements
            const specificSelectors = rootElement.querySelectorAll('header nav, footer p, main > article:first-child');
            assert.equal(specificSelectors.length, 3); // 1 nav + 1 footer p + 1 first article

            // Test descendant + adjacent sibling combination
            const complexSibling = rootElement.querySelector('article h1 + p');
            assert.notEqual(complexSibling, null);
            assert.equal(complexSibling.className, 'summary');

            // Test very specific nested selector
            const verySpecific = rootElement.querySelector('header.site-header nav.navigation ul.nav-list li.nav-item.active a[href="#home"]');
            assert.notEqual(verySpecific, null);
            assert.equal(verySpecific.textContent, 'Home');
        });

        it("should handle selector list edge cases", () => {
            createTestStructure();

            // Test selector list with empty parts - should throw error according to CSS spec
            assert.throws(() => {
                rootElement.querySelectorAll('header, , footer');
            });

            // Test selector list with whitespace-only selector - should throw error
            assert.throws(() => {
                rootElement.querySelectorAll('header,    , footer');
            });
        });

        it("should handle escaped characters in CSS identifiers", () => {
            // Create elements with special characters in class names
            const container = testDoc.createElement('div');
            const elem1 = testDoc.createElement('div');
            elem1.className = 'test:class';
            const elem2 = testDoc.createElement('div');
            elem2.className = 'test.class';
            const elem3 = testDoc.createElement('div');
            elem3.setAttribute('id', 'test:id');

            container.appendChild(elem1);
            container.appendChild(elem2);
            container.appendChild(elem3);
            rootElement.appendChild(container);

            // Test escaping colon in class name (CSS requires escaping)
            const colonClass = rootElement.querySelector('.test\\:class');
            assert.notEqual(colonClass, null);
            assert.equal(colonClass.className, 'test:class');

            // Test escaping dot in class name
            const dotClass = rootElement.querySelector('.test\\.class');
            assert.notEqual(dotClass, null);
            assert.equal(dotClass.className, 'test.class');

            // Test escaping colon in id
            const colonId = rootElement.querySelector('#test\\:id');
            assert.notEqual(colonId, null);
            assert.equal(colonId.id, 'test:id');
        });

        it("should handle unicode characters in selectors", () => {
            const container = testDoc.createElement('div');
            const elem1 = testDoc.createElement('div');
            elem1.className = '测试类名';
            const elem2 = testDoc.createElement('div');
            elem2.setAttribute('id', '测试ID');
            const elem3 = testDoc.createElement('div');
            elem3.setAttribute('data-测试', '中文属性值');

            container.appendChild(elem1);
            container.appendChild(elem2);
            container.appendChild(elem3);
            rootElement.appendChild(container);

            // Test unicode class name
            const unicodeClass = rootElement.querySelector('.测试类名');
            assert.notEqual(unicodeClass, null);
            assert.equal(unicodeClass.className, '测试类名');

            // Test unicode id
            const unicodeId = rootElement.querySelector('#测试ID');
            assert.notEqual(unicodeId, null);
            assert.equal(unicodeId.id, '测试ID');

            // Test unicode attribute name and value
            const unicodeAttr = rootElement.querySelector('[data-测试="中文属性值"]');
            assert.notEqual(unicodeAttr, null);
            assert.equal(unicodeAttr.getAttribute('data-测试'), '中文属性值');
        });

        it("should handle alternative root nodes", () => {
            createTestStructure();

            // Test querying from specific subtree
            const header = rootElement.querySelector('header');
            const navFromHeader = header.querySelectorAll('a');
            assert.equal(navFromHeader.length, 2);

            // Test that query from subtree doesn't find elements outside
            const footerFromHeader = header.querySelectorAll('footer');
            assert.equal(footerFromHeader.length, 0);

            // Test querying from detached element
            const detachedDiv = testDoc.createElement('div');
            const detachedChild = testDoc.createElement('span');
            detachedChild.className = 'detached-child';
            detachedDiv.appendChild(detachedChild);

            const foundDetached = detachedDiv.querySelector('.detached-child');
            assert.notEqual(foundDetached, null);
            assert.equal(foundDetached.className, 'detached-child');
        });

        it("should handle empty and malformed CSS selectors", () => {
            createTestStructure();

            // Test empty selector - should always throw in all implementations
            assert.throws(() => {
                rootElement.querySelector('');
            });

            // Test potentially malformed selectors - different implementations may handle these differently
            // Some might throw errors, others might return null or handle gracefully
            const potentiallyMalformedSelectors = [
                '[attr',        // unclosed bracket
                'attr]',        // unmatched bracket
                '.class[',      // mixed unclosed
                '..class',      // double dots
                '##id',         // double hashes  
                '.class.',      // trailing dot
                '#id#',         // trailing hash
                'div > > p',    // double combinator
                'div + + p',    // double combinator
                'div ~ ~ p'     // double combinator
            ];

            potentiallyMalformedSelectors.forEach(selector => {
                try {
                    const result = rootElement.querySelector(selector);
                    // If it doesn't throw, it should return null (no match for malformed selector)
                    assert.equal(result, null);
                } catch (e) {
                    // If it throws, that's also acceptable behavior for malformed selectors
                    assert.ok(true);
                }
            });
        });

        it("should maintain correct document order in results", () => {
            // Create elements in specific order
            const container = testDoc.createElement('div');
            const first = testDoc.createElement('p');
            first.className = 'ordered-test';
            first.setAttribute('data-order', '1');
            const second = testDoc.createElement('div');
            second.className = 'ordered-test';
            second.setAttribute('data-order', '2');
            const third = testDoc.createElement('span');
            third.className = 'ordered-test';
            third.setAttribute('data-order', '3');

            container.appendChild(first);
            container.appendChild(second);
            container.appendChild(third);
            rootElement.appendChild(container);

            // Test that querySelectorAll maintains document order
            const orderedResults = rootElement.querySelectorAll('.ordered-test');
            assert.equal(orderedResults.length, 3);
            assert.equal(orderedResults[0].getAttribute('data-order'), '1');
            assert.equal(orderedResults[1].getAttribute('data-order'), '2');
            assert.equal(orderedResults[2].getAttribute('data-order'), '3');

            // Test with complex selector that could potentially reorder
            const reverseOrderSelector = rootElement.querySelectorAll('span.ordered-test, div.ordered-test, p.ordered-test');
            assert.equal(reverseOrderSelector.length, 3);
            // Results should still be in document order, not selector order
            assert.equal(reverseOrderSelector[0].tagName, 'P');
            assert.equal(reverseOrderSelector[1].tagName, 'DIV');
            assert.equal(reverseOrderSelector[2].tagName, 'SPAN');
        });

        it("should handle very long selector strings", () => {
            // Create a deep nested structure
            let current = rootElement;
            for (let i = 0; i < 10; i++) {
                const div = testDoc.createElement('div');
                div.className = `level-${i}`;
                current.appendChild(div);
                current = div;
            }

            // Add target element
            const target = testDoc.createElement('span');
            target.className = 'deep-target';
            current.appendChild(target);

            // Create very long selector
            const longSelector = Array.from({ length: 10 }, (_, i) => `.level-${i}`).join(' ') + ' .deep-target';

            const result = rootElement.querySelector(longSelector);
            assert.notEqual(result, null);
            assert.equal(result.className, 'deep-target');
        });

        it("should handle advanced attribute selectors", () => {
            const container = testDoc.createElement('div');

            // Create elements for testing ~= (word match) selector
            const elem1 = testDoc.createElement('div');
            elem1.setAttribute('class', 'nav item active');

            const elem2 = testDoc.createElement('div');
            elem2.setAttribute('class', 'navigation');

            // Create elements for testing |= (language match) selector
            const elem3 = testDoc.createElement('div');
            elem3.setAttribute('lang', 'en-US');

            const elem4 = testDoc.createElement('div');
            elem4.setAttribute('lang', 'en');

            const elem5 = testDoc.createElement('div');
            elem5.setAttribute('lang', 'fr-FR');

            container.appendChild(elem1);
            container.appendChild(elem2);
            container.appendChild(elem3);
            container.appendChild(elem4);
            container.appendChild(elem5);
            rootElement.appendChild(container);

            // Test ~= selector (word match)
            const wordMatches = rootElement.querySelectorAll('[class~="item"]');
            assert.equal(wordMatches.length, 1);
            assert.equal(wordMatches[0], elem1);

            const activeMatches = rootElement.querySelectorAll('[class~="active"]');
            assert.equal(activeMatches.length, 1);
            assert.equal(activeMatches[0], elem1);

            // Test |= selector (language/prefix match)
            const langMatches = rootElement.querySelectorAll('[lang|="en"]');
            assert.equal(langMatches.length, 2); // both en-US and en

            const frenchMatches = rootElement.querySelectorAll('[lang|="fr"]');
            assert.equal(frenchMatches.length, 1);
            assert.equal(frenchMatches[0], elem5);
        });

        it("should handle complex attribute values with special characters", () => {
            const container = testDoc.createElement('div');

            // Test comma in attribute value with proper escaping
            const elem1 = testDoc.createElement('div');
            elem1.setAttribute('data-list', 'apple,banana,cherry');

            // Test quotes and spaces in attribute value
            const elem2 = testDoc.createElement('div');
            elem2.setAttribute('data-description', 'A "quoted" value with spaces');

            // Test brackets and mixed characters
            const elem3 = testDoc.createElement('div');
            elem3.setAttribute('data-complex', '[key: "value", count: 42]');

            // Test multiple consecutive spaces
            const elem4 = testDoc.createElement('div');
            elem4.setAttribute('data-spaces', 'value    with    spaces');

            container.appendChild(elem1);
            container.appendChild(elem2);
            container.appendChild(elem3);
            container.appendChild(elem4);
            rootElement.appendChild(container);

            // Test exact match with comma
            const commaMatch = rootElement.querySelector('[data-list="apple,banana,cherry"]');
            assert.notEqual(commaMatch, null);
            assert.equal(commaMatch, elem1);

            // Test exact match with quotes
            const quotesMatch = rootElement.querySelector('[data-description="A \\"quoted\\" value with spaces"]');
            assert.notEqual(quotesMatch, null);
            assert.equal(quotesMatch, elem2);

            // Test exact match with brackets
            const bracketsMatch = rootElement.querySelector('[data-complex="[key: \\"value\\", count: 42]"]');
            assert.notEqual(bracketsMatch, null);
            assert.equal(bracketsMatch, elem3);

            // Test substring match with spaces
            const spacesContain = rootElement.querySelector('[data-spaces*="with"]');
            assert.notEqual(spacesContain, null);
            assert.equal(spacesContain, elem4);
        });

        it("should handle selector list parsing edge cases", () => {
            createTestStructure();

            // Test selector list with extra commas at start/end - should throw
            assert.throws(() => {
                rootElement.querySelectorAll(',header, footer');
            });

            assert.throws(() => {
                rootElement.querySelectorAll('header, footer,');
            });

            // Test selector list with multiple consecutive commas - should throw
            assert.throws(() => {
                rootElement.querySelectorAll('header,, footer');
            });

            // Test selector with complex spacing around commas
            const complexSpacing = rootElement.querySelectorAll('header    ,     footer    ,     main');
            assert.equal(complexSpacing.length, 3);
        });

        it("should handle case sensitivity correctly", () => {
            const container = testDoc.createElement('div');

            // Create elements with mixed case attributes
            const elem1 = testDoc.createElement('DIV');
            elem1.setAttribute('ID', 'TestID');
            elem1.setAttribute('CLASS', 'TestClass');
            elem1.setAttribute('Data-Test', 'TestValue');

            container.appendChild(elem1);
            rootElement.appendChild(container);

            // In HTML mode, tag names should be case insensitive, attributes should be case insensitive for standard ones
            const byTag = container.querySelector('div');
            assert.notEqual(byTag, null);
            assert.equal(byTag, elem1);

            // ID should be case insensitive in HTML mode
            const byId = container.querySelector('#TestID');
            assert.notEqual(byId, null);
            assert.equal(byId, elem1);

            // Class should be case sensitive
            const byClass = container.querySelector('.TestClass');
            assert.notEqual(byClass, null);
            assert.equal(byClass, elem1);

            // Custom data attributes should be case sensitive
            const byDataAttr = container.querySelector('[Data-Test="TestValue"]');
            assert.notEqual(byDataAttr, null);
            assert.equal(byDataAttr, elem1);
        });

        it("should handle pseudo-class selectors correctly", () => {
            const container = testDoc.createElement('div');

            // Create a list structure for testing various pseudo-classes
            const ul = testDoc.createElement('ul');
            for (let i = 1; i <= 5; i++) {
                const li = testDoc.createElement('li');
                li.textContent = `Item ${i}`;
                li.className = `item-${i}`;
                ul.appendChild(li);
            }

            container.appendChild(ul);
            rootElement.appendChild(container);

            // Test :first-child
            const firstChild = rootElement.querySelector('li:first-child');
            assert.notEqual(firstChild, null);
            assert.equal(firstChild.textContent, 'Item 1');

            // Test :last-child
            const lastChild = rootElement.querySelector('li:last-child');
            assert.notEqual(lastChild, null);
            assert.equal(lastChild.textContent, 'Item 5');

            // Test :nth-child variations
            const secondChild = rootElement.querySelector('li:nth-child(2)');
            assert.notEqual(secondChild, null);
            assert.equal(secondChild.textContent, 'Item 2');

            const oddChildren = rootElement.querySelectorAll('li:nth-child(odd)');
            assert.equal(oddChildren.length, 3); // Items 1, 3, 5

            const evenChildren = rootElement.querySelectorAll('li:nth-child(even)');
            assert.equal(evenChildren.length, 2); // Items 2, 4
        });

        it("should handle complex selector combinations with proper precedence", () => {
            const container = testDoc.createElement('div');
            container.className = 'container';

            // Create nested structure for testing selector precedence
            const section1 = testDoc.createElement('section');
            section1.className = 'section';
            section1.setAttribute('data-priority', 'high');

            const div1 = testDoc.createElement('div');
            div1.className = 'content important';
            div1.setAttribute('data-type', 'primary');

            const p1 = testDoc.createElement('p');
            p1.className = 'text';
            p1.textContent = 'Important content';

            div1.appendChild(p1);
            section1.appendChild(div1);

            const section2 = testDoc.createElement('section');
            section2.className = 'section';
            section2.setAttribute('data-priority', 'low');

            const div2 = testDoc.createElement('div');
            div2.className = 'content';
            div2.setAttribute('data-type', 'secondary');

            const p2 = testDoc.createElement('p');
            p2.className = 'text';
            p2.textContent = 'Regular content';

            div2.appendChild(p2);
            section2.appendChild(div2);

            container.appendChild(section1);
            container.appendChild(section2);
            rootElement.appendChild(container);

            // Test complex selector with multiple conditions
            const highPriorityContent = rootElement.querySelector('.container section[data-priority="high"] .content.important p.text');
            assert.notEqual(highPriorityContent, null);
            assert.equal(highPriorityContent.textContent, 'Important content');

            // Test selector that matches multiple elements but returns first
            const anyContent = rootElement.querySelector('.container section .content p');
            assert.notEqual(anyContent, null);
            assert.equal(anyContent.textContent, 'Important content'); // Should return first match

            // Test more specific selector that targets second element
            const lowPriorityContent = rootElement.querySelector('.container section[data-priority="low"] .content[data-type="secondary"] p');
            assert.notEqual(lowPriorityContent, null);
            assert.equal(lowPriorityContent.textContent, 'Regular content');

            // Test querySelectorAll with complex conditions
            const allContentDivs = rootElement.querySelectorAll('.container section .content[data-type]');
            assert.equal(allContentDivs.length, 2);
        });
    });

    describe('Form element selectors', () => {
        it("should handle form-related pseudo-classes", () => {
            const form = testDoc.createElement('form');
            form.innerHTML = `
                <input type="radio" id="radio1" name="option" value="one" checked="true"/>
                <input type="radio" id="radio2" name="option" value="two"/>
                <input type="text" id="text1" disabled/>
                <input type="submit" id="submit1" value="Submit"/>
                <button disabled>Disabled Button</button>
                <select disabled>
                    <option>Option 1</option>
                    <option disabled>Option 2</option>
                </select>
                <textarea disabled></textarea>
            `;
            rootElement.appendChild(form);

            // Test :checked pseudo-class
            const checkedInputs = rootElement.querySelectorAll('input[name="option"]:checked');
            assert.equal(checkedInputs.length, 1);
            assert.equal(checkedInputs[0].getAttribute('value'), 'one');

            // Test :disabled pseudo-class
            const disabledElements = rootElement.querySelectorAll(':disabled');
            assert.equal(disabledElements.length, 5); // text input, button, select, option, textarea

            // Test attribute selectors with form elements
            const radioInputs = rootElement.querySelectorAll('input[type="radio"]');
            assert.equal(radioInputs.length, 2);

            const submitButton = rootElement.querySelector('input[type="submit"], button[type="submit"]');
            assert.notEqual(submitButton, null);
            assert.equal(submitButton.id, 'submit1');
        });

        it("should handle complex form selectors", () => {
            const container = testDoc.createElement('div');
            container.innerHTML = `
                <input type="submit"/>
                <input type="button"/>
                <input type="reset"/>
                <input type="text"/>
                <input/>
            `;
            rootElement.appendChild(container);

            // Test Testing Library-style selector
            const buttonTypes = rootElement.querySelectorAll('input[type=submit], input[type=button], input[type=reset]');
            assert.equal(buttonTypes.length, 3);

            // Test :not with form elements
            const nonButtons = rootElement.querySelectorAll('input:not([type=submit]):not([type=button]):not([type=reset])');
            assert.equal(nonButtons.length, 2); // text and default input
        });
    });

    describe('Advanced :not() pseudo-class tests', () => {
        it("should handle :not() with various selectors", () => {
            createTestStructure();

            // Test :not() with class selector
            const notPost = rootElement.querySelectorAll('article:not(.featured)');
            assert.equal(notPost.length, 1); // should find news article

            // Test :not() with attribute selector
            const notTech = rootElement.querySelectorAll('article:not([data-category="tech"])');
            assert.equal(notTech.length, 1); // should find news article

            // Test :not() with pseudo-class
            const notFirstChild = rootElement.querySelectorAll('li:not(:first-child)');
            assert.equal(notFirstChild.length, 1); // second li element
        });

        it("should handle :not() with complex selectors", () => {
            const container = testDoc.createElement('div');
            container.innerHTML = `
                <div data-foo data-bar class="foo bar">Both</div>
                <div data-foo class="foo">Foo only</div>
                <div data-bar class="bar">Bar only</div>
                <div>Neither</div>
            `;
            rootElement.appendChild(container);

            // Test :not() with multiple classes
            const notFooBar = rootElement.querySelectorAll('.foo:not(.bar)');
            assert.equal(notFooBar.length, 1); // "Foo only"

            const notBarFoo = rootElement.querySelectorAll('.bar:not(.foo)');
            assert.equal(notBarFoo.length, 1); // "Bar only"

            // Test :not() with data attributes
            const notDataBar = rootElement.querySelectorAll('[data-foo]:not([data-bar])');
            assert.equal(notDataBar.length, 1); // "Foo only"

            // Test :not() with tag and attribute
            const notTextarea = rootElement.querySelectorAll('[data-foo]:not(textarea)');
            assert.equal(notTextarea.length, 2); // both data-foo elements
        });

        it("should handle :not() with multiple selectors", () => {
            const container = testDoc.createElement('div');
            container.innerHTML = `
                <ul class="list">
                    <li class="list-item">Item 1</li>
                    <li class="list-item">Item 2</li>
                    <li class="list-item">Item 3</li>
                    <li class="other-item">Other</li>
                    <li>Plain</li>
                </ul>
            `;
            rootElement.appendChild(container);

            // Test :not with multiple selectors within
            const notListOrOther = container.querySelectorAll('ul > li:not(.list-item, .other-item)');
            assert.equal(notListOrOther.length, 1); // "Plain" li
        });
    });

    describe('CSS logical pseudo-classes', () => {
        it("should handle :is() pseudo-class", () => {
            createTestStructure();

            // Test :is() with multiple selectors
            const isMatches = rootElement.querySelectorAll(':is(header, footer)');
            assert.equal(isMatches.length, 2);

            // Test :is() with specific elements
            const isSpanOrP = rootElement.querySelectorAll(':is(span, p)');
            assert.ok(isSpanOrP.length >= 3); // should find spans and paragraphs

            // Test :is() with class selectors
            const isClassMatches = rootElement.querySelectorAll(':is(.nav-item, .post)');
            assert.ok(isClassMatches.length >= 3);

            // Test :is() with complex selectors
            const isComplex = rootElement.querySelectorAll(':is(div, span.summary)');
            assert.ok(isComplex.length >= 1);
        });

        it("should handle :where() pseudo-class", () => {
            createTestStructure();

            // Test :where() with multiple selectors - should work like :is()
            const whereMatches = rootElement.querySelectorAll(':where(header, footer)');
            assert.equal(whereMatches.length, 2);

            // Test :where() with specific elements
            const whereSpanOrP = rootElement.querySelectorAll(':where(span, p)');
            assert.ok(whereSpanOrP.length >= 3);
        });

        it("should handle :has() pseudo-class", () => {
            const container = testDoc.createElement('div');
            container.innerHTML = `
                <section class="section1">
                    <article class="post">
                        <h2>Title 1</h2>
                        <img src="image1.jpg" alt="Image 1"/>
                    </article>
                </section>
                <section class="section2">
                    <article class="post">
                        <h2>Title 2</h2>
                        <video src="video1.mp4"></video>
                    </article>
                </section>
                <section class="section3">
                    <article class="post">
                        <h2>Title 3</h2>
                    </article>
                </section>
                <nav>
                    <ul>
                        <li><a href="#test">Link</a></li>
                    </ul>
                </nav>
            `;
            rootElement.appendChild(container);

            // Test :has() with descendant selectors
            const hasImg = rootElement.querySelectorAll('section:has(img)');
            assert.equal(hasImg.length, 1);

            const hasVideo = rootElement.querySelectorAll('section:has(video)');
            assert.equal(hasVideo.length, 1);

            // Test :has() with direct child
            const hasDirectChild = rootElement.querySelectorAll('nav:has(> ul)');
            assert.equal(hasDirectChild.length, 1);

            // Test :has() with attribute selector
            const hasAttr = rootElement.querySelectorAll('section:has([src])');
            assert.equal(hasAttr.length, 2); // img and video both have src

            // Test :has() combined conditions
            const hasH2AndImg = rootElement.querySelectorAll('article:has(h2):has(img)');
            assert.equal(hasH2AndImg.length, 1);
        });
    });

    describe('Sibling selectors and combinators', () => {
        it("should handle adjacent sibling selector (+)", () => {
            const container = testDoc.createElement('div');
            container.innerHTML = `
                <div class="a">a1</div>
                <div class="b">b1</div>
                <div class="c">c1</div>
                <div class="a">a2</div>
                <div class="b">b2</div>
                <div class="a">a3</div>
            `;
            rootElement.appendChild(container);

            // Test adjacent sibling selector
            const firstAdjacentB = container.querySelector('.a + .b');
            assert.equal(firstAdjacentB.textContent, 'b1');

            const allAdjacentB = container.querySelectorAll('.a + .b');
            assert.equal(allAdjacentB.length, 2);
            assert.equal(allAdjacentB[0].textContent, 'b1');
            assert.equal(allAdjacentB[1].textContent, 'b2');

            const noAdjacentC = container.querySelector('.a + .c');
            assert.equal(noAdjacentC, null);
        });

        it("should handle general sibling selector (~)", () => {
            const container = testDoc.createElement('div');
            container.innerHTML = `
                <div class="a">a1</div>
                <div class="b">b1</div>
                <div class="c">c1</div>
                <div class="a">a2</div>
                <div class="b">b2</div>
                <div class="a">a3</div>
            `;
            rootElement.appendChild(container);

            // Test general sibling selector
            const firstGeneralSibling = container.querySelector('.a ~ .a');
            assert.equal(firstGeneralSibling.textContent, 'a2');

            const allGeneralSiblings = container.querySelectorAll('.a ~ .a');
            assert.equal(allGeneralSiblings.length, 2);
            assert.equal(allGeneralSiblings[0].textContent, 'a2');
            assert.equal(allGeneralSiblings[1].textContent, 'a3');
        });

        it("should handle complex combinator chains", () => {
            createTestStructure();

            // Test descendant + child + attribute
            const complex1 = rootElement.querySelectorAll('header nav > ul li[class~="active"]');
            assert.equal(complex1.length, 1);

            // Test child + class + pseudo-class
            const complex2 = rootElement.querySelectorAll('main > article:first-child');
            assert.equal(complex2.length, 1);
            assert.equal(complex2[0].className, 'post featured');

            // Test multiple combinators
            const complex3 = rootElement.querySelectorAll('header + main > article');
            assert.equal(complex3.length, 2);
        });
    });

    describe('Special characters and Unicode support', () => {
        it("should handle escaped characters in selectors", () => {
            const container = testDoc.createElement('div');

            // Create elements with special characters
            const elem1 = testDoc.createElement('div');
            elem1.className = 'test:class';
            elem1.id = 'test:id';

            const elem2 = testDoc.createElement('div');
            elem2.className = 'test.class';

            const elem3 = testDoc.createElement('div');
            elem3.className = 'test#class';

            container.appendChild(elem1);
            container.appendChild(elem2);
            container.appendChild(elem3);
            rootElement.appendChild(container);

            // Test escaping colon in class name and id
            const colonClass = rootElement.querySelector('.test\\:class');
            assert.notEqual(colonClass, null);
            assert.equal(colonClass.className, 'test:class');

            const colonId = rootElement.querySelector('#test\\:id');
            assert.notEqual(colonId, null);
            assert.equal(colonId.id, 'test:id');

            // Test escaping dot in class name
            const dotClass = rootElement.querySelector('.test\\.class');
            assert.notEqual(dotClass, null);
            assert.equal(dotClass.className, 'test.class');

            // Test escaping hash in class name
            const hashClass = rootElement.querySelector('.test\\#class');
            assert.notEqual(hashClass, null);
            assert.equal(hashClass.className, 'test#class');
        });

        it("should handle Unicode characters in selectors", () => {
            const container = testDoc.createElement('div');

            const elem1 = testDoc.createElement('div');
            elem1.className = '测试类名 «unicode-class1»';
            elem1.id = '测试ID';

            const elem2 = testDoc.createElement('div');
            elem2.setAttribute('data-测试', '中文属性值');

            container.appendChild(elem1);
            container.appendChild(elem2);
            rootElement.appendChild(container);

            // Test Unicode class name
            const unicodeClass = rootElement.querySelector('.测试类名');
            assert.notEqual(unicodeClass, null);
            assert.equal(unicodeClass.className, '测试类名 «unicode-class1»');

            const unicodeClass2 = rootElement.querySelector('.«unicode-class1»');
            assert.notEqual(unicodeClass2, null);

            // Test Unicode id
            const unicodeId = rootElement.querySelector('#测试ID');
            assert.notEqual(unicodeId, null);
            assert.equal(unicodeId.id, '测试ID');

            // Test Unicode attribute name and value
            const unicodeAttr = rootElement.querySelector('[data-测试="中文属性值"]');
            assert.notEqual(unicodeAttr, null);
            assert.equal(unicodeAttr.getAttribute('data-测试'), '中文属性值');
        });

        it("should handle special attribute values", () => {
            const container = testDoc.createElement('div');

            const elem1 = testDoc.createElement('div');
            elem1.setAttribute('data-test', 'Hello, World');

            const elem2 = testDoc.createElement('div');
            elem2.setAttribute('data-test', 'Hello "quoted" value');

            const elem3 = testDoc.createElement('div');
            elem3.setAttribute('data-brackets', '[ bracket content ]');

            const elem4 = testDoc.createElement('div');
            elem4.setAttribute('style', 'expression("test")');

            container.appendChild(elem1);
            container.appendChild(elem2);
            container.appendChild(elem3);
            container.appendChild(elem4);
            rootElement.appendChild(container);

            // Test comma in attribute value
            const withComma = rootElement.querySelector('[data-test="Hello, World"]');
            assert.notEqual(withComma, null);

            // Test quotes in attribute value (escaped)
            const withQuotes = rootElement.querySelector('[data-test="Hello \\"quoted\\" value"]');
            assert.notEqual(withQuotes, null);

            // Test brackets in attribute value
            const withBrackets = rootElement.querySelector('[data-brackets="[ bracket content ]"]');
            assert.notEqual(withBrackets, null);

            // Test expression() in style attribute
            const withExpression = rootElement.querySelector('[style*="expression("]');
            assert.notEqual(withExpression, null);
        });
    });

    describe('matches() method', () => {
        it('should convert selector values to string for matches', () => {
            const container = testDoc.createElement('div');

            // Create custom elements with the names we want to test
            const falseElem = testDoc.createElement('false');
            const trueElem = testDoc.createElement('true');
            const nullElem = testDoc.createElement('null');
            const undefinedElem = testDoc.createElement('undefined');

            container.appendChild(falseElem);
            container.appendChild(trueElem);
            container.appendChild(nullElem);
            container.appendChild(undefinedElem);
            rootElement.appendChild(container);

            // Test boolean false
            const falseElement = container.querySelector('false');
            assert.notEqual(falseElement, null);
            assert.equal(falseElement.matches(false), true);  // false -> "false"
            assert.equal(falseElement.matches(['false']), true);  // array -> "false"

            // Test boolean true
            const trueElement = container.querySelector('true');
            assert.notEqual(trueElement, null);
            assert.equal(trueElement.matches(true), true);  // true -> "true"

            // Test null
            const nullElement = container.querySelector('null');
            assert.notEqual(nullElement, null);
            assert.equal(nullElement.matches(null), true);  // null -> "null"

            // Test undefined
            const undefinedElement = container.querySelector('undefined');
            assert.notEqual(undefinedElement, null);
            assert.equal(undefinedElement.matches(undefined), true);  // undefined -> "undefined"
        });

        it('should return true when element matches selector', () => {
            createTestStructure();

            const header = rootElement.querySelector('header');
            assert.equal(header.matches('header'), true);
            assert.equal(header.matches('#main-header'), true);
            assert.equal(header.matches('.site-header'), true);
            assert.equal(header.matches('header.site-header'), true);
            assert.equal(header.matches('header#main-header'), true);

            const featuredArticle = rootElement.querySelector('.featured');
            assert.equal(featuredArticle.matches('.post.featured'), true);
            assert.equal(featuredArticle.matches('article[data-category="tech"]'), true);
        });

        it('should return false when element does not match selector', () => {
            createTestStructure();

            const header = rootElement.querySelector('header');
            assert.equal(header.matches('main'), false);
            assert.equal(header.matches('#content'), false);
            assert.equal(header.matches('.post'), false);

            const article = rootElement.querySelector('article');
            assert.equal(article.matches('header'), false);
            assert.equal(article.matches('.navigation'), false);
        });

        it('should handle complex descendant selectors in matches', () => {
            createTestStructure();

            const link = rootElement.querySelector('a');
            // Note: matches() only checks if the element itself matches, not its position in the tree
            assert.equal(link.matches('a'), true);
            assert.equal(link.matches('a[href="#home"]'), true);
        });

        it('should handle pseudo-elements correctly in matches', () => {
            createTestStructure();

            const paragraph = rootElement.querySelector('p');

            // Pseudo-elements should return false (they're not real elements)
            assert.equal(paragraph.matches('p::before'), false);
            assert.equal(paragraph.matches('p::after'), false);
            assert.equal(paragraph.matches('p::first-line'), false);
            assert.equal(paragraph.matches('p::first-letter'), false);
        });

        it('should handle grouped selectors in matches', () => {
            createTestStructure();

            const header = rootElement.querySelector('header');
            const main = rootElement.querySelector('main');

            // Test grouped selectors
            assert.equal(header.matches('header, main, footer'), true);
            assert.equal(main.matches('header, main, footer'), true);

            // Test more complex grouped selectors
            assert.equal(header.matches('.site-header, .main-content'), true);
            assert.equal(main.matches('.site-header, .main-content'), true);
        });
    });
});
