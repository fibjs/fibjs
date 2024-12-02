var test = require("test");
test.setup();

var path = require("path");

describe("path.matchesGlob", () => {
    const commonTests = {
        "Basic Pattern Tests": {
            "Basic glob patterns": [
                ["a*", "abc", true],
                ["*c", "abc", true],
                ["a*c", "abc", true],
                ["*b*", "abc", true],
                ["?bc", "abc", true],
                ["a?c", "abc", true],
                ["??c", "abc", true],
                ["a*****?c", "abc", true],
                ["?*****??", "abc", true],
                ["*****??", "abc", true],
                ["?***?****c", "abc", true],
                ["?***?****?", "abc", true],
                ["?***?****", "abc", true],
                ["*******c", "abc", true],
                ["*******?", "abc", true]
            ],

            "Multiple asterisks": [
                ["a***c", "abc", true],
                ["a*****?c", "abc", true],
                ["?*****??", "abc", true],
                ["*****??", "abc", true],
                ["?*****?c", "abc", true]
            ],

            "Empty strings": [
                ["", "", true],
                ["", "abc", false],
                ["abc", "", false]
            ],

            "Character classes": [
                ["[abc]bc", "abc", true],
                ["[abc]bc", "bbc", true],
                ["[abc]bc", "dbc", false],
                ["[a-c]b*", "abc", true],
                ["[a-c]b*", "abd", true],
                ["[a-c]b*", "abe", true],
                ["[a-c]b*", "bb", true],
                ["[a-c]b*", "cb", true],
                ["[a-c]b*", "dbc", false],
                ["[^a-c]*", "dd", true],
                ["[^a-c]*", "abc", false],
                ["[-abc]", "-", true],
                ["[abc-]", "-", true]
            ]
        },

        "Brace and Range Tests": {
            "brace expansion": {
                "Basic expansion": [
                    ["a{b,c}d", ["abd", "acd"], true],
                    ["a{b,}c", ["abc", "ac"], true],
                    ["a{1..3}b", ["a1b", "a2b", "a3b"], true],
                    ["a{b,c}d{e,f}g", ["abdeg", "abdfg", "acdeg", "acdfg"], true]
                ],

                "Nested braces": [
                    ["a{b,c{d,e}}f", ["abf", "acdf", "acef"], true],
                    ["a{b,c{d,e},f}g", ["abg", "acdg", "aceg", "afg"], true],
                    ["a{b{c,d},e{f,g}}h", ["abch", "abdh", "aefh", "aegh"], true]
                ],

                "Numeric sequences": [
                    ["a{1..5}b", ["a1b", "a2b", "a3b", "a4b", "a5b"], true],
                    ["a{01..03}b", ["a01b", "a02b", "a03b"], true],
                    ["a{-1..2}b", ["a-1b", "a0b", "a1b", "a2b"], true]
                ],

                "Special cases": [
                    ["a{b}c", ["abc"], true],
                    ["a{b,}c", ["abc", "ac"], true],
                    ["a{,b}c", ["ac", "abc"], true],
                    ["a{b,,c}d", ["abd", "ad", "acd"], true],
                    ["{a,b{,c}d", ["a", "bcd"], true]
                ],

                "Combined with other patterns": [
                    ["a{b,c}*", ["ab*", "ac*"], true],
                    ["a{b,c}/*.js", ["ab/*.js", "ac/*.js"], true],
                    ["{a,b}${c}", ["a$c", "b$c"], true],
                    ["${a}{b,c}", ["$ab", "$ac"], true]
                ],

                "non-match": [
                    ["a{1..3}b", "a4b", false],
                    ["a{b,c}d", "abd1", false],
                    ["a{b,c}d", "abcd", false],
                    ["a{01..03}b", "a1b", false],
                    ["a{b,c}/*.js", "ab/dir/test.js", false]
                ]
            },

            "range test": {
                "Number Ranges": [
                    ["{1..3}.txt", "1.txt", true],
                    ["{1..3}.txt", "2.txt", true],
                    ["{1..3}.txt", "3.txt", true],
                    ["{1..3}.txt", "4.txt", false],
                    ["{1..3}.txt", "0.txt", false],
                    ["{01..05}.txt", "01.txt", true],
                    ["{01..05}.txt", "02.txt", true],
                    ["{01..05}.txt", "05.txt", true],
                    ["{01..05}.txt", "1.txt", false],
                    ["{01..05}.txt", "5.txt", false],
                    ["{-2..2}.txt", "-1.txt", true],
                    ["{-2..2}.txt", "0.txt", true],
                    ["{-2..2}.txt", "2.txt", true],
                    ["{-2..2}.txt", "-3.txt", false],
                    ["{-2..2}.txt", "3.txt", false],
                    ["{-1..1}.txt", "-1.txt", true],
                    ["{-1..1}.txt", "0.txt", true],
                    ["{-1..1}.txt", "1.txt", true],
                    ["{40..50}.txt", "42.txt", true],
                    ["{40..50}.txt", "39.txt", false],
                    ["{40..50}.txt", "51.txt", false],
                    ["file{01..10}.txt", "file01.txt", true],
                    ["file{01..10}.txt", "file10.txt", true],
                    ["file{01..10}.txt", "file00.txt", false],
                    ["file{01..10}.txt", "file11.txt", false],
                    ["test/{1..3}.txt", "test/1.txt", true],
                    ["test/{1..3}.txt", "test/4.txt", false],
                    ["{1..2}-{1..2}.txt", "1-1.txt", true],
                    ["{1..2}-{1..2}.txt", "2-2.txt", true],
                    ["{1..2}-{1..2}.txt", "1-3.txt", false],
                    ["{1..2}-{1..2}.txt", "3-1.txt", false]
                ],

                "Mixed Number and Text": [
                    ["test{1..3}a.txt", "test1a.txt", true],
                    ["test{1..3}a.txt", "test2a.txt", true],
                    ["test{1..3}a.txt", "test3a.txt", true],
                    ["test{1..3}a.txt", "test1b.txt", false],
                    ["test{1..3}a.txt", "test4a.txt", false],
                    ["test{1..3}_*.txt", "test1_file.txt", true],
                    ["test{1..3}_*.txt", "test2_data.txt", true],
                    ["test{1..3}_*.txt", "test4_file.txt", false],
                    ["file{1..3}[[]*.txt", "file1[data].txt", true],
                    ["file{1..3}[[]*.txt", "file2[info].txt", true],
                    ["file{1..3}[[]*.txt", "file4[data].txt", false]
                ]
            }
        }
    };

    const platformTests = {
        win32: {
            "Path separators": [
                ["foo\\bar\\baz", "foo\\[bcr]ar\\baz", true],
                ["foo\\bar\\baz", "foo\\[!bcr]ar\\baz", false],
                ["foo\\bar\\baz", "foo\\[bc-r]ar\\baz", true],
                ["foo\\bar\\baz", "foo\\*\\!bar\\*\\baz", false],
                ["foo\\bar\\baz\\boo", "foo\\[bc-r]ar\\baz\\*", true],
                ["foo\\bar\\baz", "foo\\**", true],
                ["foo\\bar\\baz", "*", false],
                ["a\\b\\c", "a/**/c", true],
                ["a\\b\\c", "a/**/**/c", true],
                ["a\\b\\c\\d", "a/**/d", true],
                ["a\\b\\c\\d", "**/d", true],
                ["a\\c\\b", "a\\*\\b", true],
                ["a\\x\\y\\b", "a\\**\\b", true],
                ["a\\x\\y\\b", "a\\**\\**\\b", true],
                ["a\\b\\c", "a\\**\\c", true],
                ["a\\b\\c\\d", "a\\**\\d", true],
                ["a\\b\\c\\d", "**\\d", true]
            ],

            "Numeric patterns": [
                ["foo\\bar1\\baz", "foo\\bar[0-9]\\baz", true],
                ["foo\\bar5\\baz", "foo\\bar[0-9]\\baz", true]
            ],

            "Alphabetic patterns": [
                ["foo\\barx\\baz", "foo\\bar[a-z]\\baz", true]
            ],

            "Escaped characters": [
                ["abc", "\\*", false],
                ["*", "\\*", false]
            ],
            "complex patterns": [
                ["a\\b\\c\\d\\e", "a/**/**/e", true],
                ["a\\b\\c\\d\\e", "**/e", true],
                ["a\\b\\c\\d\\e", "a/**/d/**", true],
                ["a\\b\\c\\d\\e", "x/**/e", false]
            ]
        },

        posix: {
            "Path separators": [
                ["foo/bar/baz", "foo/[bcr]ar/baz", true],
                ["foo/bar/baz", "foo/[!bcr]ar/baz", false],
                ["foo/bar/baz", "foo/[bc-r]ar/baz", true],
                ["foo/bar/baz", "foo/*/!bar/*/baz", false],
                ["foo/bar/baz/boo", "foo/[bc-r]ar/baz/*", true],
                ["foo/bar/baz", "foo/**", true],
                ["foo/bar/baz", "*", false],
                ["a/b/c", "a/**/c", true],
                ["a/b/c", "a/**/**/c", true],
                ["a/b/c/d", "a/**/d", true],
                ["a/b/c/d", "**/d", true],
                ["a/c/b", "a/*/b", true],
                ["a/x/y/b", "a/**/b", true],
                ["a/x/y/b", "a/**/**/b", true],
                ["a/b/c", "a/**/c", true],
                ["a/b/c/d", "a/**/d", true],
                ["a/b/c/d", "**/d", true]
            ],

            "Numeric patterns": [
                ["foo/bar1/baz", "foo/bar[0-9]/baz", true],
                ["foo/bar5/baz", "foo/bar[0-9]/baz", true]
            ],

            "Alphabetic patterns": [
                ["foo/barx/baz", "foo/bar[a-z]/baz", true]
            ],

            "Escaped characters": [
                ["abc", "\\*", false],
                ["*", "\\*", true]
            ],
            "complex patterns": [
                ["a/b/c/d/e", "a/**/**/e", true],
                ["a/b/c/d/e", "**/e", true],
                ["a/b/c/d/e", "a/**/d/**", true],
                ["a/b/c/d/e", "x/**/e", false]
            ]
        }
    };

    function testSuite(name, tests) {
        if (Array.isArray(tests)) {
            it(name, () => {
                tests.forEach(([pattern, matches, expected]) => {
                    if (Array.isArray(matches)) {
                        matches.forEach(match => {
                            assert.equal(path.win32.matchesGlob(match, pattern), true);
                            assert.equal(path.posix.matchesGlob(match, pattern), true);
                        });
                    } else {
                        assert.equal(path.win32.matchesGlob(matches, pattern), expected);
                        assert.equal(path.posix.matchesGlob(matches, pattern), expected);
                    }
                });
            });
        } else {
            describe(name, () => {
                Object.entries(tests).forEach(([groupName, tests]) => {
                    testSuite(groupName, tests);
                });
            });
        }
    }

    testSuite("common patterns", commonTests);

    describe("platform specific patterns", () => {
        Object.entries(platformTests).forEach(([platform, groups]) => {
            describe(platform, () => {
                Object.entries(groups).forEach(([groupName, tests]) => {
                    it(groupName, () => {
                        tests.forEach(([str, pattern, expected]) => {
                            assert.equal(path[platform].matchesGlob(str, pattern), expected);
                        });
                    });
                });
            });
        });
    });
});

require.main === module && test.run();
