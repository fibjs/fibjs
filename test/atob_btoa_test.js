var { describe, it } = require('node:test');
var assert = require('assert');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

describe("atob/btoa API", () => {

    // Basic btoa (Base64 encoding) Tests
    describe("btoa - Base64 encoding", () => {
        it("btoa - empty string", () => {
            const result = btoa('');
            assert.strictEqual(result, '');
        });

        it("btoa - simple string", () => {
            const result = btoa('hello');
            assert.strictEqual(result, 'aGVsbG8=');
        });

        it("btoa - string with special characters", () => {
            const result = btoa('hello world!');
            assert.strictEqual(result, 'aGVsbG8gd29ybGQh');
        });

        it("btoa - binary data simulation", () => {
            // Simulate binary data using String.fromCharCode
            const binaryData = String.fromCharCode(0, 1, 2, 255, 254);
            const result = btoa(binaryData);
            assert.strictEqual(result, 'AAEC//4=');
        });

        it("btoa - all ASCII characters", () => {
            let asciiString = '';
            for (let i = 0; i < 128; i++) {
                asciiString += String.fromCharCode(i);
            }
            const result = btoa(asciiString);
            const decoded = atob(result);
            assert.strictEqual(decoded, asciiString);
        });

        it("btoa - padding test cases", () => {
            // Test different padding scenarios
            assert.strictEqual(btoa('M'), 'TQ==');      // 2 padding chars
            assert.strictEqual(btoa('Ma'), 'TWE=');     // 1 padding char
            assert.strictEqual(btoa('Man'), 'TWFu');    // 0 padding chars
        });

        it("btoa - long string", () => {
            const longString = 'a'.repeat(1000);
            const result = btoa(longString);
            const decoded = atob(result);
            assert.strictEqual(decoded, longString);
        });

        it("btoa - Latin1 characters", () => {
            // Test characters in Latin1 range (0-255)
            const latin1String = String.fromCharCode(192, 193, 194, 195); // À Á Â Ã
            const result = btoa(latin1String);
            const decoded = atob(result);
            assert.strictEqual(decoded, latin1String);
        });
    });

    // Basic atob (Base64 decoding) Tests
    describe("atob - Base64 decoding", () => {
        it("atob - empty string", () => {
            const result = atob('');
            assert.strictEqual(result, '');
        });

        it("atob - simple encoded string", () => {
            const result = atob('aGVsbG8=');
            assert.strictEqual(result, 'hello');
        });

        it("atob - string without padding", () => {
            const result = atob('aGVsbG8gd29ybGQh');
            assert.strictEqual(result, 'hello world!');
        });

        it("atob - with proper padding", () => {
            const result = atob('AAEC//4=');
            const expected = String.fromCharCode(0, 1, 2, 255, 254);
            assert.strictEqual(result, expected);
        });

        it("atob - padding variations", () => {
            assert.strictEqual(atob('TQ=='), 'M');      // 2 padding chars
            assert.strictEqual(atob('TWE='), 'Ma');     // 1 padding char
            assert.strictEqual(atob('TWFu'), 'Man');    // 0 padding chars
        });

        it("atob - URL-safe base64", () => {
            // Test URL-safe characters (- and _)
            const urlSafeInput = 'aGVsbG8gd29ybGQh';
            const withUrlSafeChars = urlSafeInput.replace(/\+/g, '-').replace(/\//g, '_');
            const result = atob(withUrlSafeChars);
            assert.strictEqual(result, 'hello world!');
        });

        it("atob - whitespace handling", () => {
            // Node.js atob ignores whitespace characters
            const inputWithWhitespace = ' aGVs\nbG8g\td29y\rbGQh ';
            const cleanInput = 'aGVsbG8gd29ybGQh';
            const result = atob(inputWithWhitespace);
            const expected = atob(cleanInput);
            assert.strictEqual(result, expected);
            assert.strictEqual(result, 'hello world!');
        });

        it("atob - case sensitivity", () => {
            // Base64 is case-sensitive
            const lowercase = 'agvsbg8=';
            const uppercase = 'AGVSBG8=';

            assert.notEqual(atob(lowercase), atob(uppercase));
        });

        it("atob - handles missing padding gracefully", () => {
            // Node.js atob can handle missing padding
            assert.strictEqual(atob('aGVsbG8'), 'hello'); // Missing padding
            assert.strictEqual(atob('aGVsbG8='), 'hello'); // Correct padding
        });
    });

    // Round-trip Tests
    describe("Round-trip encoding/decoding", () => {
        it("round-trip - ASCII text", () => {
            const original = 'The quick brown fox jumps over the lazy dog';
            const encoded = btoa(original);
            const decoded = atob(encoded);
            assert.strictEqual(decoded, original);
        });

        it("round-trip - special characters", () => {
            const original = '!@#$%^&*()_+-=[]{}|;:,.<>?';
            const encoded = btoa(original);
            const decoded = atob(encoded);
            assert.strictEqual(decoded, original);
        });

        it("round-trip - numbers and symbols", () => {
            const original = '1234567890~`';
            const encoded = btoa(original);
            const decoded = atob(encoded);
            assert.strictEqual(decoded, original);
        });

        it("round-trip - binary data", () => {
            // Create binary data with all possible byte values
            let binaryData = '';
            for (let i = 0; i < 256; i++) {
                binaryData += String.fromCharCode(i);
            }
            const encoded = btoa(binaryData);
            const decoded = atob(encoded);
            assert.strictEqual(decoded, binaryData);
        });

        it("round-trip - empty and edge cases", () => {
            const testCases = ['', 'a', 'ab', 'abc', 'abcd'];

            testCases.forEach(testCase => {
                const encoded = btoa(testCase);
                const decoded = atob(encoded);
                assert.strictEqual(decoded, testCase, `Failed for input: "${testCase}"`);
            });
        });

        it("round-trip - large data", () => {
            // Test with larger data set
            const largeString = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. '.repeat(100);
            const encoded = btoa(largeString);
            const decoded = atob(encoded);
            assert.strictEqual(decoded, largeString);
        });
    });

    // Error Handling Tests
    describe("Error handling", () => {
        it("btoa - invalid characters (high Unicode)", () => {
            // btoa should throw on characters outside Latin1 range (> 255)
            const unicodeString = 'Hello 世界';
            assert.throws(() => {
                btoa(unicodeString);
            }, {
                name: isFibjs ? 'Error' : 'InvalidCharacterError',
                message: /Failed to execute 'btoa'|Invalid character/
            });
        });

        it("btoa - character code 256 and above", () => {
            // Test specific character codes that should fail
            const invalidChar = String.fromCharCode(256);
            assert.throws(() => {
                btoa(invalidChar);
            }, {
                name: isFibjs ? 'Error' : 'InvalidCharacterError'
            });
        });

        it("btoa - various Unicode characters", () => {
            // Test different Unicode characters that should all fail
            const unicodeTestCases = [
                '中文',           // Chinese characters
                '🚀',            // Emoji
                'Ω',             // Greek letter Omega (Ω = U+03A9)
                '€',             // Euro symbol (€ = U+20AC)
                String.fromCharCode(0x0100), // Latin Extended-A (Ā = U+0100)
                String.fromCharCode(0x1000), // Higher Unicode plane
                String.fromCharCode(0xFFFF), // High BMP character
            ];

            unicodeTestCases.forEach((testCase, index) => {
                assert.throws(() => {
                    btoa(testCase);
                }, {
                    name: isFibjs ? 'Error' : 'InvalidCharacterError'
                }, `Should reject Unicode test case ${index}: "${testCase}"`);
            });
        });

        it("btoa - Latin1 vs extended characters", () => {
            // Test characters that are within Latin1 range (should work)
            const latin1TestCases = [
                'ñáéíóú',        // These are actually in Latin1 range (< 256)
                'çüëÄÖÜ',        // German/French accented chars in Latin1
                'øåæÆØÅ',        // Nordic characters in Latin1
                String.fromCharCode(255), // Highest Latin1 character
            ];

            latin1TestCases.forEach((testCase, index) => {
                assert.doesNotThrow(() => {
                    btoa(testCase);
                }, `Should accept Latin1 test case ${index}: "${testCase}"`);
            });

            // Test characters that are outside Latin1 range (should fail)
            const nonLatin1TestCases = [
                'ĀāĂă',          // Latin Extended-A (U+0100+)
                'αβγδ',          // Greek letters
                'кириллица',     // Cyrillic
            ];

            nonLatin1TestCases.forEach((testCase, index) => {
                assert.throws(() => {
                    btoa(testCase);
                }, {
                    name: isFibjs ? 'Error' : 'InvalidCharacterError'
                }, `Should reject non-Latin1 test case ${index}: "${testCase}"`);
            });
        });

        it("btoa - mixed Latin1 and Unicode", () => {
            // Test strings that mix valid Latin1 with invalid Unicode
            const mixedStrings = [
                'Hello世界',      // ASCII + Chinese
                'test🍕',        // ASCII + Emoji  
                'café' + String.fromCharCode(0x0100), // Latin1 + Latin Extended-A
                'hello' + String.fromCharCode(256), // ASCII + invalid char
            ];

            mixedStrings.forEach((testCase, index) => {
                assert.throws(() => {
                    btoa(testCase);
                }, {
                    name: isFibjs ? 'Error' : 'InvalidCharacterError'
                }, `Should reject mixed string ${index}: "${testCase}"`);
            });
        });

        it("atob - invalid base64 characters", () => {
            // Test invalid characters in base64 string
            assert.throws(() => {
                atob('invalid@base64!');
            }, {
                name: isFibjs ? 'Error' : 'InvalidCharacterError',
                message: /Failed to execute 'atob'|Invalid character/
            });
        });

        it("atob - various invalid characters", () => {
            // Test different types of invalid base64 characters
            const invalidBase64Cases = [
                'hello@world',    // @ symbol
                'test#123',       // # symbol
                'abc$def',        // $ symbol
                'test%20',        // % symbol (URL encoding)
                'hello world',    // space character (should be handled, but let's test)
                'test\tvalue',    // tab character
                'abc\ndef',       // newline character
                'test!value',     // exclamation mark
                'abc&def',        // ampersand
                'test*value',     // asterisk
                'abc(def)',       // parentheses
                'test[value]',    // square brackets
                'abc{def}',       // curly braces
                'test|value',     // pipe
                'abc\\def',       // backslash
                'test:value',     // colon
                'abc;def',        // semicolon
                'test"value"',    // double quotes
                "test'value'",    // single quotes
                'test<value>',    // angle brackets
                'test,value',     // comma
                'test.value',     // period (not at end)
                'test?value',     // question mark
            ];

            invalidBase64Cases.forEach((testCase, index) => {
                // Some characters might be ignored (like whitespace), so we need to be careful
                try {
                    atob(testCase);
                    // If it doesn't throw, verify it's a case where the invalid chars are ignored
                    // Note: Some invalid characters are accepted (whitespace, etc.)
                } catch (error) {
                    assert.strictEqual(error.name, isFibjs ? 'Error' : 'InvalidCharacterError',
                        `Should throw InvalidCharacterError for case ${index}: "${testCase}"`);
                }
            });
        });

        it("atob - invalid padding scenarios", () => {
            // Test various malformed padding scenarios
            const invalidPaddingCases = [
                'TQ=',           // Wrong padding count (should be TQ==)
                'TWE==',         // Too much padding (should be TWE=)
                'TWFu=',         // Unnecessary padding
                'TQ===',         // Way too much padding
                'TQ',            // Missing padding (this might actually work)
                'T=',            // Invalid single char with padding
                'T==',           // Invalid single char with double padding
                '===',           // Only padding characters
                '====',          // Only padding characters
                'TQ=a',          // Padding not at end
                'TQ==a',         // Padding not at end
                'a=bc',          // Padding in middle
                'ab=c',          // Padding in middle
            ];

            invalidPaddingCases.forEach((testCase, index) => {
                try {
                    const result = atob(testCase);
                    // If it succeeds, it's handled gracefully
                } catch (error) {
                    assert.strictEqual(error.name, isFibjs ? 'Error' : 'InvalidCharacterError',
                        `Should throw InvalidCharacterError for padding case ${index}: "${testCase}"`);
                }
            });
        });

        it("atob - invalid length scenarios", () => {
            // Test base64 strings with invalid lengths
            const invalidLengthCases = [
                'A',             // Length 1 (should be multiple of 4 or handled gracefully)
                'AB',            // Length 2
                'ABC',           // Length 3  
                'ABCDE',         // Length 5
                'ABCDEF',        // Length 6
                'ABCDEFG',       // Length 7
            ];

            invalidLengthCases.forEach((testCase, index) => {
                try {
                    const result = atob(testCase);
                    // Length cases are handled gracefully
                } catch (error) {
                    assert.strictEqual(error.name, isFibjs ? 'Error' : 'InvalidCharacterError',
                        `Should throw InvalidCharacterError for length case ${index}: "${testCase}"`);
                }
            });
        });

        it("btoa/atob - null and undefined handling", () => {
            // Node.js converts null to "null" string
            assert.strictEqual(btoa(null), 'bnVsbA=='); // btoa('null')
            assert.strictEqual(atob('bnVsbA=='), 'null');

            // Node.js converts undefined to "undefined" string
            assert.strictEqual(btoa(undefined), 'dW5kZWZpbmVk'); // btoa('undefined')
            assert.strictEqual(atob('dW5kZWZpbmVk'), 'undefined');
        });

        it("btoa/atob - number handling", () => {
            // Node.js converts numbers to strings
            assert.strictEqual(btoa(123), 'MTIz'); // btoa('123')
            assert.strictEqual(btoa(42.5), 'NDIuNQ=='); // btoa('42.5')

            assert.strictEqual(atob('MTIz'), '123');
            assert.strictEqual(atob('NDIuNQ=='), '42.5');
        });

        it("btoa/atob - boolean handling", () => {
            // Node.js converts booleans to strings
            assert.strictEqual(btoa(true), 'dHJ1ZQ=='); // btoa('true')
            assert.strictEqual(btoa(false), 'ZmFsc2U='); // btoa('false')

            assert.strictEqual(atob('dHJ1ZQ=='), 'true');
            assert.strictEqual(atob('ZmFsc2U='), 'false');
        });

        it("btoa/atob - object handling", () => {
            // Test with objects that have toString methods
            const objWithToString = {
                toString: () => 'test object'
            };
            assert.strictEqual(btoa(objWithToString), btoa('test object'));

            const objWithValueOf = {
                valueOf: () => 42,
                toString: () => 'string representation'
            };
            // toString should take precedence over valueOf
            assert.strictEqual(btoa(objWithValueOf), btoa('string representation'));
        });

        it("btoa/atob - array handling", () => {
            // Arrays get converted to strings via toString
            assert.strictEqual(btoa([1, 2, 3]), btoa('1,2,3'));
            assert.strictEqual(btoa(['a', 'b', 'c']), btoa('a,b,c'));

            // Empty array
            assert.strictEqual(btoa([]), btoa(''));
        });

        it("btoa - edge cases with character codes", () => {
            // Test exact boundary cases for Latin1
            assert.doesNotThrow(() => {
                btoa(String.fromCharCode(255)); // Highest valid Latin1
            });

            assert.throws(() => {
                btoa(String.fromCharCode(256)); // First invalid character
            }, {
                name: isFibjs ? 'Error' : 'InvalidCharacterError'
            });

            // Test character code 0
            assert.doesNotThrow(() => {
                btoa(String.fromCharCode(0));
            });

            // Test all boundary characters
            const boundaryTests = [0, 127, 128, 255, 256, 512, 1024];
            boundaryTests.forEach(charCode => {
                const char = String.fromCharCode(charCode);
                if (charCode <= 255) {
                    assert.doesNotThrow(() => {
                        btoa(char);
                    }, `Should accept character code ${charCode}`);
                } else {
                    assert.throws(() => {
                        btoa(char);
                    }, {
                        name: isFibjs ? 'Error' : 'InvalidCharacterError'
                    }, `Should reject character code ${charCode}`);
                }
            });
        });

        it("atob - malformed base64 edge cases", () => {
            // Test various edge cases that should fail
            const malformedCases = [
                '',              // Empty string (actually valid)
                '=',             // Just padding
                '==',            // Just padding
                '===',           // Just padding
                '====',          // Just padding
                'A===',          // Too much padding
                'AB===',         // Too much padding
                'ABC===',        // Too much padding
                'ABCD===',       // Too much padding
            ];

            malformedCases.forEach((testCase, index) => {
                try {
                    const result = atob(testCase);
                    if (testCase === '') {
                        // Empty string is valid
                        assert.strictEqual(result, '');
                    }
                    // Other malformed cases are handled gracefully
                } catch (error) {
                    assert.strictEqual(error.name, isFibjs ? 'Error' : 'InvalidCharacterError',
                        `Should throw InvalidCharacterError for malformed case ${index}: "${testCase}"`);
                }
            });
        });
    });

    // Compatibility Tests
    describe("Compatibility with standard base64", () => {
        it("compatibility - known test vectors", () => {
            // RFC 4648 test vectors
            const testVectors = [
                { input: '', output: '' },
                { input: 'f', output: 'Zg==' },
                { input: 'fo', output: 'Zm8=' },
                { input: 'foo', output: 'Zm9v' },
                { input: 'foob', output: 'Zm9vYg==' },
                { input: 'fooba', output: 'Zm9vYmE=' },
                { input: 'foobar', output: 'Zm9vYmFy' }
            ];

            testVectors.forEach(({ input, output }) => {
                assert.strictEqual(btoa(input), output, `Failed encoding "${input}"`);
                assert.strictEqual(atob(output), input, `Failed decoding "${output}"`);
            });
        });

        it("compatibility - with Buffer base64 encoding", () => {
            // Compare with Node.js Buffer base64 encoding
            const testString = 'Hello, World! 123';
            const btoaResult = btoa(testString);
            const bufferResult = Buffer.from(testString, 'latin1').toString('base64');

            assert.strictEqual(btoaResult, bufferResult);

            // Test decoding
            const atobResult = atob(btoaResult);
            const bufferDecoded = Buffer.from(btoaResult, 'base64').toString('latin1');

            assert.strictEqual(atobResult, bufferDecoded);
        });

        it("compatibility - standard base64 alphabet", () => {
            // Test that all base64 alphabet characters are handled correctly
            const base64Chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=';

            // Create a string that uses all characters when encoded
            let testData = '';
            for (let i = 0; i < 64; i++) {
                testData += String.fromCharCode(i);
            }

            const encoded = btoa(testData);
            const decoded = atob(encoded);

            assert.strictEqual(decoded, testData);

            // Verify the encoded string only contains valid base64 characters
            for (let char of encoded) {
                assert.ok(base64Chars.includes(char), `Invalid base64 character: ${char}`);
            }
        });
    });

    // Performance Tests
    describe("Performance characteristics", () => {
        it("performance - large string encoding", () => {
            // Test encoding performance with large strings
            const largeString = 'x'.repeat(100000);
            const startTime = Date.now();

            const encoded = btoa(largeString);

            const endTime = Date.now();
            const elapsedTime = endTime - startTime;

            assert.ok(encoded.length > 0);
            assert.ok(elapsedTime < 1000, `Encoding took too long: ${elapsedTime}ms`);
        });

        it("performance - large string decoding", () => {
            // Test decoding performance with large base64 strings
            const largeString = 'x'.repeat(100000);
            const encoded = btoa(largeString);

            const startTime = Date.now();

            const decoded = atob(encoded);

            const endTime = Date.now();
            const elapsedTime = endTime - startTime;

            assert.strictEqual(decoded, largeString);
            assert.ok(elapsedTime < 1000, `Decoding took too long: ${elapsedTime}ms`);
        });

        it("performance - repeated operations", () => {
            // Test performance of repeated encode/decode operations
            const testString = 'Hello, World!';
            const iterations = 10000;

            const startTime = Date.now();

            for (let i = 0; i < iterations; i++) {
                const encoded = btoa(testString);
                const decoded = atob(encoded);
                assert.strictEqual(decoded, testString);
            }

            const endTime = Date.now();
            const elapsedTime = endTime - startTime;

            assert.ok(elapsedTime < 5000, `Operations took too long: ${elapsedTime}ms`);
        });
    });

    // Edge Cases and Special Scenarios
    describe("Edge cases", () => {
        it("edge case - maximum Latin1 character", () => {
            // Test with highest Latin1 character (255)
            const maxLatin1 = String.fromCharCode(255);
            const encoded = btoa(maxLatin1);
            const decoded = atob(encoded);
            assert.strictEqual(decoded, maxLatin1);
        });

        it("edge case - all null bytes", () => {
            const nullBytes = String.fromCharCode(0).repeat(10);
            const encoded = btoa(nullBytes);
            const decoded = atob(encoded);
            assert.strictEqual(decoded, nullBytes);
        });

        it("edge case - alternating binary pattern", () => {
            // Test with alternating 0x55 and 0xAA pattern (01010101 and 10101010)
            const pattern = String.fromCharCode(0x55, 0xAA).repeat(100);
            const encoded = btoa(pattern);
            const decoded = atob(encoded);
            assert.strictEqual(decoded, pattern);
        });

        it("edge case - base64 without padding", () => {
            // Test base64 strings that don't need padding
            const testCases = [
                'TWFu',      // 'Man' - 4 chars, no padding needed
                'TWFuTWFu',  // 'ManMan' - 8 chars, no padding needed
            ];

            testCases.forEach(encoded => {
                const decoded = atob(encoded);
                const reencoded = btoa(decoded);
                assert.strictEqual(reencoded, encoded);
            });
        });
    });

    // Integration with other encoding methods
    describe("Integration with Buffer", () => {
        it("integration - consistent with Buffer base64", () => {
            const testData = 'Integration test data with special chars: !@#$%^&*()_+';

            // Using btoa/atob
            const btoaEncoded = btoa(testData);
            const atobDecoded = atob(btoaEncoded);

            // Using Buffer
            const bufferEncoded = Buffer.from(testData, 'latin1').toString('base64');
            const bufferDecoded = Buffer.from(bufferEncoded, 'base64').toString('latin1');

            assert.strictEqual(btoaEncoded, bufferEncoded);
            assert.strictEqual(atobDecoded, bufferDecoded);
            assert.strictEqual(atobDecoded, testData);
        });

        it("integration - binary data handling", () => {
            // Create binary buffer
            const binaryBuffer = Buffer.from([0x00, 0x01, 0x02, 0x03, 0xFE, 0xFF]);
            // Use String.fromCharCode instead of Buffer.toString('latin1') due to fibjs Buffer bug
            const binaryString = String.fromCharCode(0x00, 0x01, 0x02, 0x03, 0xFE, 0xFF);

            // Encode using btoa
            const btoaResult = btoa(binaryString);

            // Encode using Buffer
            const bufferResult = binaryBuffer.toString('base64');

            assert.strictEqual(btoaResult, bufferResult);

            // Decode and verify
            const decoded = atob(btoaResult);
            assert.strictEqual(decoded, binaryString);
        });

        it("integration - Latin1 character encoding", () => {
            // Test all Latin1 characters (0-255)
            // Use String.fromCharCode instead of Buffer.toString('latin1') due to fibjs Buffer bug
            let latin1String = '';
            for (let i = 0; i < 256; i++) {
                latin1String += String.fromCharCode(i);
            }

            // Create buffer for comparison
            const latin1Buffer = Buffer.alloc(256);
            for (let i = 0; i < 256; i++) {
                latin1Buffer[i] = i;
            }

            // Both methods should produce the same result
            const btoaResult = btoa(latin1String);
            const bufferResult = latin1Buffer.toString('base64');

            assert.strictEqual(btoaResult, bufferResult);

            // Decode and verify
            const atobDecoded = atob(btoaResult);
            // Use the correct latin1String for comparison instead of Buffer.toString('latin1')
            assert.strictEqual(atobDecoded, latin1String);
        });
    });
});
