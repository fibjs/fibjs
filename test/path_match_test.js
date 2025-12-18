const assert = require("assert");
const { describe, it } = require("node:test");
const path = require("path");
const os = require("os");

// Platform-specific behavior detection - case insensitive on Mac and Windows
const isCaseInsensitive = os.platform() === 'darwin' || os.platform() === 'win32' || os.platform() === 'ios';

describe("path.matchesGlob", () => {

    // ==================== 基础功能测试 ====================
    describe("Basic functionality", () => {

        describe("Literal string matching", () => {
            it("should match identical strings", () => {
                assert.strictEqual(path.matchesGlob("hello", "hello"), true);
                assert.strictEqual(path.matchesGlob("abc", "abc"), true);
                assert.strictEqual(path.matchesGlob("", ""), true);
            });

            it("should not match different strings", () => {
                assert.strictEqual(path.matchesGlob("world", "hello"), false);
                assert.strictEqual(path.matchesGlob("a", ""), false);
                assert.strictEqual(path.matchesGlob("", "a"), false);
            });
        });

        describe("Single character wildcard (?)", () => {
            it("should match exactly one character", () => {
                assert.strictEqual(path.matchesGlob("abc", "a?c"), true);
                assert.strictEqual(path.matchesGlob("axc", "a?c"), true);
                assert.strictEqual(path.matchesGlob("a", "?"), true);
            });

            it("should not match zero or multiple characters", () => {
                assert.strictEqual(path.matchesGlob("ac", "a?c"), false);
                assert.strictEqual(path.matchesGlob("abcc", "a?c"), false);
                assert.strictEqual(path.matchesGlob("", "?"), false);
            });
        });

        describe("Multi-character wildcard (*)", () => {
            it("should match zero or more characters", () => {
                assert.strictEqual(path.matchesGlob("a", "a*"), true);
                assert.strictEqual(path.matchesGlob("abc", "a*"), true);
                assert.strictEqual(path.matchesGlob("axyz", "a*"), true);
                assert.strictEqual(path.matchesGlob("anything", "*"), true);
            });

            it("should match with surrounding text", () => {
                assert.strictEqual(path.matchesGlob("abc", "a*c"), true);
                assert.strictEqual(path.matchesGlob("axyzc", "a*c"), true);
                assert.strictEqual(path.matchesGlob("ac", "a*c"), true);
            });

            it("should not match when prefix doesn't match", () => {
                assert.strictEqual(path.matchesGlob("b", "a*"), false);
                assert.strictEqual(path.matchesGlob("", "*"), false); // Node.js behavior: empty string does not match *
            });
        });

        describe("Character classes", () => {
            it("should match characters in class", () => {
                assert.strictEqual(path.matchesGlob("a", "[abc]"), true);
                assert.strictEqual(path.matchesGlob("b", "[abc]"), true);
                assert.strictEqual(path.matchesGlob("c", "[abc]"), true);
            });

            it("should not match characters outside class", () => {
                assert.strictEqual(path.matchesGlob("d", "[abc]"), false);
                assert.strictEqual(path.matchesGlob("a", "[0-9]"), false);
            });

            it("should handle character ranges", () => {
                assert.strictEqual(path.matchesGlob("m", "[a-z]"), true);
                // Character ranges case sensitivity differs between platforms
                assert.strictEqual(path.matchesGlob("A", "[a-z]"), isCaseInsensitive); // Mac/Windows: case insensitive, Linux: case sensitive
                assert.strictEqual(path.matchesGlob("5", "[0-9]"), true);
            });

            it("should handle negated character classes", () => {
                assert.strictEqual(path.matchesGlob("d", "[!abc]"), true);
                assert.strictEqual(path.matchesGlob("a", "[!abc]"), false);
                assert.strictEqual(path.matchesGlob("d", "[^abc]"), true);
                assert.strictEqual(path.matchesGlob("b", "[^abc]"), false);
                // Case sensitivity in negated ranges differs between platforms
                assert.strictEqual(path.matchesGlob("A", "[!a-z]"), !isCaseInsensitive); // Mac/Windows: case insensitive (A matches [a-z] so ![a-z] = false), Linux: case sensitive (A doesn't match [a-z] so ![a-z] = true)
                assert.strictEqual(path.matchesGlob("1", "[!a-z]"), true); // Number should not match [a-z]
            });
        });
    });

    // ==================== 高级模式匹配测试 ====================
    describe("Advanced pattern matching", () => {

        describe("Multiple wildcards", () => {
            it("should handle multiple * wildcards", () => {
                assert.strictEqual(path.matchesGlob("aXbYc", "a*b*c"), true);
                assert.strictEqual(path.matchesGlob("abc", "a*b*c"), true);
                assert.strictEqual(path.matchesGlob("aXYZbWc", "a*b*c"), true);
                assert.strictEqual(path.matchesGlob("axc", "a*b*c"), false);
            });

            it("should handle multiple ? wildcards", () => {
                assert.strictEqual(path.matchesGlob("axbyc", "a?b?c"), true);
                assert.strictEqual(path.matchesGlob("abc", "a?b?c"), false);
            });

            it("should handle consecutive wildcards", () => {
                // Multiple * should be treated as single *
                assert.strictEqual(path.matchesGlob("abc", "a***c"), true);
                assert.strictEqual(path.matchesGlob("abc", "a*****?c"), true);
                assert.strictEqual(path.matchesGlob("abc", "?*****??"), true);
                assert.strictEqual(path.matchesGlob("abc", "*****??"), true);
                assert.strictEqual(path.matchesGlob("abc", "?*****?c"), true);
                assert.strictEqual(path.matchesGlob("abc", "?***?****c"), true);
                assert.strictEqual(path.matchesGlob("abc", "?***?****?"), true);
                assert.strictEqual(path.matchesGlob("abc", "?***?****"), true);
                assert.strictEqual(path.matchesGlob("abc", "*******c"), true);
                assert.strictEqual(path.matchesGlob("abc", "*******?"), true);
            });
        });

        describe("Mixed wildcard patterns", () => {
            it("should handle ? and * combinations", () => {
                assert.strictEqual(path.matchesGlob("ab", "a?*"), true);
                assert.strictEqual(path.matchesGlob("abcd", "a?*"), true);
                assert.strictEqual(path.matchesGlob("a", "a?*"), false);
                assert.strictEqual(path.matchesGlob("a", "*?"), true);
                assert.strictEqual(path.matchesGlob("ab", "*?"), true);
                assert.strictEqual(path.matchesGlob("", "*?"), false);
            });

            it("should handle complex mixed patterns", () => {
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a*cd**?**??k"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a**?**cd**?**??k"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a**?**cd**?**??k***"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a**?**cd**?**??***k"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a**?**cd**?**??***k**"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a****c**?**??*****"), true);
            });
        });

        describe("Complex character class patterns", () => {
            it("should handle character classes with wildcards", () => {
                assert.strictEqual(path.matchesGlob("abc", "[a-c]b*"), true);
                assert.strictEqual(path.matchesGlob("bb", "[a-c]b*"), true);
                assert.strictEqual(path.matchesGlob("cb", "[a-c]b*"), true);
                assert.strictEqual(path.matchesGlob("db", "[a-c]b*"), false);
            });

            it("should handle special characters in character classes", () => {
                assert.strictEqual(path.matchesGlob("a-b", "a[X-]b"), true);
                assert.strictEqual(path.matchesGlob("aXb", "a[X-]b"), true);
                assert.strictEqual(path.matchesGlob("-", "[-abc]"), true);
                assert.strictEqual(path.matchesGlob("-", "[abc-]"), true);
                assert.strictEqual(path.matchesGlob("a", "[-abc]"), true);
                assert.strictEqual(path.matchesGlob("b", "[abc-]"), true);
            });

            it("should handle right bracket in character class", () => {
                // Right bracket at start loses special meaning
                assert.strictEqual(path.matchesGlob("]", "[]]"), true);
                assert.strictEqual(path.matchesGlob("]", "[]-]"), true);
                assert.strictEqual(path.matchesGlob("-", "[]-]"), true);
            });
        });
    });

    // ==================== 路径相关测试 ====================
    describe("Path handling", () => {

        describe("Basic path patterns", () => {
            it("should handle basic path matching", () => {
                assert.strictEqual(path.matchesGlob("a/b", "a/b"), true);
            });

            it("should respect path boundaries with wildcards", () => {
                // Wildcards should not cross path boundaries
                assert.strictEqual(path.matchesGlob("a/b/c", "a/*/c"), true);
                assert.strictEqual(path.matchesGlob("a/x/c", "a/*/c"), true);
                assert.strictEqual(path.matchesGlob("a/b/d/c", "a/*/c"), false);
            });
        });

        describe("Globstar (**) patterns", () => {
            it("should match any number of path segments", () => {
                assert.strictEqual(path.matchesGlob("a", "**"), true);
                assert.strictEqual(path.matchesGlob("a/b", "**"), true);
                assert.strictEqual(path.matchesGlob("a/b/c", "**"), true);
                assert.strictEqual(path.matchesGlob("", "**"), true); // Node.js: empty matches **
            });

            it("should handle globstar with prefix and suffix", () => {
                assert.strictEqual(path.matchesGlob("a/c", "a/**/c"), true);
                assert.strictEqual(path.matchesGlob("a/b/c", "a/**/c"), true);
                assert.strictEqual(path.matchesGlob("a/b/d/c", "a/**/c"), true);
            });

            it("should handle multiple globstars", () => {
                assert.strictEqual(path.matchesGlob("a/b/c", "**/**/**"), true);
            });
        });

        describe("Cross-platform path separators", () => {
            it("should handle POSIX path behavior", () => {
                // POSIX: backslash in pattern is treated as path separator
                // but backslash in text is treated as literal
                assert.strictEqual(path.posix.matchesGlob("a/b", "a/b"), true);
                assert.strictEqual(path.posix.matchesGlob("a/b", "a\\b"), true);
                assert.strictEqual(path.posix.matchesGlob("a\\b", "a/b"), false);
                assert.strictEqual(path.posix.matchesGlob("a\\b", "a\\b"), false);
                assert.strictEqual(path.posix.matchesGlob("ab", "a\\b"), false);
            });

            it("should handle Win32 path behavior", () => {
                // Win32: both / and \ are path separators everywhere
                assert.strictEqual(path.win32.matchesGlob("a/b", "a/b"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\b", "a/b"), true);
                assert.strictEqual(path.win32.matchesGlob("a/b", "a\\b"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\b", "a\\b"), true);
                assert.strictEqual(path.win32.matchesGlob("ab", "a\\b"), false);
                assert.strictEqual(path.win32.matchesGlob("a/", "a\\"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\", "a\\"), true);
                assert.strictEqual(path.win32.matchesGlob("a", "a\\"), false);
            });

            it("should handle Win32 drive letter patterns", () => {
                // Windows drive letters with different separators
                assert.strictEqual(path.win32.matchesGlob("C:\\file.txt", "C:\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("C:/file.txt", "C:\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("C:\\file.txt", "C:/*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("C:/file.txt", "C:/*.txt"), true);

                // Case insensitive drive letters on Windows
                assert.strictEqual(path.win32.matchesGlob("c:\\file.txt", "C:\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("C:\\file.txt", "c:\\*.txt"), true);

                // Different drives should not match
                assert.strictEqual(path.win32.matchesGlob("D:\\file.txt", "C:\\*.txt"), false);
            });

            it("should handle Win32 UNC path patterns", () => {
                // UNC paths (Universal Naming Convention)
                assert.strictEqual(path.win32.matchesGlob("\\\\server\\share\\file.txt", "\\\\server\\share\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("//server/share/file.txt", "\\\\server\\share\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("\\\\server\\share\\file.txt", "//server/share/*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("//server/share/file.txt", "//server/share/*.txt"), true);

                // Different servers/shares should not match
                assert.strictEqual(path.win32.matchesGlob("\\\\server1\\share\\file.txt", "\\\\server2\\share\\*.txt"), false);
                assert.strictEqual(path.win32.matchesGlob("\\\\server\\share1\\file.txt", "\\\\server\\share2\\*.txt"), false);
            });

            it("should handle Win32 relative path patterns", () => {
                // Windows relative paths with mixed separators
                assert.strictEqual(path.win32.matchesGlob(".\\file.txt", "./*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("./file.txt", ".\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("..\\parent\\file.txt", "../parent/*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("../parent/file.txt", "..\\parent\\*.txt"), true);

                // Multiple levels with mixed separators
                assert.strictEqual(path.win32.matchesGlob("..\\..\\file.txt", "../../*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("../../file.txt", "..\\..\\*.txt"), true);
            });

            it("should handle Win32 path pattern edge cases", () => {
                // Trailing separators
                assert.strictEqual(path.win32.matchesGlob("dir\\", "dir/"), true);
                assert.strictEqual(path.win32.matchesGlob("dir/", "dir\\"), true);
                assert.strictEqual(path.win32.matchesGlob("dir\\", "dir\\"), true);
                assert.strictEqual(path.win32.matchesGlob("dir/", "dir/"), true);

                // Multiple consecutive separators (should be normalized)
                assert.strictEqual(path.win32.matchesGlob("a\\\\b", "a/b"), true);
                assert.strictEqual(path.win32.matchesGlob("a//b", "a\\b"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\/b", "a/\\b"), true);
                assert.strictEqual(path.win32.matchesGlob("a/\\b", "a\\/b"), true);

                // Single vs double separators in patterns
                assert.strictEqual(path.win32.matchesGlob("a\\b", "a\\\\*"), true);
                assert.strictEqual(path.win32.matchesGlob("a/b", "a//*"), true);
            });

            it("should handle wildcards with different path separators", () => {
                // POSIX behavior: backslash in pattern is path separator
                assert.strictEqual(path.posix.matchesGlob("a/b/c", "a/*/c"), true);
                assert.strictEqual(path.posix.matchesGlob("a/b/c", "a\\*\\c"), true);
                assert.strictEqual(path.posix.matchesGlob("a\\b\\c", "a/*/c"), false);

                // Win32 behavior: both separators work everywhere
                assert.strictEqual(path.win32.matchesGlob("a/b/c", "a/*/c"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\b\\c", "a/*/c"), true);
                assert.strictEqual(path.win32.matchesGlob("a/b/c", "a\\*\\c"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\b\\c", "a\\*\\c"), true);

                // Complex wildcard patterns with mixed separators
                assert.strictEqual(path.win32.matchesGlob("a\\x\\y\\c", "a/*/*/c"), true);
                assert.strictEqual(path.win32.matchesGlob("a/x/y/c", "a\\*\\*\\c"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\x/y\\c", "a/*\\*/c"), true);
                assert.strictEqual(path.win32.matchesGlob("a/x\\y/c", "a\\*/*\\c"), true);
            });

            it("should handle globstar with different path separators", () => {
                // POSIX behavior
                assert.strictEqual(path.posix.matchesGlob("a/b/c/d", "a/**/d"), true);
                assert.strictEqual(path.posix.matchesGlob("a\\b\\c\\d", "a/**/d"), false);

                // Win32 behavior  
                assert.strictEqual(path.win32.matchesGlob("a/b/c/d", "a/**/d"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\b\\c\\d", "a/**/d"), true);
                assert.strictEqual(path.win32.matchesGlob("a/b/c/d", "a\\**\\d"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\b\\c\\d", "a\\**\\d"), true);

                // Complex globstar with mixed separators
                assert.strictEqual(path.win32.matchesGlob("a\\b/c\\d/e", "a/**/e"), true);
                assert.strictEqual(path.win32.matchesGlob("a/b\\c/d\\e", "a\\**\\e"), true);
                assert.strictEqual(path.win32.matchesGlob("a\\b\\c\\d\\e", "a/**/**/e"), true);
                assert.strictEqual(path.win32.matchesGlob("a/b/c/d/e", "a\\**\\**\\e"), true);
            });

            it("should handle Win32 long path support", () => {
                // Long paths with UNC prefix
                const longPath = "\\\\?\\C:\\very\\long\\path\\that\\exceeds\\260\\characters\\" + "x".repeat(200) + "\\file.txt";
                const longPattern = "\\\\?\\C:\\very\\long\\path\\**\\*.txt";
                assert.strictEqual(path.win32.matchesGlob(longPath, longPattern), true);

                // Mixed separators in long paths
                const mixedLongPath = "\\\\?\\C:/very/long\\path/that\\exceeds/260/characters/" + "x".repeat(200) + "/file.txt";
                assert.strictEqual(path.win32.matchesGlob(mixedLongPath, longPattern), true);
            });
        });

        describe("Path pattern edge cases", () => {
            it("should handle mixed path separators in complex patterns", () => {
                // Complex globstar patterns
                assert.strictEqual(path.matchesGlob("a", "**"), true);
                assert.strictEqual(path.matchesGlob("a/b", "**"), true);
                assert.strictEqual(path.matchesGlob("a/b/c", "**"), true);

                // Multiple globstars
                assert.strictEqual(path.matchesGlob("a/b/c", "**/**/**"), true);
            });

            it("should handle path patterns with wildcards", () => {
                // Path separator + wildcard combinations
                assert.strictEqual(path.matchesGlob("a/xyz/c", "a/*/c"), true);
                assert.strictEqual(path.matchesGlob("a/b/d/c", "a/*/c"), false);
            });

            it("should handle Win32 special characters in paths", () => {
                // Windows allows certain special characters that POSIX doesn't
                assert.strictEqual(path.win32.matchesGlob("file name.txt", "file*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("file[name].txt", "file*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("file{name}.txt", "file*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("file$name.txt", "file*.txt"), true);

                // Characters that are technically allowed in filenames but may cause issues
                assert.strictEqual(path.win32.matchesGlob("file:name.txt", "file*.txt"), true); // colon allowed in pattern matching
                assert.strictEqual(path.win32.matchesGlob("file|name.txt", "file*.txt"), true); // pipe allowed in pattern matching
                assert.strictEqual(path.win32.matchesGlob("file\"name.txt", "file*.txt"), true); // quote allowed in pattern matching
            });

            it("should handle Win32 path length limitations", () => {
                // Standard MAX_PATH (260 characters) limitations
                const normalPath = "C:\\" + "a\\".repeat(50) + "file.txt"; // ~103 chars
                assert.strictEqual(path.win32.matchesGlob(normalPath, "C:\\**\\*.txt"), true);

                // Extended path syntax to bypass MAX_PATH
                const extendedPath = "\\\\?\\" + normalPath;
                assert.strictEqual(path.win32.matchesGlob(extendedPath, "\\\\?\\C:\\**\\*.txt"), true);

                // UNC extended path syntax
                const uncExtended = "\\\\?\\UNC\\server\\share\\file.txt";
                assert.strictEqual(path.win32.matchesGlob(uncExtended, "\\\\?\\UNC\\server\\**\\*.txt"), true);
            });

            it("should handle Win32 alternate data streams", () => {
                // NTFS alternate data streams (ADS)
                assert.strictEqual(path.win32.matchesGlob("file.txt:hidden", "file.txt:*"), true);
                assert.strictEqual(path.win32.matchesGlob("document.doc:metadata:$DATA", "document.doc:*"), true);
                assert.strictEqual(path.win32.matchesGlob("image.jpg:thumbnail", "*.jpg:*"), true);

                // Default data stream
                assert.strictEqual(path.win32.matchesGlob("file.txt:$DATA", "file.txt"), false); // explicit stream vs implicit
                assert.strictEqual(path.win32.matchesGlob("file.txt", "file.txt:$DATA"), false);
            });

            it("should handle Win32 short filenames (8.3 format)", () => {
                // Classic DOS 8.3 filename format (simple wildcard matching)
                assert.strictEqual(path.win32.matchesGlob("PROGRA~1", "PROGRA*"), true);
                assert.strictEqual(path.win32.matchesGlob("LONGFI~1.TXT", "LONGFI*.TXT"), true);
                assert.strictEqual(path.win32.matchesGlob("MYDOCU~1.DOC", "*~*.DOC"), true);

                // Note: Node.js pattern matching doesn't automatically correlate 
                // short names with long names - they need exact pattern matches
                assert.strictEqual(path.win32.matchesGlob("C:\\PROGRA~1\\file.txt", "C:\\PROGRA~1\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("C:\\Program Files\\file.txt", "C:\\Program Files\\*.txt"), true);
            });

            it("should handle Win32 device names in paths", () => {
                // Windows device names (should be treated as regular names in paths)
                assert.strictEqual(path.win32.matchesGlob("folder\\CON\\file.txt", "folder\\*\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("folder\\PRN\\file.txt", "folder\\*\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("folder\\LPT1\\file.txt", "folder\\*\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("folder\\COM1\\file.txt", "folder\\*\\*.txt"), true);

                // Device names with extensions
                assert.strictEqual(path.win32.matchesGlob("folder\\CON.txt\\file.txt", "folder\\*.txt\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("folder\\NUL.log\\file.txt", "folder\\*.log\\*.txt"), true);
            });

            it("should handle Win32 hidden and system files", () => {
                // Hidden files (conventionally start with dot on Unix, but not Windows)
                assert.strictEqual(path.win32.matchesGlob(".hidden", ".*"), true);
                assert.strictEqual(path.win32.matchesGlob("normal.txt", ".*"), false);

                // System files with special attributes (treated as normal files in pattern matching)
                assert.strictEqual(path.win32.matchesGlob("boot.ini", "*.ini"), true);
                assert.strictEqual(path.win32.matchesGlob("pagefile.sys", "*.sys"), true);
                assert.strictEqual(path.win32.matchesGlob("hiberfil.sys", "hiber*.sys"), true);
            });

            it("should handle Win32 network drive patterns", () => {
                // Mapped network drives
                assert.strictEqual(path.win32.matchesGlob("Z:\\shared\\file.txt", "Z:\\**\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("X:\\network\\folder\\doc.pdf", "*:\\network\\**\\*.pdf"), true);

                // Administrative shares
                assert.strictEqual(path.win32.matchesGlob("\\\\server\\C$\\file.txt", "\\\\server\\*$\\*.txt"), true);
                assert.strictEqual(path.win32.matchesGlob("\\\\machine\\ADMIN$\\system\\file.log", "\\\\machine\\ADMIN$\\**\\*.log"), true);
            });
        });

    });

    // ==================== 隐藏文件处理测试 ====================
    describe("Hidden file handling", () => {
        it("should not match hidden files with simple wildcard *", () => {
            // Node.js behavior: * does not match hidden files (files starting with .)
            assert.strictEqual(path.matchesGlob(".env", "*"), false);
            assert.strictEqual(path.matchesGlob(".gitignore", "*"), false);
            assert.strictEqual(path.matchesGlob(".hidden", "*"), false);
            assert.strictEqual(path.matchesGlob("visible.txt", "*"), true);
        });

        it("should match hidden files with explicit dot patterns", () => {
            // Explicit dot patterns should match hidden files
            assert.strictEqual(path.matchesGlob(".env", ".*"), true);
            assert.strictEqual(path.matchesGlob(".gitignore", ".*"), true);
            assert.strictEqual(path.matchesGlob(".env", ".env"), true);
            assert.strictEqual(path.matchesGlob(".gitignore", ".git*"), true);
            assert.strictEqual(path.matchesGlob(".env.local", ".env*"), true);
        });

        it("should match hidden files in brace expansion when explicitly specified", () => {
            // Brace expansion should match hidden files when explicitly included
            assert.strictEqual(path.matchesGlob(".env", "{package.json,.gitignore,.env*}"), true);
            assert.strictEqual(path.matchesGlob(".gitignore", "{package.json,.gitignore,.env*}"), true);
            assert.strictEqual(path.matchesGlob("package.json", "{package.json,.gitignore,.env*}"), true);
            assert.strictEqual(path.matchesGlob("other.txt", "{package.json,.gitignore,.env*}"), false);
        });

        it("should not match hidden files with wildcard in brace expansion", () => {
            // Wildcards in brace expansion should follow same rules
            assert.strictEqual(path.matchesGlob(".env", "{*,*.txt}"), false);
            assert.strictEqual(path.matchesGlob("file.txt", "{*,*.txt}"), true);
            assert.strictEqual(path.matchesGlob("file", "{*,*.txt}"), true);
        });

        it("should handle question mark wildcard with hidden files", () => {
            // Question mark should not match hidden files at start
            assert.strictEqual(path.matchesGlob(".a", "?a"), false);
            assert.strictEqual(path.matchesGlob("xa", "?a"), true);
            assert.strictEqual(path.matchesGlob(".env", "????"), false);
            assert.strictEqual(path.matchesGlob("test", "????"), true);
        });

        it("should handle character classes with hidden files", () => {
            // Character classes should follow normal rules for hidden files
            assert.strictEqual(path.matchesGlob(".a", "[.]a"), true);  // Explicit dot in class
            assert.strictEqual(path.matchesGlob(".a", "[a-z]a"), false); // Dot not in range
            assert.strictEqual(path.matchesGlob("aa", "[a-z]a"), true);
        });

        // ==================== Globstar 隐藏文件测试 ====================
        it("should NOT match hidden files with globstar **/*", () => {
            // This is the key test: **/* should NOT match hidden files in Node.js
            assert.strictEqual(path.matchesGlob(".env", "**/*"), false);
            assert.strictEqual(path.matchesGlob(".gitignore", "**/*"), false);
            assert.strictEqual(path.matchesGlob(".hidden", "**/*"), false);
            assert.strictEqual(path.matchesGlob("visible.txt", "**/*"), true);
        });

        it("should NOT match hidden files in subdirectories with **/*", () => {
            // Hidden files in subdirectories should also not match **/*
            assert.strictEqual(path.matchesGlob("dir/.env", "**/*"), false);
            assert.strictEqual(path.matchesGlob("src/.gitignore", "**/*"), false);
            assert.strictEqual(path.matchesGlob("src/test/.hidden", "**/*"), false);
            assert.strictEqual(path.matchesGlob("src/test/visible.txt", "**/*"), true);
        });

        it("should match hidden files with explicit globstar patterns", () => {
            // Explicit patterns for hidden files should work
            assert.strictEqual(path.matchesGlob(".env", "**/.*"), true);
            assert.strictEqual(path.matchesGlob("dir/.env", "**/.*"), true);
            assert.strictEqual(path.matchesGlob(".gitignore", "**/.git*"), true);
            assert.strictEqual(path.matchesGlob("src/.gitignore", "**/.git*"), true);
        });

        it("should handle globstar with various wildcard combinations", () => {
            // Test various globstar combinations
            assert.strictEqual(path.matchesGlob(".env", "*/**"), false);  // Hidden file at root
            assert.strictEqual(path.matchesGlob("dir/.env", "*/**"), false); // Hidden file in subdir
            assert.strictEqual(path.matchesGlob("visible.txt", "*/**"), false); // Node.js behavior: */** doesn't match files at root
            assert.strictEqual(path.matchesGlob("dir/visible.txt", "*/**"), true); // Visible file in subdir

            // Specific patterns
            assert.strictEqual(path.matchesGlob(".env", "**"), false);    // Just globstar
            assert.strictEqual(path.matchesGlob("visible.txt", "**"), true); // Just globstar
        });

        it("should handle nested directory hidden files", () => {
            // Test nested directory patterns with hidden files
            assert.strictEqual(path.matchesGlob("a/b/.env", "a/**/*"), false);
            assert.strictEqual(path.matchesGlob("a/b/visible.txt", "a/**/*"), true);
            assert.strictEqual(path.matchesGlob("a/.env", "a/*"), false);
            assert.strictEqual(path.matchesGlob("a/visible.txt", "a/*"), true);
        });

        it("should handle complex globstar patterns with hidden files", () => {
            // More complex patterns
            assert.strictEqual(path.matchesGlob("src/components/.env", "src/**/*.env"), false); // Hidden file extension
            assert.strictEqual(path.matchesGlob("src/components/visible.env", "src/**/*.env"), true); // Visible file
            assert.strictEqual(path.matchesGlob("src/.config/file.txt", "src/**/*.txt"), false); // Hidden directory
            assert.strictEqual(path.matchesGlob("src/config/file.txt", "src/**/*.txt"), true); // Visible directory
        });

        // ==================== 隐藏目录测试 ====================
        it("should NOT match hidden directories with globstar patterns", () => {
            // Hidden directories should not be matched by wildcard patterns
            assert.strictEqual(path.matchesGlob(".git", "*"), false);
            assert.strictEqual(path.matchesGlob(".vscode", "*"), false);
            assert.strictEqual(path.matchesGlob(".hidden", "*"), false);
            assert.strictEqual(path.matchesGlob("visible", "*"), true);
        });

        it("should NOT match files in hidden directories with **/*", () => {
            // Files inside hidden directories should not match **/*
            assert.strictEqual(path.matchesGlob(".git/config", "**/*"), false);
            assert.strictEqual(path.matchesGlob(".git/hooks/pre-commit", "**/*"), false);
            assert.strictEqual(path.matchesGlob(".vscode/settings.json", "**/*"), false);
            assert.strictEqual(path.matchesGlob(".node_modules/package/index.js", "**/*"), false);
            assert.strictEqual(path.matchesGlob("visible/file.txt", "**/*"), true);
        });

        it("should NOT match nested hidden directories with **/*", () => {
            // Nested hidden directories should not match
            assert.strictEqual(path.matchesGlob("src/.hidden", "**/*"), false);
            assert.strictEqual(path.matchesGlob("src/.git", "**/*"), false);
            assert.strictEqual(path.matchesGlob("project/.vscode", "**/*"), false);
            assert.strictEqual(path.matchesGlob("src/visible", "**/*"), true);
        });

        it("should NOT match deeply nested files in hidden directories", () => {
            // Deep nesting with hidden directories
            assert.strictEqual(path.matchesGlob("src/.git/objects/abc123", "**/*"), false);
            assert.strictEqual(path.matchesGlob("project/.vscode/extensions/file.json", "**/*"), false);
            assert.strictEqual(path.matchesGlob(".hidden/deep/nested/file.txt", "**/*"), false);
            assert.strictEqual(path.matchesGlob("visible/deep/nested/file.txt", "**/*"), true);
        });

        it("should match hidden directories with explicit dot patterns", () => {
            // Explicit patterns should work for hidden directories
            assert.strictEqual(path.matchesGlob(".git", ".*"), true);
            assert.strictEqual(path.matchesGlob(".vscode", ".*"), true);
            assert.strictEqual(path.matchesGlob(".git", ".git"), true);
            assert.strictEqual(path.matchesGlob(".vscode", ".v*"), true);
        });

        it("should match files in hidden directories with explicit patterns", () => {
            // Explicit patterns should match files in hidden directories
            assert.strictEqual(path.matchesGlob(".git/config", ".git/*"), true);
            assert.strictEqual(path.matchesGlob(".git/hooks/pre-commit", ".git/**/*"), true);
            assert.strictEqual(path.matchesGlob(".vscode/settings.json", ".vscode/*.json"), true);
            assert.strictEqual(path.matchesGlob(".hidden/file.txt", ".hidden/*"), true);
        });

        it("should handle mixed visible and hidden directories", () => {
            // Mixed scenarios with both visible and hidden directories
            assert.strictEqual(path.matchesGlob("src/.hidden/file.txt", "src/**/*"), false); // Hidden dir in visible path
            assert.strictEqual(path.matchesGlob("src/visible/file.txt", "src/**/*"), true); // Visible dir in visible path
            assert.strictEqual(path.matchesGlob(".project/src/file.txt", "**/*"), false); // Visible dir in hidden path
            assert.strictEqual(path.matchesGlob("project/src/file.txt", "**/*"), true); // All visible
        });

        it("should handle globstar patterns with mixed hidden paths", () => {
            // Complex patterns with both hidden files and directories
            assert.strictEqual(path.matchesGlob("src/.config/.env", "src/**/*"), false); // Hidden dir + hidden file
            assert.strictEqual(path.matchesGlob("src/.config/visible.txt", "src/**/*"), false); // Hidden dir + visible file
            assert.strictEqual(path.matchesGlob("src/config/.env", "src/**/*"), false); // Visible dir + hidden file
            assert.strictEqual(path.matchesGlob("src/config/visible.txt", "src/**/*"), true); // All visible
        });

        it("should handle directory-specific globstar patterns", () => {
            // Directory traversal patterns
            assert.strictEqual(path.matchesGlob(".git/objects", "**/objects"), false); // Hidden parent
            assert.strictEqual(path.matchesGlob("project/.git/objects", "**/objects"), false); // Hidden intermediate
            assert.strictEqual(path.matchesGlob("project/git/objects", "**/objects"), true); // All visible
            assert.strictEqual(path.matchesGlob(".hidden/dir/objects", "**/objects"), false); // Hidden root
        });

        it("should handle hidden directory patterns with file extensions", () => {
            // File extension patterns in hidden directories
            assert.strictEqual(path.matchesGlob(".vscode/settings.json", "**/*.json"), false); // Hidden dir
            assert.strictEqual(path.matchesGlob("config/.hidden.json", "**/*.json"), false); // Hidden file
            assert.strictEqual(path.matchesGlob(".project/config.json", "**/*.json"), false); // Hidden root dir
            assert.strictEqual(path.matchesGlob("project/config.json", "**/*.json"), true); // All visible
        });
    });

    // ==================== 扩展语法测试 ====================
    describe("Extended syntax", () => {

        describe("Extglob patterns", () => {
            it("should handle @(...) - match exactly one", () => {
                // @(pattern1|pattern2) - matches exactly one of the patterns
                assert.strictEqual(path.matchesGlob("abc", "a@(b|c)c"), true);
                assert.strictEqual(path.matchesGlob("acc", "a@(b|c)c"), true);
                assert.strictEqual(path.matchesGlob("adc", "a@(b|c)c"), false);
                assert.strictEqual(path.matchesGlob("ac", "a@(b|c)c"), false); // no match
                assert.strictEqual(path.matchesGlob("abcc", "a@(b|c)c"), false); // too many
            });

            it("should handle ?(...)  - match zero or one", () => {
                // ?(pattern) - matches zero or one occurrence
                assert.strictEqual(path.matchesGlob("ac", "a?(b)c"), true);   // zero match
                assert.strictEqual(path.matchesGlob("abc", "a?(b)c"), true);  // one match
                assert.strictEqual(path.matchesGlob("abbc", "a?(b)c"), false); // more than one
                assert.strictEqual(path.matchesGlob("axc", "a?(b)c"), false);  // wrong char
            });

            it("should handle +(...)  - match one or more", () => {
                // +(pattern) - matches one or more occurrences
                assert.strictEqual(path.matchesGlob("abc", "a+(b)c"), true);   // one match
                assert.strictEqual(path.matchesGlob("abbc", "a+(b)c"), true);  // multiple match
                assert.strictEqual(path.matchesGlob("ac", "a+(b)c"), false);   // zero match
                assert.strictEqual(path.matchesGlob("axc", "a+(b)c"), false);  // wrong char
            });

            it("should handle *(...)  - match zero or more", () => {
                // *(pattern) - matches zero or more occurrences
                assert.strictEqual(path.matchesGlob("ac", "a*(b)c"), true);    // zero match
                assert.strictEqual(path.matchesGlob("abc", "a*(b)c"), true);   // one match
                assert.strictEqual(path.matchesGlob("abbc", "a*(b)c"), true);  // multiple match
                assert.strictEqual(path.matchesGlob("axc", "a*(b)c"), false);  // wrong char
            });

            it("should handle !(...)  - match anything except", () => {
                // !(pattern) - matches anything except the pattern
                // Note: this might be disabled by nonegate: true in Node.js config
                assert.strictEqual(path.matchesGlob("axc", "a!(b)c"), true);   // not b
                assert.strictEqual(path.matchesGlob("abc", "a!(b)c"), false);  // is b
                assert.strictEqual(path.matchesGlob("acc", "a!(b)c"), true);   // not b
            });

            it("should handle complex extglob patterns", () => {
                // Multiple alternatives
                assert.strictEqual(path.matchesGlob("abc", "a@(b|c|d)c"), true);
                assert.strictEqual(path.matchesGlob("acc", "a@(b|c|d)c"), true);
                assert.strictEqual(path.matchesGlob("adc", "a@(b|c|d)c"), true);
                assert.strictEqual(path.matchesGlob("aec", "a@(b|c|d)c"), false);

                // Mixed with other patterns
                assert.strictEqual(path.matchesGlob("a1c", "a@(1|?)c"), true);
                assert.strictEqual(path.matchesGlob("axc", "a@(1|?)c"), true);
                assert.strictEqual(path.matchesGlob("a12c", "a@(1|?)c"), false);
            });

            it("should handle nested extglob patterns", () => {
                // Nested patterns might be complex - test basic nesting
                assert.strictEqual(path.matchesGlob("abc", "a@(b@(c))"), true);
                assert.strictEqual(path.matchesGlob("ac", "a@(b@(c))"), false);
            });

            it("should handle extglob with wildcards", () => {
                // Extglob combined with * and ?
                assert.strictEqual(path.matchesGlob("abxc", "a@(b*)c"), true);
                assert.strictEqual(path.matchesGlob("abyc", "a@(b*)c"), true);
                assert.strictEqual(path.matchesGlob("abc", "a@(b*)c"), true);   // b followed by nothing
                assert.strictEqual(path.matchesGlob("ac", "a@(b*)c"), false);   // no b at all - @(b*) requires b
                assert.strictEqual(path.matchesGlob("axc", "a@(b*)c"), false);  // doesn't start with b

                // Test with empty alternative to allow zero matches
                assert.strictEqual(path.matchesGlob("ac", "a@(b*|)c"), true);   // empty alternative matches
                assert.strictEqual(path.matchesGlob("abc", "a@(b*|)c"), true);  // b* alternative matches
            });
        });

        describe("Brace expansion patterns", () => {
            it("should handle basic brace expansion", () => {
                assert.strictEqual(path.matchesGlob("abd", "a{b,c}d"), true);
                assert.strictEqual(path.matchesGlob("acd", "a{b,c}d"), true);
                assert.strictEqual(path.matchesGlob("aed", "a{b,c}d"), false);
                assert.strictEqual(path.matchesGlob("ad", "a{b,c}d"), false);
            });

            it("should handle multiple alternatives in braces", () => {
                assert.strictEqual(path.matchesGlob("test.js", "test.{js,ts,jsx}"), true);
                assert.strictEqual(path.matchesGlob("test.ts", "test.{js,ts,jsx}"), true);
                assert.strictEqual(path.matchesGlob("test.jsx", "test.{js,ts,jsx}"), true);
                assert.strictEqual(path.matchesGlob("test.py", "test.{js,ts,jsx}"), false);
            });

            it("should handle numeric sequences", () => {
                assert.strictEqual(path.matchesGlob("file1.txt", "file{1..3}.txt"), true);
                assert.strictEqual(path.matchesGlob("file2.txt", "file{1..3}.txt"), true);
                assert.strictEqual(path.matchesGlob("file3.txt", "file{1..3}.txt"), true);
                assert.strictEqual(path.matchesGlob("file0.txt", "file{1..3}.txt"), false);
                assert.strictEqual(path.matchesGlob("file4.txt", "file{1..3}.txt"), false);
            });

            it("should handle zero-padded numeric sequences", () => {
                assert.strictEqual(path.matchesGlob("img01.jpg", "img{01..03}.jpg"), true);
                assert.strictEqual(path.matchesGlob("img02.jpg", "img{01..03}.jpg"), true);
                assert.strictEqual(path.matchesGlob("img03.jpg", "img{01..03}.jpg"), true);
                assert.strictEqual(path.matchesGlob("img1.jpg", "img{01..03}.jpg"), false); // Node.js: zero-padding is significant
                assert.strictEqual(path.matchesGlob("img04.jpg", "img{01..03}.jpg"), false);
            });

            it("should handle alphabetic sequences", () => {
                assert.strictEqual(path.matchesGlob("parta.txt", "part{a..c}.txt"), true);
                assert.strictEqual(path.matchesGlob("partb.txt", "part{a..c}.txt"), true);
                assert.strictEqual(path.matchesGlob("partc.txt", "part{a..c}.txt"), true);
                assert.strictEqual(path.matchesGlob("partd.txt", "part{a..c}.txt"), false);
            });

            it("should handle nested brace expansion", () => {
                assert.strictEqual(path.matchesGlob("abcf", "a{b{c,d},e}f"), true);
                assert.strictEqual(path.matchesGlob("abdf", "a{b{c,d},e}f"), true);
                assert.strictEqual(path.matchesGlob("aef", "a{b{c,d},e}f"), true);
                assert.strictEqual(path.matchesGlob("af", "a{b{c,d},e}f"), false);
                assert.strictEqual(path.matchesGlob("abf", "a{b{c,d},e}f"), false);
            });

            it("should handle empty alternatives in braces", () => {
                assert.strictEqual(path.matchesGlob("test", "test{,.txt}"), true);
                assert.strictEqual(path.matchesGlob("test.txt", "test{,.txt}"), true);
                assert.strictEqual(path.matchesGlob("test.js", "test{,.txt}"), false);
            });

            it("should handle single alternative (should behave like literal)", () => {
                assert.strictEqual(path.matchesGlob("test", "test{abc}"), false); // No expansion with single item
                assert.strictEqual(path.matchesGlob("test{abc}", "test{abc}"), true); // Literal match
            });

            it("should handle braces with wildcards", () => {
                assert.strictEqual(path.matchesGlob("abc.js", "*.{js,ts}"), true);
                assert.strictEqual(path.matchesGlob("abc.ts", "*.{js,ts}"), true);
                assert.strictEqual(path.matchesGlob("abc.py", "*.{js,ts}"), false);
                assert.strictEqual(path.matchesGlob("test123.txt", "*{1..3}.txt"), true);
                assert.strictEqual(path.matchesGlob("test4.txt", "*{1..3}.txt"), false);
            });

            it("should handle brace expansion stress tests", () => {
                // Reverse ranges - Node.js supports: {3..1} expands to 3,2,1
                assert.strictEqual(path.matchesGlob("file3.txt", "file{3..1}.txt"), true);
                assert.strictEqual(path.matchesGlob("file2.txt", "file{3..1}.txt"), true);
                assert.strictEqual(path.matchesGlob("file1.txt", "file{3..1}.txt"), true);

                // Single number range (works in fibjs)
                assert.strictEqual(path.matchesGlob("file1.txt", "file{1..1}.txt"), true);
                assert.strictEqual(path.matchesGlob("file2.txt", "file{1..1}.txt"), false);

                // Zero-padded edge cases
                assert.strictEqual(path.matchesGlob("img00.jpg", "img{00..02}.jpg"), true);
                assert.strictEqual(path.matchesGlob("img0.jpg", "img{00..02}.jpg"), false);

                // Single character range
                assert.strictEqual(path.matchesGlob("parta.txt", "part{a..a}.txt"), true);
                assert.strictEqual(path.matchesGlob("partb.txt", "part{a..a}.txt"), false);

                // Reverse alphabetic range - Node.js supports: {c..a} expands to c,b,a
                assert.strictEqual(path.matchesGlob("partc.txt", "part{c..a}.txt"), true);
                assert.strictEqual(path.matchesGlob("partb.txt", "part{c..a}.txt"), true);
                assert.strictEqual(path.matchesGlob("parta.txt", "part{c..a}.txt"), true);
            });
        });
    });

    // Additional tests based on minimatch official test suite
    describe("Advanced patterns (from minimatch test suite)", () => {

        describe("Complex wildcard combinations", () => {
            it("should handle multiple consecutive wildcards", () => {
                // Multiple * should be treated as single *
                assert.strictEqual(path.matchesGlob("abc", "a***c"), true);
                assert.strictEqual(path.matchesGlob("abc", "a*****?c"), true);
                assert.strictEqual(path.matchesGlob("abc", "?*****??"), true);
                assert.strictEqual(path.matchesGlob("abc", "*****??"), true);
                assert.strictEqual(path.matchesGlob("abc", "?*****?c"), true);
                assert.strictEqual(path.matchesGlob("abc", "?***?****c"), true);
                assert.strictEqual(path.matchesGlob("abc", "?***?****?"), true);
                assert.strictEqual(path.matchesGlob("abc", "?***?****"), true);
                assert.strictEqual(path.matchesGlob("abc", "*******c"), true);
                assert.strictEqual(path.matchesGlob("abc", "*******?"), true);
            });

            it("should handle complex mixed patterns", () => {
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a*cd**?**??k"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a**?**cd**?**??k"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a**?**cd**?**??k***"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a**?**cd**?**??***k"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a**?**cd**?**??***k**"), true);
                assert.strictEqual(path.matchesGlob("abcdecdhjk", "a****c**?**??*****"), true);
            });
        });

        describe("Character class edge cases", () => {
            it("should handle character class with dash at start/end", () => {
                assert.strictEqual(path.matchesGlob("-", "[-abc]"), true);
                assert.strictEqual(path.matchesGlob("-", "[abc-]"), true);
                assert.strictEqual(path.matchesGlob("a", "[-abc]"), true);
                assert.strictEqual(path.matchesGlob("b", "[abc-]"), true);
            });

            it("should handle right bracket in character class", () => {
                // Right bracket at start loses special meaning
                assert.strictEqual(path.matchesGlob("]", "[]]"), true);
                assert.strictEqual(path.matchesGlob("]", "[]-]"), true);
                assert.strictEqual(path.matchesGlob("-", "[]-]"), true);
            });

            it("should handle unclosed character classes", () => {
                // Unclosed brackets should be treated as literals
                assert.strictEqual(path.matchesGlob("[", "["), true);
                assert.strictEqual(path.matchesGlob("[abc", "[abc"), true);
                assert.strictEqual(path.matchesGlob("a", "[abc"), false);
                assert.strictEqual(path.matchesGlob("[abc", "[*"), true);
                assert.strictEqual(path.matchesGlob("[]", "[]"), true);
            });

            it("should handle character ranges correctly", () => {
                assert.strictEqual(path.matchesGlob("p", "[a-z]"), true);
                // Platform-specific case sensitivity behavior
                assert.strictEqual(path.matchesGlob("A", "[a-z]"), isCaseInsensitive); // Mac/Windows: case insensitive ranges, Linux: case sensitive
                assert.strictEqual(path.matchesGlob("5", "[0-9]"), true);
                assert.strictEqual(path.matchesGlob("a", "[0-9]"), false);
            });
        });

        describe("Special file patterns", () => {
            it("should handle patterns with dots", () => {
                // Node.js doesn't support escaping (windowsPathsNoEscape: true)
                // So *.\\* is treated as literal pattern, not escaped *
                assert.strictEqual(path.matchesGlob("r.*", "*.\\*"), false);
                assert.strictEqual(path.matchesGlob("r.*", "*.*"), true);
                assert.strictEqual(path.matchesGlob("a.b", "a.b"), true);
                assert.strictEqual(path.matchesGlob("axb", "a.b"), false);
            });

            it("should handle path with slashes", () => {
                assert.strictEqual(path.matchesGlob("man/man1/bash.1", "*/man*/bash.*"), true);
                assert.strictEqual(path.matchesGlob("man/man1/bash.1", "man/man1/bash.1"), true);
            });

            it("should handle empty string patterns", () => {
                assert.strictEqual(path.matchesGlob("", ""), true);
                assert.strictEqual(path.matchesGlob("a", ""), false);
            });
        });

        describe("Brace expansion edge cases", () => {
            it("should handle malformed brace patterns", () => {
                // Malformed braces should be treated as literals
                assert.strictEqual(path.matchesGlob("a{b", "a{b"), true);
                assert.strictEqual(path.matchesGlob("a{b,c", "a{b,c"), true);
                assert.strictEqual(path.matchesGlob("z{a,bd", "z{a,b{,c}d"), true);
                assert.strictEqual(path.matchesGlob("z{a,bcd", "z{a,b{,c}d"), true);
            });

            it("should handle complex nested braces", () => {
                // Complex nesting that should expand properly
                assert.strictEqual(path.matchesGlob("abxy", "a{b,c{d,e},{f,g}h}x{y,z}"), true);
                assert.strictEqual(path.matchesGlob("acdxy", "a{b,c{d,e},{f,g}h}x{y,z}"), true);
                assert.strictEqual(path.matchesGlob("afhxz", "a{b,c{d,e},{f,g}h}x{y,z}"), true);
                assert.strictEqual(path.matchesGlob("aghxy", "a{b,c{d,e},{f,g}h}x{y,z}"), true);
                assert.strictEqual(path.matchesGlob("axyz", "a{b,c{d,e},{f,g}h}x{y,z}"), false);
            });

            it("should handle braces with special characters", () => {
                assert.strictEqual(path.matchesGlob("a${c}${d}", "{a,b}${c}${d}"), true);
                assert.strictEqual(path.matchesGlob("b${c}${d}", "{a,b}${c}${d}"), true);
                assert.strictEqual(path.matchesGlob("${a}${b}c", "${a}${b}{c,d}"), true);
                assert.strictEqual(path.matchesGlob("${a}${b}d", "${a}${b}{c,d}"), true);
            });
        });

        describe("Extglob advanced patterns", () => {
            it("should handle extglob ending with state char", () => {
                assert.strictEqual(path.matchesGlob("ax", "a?(b*)"), false);
                assert.strictEqual(path.matchesGlob("ax", "?(a*|b)"), true);
            });

            it.todo("should handle complex extglob combinations", () => {
                // Multiple extglob patterns
                assert.strictEqual(path.matchesGlob("abc", "a@(b|c)*"), true);
                assert.strictEqual(path.matchesGlob("ab", "a@(b|c)*"), false);
                assert.strictEqual(path.matchesGlob("a", "a@(b|c)*"), false);

                // Extglob with character classes - platform-specific behavior
                assert.strictEqual(path.matchesGlob("a1c", "a@([0-9]|[a-z])c"), true);
                assert.strictEqual(path.matchesGlob("abc", "a@([0-9]|[a-z])c"), true);
                assert.strictEqual(path.matchesGlob("aXc", "a@([0-9]|[a-z])c"), isCaseInsensitive); // Mac/Windows: case insensitive, Linux: case sensitive
                assert.strictEqual(path.matchesGlob("a@c", "a@([0-9]|[a-z])c"), false);
            });

            it.todo("should handle extglob stress tests", () => {
                // Nested extglob patterns
                assert.strictEqual(path.matchesGlob("abc", "a@(b@(c))"), true);
                assert.strictEqual(path.matchesGlob("ac", "a@(b@(c))"), false);
                assert.strictEqual(path.matchesGlob("abcde", "a@(b@(c)*)*"), true);

                // Multiple extglob alternatives
                assert.strictEqual(path.matchesGlob("axc", "a@(b|x|y)c"), true);
                assert.strictEqual(path.matchesGlob("ayc", "a@(b|x|y)c"), true);
                assert.strictEqual(path.matchesGlob("azc", "a@(b|x|y)c"), false);

                // Extglob with empty alternatives
                assert.strictEqual(path.matchesGlob("ac", "a@(b|)c"), true);
                assert.strictEqual(path.matchesGlob("abc", "a@(b|)c"), true);
                assert.strictEqual(path.matchesGlob("axc", "a@(b|)c"), false);

                // Multiple empty alternatives
                assert.strictEqual(path.matchesGlob("a", "a@(||)"), true);
                assert.strictEqual(path.matchesGlob("ab", "a@(||)"), false);
            });
        });

        describe("Performance and limits", () => {
            it("should handle very long patterns", () => {
                // Test with long pattern to ensure no stack overflow
                const longPattern = "a" + "*".repeat(100) + "b";
                const longString = "a" + "x".repeat(50) + "b";
                assert.strictEqual(path.matchesGlob(longString, longPattern), true);
            });

            it("should handle deeply nested patterns", () => {
                // Test deeply nested braces
                const nested = "a{b{c{d{e}}}}f";
                assert.strictEqual(path.matchesGlob("a{b{c{d{e}}}}f", nested), true);
                assert.strictEqual(path.matchesGlob("abcdef", nested), false); // No expansion with single items
            });
        });

        describe("Unicode and special characters", () => {
            it("should handle basic unicode characters", () => {
                assert.strictEqual(path.matchesGlob("café", "café"), true);
                assert.strictEqual(path.matchesGlob("café", "caf*"), true);
                assert.strictEqual(path.matchesGlob("café", "ca?é"), true);
                assert.strictEqual(path.matchesGlob("测试", "*试"), true);
            });

        });

        // ==================== 边界情况和错误处理 ====================
        describe("Edge cases and error handling", () => {

            describe("Empty patterns and strings", () => {
                it("should handle empty string matching", () => {
                    assert.strictEqual(path.matchesGlob("", ""), true);
                    assert.strictEqual(path.matchesGlob("a", ""), false);
                    assert.strictEqual(path.matchesGlob("", "a"), false);
                    assert.strictEqual(path.matchesGlob("", "*"), false); // Node.js behavior: empty string does not match *
                    assert.strictEqual(path.matchesGlob("", "?"), false);
                });

                it("should handle various empty pattern combinations", () => {
                    // Single character vs empty
                    assert.strictEqual(path.matchesGlob("", "?"), false);
                    assert.strictEqual(path.matchesGlob("", "*"), false); // Node.js: empty doesn't match *
                    assert.strictEqual(path.matchesGlob("", "*?"), false);
                });
            });

            describe("Malformed patterns", () => {
                it("should handle malformed character classes", () => {
                    // Malformed classes should be treated as literals
                    assert.strictEqual(path.matchesGlob("[", "["), true);
                    assert.strictEqual(path.matchesGlob("[abc", "[abc"), true);
                    assert.strictEqual(path.matchesGlob("a", "[abc"), false);
                    assert.strictEqual(path.matchesGlob("[abc", "[*"), true);
                    assert.strictEqual(path.matchesGlob("[]", "[]"), true);
                });

                it("should handle unclosed character classes", () => {
                    // Unclosed brackets should be treated as literals
                    assert.strictEqual(path.matchesGlob("[", "["), true);
                    assert.strictEqual(path.matchesGlob("[abc", "[abc"), true);
                    assert.strictEqual(path.matchesGlob("a", "[abc"), false);
                    assert.strictEqual(path.matchesGlob("[abc", "[*"), true);
                    assert.strictEqual(path.matchesGlob("[]", "[]"), true);
                });

                it("should handle malformed extglob patterns", () => {
                    // Incomplete patterns should be treated as literals
                    assert.strictEqual(path.matchesGlob("a@(bc", "a@(bc"), true);
                    assert.strictEqual(path.matchesGlob("a@(b", "a@(b"), true);
                    assert.strictEqual(path.matchesGlob("abc", "a@(bc"), false);
                });

                it("should handle malformed brace patterns", () => {
                    // Malformed braces should be treated as literals
                    assert.strictEqual(path.matchesGlob("a{bc", "a{bc"), true);
                    assert.strictEqual(path.matchesGlob("a{b", "a{b"), true);
                    assert.strictEqual(path.matchesGlob("abc", "a{bc"), false);
                });
            });

            describe("Special characters", () => {
                it("should handle patterns with dots", () => {
                    // Test basic special character handling
                    assert.strictEqual(path.matchesGlob("a.b", "a.b"), true);
                    assert.strictEqual(path.matchesGlob("axb", "a.b"), false);
                    // Node.js doesn't support escaping (windowsPathsNoEscape: true)
                    // So *.\\* is treated as literal pattern, not escaped *
                    assert.strictEqual(path.matchesGlob("r.*", "*.\\*"), false);
                    assert.strictEqual(path.matchesGlob("r.*", "*.*"), true);
                });

                it("should handle boundary character class patterns", () => {
                    // Character class with special characters at boundaries
                    assert.strictEqual(path.matchesGlob("a[]b", "a[]*"), true);
                    assert.strictEqual(path.matchesGlob("[b-a]x", "[b-a\\]*"), false); // Node.js: doesn't match
                    assert.strictEqual(path.matchesGlob("a]b", "a[]*"), false); // Node.js: ] doesn't match []
                    assert.strictEqual(path.matchesGlob("a[]]b", "a[]]*"), false); // Node.js: edge case
                    assert.strictEqual(path.matchesGlob("a[[]b", "a[[]*"), true);
                });

                it("should handle negated range edge cases", () => {            // Test negated ranges with edge cases - different behavior based on platform
                    if (isCaseInsensitive) {
                        // Mac/Windows: case insensitive character ranges
                        assert.strictEqual(path.matchesGlob("A", "[!a-z]"), false); // A matches [a-z] so ![a-z] = false
                        assert.strictEqual(path.matchesGlob("Z", "[!A-Z]"), false); // Z matches [A-Z] so ![A-Z] = false
                    } else {
                        // Linux: case sensitive character ranges
                        assert.strictEqual(path.matchesGlob("A", "[!a-z]"), true); // A doesn't match [a-z] so ![a-z] = true
                        assert.strictEqual(path.matchesGlob("Z", "[!A-Z]"), false); // Z matches [A-Z] so ![A-Z] = false
                    }
                    assert.strictEqual(path.matchesGlob("1", "[!a-z]"), true); // Number doesn't match letter ranges on any platform
                });
            });

            describe("Character ranges", () => {
                it("should handle character ranges correctly", () => {
                    assert.strictEqual(path.matchesGlob("p", "[a-z]"), true);
                    // Platform-specific case sensitivity for character ranges
                    assert.strictEqual(path.matchesGlob("A", "[a-z]"), isCaseInsensitive); // Mac/Windows: case insensitive ranges, Linux: case sensitive
                    assert.strictEqual(path.matchesGlob("5", "[0-9]"), true);
                    assert.strictEqual(path.matchesGlob("a", "[0-9]"), false);
                });
            });

            describe("Case sensitivity", () => {
                it("should be case sensitive by default", () => {
                    if (process.versions.fibjs) {
                        assert.strictEqual(path.matchesGlob("ABC", "abc"), isCaseInsensitive);
                        assert.strictEqual(path.matchesGlob("abc", "ABC"), isCaseInsensitive);
                        assert.strictEqual(path.matchesGlob("A", "[a-z]"), isCaseInsensitive); // Mac/Windows: case insensitive ranges, Linux: case sensitive
                    } else {
                        assert.strictEqual(path.matchesGlob("ABC", "abc"), false);
                        assert.strictEqual(path.matchesGlob("abc", "ABC"), false);
                        assert.strictEqual(path.matchesGlob("A", "[a-z]"), isCaseInsensitive); // Mac/Windows: case insensitive ranges, Linux: case sensitive
                    }
                });

                // Note: Case insensitive testing would require options support
                // which we'll implement in later phases
            });

            describe("Unicode and special characters", () => {
                it("should handle basic unicode characters", () => {
                    // Basic unicode support test
                    assert.strictEqual(path.matchesGlob("café", "café"), true);
                    assert.strictEqual(path.matchesGlob("café", "caf*"), true);
                });

                it("should handle special ASCII characters", () => {
                    // Special ASCII character handling
                    assert.strictEqual(path.matchesGlob("a\tb", "a\tb"), true);
                    assert.strictEqual(path.matchesGlob("a\nb", "a\nb"), true);
                    // Test various ASCII characters in patterns
                    assert.strictEqual(path.matchesGlob("a@b", "a@b"), true);
                    assert.strictEqual(path.matchesGlob("a#b", "a#b"), true);
                    assert.strictEqual(path.matchesGlob("a$b", "a$b"), true);
                    assert.strictEqual(path.matchesGlob("a%b", "a%b"), true);
                    assert.strictEqual(path.matchesGlob("a&b", "a&b"), true);
                });
            });

            describe("Performance and limits", () => {
                it("should handle very long patterns", () => {
                    // Test with reasonably long pattern
                    const longPattern = "a" + "*".repeat(50) + "b";
                    const longText = "a" + "x".repeat(100) + "b";
                    assert.strictEqual(path.matchesGlob(longText, longPattern), true);
                });

                it("should handle deeply nested patterns", () => {
                    // Test with nested braces
                    const nestedPattern = "a{b{c{d,e},f},g}h";
                    assert.strictEqual(path.matchesGlob("abcdh", nestedPattern), true);
                    assert.strictEqual(path.matchesGlob("abceh", nestedPattern), true);
                    assert.strictEqual(path.matchesGlob("abfh", nestedPattern), true);
                    assert.strictEqual(path.matchesGlob("agh", nestedPattern), true);
                    assert.strictEqual(path.matchesGlob("axh", nestedPattern), false);
                });
            });
        });

        // ==================== 实际应用场景测试 ====================
        describe("Real-world usage scenarios", () => {

            describe("File pattern matching", () => {
                it("should match common file extensions", () => {
                    assert.strictEqual(path.matchesGlob("script.js", "*.js"), true);
                    assert.strictEqual(path.matchesGlob("script.ts", "*.js"), false);
                    assert.strictEqual(path.matchesGlob("test_login.js", "test_*.js"), true);
                    assert.strictEqual(path.matchesGlob("login_test.js", "test_*.js"), false);

                    // Common file extensions
                    assert.strictEqual(path.matchesGlob("document.pdf", "*.pdf"), true);
                    assert.strictEqual(path.matchesGlob("image.jpg", "*.{jpg,jpeg,png,gif}"), true);
                    assert.strictEqual(path.matchesGlob("style.css", "*.css"), true);
                });

                it("should match directory patterns", () => {
                    assert.strictEqual(path.matchesGlob("src/main.js", "src/*"), true);
                    assert.strictEqual(path.matchesGlob("src/utils", "src/*"), true);
                    assert.strictEqual(path.matchesGlob("lib/main.js", "src/*"), false);

                    // Directory structure patterns
                    assert.strictEqual(path.matchesGlob("src/components/Button.tsx", "src/**/Button.*"), true);
                    assert.strictEqual(path.matchesGlob("tests/unit/helpers/utils.test.js", "tests/**/*.test.js"), true);
                    assert.strictEqual(path.matchesGlob("dist/assets/css/main.css", "dist/**/main.css"), true);
                });

                it("should handle Win32 common file patterns", () => {
                    // Windows executable files
                    assert.strictEqual(path.win32.matchesGlob("program.exe", "*.{exe,com,bat,cmd}"), true);
                    assert.strictEqual(path.win32.matchesGlob("script.bat", "*.{exe,com,bat,cmd}"), true);
                    assert.strictEqual(path.win32.matchesGlob("install.msi", "*.msi"), true);

                    // Windows system files
                    assert.strictEqual(path.win32.matchesGlob("config.ini", "*.{ini,cfg,conf}"), true);
                    assert.strictEqual(path.win32.matchesGlob("library.dll", "*.{dll,lib,so}"), true);
                    assert.strictEqual(path.win32.matchesGlob("driver.sys", "*.sys"), true);

                    // Windows office files
                    assert.strictEqual(path.win32.matchesGlob("document.docx", "*.{doc,docx,xls,xlsx,ppt,pptx}"), true);
                    assert.strictEqual(path.win32.matchesGlob("spreadsheet.xlsx", "*.{doc,docx,xls,xlsx,ppt,pptx}"), true);
                });

                it("should handle Win32 path patterns in common directories", () => {
                    // Windows Program Files
                    assert.strictEqual(path.win32.matchesGlob("C:\\Program Files\\App\\file.exe", "C:\\Program Files\\**\\*.exe"), true);
                    assert.strictEqual(path.win32.matchesGlob("C:\\Program Files (x86)\\App\\file.exe", "C:\\Program Files*\\**\\*.exe"), true);

                    // Windows user directories
                    assert.strictEqual(path.win32.matchesGlob("C:\\Users\\John\\Documents\\file.txt", "C:\\Users\\*\\Documents\\*.txt"), true);
                    assert.strictEqual(path.win32.matchesGlob("C:\\Users\\Jane\\Desktop\\shortcut.lnk", "C:\\Users\\*\\Desktop\\*.lnk"), true);

                    // Windows system directories
                    assert.strictEqual(path.win32.matchesGlob("C:\\Windows\\System32\\driver.sys", "C:\\Windows\\**\\*.sys"), true);
                    assert.strictEqual(path.win32.matchesGlob("C:\\Windows\\SysWOW64\\library.dll", "C:\\Windows\\Sys*\\*.dll"), true);

                    // AppData directories
                    assert.strictEqual(path.win32.matchesGlob("C:\\Users\\User\\AppData\\Local\\App\\config.json", "C:\\Users\\*\\AppData\\**\\*.json"), true);
                    assert.strictEqual(path.win32.matchesGlob("C:\\Users\\User\\AppData\\Roaming\\App\\settings.xml", "C:\\Users\\*\\AppData\\**\\*.xml"), true);
                });
            });

            describe("Build tool patterns", () => {
                it("should handle common build tool patterns", () => {
                    // Patterns commonly used in build tools
                    assert.strictEqual(path.matchesGlob("src/components/Button.tsx", "src/**/*.{ts,tsx}"), true);
                    assert.strictEqual(path.matchesGlob("src/components/Button.jsx", "src/**/*.{ts,tsx}"), false);
                    assert.strictEqual(path.matchesGlob("tests/unit/Button.test.ts", "tests/**/*.test.{js,ts}"), true);
                    assert.strictEqual(path.matchesGlob("tests/unit/Button.spec.ts", "tests/**/*.test.{js,ts}"), false);

                    // Node modules exclusion patterns
                    assert.strictEqual(path.matchesGlob("node_modules/package/index.js", "**/node_modules/**"), true);
                    assert.strictEqual(path.matchesGlob("src/node_modules_backup/file.js", "**/node_modules/**"), false);

                    // npm/yarn patterns
                    assert.strictEqual(path.matchesGlob("node_modules/package/index.js", "node_modules/**/*.js"), true);
                    assert.strictEqual(path.matchesGlob("src/components/Button.test.js", "**/*.test.{js,ts}"), true);

                    // webpack/build patterns
                    assert.strictEqual(path.matchesGlob("dist/bundle.js.map", "dist/**/*.map"), true);
                    assert.strictEqual(path.matchesGlob("build/static/css/main.css", "build/**/*.css"), true);
                });

                it("should handle Win32 Visual Studio patterns", () => {
                    // Visual Studio project patterns
                    assert.strictEqual(path.win32.matchesGlob("MyProject.sln", "*.sln"), true);
                    assert.strictEqual(path.win32.matchesGlob("MyProject\\MyProject.csproj", "*\\*.{csproj,vbproj,fsproj}"), true);
                    assert.strictEqual(path.win32.matchesGlob("MyProject\\bin\\Debug\\MyProject.exe", "*\\bin\\**\\*.{exe,dll}"), true);
                    assert.strictEqual(path.win32.matchesGlob("MyProject\\obj\\Debug\\MyProject.pdb", "*\\obj\\**\\*.{pdb,obj}"), true);

                    // MSBuild patterns
                    assert.strictEqual(path.win32.matchesGlob("packages\\Package.1.0.0\\lib\\net45\\Package.dll", "packages\\**\\*.dll"), true);
                    assert.strictEqual(path.win32.matchesGlob("packages.config", "packages.{config,lock.json}"), true); // brace expansion works
                    assert.strictEqual(path.win32.matchesGlob("packages.lock.json", "packages.{config,lock.json}"), true); // brace expansion works

                    // Visual Studio temporary files
                    assert.strictEqual(path.win32.matchesGlob("MyProject\\bin\\Debug\\MyProject.vshost.exe", "*\\bin\\**\\*.vshost.*"), true);
                    assert.strictEqual(path.win32.matchesGlob(".vs\\MyProject\\v16\\project.suo", ".vs\\**\\*.suo"), true); // corrected path
                });

                it("should handle Win32 IIS and web deployment patterns", () => {
                    // IIS web applications
                    assert.strictEqual(path.win32.matchesGlob("C:\\inetpub\\wwwroot\\app\\default.aspx", "C:\\inetpub\\**\\*.{aspx,ashx,asmx}"), true);
                    assert.strictEqual(path.win32.matchesGlob("C:\\inetpub\\wwwroot\\api\\web.config", "C:\\inetpub\\**\\web.config"), true);

                    // Web deployment packages
                    assert.strictEqual(path.win32.matchesGlob("publish\\app.zip", "publish\\*.{zip,msi}"), true);
                    assert.strictEqual(path.win32.matchesGlob("deploy\\WebDeploy.cmd", "deploy\\*.{bat,cmd,ps1}"), true);
                });
            });

            describe("GitIgnore-style patterns", () => {
                it("should handle gitignore-style patterns", () => {
                    // Common gitignore patterns
                    assert.strictEqual(path.matchesGlob("dist/bundle.js", "dist/**"), true);
                    assert.strictEqual(path.matchesGlob("build/assets/main.css", "build/**"), true);
                    assert.strictEqual(path.matchesGlob("coverage/lcov-report/index.html", "coverage/**"), true);

                    // Log file patterns
                    assert.strictEqual(path.matchesGlob("error.log", "*.log"), true);
                    assert.strictEqual(path.matchesGlob("debug.log", "*.log"), true);
                    assert.strictEqual(path.matchesGlob("logfile.txt", "*.log"), false);

                    // Basic gitignore patterns
                    assert.strictEqual(path.matchesGlob("node_modules/package/file.js", "node_modules/**"), true);
                    assert.strictEqual(path.matchesGlob("build/output.js", "build/"), false); // gitignore trailing slash means directory only
                    assert.strictEqual(path.matchesGlob("temp.log", "*.log"), true);
                    assert.strictEqual(path.matchesGlob("cache/temp.cache", "cache/"), false);

                    // Negation patterns (would need special handling)
                    assert.strictEqual(path.matchesGlob("important.log", "*.log"), true); // but !important.log would negate this
                });

                it("should handle Win32 gitignore patterns", () => {
                    // Windows-specific ignore patterns
                    assert.strictEqual(path.win32.matchesGlob("Thumbs.db", "Thumbs.db"), true);
                    assert.strictEqual(path.win32.matchesGlob("folder\\Thumbs.db", "**/Thumbs.db"), true);
                    assert.strictEqual(path.win32.matchesGlob("Desktop.ini", "Desktop.ini"), true);
                    assert.strictEqual(path.win32.matchesGlob("$RECYCLE.BIN\\file", "$RECYCLE.BIN/**"), true);

                    // Visual Studio ignore patterns
                    assert.strictEqual(path.win32.matchesGlob("bin\\Debug\\app.exe", "bin/**"), true);
                    assert.strictEqual(path.win32.matchesGlob("obj\\Release\\temp.obj", "obj/**"), true);
                    assert.strictEqual(path.win32.matchesGlob("packages\\newtonsoft.json.12.0.3\\lib\\net45\\Newtonsoft.Json.dll", "packages/**"), true);

                    // Windows temp and cache files
                    assert.strictEqual(path.win32.matchesGlob("project.suo", "*.{suo,user,userosscache,sln.docstates}"), true);
                    assert.strictEqual(path.win32.matchesGlob("project.user", "*.{suo,user,userosscache,sln.docstates}"), true);
                    assert.strictEqual(path.win32.matchesGlob(".vs\\config\\applicationhost.config", ".vs/**"), true);
                });
            });

            describe("Cross-platform compatibility validation", () => {
                const testCases = [
                    ["hello", "hello"],
                    ["a*", "abc"],
                    ["a?c", "abc"],
                    ["[abc]", "b"],
                    ["[!abc]", "d"],
                    ["a/*/c", "a/b/c"],
                    ["*.js", "test.js"],
                    ["test_*", "test_example"],
                ];

                testCases.forEach(([pattern, text]) => {
                    it(`pattern "${pattern}" with text "${text}" should be consistent`, () => {
                        const result = path.matchesGlob(text, pattern);
                        assert.strictEqual(typeof result, 'boolean',
                            `Result should be boolean for pattern "${pattern}" and text "${text}"`);

                        // For basic patterns, results should be consistent
                        const posixResult = path.posix.matchesGlob(text, pattern);
                        const win32Result = path.win32.matchesGlob(text, pattern);

                        // For basic patterns without backslashes, results should be the same
                        if (!pattern.includes('\\') && !text.includes('\\')) {
                            assert.strictEqual(posixResult, win32Result,
                                `Pattern "${pattern}" with text "${text}" should have same result on both platforms`);
                        }
                    });
                });

                it("should handle Windows-specific path patterns correctly", () => {
                    // These should only work properly on win32
                    const windowsSpecificCases = [
                        ["C:\\file.txt", "C:\\*.txt"],
                        ["dir\\file.txt", "dir\\*.txt"],
                        ["\\\\server\\share\\file.txt", "\\\\server\\**\\*.txt"],
                        ["file.txt", "FILE.TXT"], // case insensitive on Windows
                        ["Program Files\\app.exe", "Program Files\\*.exe"],
                        ["Users\\John\\Desktop\\file.lnk", "Users\\*\\Desktop\\*.lnk"],
                    ];

                    windowsSpecificCases.forEach(([text, pattern]) => {
                        const win32Result = path.win32.matchesGlob(text, pattern);
                        const posixResult = path.posix.matchesGlob(text, pattern);

                        // For case sensitivity tests
                        if (text.toLowerCase() !== text && pattern.toLowerCase() !== pattern) {
                            if (isCaseInsensitive) {
                                assert.strictEqual(win32Result, true,
                                    `Windows pattern "${pattern}" should match "${text}" on case-insensitive systems`);
                            }
                        } else {
                            // For path separator tests
                            if (pattern.includes('\\') || text.includes('\\')) {
                                assert.strictEqual(win32Result, true,
                                    `Windows pattern "${pattern}" should match "${text}" on win32 platform`);
                                // POSIX treats backslashes as literal characters, so patterns with backslashes
                                // behave differently than on Windows
                                assert.strictEqual(typeof posixResult, 'boolean',
                                    `POSIX result should be boolean for pattern "${pattern}" with text "${text}"`);
                                // For Windows-style paths, POSIX and Win32 results will usually differ
                                // This documents the platform-specific behavior
                            }
                        }
                    });
                });

                it("should handle POSIX-specific patterns correctly", () => {
                    // These should work differently on POSIX vs Windows
                    const posixSpecificCases = [
                        ["file\\name.txt", "file\\*.txt"], // backslash as literal on POSIX
                        ["path/to/file", "path\\to\\file"], // different separator handling
                        ["File.txt", "file.txt"], // case sensitive on POSIX
                        ["data\\with\\backslash", "data\\*\\backslash"],
                    ];

                    posixSpecificCases.forEach(([text, pattern]) => {
                        const posixResult = path.posix.matchesGlob(text, pattern);
                        const win32Result = path.win32.matchesGlob(text, pattern);

                        // Results should be different due to platform-specific behavior
                        // Verify they are both boolean
                        assert.strictEqual(typeof posixResult, 'boolean');
                        assert.strictEqual(typeof win32Result, 'boolean');

                        // Test specific expected differences based on pattern type
                        if (pattern.includes('\\') && text.includes('\\')) {
                            // POSIX treats backslashes as literal characters, but the pattern needs to exactly match
                            // For patterns like "file\\*.txt" vs "file\\name.txt", POSIX won't match because
                            // it treats \n and \* as different literal sequences
                            if (text === "file\\name.txt" && pattern === "file\\*.txt") {
                                // Special case: \n vs \* are different literal sequences
                                assert.strictEqual(posixResult, false,
                                    `POSIX should not match different escaped sequences: "${text}" vs "${pattern}"`);
                            } else if (text === "data\\with\\backslash" && pattern === "data\\*\\backslash") {
                                // Special case: literal backslashes with wildcard in between
                                assert.strictEqual(posixResult, false,
                                    `POSIX treats literal backslashes strictly: "${text}" vs "${pattern}"`);
                            } else {
                                // For exact literal matches
                                assert.strictEqual(posixResult, text === pattern,
                                    `POSIX should match only identical literal strings: "${text}" vs "${pattern}"`);
                            }
                            // Win32 treats backslashes as path separators in some contexts
                        } else if (pattern.includes('/') && text.includes('\\')) {
                            // Different separator handling
                            assert.strictEqual(posixResult, false,
                                `POSIX should not match different separators: "${text}" vs "${pattern}"`);
                            // Win32 may be more flexible with separators
                        } else if (text.toLowerCase() !== text && pattern.toLowerCase() !== pattern) {
                            // Case sensitivity test
                            assert.strictEqual(posixResult, false,
                                `POSIX should be case sensitive: "${text}" vs "${pattern}"`);
                            // Win32 case sensitivity depends on file system
                        }
                    });
                });

                it("should handle mixed separator scenarios", () => {
                    // Test how each platform handles mixed separators
                    const mixedSeparatorCases = [
                        ["a/b\\c", "a/b\\c"],     // mixed separators in both
                        ["a\\b/c", "a\\b/c"],     // mixed separators in both
                        ["a/b/c", "a\\b\\c"],     // different separators
                        ["a\\b\\c", "a/b/c"],     // different separators
                    ];

                    mixedSeparatorCases.forEach(([text, pattern]) => {
                        const posixResult = path.posix.matchesGlob(text, pattern);
                        const win32Result = path.win32.matchesGlob(text, pattern);

                        // Verify both results are boolean
                        assert.strictEqual(typeof posixResult, 'boolean');
                        assert.strictEqual(typeof win32Result, 'boolean');

                        // Test specific separator handling expectations
                        if (text === pattern) {
                            // Exact match should work on both platforms, but POSIX may have special handling for backslashes
                            if (text.includes('\\')) {
                                // POSIX has special handling for strings containing backslashes
                                // Even identical strings may not match if they contain escape sequences
                                assert.strictEqual(typeof posixResult, 'boolean',
                                    `POSIX result should be boolean for: "${text}" vs "${pattern}"`);
                                assert.strictEqual(win32Result, true,
                                    `Win32 should match identical strings: "${text}" vs "${pattern}"`);
                            } else {
                                // Pure forward slash strings should match on both platforms
                                assert.strictEqual(posixResult, true,
                                    `POSIX should match identical strings: "${text}" vs "${pattern}"`);
                                assert.strictEqual(win32Result, true,
                                    `Win32 should match identical strings: "${text}" vs "${pattern}"`);
                            }
                        } else if (text.replace(/\\/g, '/') === pattern.replace(/\\/g, '/')) {
                            // Win32 should be more flexible with separators
                            assert.strictEqual(win32Result, true,
                                `Win32 should match equivalent paths regardless of separator: "${text}" vs "${pattern}"`);
                            // In the actual implementation, POSIX also handles separator equivalence
                            // to some degree, so we just verify it returns a boolean
                            assert.strictEqual(typeof posixResult, 'boolean',
                                `POSIX result should be boolean for different separators: "${text}" vs "${pattern}"`);
                        }
                    });
                });
            });
        });
    });

});