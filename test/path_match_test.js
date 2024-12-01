var test = require("test");
test.setup();

var path = require("path");

describe("path.matchesGlob", () => {
    const basicTests = {
        "Basic glob patterns": [
            ["abc", "a*", true],
            ["abc", "*c", true],
            ["abc", "a*c", true],
            ["abc", "*b*", true],
            ["abc", "?bc", true],
            ["abc", "a?c", true],
            ["abc", "??c", true],
            ["abc", "a*****?c", true],
            ["abc", "?*****??", true],
            ["abc", "*****??", true],
            ["abc", "?***?****c", true],
            ["abc", "?***?****?", true],
            ["abc", "?***?****", true],
            ["abc", "*******c", true],
            ["abc", "*******?", true]
        ],

        "Character classes": [
            ["abc", "[abc]bc", true],
            ["bbc", "[abc]bc", true],
            ["dbc", "[abc]bc", false],
            ["abc", "[a-c]b*", true],
            ["abd", "[a-c]b*", true],
            ["abe", "[a-c]b*", true],
            ["bb", "[a-c]b*", true],
            ["cb", "[a-c]b*", true],
            ["dbc", "[a-c]b*", false],
            ["dd", "[^a-c]*", true],
            ["abc", "[^a-c]*", false],
            ["-", "[-abc]", true],
            ["-", "[abc-]", true]
        ],

        "Empty strings": [
            ["", "", true],
            ["abc", "", false],
            ["", "abc", false]
        ],

        "Multiple asterisks": [
            ["abc", "a***c", true],
            ["abc", "a*****?c", true],
            ["abc", "?*****??", true],
            ["abc", "*****??", true],
            ["abc", "?*****?c", true]
        ]
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
            ]
        }
    };

    const complexTests = [
        ["a\\b\\c\\d\\e", "a/**/**/e", true, "win32"],
        ["a\\b\\c\\d\\e", "**/e", true, "win32"],
        ["a\\b\\c\\d\\e", "a/**/d/**", true, "win32"],
        ["a\\b\\c\\d\\e", "x/**/e", false, "win32"],
        ["a/b/c/d/e", "a/**/**/e", true, "posix"],
        ["a/b/c/d/e", "**/e", true, "posix"],
        ["a/b/c/d/e", "a/**/d/**", true, "posix"],
        ["a/b/c/d/e", "x/**/e", false, "posix"]
    ];

    const braceTests = {
        "Basic expansion": [
            ["a{b,c}d", ["abd", "acd"]],
            ["a{b,}c", ["abc", "ac"]],
            ["a{1..3}b", ["a1b", "a2b", "a3b"]],
            ["a{b,c}d{e,f}g", ["abdeg", "abdfg", "acdeg", "acdfg"]]
        ],
        "Nested braces": [
            ["a{b,c{d,e}}f", ["abf", "acdf", "acef"]],
            ["a{b,c{d,e},f}g", ["abg", "acdg", "aceg", "afg"]],
            ["a{b{c,d},e{f,g}}h", ["abch", "abdh", "aefh", "aegh"]]
        ],
        "Numeric sequences": [
            ["a{1..5}b", ["a1b", "a2b", "a3b", "a4b", "a5b"]],
            ["a{01..03}b", ["a01b", "a02b", "a03b"]],
            ["a{-1..2}b", ["a-1b", "a0b", "a1b", "a2b"]]
        ],
        "Special cases": [
            ["a{b}c", ["abc"]],
            ["a{b,}c", ["abc", "ac"]],
            ["a{,b}c", ["ac", "abc"]],
            ["a{b,,c}d", ["abd", "ad", "acd"]],
            ["{a,b{,c}d", ["a", "bcd"]]
        ],
        "Combined with other patterns": [
            ["a{b,c}*", ["ab*", "ac*"]],
            ["a{b,c}/*.js", ["ab/*.js", "ac/*.js"]],
            ["{a,b}${c}", ["a$c", "b$c"]],
            ["${a}{b,c}", ["$ab", "$ac"]]
        ]
    };

    describe("common patterns", () => {
        Object.entries(basicTests).forEach(([groupName, tests]) => {
            it(groupName, () => {
                tests.forEach(([str, pattern, expected]) => {
                    assert.equal(path.matchesGlob(str, pattern), expected);
                });
            });
        });
    });

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

    it("complex patterns", () => {
        complexTests.forEach(([str, pattern, expected, platform]) => {
            assert.equal(path[platform].matchesGlob(str, pattern), expected);
        });
    });

    describe("brace expansion", () => {
        Object.entries(braceTests).forEach(([groupName, tests]) => {
            it(groupName, () => {
                tests.forEach(([pattern, matches]) => {
                    matches.forEach(match => {
                        assert.equal(path.win32.matchesGlob(match, pattern), true);
                        assert.equal(path.posix.matchesGlob(match, pattern), true);
                    });
                });
            });
        });

        const nonMatchTests = [
            ["a{1..3}b", "a4b"],
            ["a{b,c}d", "abd1"],
            ["a{b,c}d", "abcd"],
            ["a{01..03}b", "a1b"],
            ["a{b,c}/*.js", "ab/dir/test.js"]
        ];

        it("non-matches", () => {
            nonMatchTests.forEach(([pattern, nonMatch]) => {
                assert.equal(path.win32.matchesGlob(nonMatch, pattern), false);
                assert.equal(path.posix.matchesGlob(nonMatch, pattern), false);
            });
        });
    });

    const rangeTests = {
        "Number Ranges": [
            ["1.txt", "{1..3}.txt", true],
            ["2.txt", "{1..3}.txt", true],
            ["3.txt", "{1..3}.txt", true],
            ["4.txt", "{1..3}.txt", false],
            ["0.txt", "{1..3}.txt", false],
            ["01.txt", "{01..05}.txt", true],
            ["02.txt", "{01..05}.txt", true],
            ["05.txt", "{01..05}.txt", true],
            ["1.txt", "{01..05}.txt", false],
            ["5.txt", "{01..05}.txt", false],
            ["-1.txt", "{-2..2}.txt", true],
            ["0.txt", "{-2..2}.txt", true],
            ["2.txt", "{-2..2}.txt", true],
            ["-3.txt", "{-2..2}.txt", false],
            ["3.txt", "{-2..2}.txt", false],
            ["-1.txt", "{-1..1}.txt", true],
            ["0.txt", "{-1..1}.txt", true],
            ["1.txt", "{-1..1}.txt", true],
            ["42.txt", "{40..50}.txt", true],
            ["39.txt", "{40..50}.txt", false],
            ["51.txt", "{40..50}.txt", false],
            ["file01.txt", "file{01..10}.txt", true],
            ["file10.txt", "file{01..10}.txt", true],
            ["file00.txt", "file{01..10}.txt", false],
            ["file11.txt", "file{01..10}.txt", false],
            ["test/1.txt", "test/{1..3}.txt", true],
            ["test/4.txt", "test/{1..3}.txt", false],
            ["1-1.txt", "{1..2}-{1..2}.txt", true],
            ["2-2.txt", "{1..2}-{1..2}.txt", true],
            ["1-3.txt", "{1..2}-{1..2}.txt", false],
            ["3-1.txt", "{1..2}-{1..2}.txt", false]
        ],

        "Mixed Number and Text": [
            ["test1a.txt", "test{1..3}a.txt", true],
            ["test2a.txt", "test{1..3}a.txt", true],
            ["test3a.txt", "test{1..3}a.txt", true],
            ["test1b.txt", "test{1..3}a.txt", false],
            ["test4a.txt", "test{1..3}a.txt", false],
            ["test1_file.txt", "test{1..3}_*.txt", true],
            ["test2_data.txt", "test{1..3}_*.txt", true],
            ["test4_file.txt", "test{1..3}_*.txt", false],
            ["file1[data].txt", "file{1..3}[[]*.txt", true],
            ["file2[info].txt", "file{1..3}[[]*.txt", true],
            ["file4[data].txt", "file{1..3}[[]*.txt", false]
        ]
    };

    Object.entries(rangeTests).forEach(([groupName, testCases]) => {
        it(groupName, () => {
            testCases.forEach(([str, pattern, expected]) => {
                assert.equal(path.win32.matchesGlob(str, pattern), expected);
                assert.equal(path.posix.matchesGlob(str, pattern), expected);
            });
        });
    });
});

require.main === module && test.run();
