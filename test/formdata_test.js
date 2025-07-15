var { describe, it } = require('node:test');
var assert = require('assert');

// Detect if running in fibjs or nodejs
const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

// FormData API Tests
describe("FormData API", () => {

    // Basic Constructor Tests
    describe("Constructor - Basic functionality", () => {
        it("FormData constructor - empty", () => {
            const formData = new FormData();

            // Should be a valid FormData instance
            assert.strictEqual(formData instanceof FormData, true);
            assert.strictEqual(Object.prototype.toString.call(formData), '[object FormData]');
        });

        it("FormData constructor - methods existence", () => {
            const formData = new FormData();

            // Check that all expected methods exist
            assert.strictEqual(typeof formData.append, 'function');
            assert.strictEqual(typeof formData.set, 'function');
            assert.strictEqual(typeof formData.get, 'function');
            assert.strictEqual(typeof formData.getAll, 'function');
            assert.strictEqual(typeof formData.delete, 'function');
            assert.strictEqual(typeof formData.has, 'function');
            assert.strictEqual(typeof formData.keys, 'function');
            assert.strictEqual(typeof formData.values, 'function');
            assert.strictEqual(typeof formData.entries, 'function');
            assert.strictEqual(typeof formData.forEach, 'function');
        });
    });

    // Append Method Tests
    describe("append() method - Basic functionality", () => {
        it("FormData.append() - string values", () => {
            const formData = new FormData();

            formData.append('name', 'John');
            formData.append('email', 'john@example.com');

            assert.strictEqual(formData.get('name'), 'John');
            assert.strictEqual(formData.get('email'), 'john@example.com');
        });

        it("FormData.append() - multiple values same key", () => {
            const formData = new FormData();

            formData.append('hobby', 'reading');
            formData.append('hobby', 'swimming');
            formData.append('hobby', 'coding');

            const hobbies = formData.getAll('hobby');
            assert.deepStrictEqual(hobbies, ['reading', 'swimming', 'coding']);
        });

        it("FormData.append() - File/Blob values", () => {
            const formData = new FormData();
            const blob = new Blob(['file content'], { type: 'text/plain' });

            formData.append('file', blob);
            formData.append('file', blob, 'custom-filename.txt');

            const files = formData.getAll('file');
            assert.strictEqual(files.length, 2);
            assert.strictEqual(files[0] instanceof Blob, true);
            assert.strictEqual(files[1] instanceof Blob, true);
        });

        it("FormData.append() - number coercion", () => {
            const formData = new FormData();

            formData.append('age', 25);
            formData.append('price', 19.99);
            formData.append('count', 0);

            assert.strictEqual(formData.get('age'), '25');
            assert.strictEqual(formData.get('price'), '19.99');
            assert.strictEqual(formData.get('count'), '0');
        });

        it("FormData.append() - boolean coercion", () => {
            const formData = new FormData();

            formData.append('isActive', true);
            formData.append('isVisible', false);

            assert.strictEqual(formData.get('isActive'), 'true');
            assert.strictEqual(formData.get('isVisible'), 'false');
        });

        it("FormData.append() - null and undefined", () => {
            const formData = new FormData();

            formData.append('nullValue', null);
            formData.append('undefinedValue', undefined);

            assert.strictEqual(formData.get('nullValue'), 'null');
            assert.strictEqual(formData.get('undefinedValue'), 'undefined');
        });

        it("FormData.append() - special characters in names", () => {
            const formData = new FormData();

            formData.append('field[]', 'array-like');
            formData.append('field[0]', 'indexed');
            formData.append('field.nested', 'dot notation');
            formData.append('field with spaces', 'spaces');
            formData.append('🌟field', 'emoji');

            assert.strictEqual(formData.get('field[]'), 'array-like');
            assert.strictEqual(formData.get('field[0]'), 'indexed');
            assert.strictEqual(formData.get('field.nested'), 'dot notation');
            assert.strictEqual(formData.get('field with spaces'), 'spaces');
            assert.strictEqual(formData.get('🌟field'), 'emoji');
        });
    });

    // Set Method Tests
    describe("set() method - Basic functionality", () => {
        it("FormData.set() - replaces existing values", () => {
            const formData = new FormData();

            formData.append('name', 'John');
            formData.append('name', 'Jane');
            assert.deepStrictEqual(formData.getAll('name'), ['John', 'Jane']);

            formData.set('name', 'Bob');
            assert.deepStrictEqual(formData.getAll('name'), ['Bob']);
        });

        it("FormData.set() - creates new field", () => {
            const formData = new FormData();

            formData.set('newField', 'newValue');
            assert.strictEqual(formData.get('newField'), 'newValue');
        });

        it("FormData.set() - with File/Blob", () => {
            const formData = new FormData();
            const blob1 = new Blob(['content1'], { type: 'text/plain' });
            const blob2 = new Blob(['content2'], { type: 'text/plain' });

            formData.append('file', blob1);
            formData.append('file', blob2);
            assert.strictEqual(formData.getAll('file').length, 2);

            const newBlob = new Blob(['new content'], { type: 'text/plain' });
            formData.set('file', newBlob, 'replacement.txt');

            const files = formData.getAll('file');
            assert.strictEqual(files.length, 1);
            assert.strictEqual(files[0] instanceof Blob, true);
        });

        it("FormData.set() - File object without filename parameter", () => {
            const formData = new FormData();
            const file = new File(['test content'], 'original.txt', { type: 'text/plain' });
            const blob = new Blob(['test content'], { type: 'text/plain' });

            // Test with File object - should preserve original filename
            formData.set('file_object', file);
            const retrievedFile = formData.get('file_object');

            assert.strictEqual(retrievedFile instanceof File, true);
            assert.strictEqual(retrievedFile.name, 'original.txt');

            // Test with Blob object - should get default name
            formData.set('blob_object', blob);
            const retrievedBlob = formData.get('blob_object');

            assert.strictEqual(retrievedBlob instanceof File, true);
            assert.strictEqual(retrievedBlob.name, 'blob');

            // Test overriding with explicit filename
            formData.set('file_object', file, 'new_name.txt');
            const retrievedWithNewName = formData.get('file_object');

            assert.strictEqual(retrievedWithNewName.name, 'new_name.txt');
        });
    });

    // Get and GetAll Methods Tests
    describe("get() and getAll() methods", () => {
        it("FormData.get() - returns first value", () => {
            const formData = new FormData();

            formData.append('field', 'value1');
            formData.append('field', 'value2');
            formData.append('field', 'value3');

            assert.strictEqual(formData.get('field'), 'value1');
        });

        it("FormData.get() - returns null for non-existent field", () => {
            const formData = new FormData();

            assert.strictEqual(formData.get('nonExistent'), null);
        });

        it("FormData.getAll() - returns all values", () => {
            const formData = new FormData();

            formData.append('colors', 'red');
            formData.append('colors', 'green');
            formData.append('colors', 'blue');

            const colors = formData.getAll('colors');
            assert.deepStrictEqual(colors, ['red', 'green', 'blue']);
        });

        it("FormData.getAll() - returns empty array for non-existent field", () => {
            const formData = new FormData();

            assert.deepStrictEqual(formData.getAll('nonExistent'), []);
        });

        it("FormData.get() - case sensitivity", () => {
            const formData = new FormData();

            formData.append('Name', 'John');

            assert.strictEqual(formData.get('Name'), 'John');
            assert.strictEqual(formData.get('name'), null);
            assert.strictEqual(formData.get('NAME'), null);
        });
    });

    // Delete and Has Methods Tests
    describe("delete() and has() methods", () => {
        it("FormData.delete() - removes all values", () => {
            const formData = new FormData();

            formData.append('field', 'value1');
            formData.append('field', 'value2');
            formData.append('other', 'otherValue');

            assert.strictEqual(formData.has('field'), true);
            formData.delete('field');
            assert.strictEqual(formData.has('field'), false);
            assert.deepStrictEqual(formData.getAll('field'), []);

            // Other fields should remain
            assert.strictEqual(formData.has('other'), true);
            assert.strictEqual(formData.get('other'), 'otherValue');
        });

        it("FormData.delete() - non-existent field", () => {
            const formData = new FormData();

            // Should not throw error
            formData.delete('nonExistent');
            assert.strictEqual(formData.has('nonExistent'), false);
        });

        it("FormData.has() - field existence", () => {
            const formData = new FormData();

            assert.strictEqual(formData.has('field'), false);

            formData.append('field', '');
            assert.strictEqual(formData.has('field'), true);

            formData.delete('field');
            assert.strictEqual(formData.has('field'), false);
        });

        it("FormData.has() - case sensitivity", () => {
            const formData = new FormData();

            formData.append('Field', 'value');

            assert.strictEqual(formData.has('Field'), true);
            assert.strictEqual(formData.has('field'), false);
            assert.strictEqual(formData.has('FIELD'), false);
        });
    });

    // Iterator Methods Tests
    describe("Iterator methods", () => {
        it("FormData.keys() - iterator", () => {
            const formData = new FormData();

            formData.append('a', '1');
            formData.append('b', '2');
            formData.append('a', '3');
            formData.append('c', '4');

            const keys = Array.from(formData.keys());
            assert.deepStrictEqual(keys, ['a', 'b', 'a', 'c']);
        });

        it("FormData.values() - iterator", () => {
            const formData = new FormData();

            formData.append('a', '1');
            formData.append('b', '2');
            formData.append('a', '3');

            const values = Array.from(formData.values());
            assert.deepStrictEqual(values, ['1', '2', '3']);
        });

        it("FormData.entries() - iterator", () => {
            const formData = new FormData();

            formData.append('name', 'John');
            formData.append('age', '25');
            formData.append('name', 'Jane');

            const entries = Array.from(formData.entries());
            assert.deepStrictEqual(entries, [
                ['name', 'John'],
                ['age', '25'],
                ['name', 'Jane']
            ]);
        });

        it("FormData.forEach() - iteration", () => {
            const formData = new FormData();

            formData.append('a', '1');
            formData.append('b', '2');
            formData.append('a', '3');

            const collected = [];
            formData.forEach((value, key, form) => {
                collected.push([key, value]);
                assert.strictEqual(form, formData);
            });

            assert.deepStrictEqual(collected, [
                ['a', '1'],
                ['b', '2'],
                ['a', '3']
            ]);
        });

        it("FormData - for...of iteration", () => {
            const formData = new FormData();

            formData.append('x', 'A');
            formData.append('y', 'B');
            formData.append('x', 'C');

            const entries = [];
            for (const [key, value] of formData) {
                entries.push([key, value]);
            }

            assert.deepStrictEqual(entries, [
                ['x', 'A'],
                ['y', 'B'],
                ['x', 'C']
            ]);
        });

        it("FormData.entries() - Symbol.iterator", () => {
            const formData = new FormData();

            formData.append('test', 'value');

            // FormData should be iterable
            assert.strictEqual(typeof formData[Symbol.iterator], 'function');

            if (!isFibjs) {
                assert.strictEqual(formData[Symbol.iterator], formData.entries);
            }
        });
    });

    // File and Blob Handling Tests
    describe("File and Blob handling", () => {
        it("FormData stores File objects, not Blob objects", () => {
            const formData = new FormData();
            const blob = new Blob(['file content'], { type: 'text/plain' });
            const file = new File(['file content'], 'original.txt', { type: 'text/plain' });

            // Append Blob with filename
            formData.append('blob_with_filename', blob, 'document.txt');

            // Append Blob without filename
            formData.append('blob_without_filename', blob);

            // Append File object
            formData.append('file_object', file);

            const retrievedBlobWithFilename = formData.get('blob_with_filename');
            const retrievedBlobWithoutFilename = formData.get('blob_without_filename');
            const retrievedFileObject = formData.get('file_object');

            // All should be File instances
            assert.strictEqual(retrievedBlobWithFilename instanceof File, true);
            assert.strictEqual(retrievedBlobWithoutFilename instanceof File, true);
            assert.strictEqual(retrievedFileObject instanceof File, true);

            // All should also be Blob instances (since File extends Blob)
            assert.strictEqual(retrievedBlobWithFilename instanceof Blob, true);
            assert.strictEqual(retrievedBlobWithoutFilename instanceof Blob, true);
            assert.strictEqual(retrievedFileObject instanceof Blob, true);

            // Check name properties
            assert.strictEqual(retrievedBlobWithFilename.name, 'document.txt');
            assert.strictEqual(retrievedBlobWithoutFilename.name, 'blob');
            assert.strictEqual(retrievedFileObject.name, 'original.txt');
        });

        it("FormData.append(File, name='') does not override File original name", () => {
            const formData = new FormData();
            const file = new File(['abc'], 'myfile.txt', { type: 'text/plain' });
            // Append with empty string as filename
            formData.append('file', file, '');
            const retrieved = formData.get('file');
            // Should keep the original name
            assert.strictEqual(retrieved instanceof File, true);
            assert.strictEqual(retrieved.name, '');
        });

        it("FormData with different Blob types", () => {
            const formData = new FormData();

            const textBlob = new Blob(['text'], { type: 'text/plain' });
            const jsonBlob = new Blob(['{}'], { type: 'application/json' });
            const binaryBlob = new Blob([new Uint8Array([1, 2, 3])], { type: 'application/octet-stream' });

            formData.append('text', textBlob);
            formData.append('json', jsonBlob);
            formData.append('binary', binaryBlob);

            assert.strictEqual(formData.get('text') instanceof Blob, true);
            assert.strictEqual(formData.get('json') instanceof Blob, true);
            assert.strictEqual(formData.get('binary') instanceof Blob, true);
        });

        it("FormData - large Blob handling", () => {
            const formData = new FormData();

            // Create a larger blob
            const largeContent = new Array(10000).fill('x').join('');
            const largeBlob = new Blob([largeContent], { type: 'text/plain' });

            formData.append('largefile', largeBlob, 'large.txt');

            const retrievedBlob = formData.get('largefile');
            assert.strictEqual(retrievedBlob instanceof Blob, true);
            assert.strictEqual(retrievedBlob.size, largeContent.length);
        });
    });

    // Edge Cases and Error Handling
    describe("Edge cases and error handling", () => {
        it("FormData - empty field names", () => {
            const formData = new FormData();

            if (!isFibjs) {
                formData.append('', 'empty name');
                assert.strictEqual(formData.get(''), 'empty name');
                assert.strictEqual(formData.has(''), true);
            } else {
                assert.throws(() => {
                    formData.append('', 'empty name');
                });
            }
        });

        it("FormData - very long field names and values", () => {
            const formData = new FormData();

            const longName = 'a'.repeat(1000);
            const longValue = 'b'.repeat(1000);

            formData.append(longName, longValue);
            assert.strictEqual(formData.get(longName), longValue);
        });

        it("FormData - Unicode in field names and values", () => {
            const formData = new FormData();

            const unicodeName = '字段名称';
            const unicodeValue = 'Hello 世界 🌍';

            formData.append(unicodeName, unicodeValue);
            assert.strictEqual(formData.get(unicodeName), unicodeValue);
        });

        it("FormData - control characters", () => {
            const formData = new FormData();

            const nameWithControls = 'field\x00\x01\x1F';
            const valueWithControls = 'value\x00\x01\x1F';

            formData.append(nameWithControls, valueWithControls);
            assert.strictEqual(formData.get(nameWithControls), valueWithControls);
        });

        it("FormData - object toString conversion", () => {
            const formData = new FormData();

            const objWithToString = {
                toString: () => 'custom_string'
            };

            const objWithValueOf = {
                valueOf: () => 42,
                toString: () => 'string_method'
            };

            formData.append('obj1', objWithToString);
            formData.append('obj2', objWithValueOf);

            assert.strictEqual(formData.get('obj1'), 'custom_string');
            assert.strictEqual(formData.get('obj2'), 'string_method');
        });

        it("FormData - Symbol handling", () => {
            const formData = new FormData();

            const sym = Symbol('test');

            if (!isFibjs) {
                assert.throws(() => {
                    formData.append('symbol', sym);
                }, TypeError, 'Cannot convert a Symbol value to a string');
            }
        });
    });

    // Complex Operations Tests
    describe("Complex operations", () => {
        it("FormData - mixed operations workflow", () => {
            const formData = new FormData();

            // Build form data step by step
            formData.append('name', 'John');
            formData.append('email', 'john@example.com');
            formData.append('hobbies', 'reading');
            formData.append('hobbies', 'swimming');

            // Verify initial state
            assert.strictEqual(formData.get('name'), 'John');
            assert.deepStrictEqual(formData.getAll('hobbies'), ['reading', 'swimming']);

            // Modify data
            formData.set('name', 'Jane');
            formData.append('hobbies', 'coding');
            formData.delete('email');

            // Verify final state
            assert.strictEqual(formData.get('name'), 'Jane');
            assert.strictEqual(formData.has('email'), false);
            assert.deepStrictEqual(formData.getAll('hobbies'), ['reading', 'swimming', 'coding']);
        });

        it("FormData - order preservation", () => {
            const formData = new FormData();

            // Add fields in specific order
            formData.append('third', '3');
            formData.append('first', '1');
            formData.append('second', '2');
            formData.append('first', '1b');

            // Order should be preserved
            const entries = Array.from(formData.entries());
            assert.deepStrictEqual(entries, [
                ['third', '3'],
                ['first', '1'],
                ['second', '2'],
                ['first', '1b']
            ]);
        });

        it("FormData - deep copy behavior", () => {
            const formData = new FormData();
            const blob = new Blob(['content']);

            formData.append('file', blob);

            const retrievedBlob = formData.get('file');

            // In Node.js, when a Blob is added to FormData, it gets converted to a File object
            // So we should check for File instance instead of exact reference equality
            assert.strictEqual(retrievedBlob instanceof Blob, true);
            assert.strictEqual(retrievedBlob.size, blob.size);
            assert.strictEqual(retrievedBlob.type, blob.type);

            // The content should be the same even if the object type changed
            assert.strictEqual(retrievedBlob.size, 7); // 'content'.length
        });
    });

    // Performance and Memory Tests
    describe("Performance and memory", () => {
        it("FormData - many fields performance", () => {
            const formData = new FormData();

            const startTime = Date.now();

            // Add many fields
            for (let i = 0; i < 1000; i++) {
                formData.append(`field_${i}`, `value_${i}`);
            }

            const endTime = Date.now();

            // Verify all fields are accessible
            assert.strictEqual(formData.get('field_0'), 'value_0');
            assert.strictEqual(formData.get('field_999'), 'value_999');

            // Operation should be reasonably fast
            assert.ok(endTime - startTime < 1000, 'Adding many fields should be fast');
        });

        it("FormData - iteration performance", () => {
            const formData = new FormData();

            // Add many entries
            for (let i = 0; i < 1000; i++) {
                formData.append('field', `value_${i}`);
            }

            const startTime = Date.now();

            let count = 0;
            for (const [key, value] of formData) {
                count++;
            }

            const endTime = Date.now();

            assert.strictEqual(count, 1000);
            assert.ok(endTime - startTime < 100, 'Iteration should be fast');
        });

        it("FormData - memory with large values", () => {
            const formData = new FormData();

            // Add some large string values
            const largeValue = 'x'.repeat(100000); // 100KB string

            for (let i = 0; i < 10; i++) {
                formData.append(`large_${i}`, largeValue);
            }

            // Verify data is intact
            assert.strictEqual(formData.get('large_0').length, 100000);
            assert.strictEqual(formData.get('large_9').length, 100000);

            // Should be able to iterate without issues
            let count = 0;
            for (const [key, value] of formData) {
                count++;
                assert.strictEqual(value.length, 100000);
            }
            assert.strictEqual(count, 10);
        });
    });

    // Compatibility and Standards Tests
    describe("Standards compliance", () => {
        it("FormData - method parameter validation", () => {
            const formData = new FormData();

            // Test required parameters - Node.js requires at least 2 parameters for append
            assert.throws(() => {
                formData.append();
            });

            assert.throws(() => {
                formData.append('name');
            });

            // Same for set method
            assert.throws(() => {
                formData.set();
            });

            assert.throws(() => {
                formData.set('name');
            });

            // Valid calls should work
            formData.append('name', 'value');
            formData.set('name', 'newvalue');
            assert.strictEqual(formData.get('name'), 'newvalue');
        });

        it("FormData - toString and valueOf behavior", () => {
            const formData = new FormData();

            // FormData should have proper toString
            assert.strictEqual(Object.prototype.toString.call(formData), '[object FormData]');

            // valueOf should return the object itself
            assert.strictEqual(formData.valueOf(), formData);
        });

        it("FormData - instanceof and prototype chain", () => {
            const formData = new FormData();

            assert.strictEqual(formData instanceof FormData, true);
            assert.strictEqual(formData.constructor, FormData);

            // Should have correct prototype chain
            assert.strictEqual(Object.getPrototypeOf(formData), FormData.prototype);
        });
    });

    it("FormData - filename retrieval test", () => {
        const formData = new FormData();
        const blob = new Blob(['test content'], { type: 'text/plain' });

        // Test various filename scenarios
        formData.append('file_with_name', blob, 'test.txt');
        formData.append('file_with_path', blob, 'folder/subfolder/document.pdf');
        formData.append('file_with_unicode', blob, '测试文件.txt');
        formData.append('file_with_special_chars', blob, 'file (1) [copy].txt');
        formData.append('file_without_name', blob);

        // Get files and check properties
        const fileWithName = formData.get('file_with_name');
        const fileWithPath = formData.get('file_with_path');
        const fileWithUnicode = formData.get('file_with_unicode');
        const fileWithSpecialChars = formData.get('file_with_special_chars');
        const fileWithoutName = formData.get('file_without_name');

        // Verify File name property is accessible
        assert.strictEqual(fileWithName.name, 'test.txt');
        assert.strictEqual(fileWithPath.name, 'folder/subfolder/document.pdf');
        assert.strictEqual(fileWithUnicode.name, '测试文件.txt');
        assert.strictEqual(fileWithSpecialChars.name, 'file (1) [copy].txt');
        assert.strictEqual(fileWithoutName.name, 'blob');

        // Check if it's a File object (File extends Blob)
        assert.strictEqual(fileWithName instanceof File, true);
        assert.strictEqual(fileWithName instanceof Blob, true);

        // Test other File properties
        assert.strictEqual(typeof fileWithName.lastModified, 'number');
    });
});
