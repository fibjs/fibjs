var test = require("test");
test.setup();

var path = require("path");
var assert = require("assert");

describe("path.matchesGlob", () => {
    const globs = {
        win32: [
            // Basic patterns
            ["foo\\bar\\baz", "foo\\[bcr]ar\\baz", true],
            ["foo\\bar\\baz", "foo\\[!bcr]ar\\baz", false],
            ["foo\\bar\\baz", "foo\\[bc-r]ar\\baz", true],
            ["foo\\bar\\baz", "foo\\*\\!bar\\*\\baz", false],
            ["foo\\bar1\\baz", "foo\\bar[0-9]\\baz", true],
            ["foo\\bar5\\baz", "foo\\bar[0-9]\\baz", true],
            ["foo\\barx\\baz", "foo\\bar[a-z]\\baz", true],
            ["foo\\bar\\baz\\boo", "foo\\[bc-r]ar\\baz\\*", true],
            ["foo\\bar\\baz", "foo\\**", true],
            ["foo\\bar\\baz", "*", false],
            
            // Additional patterns from minimatch
            ["abc", "a*", true],
            ["abc", "a*c", true],
            ["abc", "a*b*c", true],
            ["abc", "a*****?c", true],
            ["abc", "?*****??", true],
            ["abc", "*****??", true],
            ["abc", "?***?****c", true],
            ["abc", "?***?****?", true],
            ["abc", "?***?****", true],
            ["abc", "*******c", true],
            ["abc", "*******?", true],
            
            // Path separator handling
            ["a\\b\\c", "a/**/c", true],
            ["a\\b\\c", "a/**/**/c", true],
            ["a\\b\\c\\d", "a/**/d", true],
            ["a\\b\\c\\d", "**/d", true],
            
            // Character classes
            ["abc", "[a-c]b*", true],
            ["abd", "[a-c]b*", true],
            ["abe", "[a-c]b*", true],
            ["bb", "[a-c]b*", true],
            ["cb", "[a-c]b*", true],
            ["dbc", "[a-c]b*", false],
            
            // Edge cases
            ["", "", true],
            ["abc", "", false],
            ["", "abc", false],
            ["abc", "\\*", false],
            ["*", "\\*", false],

            // Empty string tests
            ["", "", true],
            ["abc", "", false],
            ["", "abc", false],

            // Character class tests
            ["abc", "[a-c]b*", true],
            ["bb", "[a-c]b*", true],
            ["cb", "[a-c]b*", true],
            ["dd", "[^a-c]*", true],
            ["abc", "[^a-c]*", false],
            ["-", "[-abc]", true],
            ["-", "[abc-]", true],

            // Multiple asterisk tests
            ["abc", "a***c", true],
            ["abc", "a*****?c", true],
            ["abc", "?*****??", true],
            ["abc", "*****??", true],
            ["abc", "?*****?c", true],

            // Path separator tests
            ["a\\c\\b", "a\\*\\b", true],
            ["a\\x\\y\\b", "a\\**\\b", true],
            ["a\\x\\y\\b", "a\\**\\**\\b", true],
            ["a\\b\\c", "a\\**\\c", true],
            ["a\\b\\c\\d", "a\\**\\d", true],
            ["a\\b\\c\\d", "**\\d", true],
        ],
        posix: [
            // Basic patterns
            ["foo/bar/baz", "foo/[bcr]ar/baz", true],
            ["foo/bar/baz", "foo/[!bcr]ar/baz", false],
            ["foo/bar/baz", "foo/[bc-r]ar/baz", true],
            ["foo/bar/baz", "foo/*/!bar/*/baz", false],
            ["foo/bar1/baz", "foo/bar[0-9]/baz", true],
            ["foo/bar5/baz", "foo/bar[0-9]/baz", true],
            ["foo/barx/baz", "foo/bar[a-z]/baz", true],
            ["foo/bar/baz/boo", "foo/[bc-r]ar/baz/*", true],
            ["foo/bar/baz", "foo/**", true],
            ["foo/bar/baz", "*", false],
            
            // Additional patterns from minimatch
            ["abc", "a*", true],
            ["abc", "a*c", true],
            ["abc", "a*b*c", true],
            ["abc", "a*****?c", true],
            ["abc", "?*****??", true],
            ["abc", "*****??", true],
            ["abc", "?***?****c", true],
            ["abc", "?***?****?", true],
            ["abc", "?***?****", true],
            ["abc", "*******c", true],
            ["abc", "*******?", true],
            
            // Path separator handling
            ["a/b/c", "a/**/c", true],
            ["a/b/c", "a/**/**/c", true],
            ["a/b/c/d", "a/**/d", true],
            ["a/b/c/d", "**/d", true],
            
            // Character classes
            ["abc", "[a-c]b*", true],
            ["abd", "[a-c]b*", true],
            ["abe", "[a-c]b*", true],
            ["bb", "[a-c]b*", true],
            ["cb", "[a-c]b*", true],
            ["dbc", "[a-c]b*", false],
            
            // Edge cases
            ["", "", true],
            ["abc", "", false],
            ["", "abc", false],
            ["abc", "\\*", false],
            ["*", "\\*", false],

            // Empty string tests
            ["", "", true],
            ["abc", "", false],
            ["", "abc", false],

            // Character class tests
            ["abc", "[a-c]b*", true],
            ["bb", "[a-c]b*", true],
            ["cb", "[a-c]b*", true],
            ["dd", "[^a-c]*", true],
            ["abc", "[^a-c]*", false],
            ["-", "[-abc]", true],
            ["-", "[abc-]", true],

            // Multiple asterisk tests
            ["abc", "a***c", true],
            ["abc", "a*****?c", true],
            ["abc", "?*****??", true],
            ["abc", "*****??", true],
            ["abc", "?*****?c", true],

            // Path separator tests
            ["a/c/b", "a/*/b", true],
            ["a/x/y/b", "a/**/b", true],
            ["a/x/y/b", "a/**/**/b", true],
            ["a/b/c", "a/**/c", true],
            ["a/b/c/d", "a/**/d", true],
            ["a/b/c/d", "**/d", true],
        ]
    };

    describe("platform specific glob matching", () => {
        Object.entries(globs).forEach(([platform, platformGlobs]) => {
            describe(`platform ${platform}`, () => {
                platformGlobs.forEach(([pathStr, glob, expected]) => {
                    it(`'${pathStr}' ${expected? 'matches' : 'does not match'} '${glob}'`, () => {
                        const actual = path[platform].matchesGlob(pathStr, glob);
                        assert.strictEqual(actual, expected);
                    });
                });
            });
        });
    });

    // Additional test cases for both platforms
    it("additional glob patterns", () => {
        // Test cases for win32
        assert.strictEqual(path.win32.matchesGlob("a\\b\\c\\d\\e", "a/**/**/e"), true);
        assert.strictEqual(path.win32.matchesGlob("a\\b\\c\\d\\e", "**/e"), true);
        assert.strictEqual(path.win32.matchesGlob("a\\b\\c\\d\\e", "a/**/d/**"), true);
        assert.strictEqual(path.win32.matchesGlob("a\\b\\c\\d\\e", "x/**/e"), false);
        
        // Test cases for posix
        assert.strictEqual(path.posix.matchesGlob("a/b/c/d/e", "a/**/**/e"), true);
        assert.strictEqual(path.posix.matchesGlob("a/b/c/d/e", "**/e"), true);
        assert.strictEqual(path.posix.matchesGlob("a/b/c/d/e", "a/**/d/**"), true);
        assert.strictEqual(path.posix.matchesGlob("a/b/c/d/e", "x/**/e"), false);
    });
});

require.main === module && test.run(console.DEBUG);
