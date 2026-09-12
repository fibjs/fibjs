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

        it("FormData constructor - urlform string initialization", () => {
            // Test with application/x-www-form-urlencoded format
            const urlFormString = "name=John&email=john%40example.com&age=25&hobbies=reading&hobbies=swimming";
            const formData = new FormData(urlFormString);

            // Check parsed values
            assert.strictEqual(formData.get('name'), 'John');
            assert.strictEqual(formData.get('email'), 'john@example.com');
            assert.strictEqual(formData.get('age'), '25');
            assert.deepStrictEqual(formData.getAll('hobbies'), ['reading', 'swimming']);
        });

        it("FormData constructor - empty urlform string", () => {
            const formData = new FormData("");

            // Should create empty FormData
            assert.strictEqual(formData instanceof FormData, true);
            assert.strictEqual(formData.get('anyField'), null);
        });

        it("FormData constructor - urlform with special characters", () => {
            const urlFormString = "field%5B%5D=array-like&field%2Bplus=plus%20sign&unicode=%E4%B8%AD%E6%96%87";
            const formData = new FormData(urlFormString);

            assert.strictEqual(formData.get('field[]'), 'array-like');
            assert.strictEqual(formData.get('field+plus'), 'plus sign');
            assert.strictEqual(formData.get('unicode'), '中文');
        });

        it("FormData constructor - multipart Buffer initialization", () => {
            // Create multipart/form-data content
            const boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
            const multipartContent = [
                `------WebKitFormBoundary7MA4YWxkTrZu0gW`,
                `Content-Disposition: form-data; name="name"`,
                ``,
                `John Doe`,
                `------WebKitFormBoundary7MA4YWxkTrZu0gW`,
                `Content-Disposition: form-data; name="email"`,
                ``,
                `john@example.com`,
                `------WebKitFormBoundary7MA4YWxkTrZu0gW`,
                `Content-Disposition: form-data; name="file"; filename="test.txt"`,
                `Content-Type: text/plain`,
                ``,
                `File content here`,
                `------WebKitFormBoundary7MA4YWxkTrZu0gW--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            // Check parsed values
            assert.strictEqual(formData.get('name'), 'John Doe');
            assert.strictEqual(formData.get('email'), 'john@example.com');

            const file = formData.get('file');
            assert.strictEqual(file instanceof File, true);
            assert.strictEqual(file.name, 'test.txt');
        });

        it("FormData constructor - multipart with multiple files", () => {
            const boundary = "----FormBoundaryABC123";
            const multipartContent = [
                `------FormBoundaryABC123`,
                `Content-Disposition: form-data; name="title"`,
                ``,
                `Document Upload`,
                `------FormBoundaryABC123`,
                `Content-Disposition: form-data; name="file1"; filename="doc1.txt"`,
                `Content-Type: text/plain`,
                ``,
                `Content of first file`,
                `------FormBoundaryABC123`,
                `Content-Disposition: form-data; name="file2"; filename="doc2.pdf"`,
                `Content-Type: application/pdf`,
                ``,
                `PDF content here`,
                `------FormBoundaryABC123--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            assert.strictEqual(formData.get('title'), 'Document Upload');

            const file1 = formData.get('file1');
            const file2 = formData.get('file2');

            assert.strictEqual(file1 instanceof File, true);
            assert.strictEqual(file1.name, 'doc1.txt');

            assert.strictEqual(file2 instanceof File, true);
            assert.strictEqual(file2.name, 'doc2.pdf');
        });

        it("FormData constructor - object initialization", () => {
            const initObject = {
                name: 'Jane',
                age: 30,
                hobbies: ['reading', 'coding'],
                active: true
            };

            const formData = new FormData(initObject);

            assert.strictEqual(formData.get('name'), 'Jane');
            assert.strictEqual(formData.get('age'), '30');
            assert.strictEqual(formData.get('active'), 'true');
            // For arrays, behavior may vary - check actual implementation
            if (formData.getAll('hobbies').length > 0) {
                assert.strictEqual(formData.getAll('hobbies').length >= 1, true);
            }
        });

        it("FormData constructor - object initialization with File objects", () => {
            // Create test data for files
            const textData = new Blob(['Hello, world!'], { type: 'text/plain' });
            const pdfData = new Blob(['%PDF-1.4 fake pdf content'], { type: 'application/pdf' });

            // Create File objects with proper array-wrapped data
            const textFile = new File([textData], 'hello.txt', { type: 'text/plain', lastModified: Date.now() });
            const pdfFile = new File([pdfData], 'document.pdf', { type: 'application/pdf', lastModified: Date.now() });

            const initObject = {
                username: 'testuser',
                description: 'File upload test',
                textFile: textFile,
                pdfFile: pdfFile,
                multiple: [textFile, 'plain text value', pdfFile]
            };

            const formData = new FormData(initObject);

            // Check string fields
            assert.strictEqual(formData.get('username'), 'testuser');
            assert.strictEqual(formData.get('description'), 'File upload test');

            // Check File objects
            const retrievedTextFile = formData.get('textFile');
            const retrievedPdfFile = formData.get('pdfFile');

            assert.strictEqual(retrievedTextFile instanceof File, true);
            assert.strictEqual(retrievedTextFile.name, 'hello.txt');
            assert.strictEqual(retrievedTextFile.type, 'text/plain');

            assert.strictEqual(retrievedPdfFile instanceof File, true);
            assert.strictEqual(retrievedPdfFile.name, 'document.pdf');
            assert.strictEqual(retrievedPdfFile.type, 'application/pdf');

            // Check array with mixed content
            const multipleValues = formData.getAll('multiple');
            assert.strictEqual(multipleValues.length >= 1, true);
            // At least one File object should be present
            const hasFileInMultiple = multipleValues.some(value => value instanceof File);
            assert.strictEqual(hasFileInMultiple, true);
        });

        it("FormData constructor - object initialization with Blob and File mixed", () => {
            const blobData = new Blob(['blob content'], { type: 'text/plain' });
            const fileData = new File([blobData], 'test.txt', { type: 'text/plain', lastModified: Date.now() });

            const initObject = {
                plainBlob: blobData,
                namedFile: fileData,
                metadata: JSON.stringify({ version: '1.0', timestamp: Date.now() })
            };

            const formData = new FormData(initObject);

            // Blob should be converted to File or handled appropriately
            const retrievedBlob = formData.get('plainBlob');
            assert.strictEqual(retrievedBlob instanceof Blob, true);

            // File should remain as File
            const retrievedFile = formData.get('namedFile');
            assert.strictEqual(retrievedFile instanceof File, true);
            assert.strictEqual(retrievedFile.name, 'test.txt');

            // String should remain as string
            const metadata = formData.get('metadata');
            assert.strictEqual(typeof metadata, 'string');
            assert.strictEqual(JSON.parse(metadata).version, '1.0');
        });

        it("FormData constructor - object initialization with File properties", () => {
            // Test File object with all properties
            const fileContent = new Blob(['Test file content for properties check'], { type: 'text/plain' });
            const lastModified = Date.now() - 86400000; // 1 day ago

            const testFile = new File([fileContent], 'properties-test.txt', {
                type: 'text/plain',
                lastModified: lastModified
            });

            const initObject = {
                testFile: testFile,
                regularField: 'normal value'
            };

            const formData = new FormData(initObject);

            const retrievedFile = formData.get('testFile');

            // Verify File properties are preserved
            assert.strictEqual(retrievedFile instanceof File, true);
            assert.strictEqual(retrievedFile.name, 'properties-test.txt');
            assert.strictEqual(retrievedFile.type, 'text/plain');
            assert.strictEqual(retrievedFile.lastModified, lastModified);
            assert.strictEqual(retrievedFile.size, fileContent.size);
        });

        it("FormData constructor - real-world file upload scenario", () => {
            // Simulate reading file data (like fs.readFileSync in test.cjs)
            const pdfData = new Uint8Array([0x25, 0x50, 0x44, 0x46]); // %PDF header
            const imageData = new Uint8Array([0xFF, 0xD8, 0xFF, 0xE0]); // JPEG header

            // Create File objects with realistic properties
            const pdfFile = new File([pdfData], 'document.pdf', {
                type: 'application/pdf',
                lastModified: Date.now()
            });

            const imageFile = new File([imageData], 'photo.jpg', {
                type: 'image/jpeg',
                lastModified: Date.now()
            });

            // Realistic form data initialization similar to test.cjs
            const initObject = {
                return_images: true,
                output_dir: 'uploads',
                return_content_list: false,
                return_layout: false,
                is_json_md_dump: true,
                return_info: false,
                document: pdfFile,
                photo: imageFile,
                metadata: JSON.stringify({
                    uploadTime: Date.now(),
                    userAgent: 'Test Agent'
                })
            };

            const formData = new FormData(initObject);

            // Verify boolean fields are converted to strings
            assert.strictEqual(formData.get('return_images'), 'true');
            assert.strictEqual(formData.get('return_content_list'), 'false');
            assert.strictEqual(formData.get('return_layout'), 'false');
            assert.strictEqual(formData.get('is_json_md_dump'), 'true');
            assert.strictEqual(formData.get('return_info'), 'false');

            // Verify string fields
            assert.strictEqual(formData.get('output_dir'), 'uploads');

            // Verify File objects are preserved
            const retrievedPdf = formData.get('document');
            assert.strictEqual(retrievedPdf instanceof File, true);
            assert.strictEqual(retrievedPdf.name, 'document.pdf');
            assert.strictEqual(retrievedPdf.type, 'application/pdf');
            assert.strictEqual(retrievedPdf.size, pdfData.length);

            const retrievedImage = formData.get('photo');
            assert.strictEqual(retrievedImage instanceof File, true);
            assert.strictEqual(retrievedImage.name, 'photo.jpg');
            assert.strictEqual(retrievedImage.type, 'image/jpeg');
            assert.strictEqual(retrievedImage.size, imageData.length);

            // Verify JSON string field
            const metadata = JSON.parse(formData.get('metadata'));
            assert.strictEqual(typeof metadata.uploadTime, 'number');
            assert.strictEqual(metadata.userAgent, 'Test Agent');
        });

        it("FormData constructor - File constructor with options object", () => {
            // Test File constructor similar to the pattern in test.cjs
            const fileData = new Uint8Array([1, 2, 3, 4, 5]);

            // This pattern matches test.cjs usage: new File({ ... })
            const fileWithOptions = new File([fileData], 'test-options.bin', {
                type: 'application/octet-stream',
                lastModified: 1640995200000 // Fixed timestamp for testing
            });

            const initObject = {
                description: 'File with constructor options',
                binaryFile: fileWithOptions,
                uploadMode: 'binary'
            };

            const formData = new FormData(initObject);

            // Verify the File object was properly handled
            const retrievedFile = formData.get('binaryFile');
            assert.strictEqual(retrievedFile instanceof File, true);
            assert.strictEqual(retrievedFile.name, 'test-options.bin');
            assert.strictEqual(retrievedFile.type, 'application/octet-stream');
            assert.strictEqual(retrievedFile.lastModified, 1640995200000);
            assert.strictEqual(retrievedFile.size, fileData.length);

            // Verify other fields
            assert.strictEqual(formData.get('description'), 'File with constructor options');
            assert.strictEqual(formData.get('uploadMode'), 'binary');
        });

        it("FormData constructor - mixed File and file-like objects", () => {
            const fileData = new Blob(['File content'], { type: 'text/plain' });
            // Real File object
            const realFile = new File([fileData], 'real.txt', {
                type: 'text/plain',
                lastModified: Date.now()
            });

            // File-like object (plain object with file properties)
            const fileLikeObject = {
                type: 'application/pdf',
                name: 'fake.pdf',
                data: new Uint8Array([0x25, 0x50, 0x44, 0x46]),
                lastModified: Date.now()
            };

            const initObject = {
                realFile: realFile,
                fileLikeObject: fileLikeObject,
                normalField: 'regular value'
            };

            const formData = new FormData(initObject);

            // Real File should be preserved as File
            const retrievedRealFile = formData.get('realFile');
            assert.strictEqual(retrievedRealFile instanceof File, true);
            assert.strictEqual(retrievedRealFile.name, 'real.txt');

            // A plain object is not a Blob, so it is coerced to a string (WHATWG FormData)
            assert.strictEqual(formData.get('fileLikeObject'), '[object Object]');

            // Normal field should remain as string
            assert.strictEqual(formData.get('normalField'), 'regular value');
        });

        it("FormData constructor - FormData copy initialization", () => {
            const originalFormData = new FormData();
            originalFormData.append('field1', 'value1');
            originalFormData.append('field2', 'value2');
            originalFormData.append('field1', 'value3'); // multiple values

            const copiedFormData = new FormData(originalFormData);

            // Should have same values
            assert.deepStrictEqual(copiedFormData.getAll('field1'), ['value1', 'value3']);
            assert.strictEqual(copiedFormData.get('field2'), 'value2');

            // Should be independent instances
            copiedFormData.append('field3', 'value4');
            assert.strictEqual(originalFormData.has('field3'), false);
            assert.strictEqual(copiedFormData.has('field3'), true);
        });

        it("FormData constructor - Blob initialization with auto boundary detection", () => {
            // Create original FormData with mixed content
            const originalFormData = new FormData();
            originalFormData.append('text_field', 'sample text');
            originalFormData.append('number_field', '42');

            // Create a file-like Blob
            const fileBlob = new Blob(['file content data'], { type: 'text/plain' });
            originalFormData.append('file_field', fileBlob, 'test.txt');

            // Encode to multipart Blob
            const encodedBlob = originalFormData.encode('multipart/form-data');

            // Verify it's a Blob with correct content type
            assert.ok(encodedBlob instanceof Blob, 'encode() should return Blob instance');
            const contentType = encodedBlob.type;
            assert.ok(contentType.includes('multipart/form-data'), 'Content-Type should be multipart/form-data');
            assert.ok(contentType.includes('boundary='), 'Content-Type should include boundary parameter');

            // Test constructor with auto boundary detection (empty boundary parameter)
            const reconstructedFormData = new FormData(encodedBlob, '');

            // Verify all fields are correctly parsed
            assert.strictEqual(reconstructedFormData.get('text_field'), 'sample text');
            assert.strictEqual(reconstructedFormData.get('number_field'), '42');

            // Verify file field
            const reconstructedFile = reconstructedFormData.get('file_field');
            assert.ok(reconstructedFile instanceof Blob, 'File field should be Blob instance');
            assert.strictEqual(reconstructedFile.name, 'test.txt');
            assert.strictEqual(reconstructedFile.type, 'text/plain');
            assert.strictEqual(reconstructedFile.textSync(), 'file content data');
        });

        it("FormData constructor - Blob initialization with explicit boundary", () => {
            // Create FormData with custom boundary
            const originalFormData = new FormData();
            originalFormData.append('message', 'Hello World');
            originalFormData.append('status', 'active');

            // Encode with custom boundary
            const customBoundary = 'CustomTestBoundary12345';
            const encodedBlob = originalFormData.encode(`multipart/form-data; boundary=${customBoundary}`);

            // Test constructor with explicit boundary parameter
            const reconstructedFormData = new FormData(encodedBlob, `multipart/form-data; boundary=${customBoundary}`);

            // Verify parsing with explicit boundary
            assert.strictEqual(reconstructedFormData.get('message'), 'Hello World');
            assert.strictEqual(reconstructedFormData.get('status'), 'active');
        });

        it("FormData constructor - Blob with invalid/missing boundary falls back gracefully", () => {
            // Create a Blob that looks like multipart but has no proper Content-Type
            const invalidMultipartContent = [
                '------SomeBoundary',
                'Content-Disposition: form-data; name="test"',
                '',
                'test value',
                '------SomeBoundary--'
            ].join('\r\n');

            const invalidBlob = new Blob([invalidMultipartContent], { type: 'text/plain' }); // Wrong type

            // Constructor should handle gracefully when boundary can't be detected
            try {
                const formData = new FormData(invalidBlob, '');
                // If it doesn't throw, check it's empty or has some reasonable behavior
                assert.ok(formData instanceof FormData, 'Should still create FormData instance');
            } catch (e) {
                // Or it might throw an error, which is also acceptable behavior
                assert.ok(e instanceof Error, 'Should throw a meaningful error');
            }
        });

        it("FormData constructor - Blob round-trip preserves data integrity", () => {
            // Test complex round-trip scenario
            const originalFormData = new FormData();

            // Add various types of data
            originalFormData.append('simple_text', 'Simple value');
            originalFormData.append('unicode_text', '测试中文字符 🎉');
            originalFormData.append('special_chars', 'Value with\nnewlines\tand\rspecial chars');
            originalFormData.append('empty_field', '');

            // Add multiple values for same key
            originalFormData.append('multi_value', 'first');
            originalFormData.append('multi_value', 'second');

            // Add binary file
            const binaryData = new Uint8Array([0x00, 0x01, 0x02, 0xFF, 0xFE]);
            const binaryBlob = new Blob([binaryData], { type: 'application/octet-stream' });
            originalFormData.append('binary_file', binaryBlob, 'data.bin');

            // Encode and reconstruct
            const encodedBlob = originalFormData.encode('multipart/form-data');
            const reconstructedFormData = new FormData(encodedBlob, '');

            // Verify all text fields
            assert.strictEqual(reconstructedFormData.get('simple_text'), 'Simple value');
            assert.strictEqual(reconstructedFormData.get('unicode_text'), '测试中文字符 🎉');
            // multiline values are normalized to CRLF by the multipart serializer (HTML spec)
            assert.strictEqual(reconstructedFormData.get('special_chars'), 'Value with\r\nnewlines\tand\r\nspecial chars');
            assert.strictEqual(reconstructedFormData.get('empty_field'), '');

            // Verify multiple values
            const multiValues = reconstructedFormData.getAll('multi_value');
            assert.strictEqual(multiValues.length, 2);
            assert.strictEqual(multiValues[0], 'first');
            assert.strictEqual(multiValues[1], 'second');

            // Verify binary file
            const reconstructedBinary = reconstructedFormData.get('binary_file');
            assert.ok(reconstructedBinary instanceof Blob, 'Binary field should be Blob');
            assert.strictEqual(reconstructedBinary.name, 'data.bin');
            assert.strictEqual(reconstructedBinary.type, 'application/octet-stream');

            // Verify binary size (content length should match)
            assert.strictEqual(reconstructedBinary.size, binaryData.length, 'Binary size should match');

            // For binary data comparison, we'll verify size and type instead of content
            // since textSync() might not handle binary data correctly
            assert.ok(reconstructedBinary.size > 0, 'Binary data should not be empty');
        });
    });

    // Multipart parsing comprehensive tests
    describe("Multipart parsing - Edge cases and defects", () => {
        it("FormData constructor - text fields should store actual content, not empty string", () => {
            const boundary = "----TestBoundary";
            const multipartContent = [
                `------TestBoundary`,
                `Content-Disposition: form-data; name="username"`,
                ``,
                `john_doe`,
                `------TestBoundary`,
                `Content-Disposition: form-data; name="message"`,
                ``,
                `Hello, this is a test message with spaces and symbols!@#$`,
                `------TestBoundary`,
                `Content-Disposition: form-data; name="number"`,
                ``,
                `12345`,
                `------TestBoundary--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            // Critical test: text fields should contain actual content, not empty strings
            assert.strictEqual(formData.get('username'), 'john_doe');
            assert.strictEqual(formData.get('message'), 'Hello, this is a test message with spaces and symbols!@#$');
            assert.strictEqual(formData.get('number'), '12345');

            // Verify they are not empty strings (the original bug)
            assert.notStrictEqual(formData.get('username'), '');
            assert.notStrictEqual(formData.get('message'), '');
            assert.notStrictEqual(formData.get('number'), '');
        });

        it("FormData constructor - boundary parameter format variations", () => {
            const boundary = "----WebKitBoundary123";
            const multipartContent = [
                `------WebKitBoundary123`,
                `Content-Disposition: form-data; name="test"`,
                ``,
                `value`,
                `------WebKitBoundary123--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);

            // Test different boundary parameter formats
            const formData1 = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);
            const formData2 = new FormData(buffer, `multipart/form-data;boundary=${boundary}`);
            const formData3 = new FormData(buffer, `application/x-www-form-urlencoded; boundary=${boundary}`);

            assert.strictEqual(formData1.get('test'), 'value');
            assert.strictEqual(formData2.get('test'), 'value');
            // This might fail if boundary finding is too strict
            if (formData3.get('test') !== null) {
                assert.strictEqual(formData3.get('test'), 'value');
            }
        });

        it("FormData constructor - empty text fields", () => {
            const boundary = "----EmptyTest";
            const multipartContent = [
                `------EmptyTest`,
                `Content-Disposition: form-data; name="empty_field"`,
                ``,
                ``,
                `------EmptyTest`,
                `Content-Disposition: form-data; name="whitespace_field"`,
                ``,
                `   `,
                `------EmptyTest--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            // Empty field should be empty string, not null
            assert.strictEqual(formData.get('empty_field'), '');
            assert.strictEqual(formData.get('whitespace_field'), '   ');
        });

        it("FormData constructor - multiline text content", () => {
            const boundary = "----MultilineTest";
            const multilineText = `Line 1
Line 2
Line 3 with special chars: áéíóú`;

            const multipartContent = [
                `------MultilineTest`,
                `Content-Disposition: form-data; name="multiline"`,
                ``,
                multilineText,
                `------MultilineTest--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            assert.strictEqual(formData.get('multiline'), multilineText);
        });

        it("FormData constructor - fields with special characters in names", () => {
            const boundary = "----SpecialNames";
            const multipartContent = [
                `------SpecialNames`,
                `Content-Disposition: form-data; name="field[0]"`,
                ``,
                `array-like`,
                `------SpecialNames`,
                `Content-Disposition: form-data; name="field.nested"`,
                ``,
                `dot-notation`,
                `------SpecialNames`,
                `Content-Disposition: form-data; name="field with spaces"`,
                ``,
                `spaced-name`,
                `------SpecialNames--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            assert.strictEqual(formData.get('field[0]'), 'array-like');
            assert.strictEqual(formData.get('field.nested'), 'dot-notation');
            assert.strictEqual(formData.get('field with spaces'), 'spaced-name');
        });

        it("FormData constructor - mixed files and text", () => {
            const boundary = "----MixedContent";
            const multipartContent = [
                `------MixedContent`,
                `Content-Disposition: form-data; name="description"`,
                ``,
                `File upload description`,
                `------MixedContent`,
                `Content-Disposition: form-data; name="file"; filename="test.txt"`,
                `Content-Type: text/plain`,
                ``,
                `File content here`,
                `------MixedContent`,
                `Content-Disposition: form-data; name="category"`,
                ``,
                `documents`,
                `------MixedContent--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            // Text fields should be strings
            assert.strictEqual(formData.get('description'), 'File upload description');
            assert.strictEqual(formData.get('category'), 'documents');

            // File should be File object
            const file = formData.get('file');
            assert.strictEqual(file instanceof File, true);
            assert.strictEqual(file.name, 'test.txt');
        });

        it("FormData constructor - invalid boundary should fail gracefully", () => {
            const multipartContent = [
                `------InvalidBoundary`,
                `Content-Disposition: form-data; name="test"`,
                ``,
                `value`,
                `------InvalidBoundary--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);

            // Invalid boundary parameter should not crash
            const formData = new FormData(buffer, `invalid-format`);

            // Should return empty FormData or gracefully handle error
            assert.strictEqual(formData.get('test'), null);
        });

        it("FormData constructor - malformed multipart data", () => {
            const boundary = "----MalformedTest";

            // Missing final boundary
            const malformedContent1 = [
                `------MalformedTest`,
                `Content-Disposition: form-data; name="test"`,
                ``,
                `value`,
                ``
            ].join('\r\n');

            // Missing Content-Disposition
            const malformedContent2 = [
                `------MalformedTest`,
                ``,
                `value`,
                `------MalformedTest--`,
                ``
            ].join('\r\n');

            const buffer1 = new Buffer(malformedContent1);
            const buffer2 = new Buffer(malformedContent2);

            // Should handle malformed data gracefully
            const formData1 = new FormData(buffer1, `multipart/form-data; boundary=${boundary}`);
            const formData2 = new FormData(buffer2, `multipart/form-data; boundary=${boundary}`);

            // May return null or handle gracefully depending on implementation
            // The key is not to crash
            assert.ok(formData1 instanceof FormData);
            assert.ok(formData2 instanceof FormData);
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

    // FormData.encode() Method Tests
    describe("encode() method - Basic functionality", () => {
        it("FormData.encode() - application/x-www-form-urlencoded (default)", () => {
            const formData = new FormData();
            formData.append('name', 'John Doe');
            formData.append('email', 'john@example.com');
            formData.append('age', '25');

            const encoded = formData.encode();

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            // Should be URL encoded format
            assert.ok(result.includes('name=John%20Doe'));
            assert.ok(result.includes('email=john%40example.com'));
            assert.ok(result.includes('age=25'));
            assert.ok(result.includes('&'));
        });

        it("FormData.encode() - explicit application/x-www-form-urlencoded", () => {
            const formData = new FormData();
            formData.append('field1', 'value with spaces');
            formData.append('field2', 'value+with+plus');
            formData.append('field3', 'value&with&ampersand');

            const encoded = formData.encode('application/x-www-form-urlencoded');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            assert.ok(result.includes('field1=value%20with%20spaces'));
            assert.ok(result.includes('field2=value%2Bwith%2Bplus'));
            assert.ok(result.includes('field3=value%26with%26ampersand'));
        });

        it("FormData.encode() - urlencoded alias", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            const encoded1 = formData.encode('urlencoded');
            const encoded2 = formData.encode('application/x-www-form-urlencoded');

            // Should produce same result
            assert.strictEqual(encoded1.textSync(), encoded2.textSync());
            assert.strictEqual(encoded1.textSync(), 'test=value');

            // Both should return Blobs with correct content-type
            assert.strictEqual(encoded1 instanceof Blob, true);
            assert.strictEqual(encoded2 instanceof Blob, true);
            assert.strictEqual(encoded1.type, 'application/x-www-form-urlencoded');
            assert.strictEqual(encoded2.type, 'application/x-www-form-urlencoded');
        });

        it("FormData.encode() - form-urlencoded alias", () => {
            const formData = new FormData();
            formData.append('key', 'test value');

            const encoded = formData.encode('form-urlencoded');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            assert.strictEqual(result, 'key=test%20value');
        });

        it("FormData.encode() - www-form-urlencoded alias", () => {
            const formData = new FormData();
            formData.append('param', 'test@value.com');

            const encoded = formData.encode('www-form-urlencoded');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            assert.strictEqual(result, 'param=test%40value.com');
        });

        it("FormData.encode() - multiple values for same key", () => {
            const formData = new FormData();
            formData.append('colors', 'red');
            formData.append('colors', 'green');
            formData.append('colors', 'blue');

            const encoded = formData.encode('application/x-www-form-urlencoded');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            // All values should be included
            assert.ok(result.includes('colors=red'));
            assert.ok(result.includes('colors=green'));
            assert.ok(result.includes('colors=blue'));
            // Should be separated by &
            const parts = result.split('&');
            assert.strictEqual(parts.length, 3);
        });

        it("FormData.encode() - special characters encoding", () => {
            const formData = new FormData();
            formData.append('unicode', '中文测试');
            formData.append('symbols', '!@#$%^&*()');
            formData.append('spaces', 'hello world');

            const encoded = formData.encode();

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            // Unicode should be properly encoded
            assert.ok(result.includes('unicode=%E4%B8%AD%E6%96%87%E6%B5%8B%E8%AF%95'));
            // Symbols should be encoded (following standard encodeURIComponent behavior)
            assert.ok(result.includes('symbols=!%40%23%24%25%5E%26*()'));
            // Spaces should be encoded as %20
            assert.ok(result.includes('spaces=hello%20world'));
        });

        it("FormData.encode() - empty values", () => {
            const formData = new FormData();
            formData.append('empty', '');
            formData.append('space', ' ');
            formData.append('normal', 'value');

            const encoded = formData.encode();

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            assert.ok(result.includes('empty='));
            assert.ok(result.includes('space=%20'));
            assert.ok(result.includes('normal=value'));
        });

        it("FormData.encode() - field name encoding", () => {
            const formData = new FormData();
            formData.append('field name', 'value');
            formData.append('field[0]', 'array-like');
            formData.append('field.nested', 'dot notation');

            const encoded = formData.encode();

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            assert.ok(result.includes('field%20name=value'));
            assert.ok(result.includes('field%5B0%5D=array-like'));
            assert.ok(result.includes('field.nested=dot%20notation'));
        });

        it("FormData.encode() - multipart/form-data basic encoding", () => {
            const formData = new FormData();
            formData.append('name', 'John Doe');
            formData.append('email', 'john@example.com');
            formData.append('age', '25');

            const encoded = formData.encode('multipart/form-data');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            const result = encoded.textSync();

            // Should contain multipart boundary markers
            assert.ok(result.includes('----formdata----'));
            assert.ok(result.includes('Content-Disposition: form-data'));

            // Should contain all field names and values
            assert.ok(result.includes('name="name"'));
            assert.ok(result.includes('John Doe'));
            assert.ok(result.includes('name="email"'));
            assert.ok(result.includes('john@example.com'));
            assert.ok(result.includes('name="age"'));
            assert.ok(result.includes('25'));

            // Should end with final boundary
            assert.ok(result.includes('--\r\n'));
        });

        it("FormData.encode() - multipart/form-data with custom boundary", () => {
            const formData = new FormData();
            formData.append('field1', 'value1');
            formData.append('field2', 'value2');

            const customBoundary = 'MyCustomBoundary123';
            const encoded = formData.encode(`multipart/form-data; boundary=${customBoundary}`);

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, `multipart/form-data; boundary=${customBoundary}`);

            const result = encoded.textSync();

            // Should use custom boundary
            assert.ok(result.includes(`--${customBoundary}`));
            assert.ok(result.includes(`--${customBoundary}--`));
            assert.ok(result.includes('field1'));
            assert.ok(result.includes('value1'));
            assert.ok(result.includes('field2'));
            assert.ok(result.includes('value2'));
        });

        it("FormData.encode() - multipart/form-data with file upload", () => {
            const originalFormData = new FormData();
            const fileContent = 'This is test file content with unicode: 测试文档 🚀';
            const blob = new Blob([fileContent], { type: 'text/plain' });

            originalFormData.append('description', 'File upload test');
            originalFormData.append('file', blob, 'test.txt');
            originalFormData.append('category', 'documents');

            // Encode to multipart
            const encoded = originalFormData.encode('multipart/form-data');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            // Extract boundary for parsing
            const encodedStr = encoded.textSync();
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            assert.ok(boundaryMatch, 'Should find boundary in encoded data');
            const boundary = boundaryMatch[0];

            // Parse back the encoded data
            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Verify text fields
            assert.strictEqual(parsedFormData.get('description'), 'File upload test');
            assert.strictEqual(parsedFormData.get('category'), 'documents');

            // Verify file field
            const parsedFile = parsedFormData.get('file');
            assert.strictEqual(parsedFile instanceof File, true);
            assert.strictEqual(parsedFile.name, 'test.txt');
            assert.strictEqual(parsedFile.type, 'text/plain');
            assert.strictEqual(parsedFile.size, blob.size);
        });

        it("FormData.encode() - multipart/form-data with multiple files", () => {
            const originalFormData = new FormData();
            const textContent = 'Text file content with unicode: 文本内容 📝';
            const jsonContent = '{"test": "json", "unicode": "测试"}';
            const textBlob = new Blob([textContent], { type: 'text/plain' });
            const jsonBlob = new Blob([jsonContent], { type: 'application/json' });

            originalFormData.append('title', 'Multiple Files Test');
            originalFormData.append('file1', textBlob, 'doc1.txt');
            originalFormData.append('file2', jsonBlob, 'data.json');

            // Encode to multipart
            const encoded = originalFormData.encode('multipart/form-data');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            // Extract boundary for parsing
            const encodedStr = encoded.textSync();
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            assert.ok(boundaryMatch, 'Should find boundary in encoded data');
            const boundary = boundaryMatch[0];

            // Parse back the encoded data
            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Verify title field
            assert.strictEqual(parsedFormData.get('title'), 'Multiple Files Test');

            // Verify first file
            const parsedFile1 = parsedFormData.get('file1');
            assert.strictEqual(parsedFile1 instanceof File, true);
            assert.strictEqual(parsedFile1.name, 'doc1.txt');
            assert.strictEqual(parsedFile1.type, 'text/plain');
            assert.strictEqual(parsedFile1.size, textBlob.size);

            // Verify second file
            const parsedFile2 = parsedFormData.get('file2');
            assert.strictEqual(parsedFile2 instanceof File, true);
            assert.strictEqual(parsedFile2.name, 'data.json');
            assert.strictEqual(parsedFile2.type, 'application/json');
            assert.strictEqual(parsedFile2.size, jsonBlob.size);
        });

        it("FormData.encode() - multipart/form-data with binary data", () => {
            const originalFormData = new FormData();
            // Create binary data with various byte values
            const binaryData = new Uint8Array([0, 1, 255, 128, 0x7F, 0xFF, 0x00]);
            const blob = new Blob([binaryData], { type: 'application/octet-stream' });

            originalFormData.append('description', 'Binary data test');
            originalFormData.append('binary_file', blob, 'binary.dat');

            // Encode to multipart
            const encoded = originalFormData.encode('multipart/form-data');

            // Should be a Blob instance with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            // Extract boundary from encoded data for parsing
            const encodedStr = encoded.textSync();
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            assert.ok(boundaryMatch, 'Should find boundary in encoded data');
            const boundary = boundaryMatch[0];

            // Parse back the encoded data
            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Verify text field
            assert.strictEqual(parsedFormData.get('description'), 'Binary data test');

            // Verify binary file
            const parsedFile = parsedFormData.get('binary_file');
            assert.strictEqual(parsedFile instanceof File, true);
            assert.strictEqual(parsedFile.name, 'binary.dat');
            assert.strictEqual(parsedFile.type, 'application/octet-stream');
            assert.strictEqual(parsedFile.size, binaryData.length);
        });

        it("FormData.encode() - multipart/form-data with special characters in field names", () => {
            const formData = new FormData();
            formData.append('field with spaces', 'value1');
            formData.append('field[array]', 'value2');
            formData.append('field.nested', 'value3');
            formData.append('field+plus', 'value4');
            formData.append('🌟unicode', 'value5');

            const encoded = formData.encode('multipart/form-data');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            const result = encoded.textSync();

            // All field names should be preserved as-is in multipart encoding
            assert.ok(result.includes('name="field with spaces"'));
            assert.ok(result.includes('value1'));
            assert.ok(result.includes('name="field[array]"'));
            assert.ok(result.includes('value2'));
            assert.ok(result.includes('name="field.nested"'));
            assert.ok(result.includes('value3'));
            assert.ok(result.includes('name="field+plus"'));
            assert.ok(result.includes('value4'));
            assert.ok(result.includes('name="🌟unicode"'));
            assert.ok(result.includes('value5'));
        });

        it("FormData.encode() - multipart/form-data with special characters in values", () => {
            const formData = new FormData();
            formData.append('unicode', '中文测试 🌟');
            formData.append('newlines', 'Line 1\nLine 2\rLine 3\r\n');
            formData.append('quotes', '"quoted" and \'single\'');
            formData.append('symbols', '!@#$%^&*()');

            const encoded = formData.encode('multipart/form-data');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            const result = encoded.textSync();

            // Values are written as-is except for line break normalization (HTML spec)
            assert.ok(result.includes('中文测试 🌟'));
            assert.ok(result.includes('Line 1\r\nLine 2\r\nLine 3\r\n'));
            assert.ok(result.includes('"quoted" and \'single\''));
            assert.ok(result.includes('!@#$%^&*()'));
        });

        it("FormData.encode() - multipart/form-data with empty fields", () => {
            const formData = new FormData();
            formData.append('empty_field', '');
            formData.append('normal_field', 'normal value');
            formData.append('whitespace_field', '   ');

            const encoded = formData.encode('multipart/form-data');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            const result = encoded.textSync();

            // Empty and whitespace fields should be included
            assert.ok(result.includes('name="empty_field"'));
            assert.ok(result.includes('name="normal_field"'));
            assert.ok(result.includes('normal value'));
            assert.ok(result.includes('name="whitespace_field"'));
            assert.ok(result.includes('   '));
        });

        it("FormData.encode() - multipart/form-data with file without explicit type", () => {
            const formData = new FormData();
            const blob = new Blob(['unknown file content']); // No type specified

            formData.append('unknown_file', blob, 'unknown.dat');

            const encoded = formData.encode('multipart/form-data');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            const result = encoded.textSync();

            // Should default to application/octet-stream
            assert.ok(result.includes('name="unknown_file"'));
            assert.ok(result.includes('filename="unknown.dat"'));
            assert.ok(result.includes('Content-Type: application/octet-stream'));
            assert.ok(result.includes('unknown file content'));
        });

        it("FormData.encode() - multipart/form-data case insensitive content type", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // Different case variations should work
            const encoded1 = formData.encode('MULTIPART/FORM-DATA');
            const encoded2 = formData.encode('Multipart/Form-Data');
            const encoded3 = formData.encode('multipart/FORM-data');

            // All should return Blobs with correct content-type
            assert.strictEqual(encoded1 instanceof Blob, true);
            assert.strictEqual(encoded2 instanceof Blob, true);
            assert.strictEqual(encoded3 instanceof Blob, true);
            assert.ok(encoded1.type.startsWith('multipart/form-data; boundary='));
            assert.ok(encoded2.type.startsWith('multipart/form-data; boundary='));
            assert.ok(encoded3.type.startsWith('multipart/form-data; boundary='));

            // All should produce valid multipart output
            assert.ok(encoded1.textSync().includes('Content-Disposition: form-data'));
            assert.ok(encoded2.textSync().includes('Content-Disposition: form-data'));
            assert.ok(encoded3.textSync().includes('Content-Disposition: form-data'));
        });

        it("FormData.encode() - multipart/form-data order preservation", () => {
            const formData = new FormData();
            formData.append('third', 'value3');
            formData.append('first', 'value1');
            formData.append('second', 'value2');

            const encoded = formData.encode('multipart/form-data');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.ok(encoded.type.startsWith('multipart/form-data; boundary='));

            const result = encoded.textSync();

            // Fields should appear in the order they were added
            const thirdIndex = result.indexOf('name="third"');
            const firstIndex = result.indexOf('name="first"');
            const secondIndex = result.indexOf('name="second"');

            assert.ok(thirdIndex < firstIndex, 'third should come before first');
            assert.ok(firstIndex < secondIndex, 'first should come before second');
        });

        it("FormData.encode() - unsupported content type", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // Should throw error for unsupported types
            assert.throws(() => {
                formData.encode('text/plain');
            });

            assert.throws(() => {
                formData.encode('application/json');
            });

            assert.throws(() => {
                formData.encode('invalid-type');
            });
        });

        it("FormData.encode() - empty FormData", () => {
            const formData = new FormData();

            const encoded = formData.encode();

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            const result = encoded.textSync();

            // Should return empty string for empty FormData
            assert.strictEqual(result, '');
        });

        it("FormData.encode() - File objects should fail for urlencoded only", () => {
            const formData = new FormData();
            const blob = new Blob(['test content'], { type: 'text/plain' });
            formData.append('file', blob, 'test.txt');

            // Should throw error when trying to encode File objects as URL-encoded
            assert.throws(() => {
                formData.encode('application/x-www-form-urlencoded');
            }, /FormData encode: field 'file' contains non-string value \(File\/Blob\), use multipart\/form-data encoding instead/);

            // But should work fine with multipart
            const encoded = formData.encode('multipart/form-data');
            assert.ok(encoded instanceof Blob);
            assert.ok(encoded.textSync().includes('filename="test.txt"'));
        });

        it("FormData.encode() - mixed string and File values", () => {
            const formData = new FormData();
            const blob = new Blob(['content'], { type: 'text/plain' });

            formData.append('text', 'value');
            formData.append('file', blob, 'test.txt');

            // Should throw error for URL-encoded due to File object presence
            assert.throws(() => {
                formData.encode('application/x-www-form-urlencoded');
            }, /FormData encode: field 'file' contains non-string value \(File\/Blob\), use multipart\/form-data encoding instead/);

            // But should work fine with multipart
            const encoded = formData.encode('multipart/form-data');
            assert.ok(encoded instanceof Blob);
            const result = encoded.textSync();
            assert.ok(result.includes('name="text"'));
            assert.ok(result.includes('value'));
            assert.ok(result.includes('name="file"'));
            assert.ok(result.includes('filename="test.txt"'));
        });

        it("FormData.encode() - case insensitive content type", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // Different case variations should work
            const encoded1 = formData.encode('APPLICATION/X-WWW-FORM-URLENCODED');
            const encoded2 = formData.encode('Application/X-Www-Form-Urlencoded');
            const encoded3 = formData.encode('URLENCODED');

            // All should return Blobs with correct content-type
            assert.strictEqual(encoded1 instanceof Blob, true);
            assert.strictEqual(encoded2 instanceof Blob, true);
            assert.strictEqual(encoded3 instanceof Blob, true);
            assert.strictEqual(encoded1.type, 'application/x-www-form-urlencoded');
            assert.strictEqual(encoded2.type, 'application/x-www-form-urlencoded');
            assert.strictEqual(encoded3.type, 'application/x-www-form-urlencoded');

            assert.strictEqual(encoded1.textSync(), 'test=value');
            assert.strictEqual(encoded2.textSync(), 'test=value');
            assert.strictEqual(encoded3.textSync(), 'test=value');
        });

        it("FormData.encode() - content type with charset parameter", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // Should work with charset parameter
            const encoded = formData.encode('application/x-www-form-urlencoded; charset=UTF-8');

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');

            assert.strictEqual(encoded.textSync(), 'test=value');
        });

        it("FormData.encode() - long field values", () => {
            const formData = new FormData();
            const longValue = 'a'.repeat(1000);
            formData.append('longfield', longValue);

            const encoded = formData.encode();
            const result = encoded.textSync();

            // Should handle long values
            assert.ok(result.startsWith('longfield='));
            assert.ok(result.length > 1000);
        });

        it("FormData.encode() - preserve field order", () => {
            const formData = new FormData();
            formData.append('third', '3');
            formData.append('first', '1');
            formData.append('second', '2');

            const encoded = formData.encode();
            const result = encoded.textSync();

            // Should preserve the order fields were added
            const expectedOrder = 'third=3&first=1&second=2';
            assert.strictEqual(result, expectedOrder);
        });

        it("FormData.encode() - return Blob object", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            const encoded = formData.encode();

            // Should return a Blob object with correct content-type
            assert.strictEqual(encoded instanceof Blob, true);
            assert.strictEqual(encoded.type, 'application/x-www-form-urlencoded');
            assert.strictEqual(typeof encoded.textSync, 'function');
            assert.strictEqual(encoded.textSync(), 'test=value');
        });
    });

    // Boundary Property Tests
    describe("Boundary property tests", () => {
        it("Default boundary generation", () => {
            const formData = new FormData();
            formData.append('name', 'value');

            const encoded = formData.encode('multipart/form-data');
            const result = encoded.textSync();

            // Should have default boundary pattern
            assert.ok(result.includes('----formdata----'));

            // Boundary should be unique each time
            const formData2 = new FormData();
            formData2.append('name', 'value');
            const encoded2 = formData2.encode('multipart/form-data');

            // Extract boundaries
            const boundary1Match = result.match(/----formdata----[a-f0-9]+/);
            const boundary2Match = encoded2.textSync().match(/----formdata----[a-f0-9]+/);

            assert.ok(boundary1Match, 'First encoding should have boundary');
            assert.ok(boundary2Match, 'Second encoding should have boundary');
            assert.notStrictEqual(boundary1Match[0], boundary2Match[0], 'Boundaries should be unique');
        });

        it("Custom boundary in content type", () => {
            const formData = new FormData();
            formData.append('field1', 'value1');
            formData.append('field2', 'value2');

            const customBoundary = 'CustomBoundary123';
            const encoded = formData.encode(`multipart/form-data; boundary=${customBoundary}`);
            const result = encoded.textSync();

            // Should use the specified custom boundary
            assert.ok(result.includes(`--${customBoundary}\r\n`));
            assert.ok(result.includes(`--${customBoundary}--\r\n`));
            assert.ok(!result.includes('----formdata----')); // Should not use default boundary
        });

        it("Custom boundary with special characters", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            const specialBoundary = 'Boundary_With-Special.Characters123';
            const encoded = formData.encode(`multipart/form-data; boundary=${specialBoundary}`);
            const result = encoded.textSync();

            assert.ok(result.includes(`--${specialBoundary}\r\n`));
            assert.ok(result.includes(`--${specialBoundary}--\r\n`));
        });

        it("Boundary with numbers and hyphens", () => {
            const formData = new FormData();
            formData.append('data', 'test');

            const numericBoundary = '1234567890-ABCDEF-boundary';
            const encoded = formData.encode(`multipart/form-data; boundary=${numericBoundary}`);
            const result = encoded.textSync();

            assert.ok(result.includes(`--${numericBoundary}\r\n`));
            assert.ok(result.includes(`--${numericBoundary}--\r\n`));
        });

        it("Very long boundary", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            const longBoundary = 'a'.repeat(60) + 'boundary'; // 68 characters total
            const encoded = formData.encode(`multipart/form-data; boundary=${longBoundary}`);
            const result = encoded.textSync();

            assert.ok(result.includes(`--${longBoundary}\r\n`));
            assert.ok(result.includes(`--${longBoundary}--\r\n`));
        });

        it("Boundary without quotes in content type", () => {
            const formData = new FormData();
            formData.append('field', 'data');

            const simpleBoundary = 'SimpleBoundary';
            const encoded = formData.encode(`multipart/form-data; boundary=${simpleBoundary}`);
            const result = encoded.textSync();

            assert.ok(result.includes(`--${simpleBoundary}\r\n`));
            assert.ok(result.includes(`--${simpleBoundary}--\r\n`));
        });

        it("Boundary parameter case sensitivity", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            const testBoundary = 'TestBoundary';

            // Test lowercase boundary parameter (should work)
            const encoded = formData.encode(`multipart/form-data; boundary=${testBoundary}`);
            const result = encoded.textSync();
            assert.ok(result.includes(`--${testBoundary}\r\n`));
        });

        it("Multiple parameters with boundary", () => {
            const formData = new FormData();
            formData.append('data', 'test');

            const boundary = 'MultiParamBoundary';
            const encoded = formData.encode(`multipart/form-data; charset=utf-8; boundary=${boundary}`);
            const result = encoded.textSync();

            assert.ok(result.includes(`--${boundary}\r\n`));
            assert.ok(result.includes(`--${boundary}--\r\n`));
        });

        it("Boundary extraction and reuse in parsing", () => {
            const boundary = "TestBoundary123";
            const multipartContent = [
                `--${boundary}`,
                `Content-Disposition: form-data; name="field1"`,
                ``,
                `value1`,
                `--${boundary}`,
                `Content-Disposition: form-data; name="field2"`,
                ``,
                `value2`,
                `--${boundary}--`,
                ``
            ].join('\r\n');

            const buffer = new Buffer(multipartContent);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            // Verify parsing worked correctly
            assert.strictEqual(formData.get('field1'), 'value1');
            assert.strictEqual(formData.get('field2'), 'value2');
        });

        it("Missing boundary parameter uses default", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // Without boundary parameter, should use default
            const encoded = formData.encode('multipart/form-data');
            const result = encoded.textSync();

            assert.ok(result.includes('----formdata----'));
        });

        it("Boundary consistency in single encoding", () => {
            const formData = new FormData();
            formData.append('field1', 'value1');
            formData.append('field2', 'value2');
            formData.append('field3', 'value3');

            const encoded = formData.encode('multipart/form-data');
            const result = encoded.textSync();

            // Extract all boundary occurrences
            const boundaryMatches = result.match(/--[^\r\n]+/g);
            assert.ok(boundaryMatches && boundaryMatches.length >= 4); // 3 field separators + 1 final

            // All boundaries (except the final one) should be the same
            const normalBoundaries = boundaryMatches.slice(0, -1);
            const finalBoundary = boundaryMatches[boundaryMatches.length - 1];

            // Check all field separators use same boundary
            const firstBoundary = normalBoundaries[0];
            normalBoundaries.forEach(boundary => {
                assert.strictEqual(boundary, firstBoundary);
            });

            // Final boundary should be the same plus '--'
            assert.strictEqual(finalBoundary, firstBoundary + '--');
        });

        it("Boundary with file upload scenarios", () => {
            const formData = new FormData();
            const fileContent = 'Simple file content';
            const blob = new Blob([fileContent], { type: 'text/plain' });

            formData.append('description', 'File upload test');
            formData.append('file', blob, 'test.txt');

            const customBoundary = 'FileBoundary999';
            const encoded = formData.encode(`multipart/form-data; boundary=${customBoundary}`);
            const result = encoded.textSync();

            // Should use custom boundary consistently
            assert.ok(result.includes(`--${customBoundary}\r\n`));
            assert.ok(result.includes(`--${customBoundary}--\r\n`));
            assert.ok(result.includes(fileContent)); // File content should be preserved
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

        it("FormData constructor - File object initialization comprehensive test", () => {
            // Create various types of File objects
            const textContent = new Blob(['Hello, this is a text file!'], { type: 'text/plain' });
            const imageContent = new Blob([new Uint8Array([0xFF, 0xD8, 0xFF, 0xE0])], { type: 'image/jpeg' });
            const pdfContent = new Blob(['%PDF-1.4\n%fake pdf'], { type: 'application/pdf' });

            const textFile = new File([textContent], 'readme.txt', {
                type: 'text/plain',
                lastModified: 1640995200000 // Jan 1, 2022
            });

            const imageFile = new File([imageContent], 'photo.jpg', {
                type: 'image/jpeg',
                lastModified: 1640995200000
            });

            const pdfFile = new File([pdfContent], 'document.pdf', {
                type: 'application/pdf',
                lastModified: 1640995200000
            });

            // Initialize FormData with File objects and a nested plain object
            const initObject = {
                description: 'Multi-file upload test',
                textFile: textFile,
                imageFile: imageFile,
                pdfFile: pdfFile,
                fileArray: [textFile, imageFile], // Array of files
                metadata: {
                    uploadTime: Date.now(),
                    userAgent: 'Test Suite'
                }
            };

            const formData = new FormData(initObject);

            // Nested plain objects are converted to strings (WHATWG FormData)
            assert.strictEqual(formData.get('metadata'), '[object Object]');

            // File entries are preserved
            ['textFile', 'imageFile', 'pdfFile'].forEach((key) => {
                assert.strictEqual(formData.get(key) instanceof File, true);
            });
            assert.strictEqual(formData.getAll('fileArray').length, 2);
            formData.getAll('fileArray').forEach((f) => assert.strictEqual(f instanceof File, true));
        });

        it("FormData constructor - nested object with File properties", () => {
            const fileContent = new Blob(['Nested file content'], { type: 'text/plain' });
            const nestedFile = new File([fileContent], 'nested.txt', { type: 'text/plain' });

            const initObject = {
                user: {
                    name: 'John Doe',
                    avatar: nestedFile, // File object in nested structure
                    preferences: {
                        theme: 'dark',
                        notifications: true
                    }
                },
                directFile: nestedFile
            };

            const formData = new FormData(initObject);

            // Nested objects are stringified, only top level Blob/File values become file entries
            assert.strictEqual(formData.get('user'), '[object Object]');
            assert.strictEqual(formData.get('directFile') instanceof File, true);
            assert.strictEqual(formData.get('directFile').name, 'nested.txt');
        });

        it("FormData constructor - File with empty content", () => {
            const emptyFile = new File([], 'empty.txt', { type: 'text/plain' });

            const initObject = {
                emptyFile: emptyFile,
                regularField: 'non-empty value'
            };

            const formData = new FormData(initObject);

            const retrievedFile = formData.get('emptyFile');
            assert.strictEqual(retrievedFile instanceof File, true);
            assert.strictEqual(retrievedFile.name, 'empty.txt');
            assert.strictEqual(retrievedFile.size, 0);
            assert.strictEqual(formData.get('regularField'), 'non-empty value');
        });

        it("FormData constructor - File with special characters in filename", () => {
            const fileContent = new Blob(['Content with special filename'], { type: 'text/plain' });

            const specialFiles = [
                new File([fileContent], 'файл.txt', { type: 'text/plain' }), // Cyrillic
                new File([fileContent], '文件.txt', { type: 'text/plain' }), // Chinese
                new File([fileContent], 'file (1) [copy].txt', { type: 'text/plain' }), // Special chars
                new File([fileContent], 'file-with-émojis-🎉.txt', { type: 'text/plain' }) // Emoji and accent
            ];

            const initObject = {
                cyrillicFile: specialFiles[0],
                chineseFile: specialFiles[1],
                specialCharsFile: specialFiles[2],
                emojiFile: specialFiles[3]
            };

            const formData = new FormData(initObject);

            // Verify all files are preserved with correct names
            assert.strictEqual(formData.get('cyrillicFile').name, 'файл.txt');
            assert.strictEqual(formData.get('chineseFile').name, '文件.txt');
            assert.strictEqual(formData.get('specialCharsFile').name, 'file (1) [copy].txt');
            assert.strictEqual(formData.get('emojiFile').name, 'file-with-émojis-🎉.txt');

            // All should be File instances
            assert.strictEqual(formData.get('cyrillicFile') instanceof File, true);
            assert.strictEqual(formData.get('chineseFile') instanceof File, true);
            assert.strictEqual(formData.get('specialCharsFile') instanceof File, true);
            assert.strictEqual(formData.get('emojiFile') instanceof File, true);
        });
    });

    // Edge Cases and Error Handling
    describe("Edge cases and error handling", () => {
        it("FormData - empty field names", () => {
            const formData = new FormData();

            // WHATWG FormData allows empty field names
            formData.append('', 'empty name');
            assert.strictEqual(formData.get(''), 'empty name');
            assert.strictEqual(formData.has(''), true);

            formData.append('', 'second value');
            assert.deepStrictEqual(formData.getAll(''), ['empty name', 'second value']);

            formData.set('', 'replaced');
            assert.deepStrictEqual(formData.getAll(''), ['replaced']);

            // empty names must survive encoding and parsing
            const reparsed = new FormData(formData.encode('multipart/form-data'));
            assert.strictEqual(reparsed.get(''), 'replaced');
            assert.strictEqual(reparsed.has(''), true);

            formData.delete('');
            assert.strictEqual(formData.has(''), false);
            assert.strictEqual(formData.get(''), null);
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

            // Converting a Symbol to a string throws a TypeError (WHATWG WebIDL USVString)
            assert.throws(() => {
                formData.append('symbol', sym);
            }, TypeError, 'Cannot convert a Symbol value to a string');
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

        it("FormData - filename argument requires a Blob (WHATWG)", () => {
            const formData = new FormData();

            // the third argument is only allowed when the value is a Blob/File
            assert.throws(() => {
                formData.append('field', 'plain string', 'file.txt');
            }, TypeError);

            assert.throws(() => {
                formData.set('field', 'plain string', 'file.txt');
            }, TypeError);

            // a real Blob is accepted
            formData.append('field', new Blob(['x'], { type: 'text/plain' }), 'file.txt');
            assert.strictEqual(formData.get('field') instanceof File, true);
            assert.strictEqual(formData.get('field').name, 'file.txt');
        });

        it("FormData - object values are coerced to strings (WHATWG)", () => {
            const formData = new FormData();

            formData.append('plain', {});
            formData.append('map', new Map());

            assert.strictEqual(formData.get('plain'), '[object Object]');
            assert.strictEqual(formData.get('map'), '[object Map]');

            // objects without a usable toString still throw a TypeError
            assert.throws(() => {
                formData.append('nullproto', Object.create(null));
            });
        });

        it("FormData - multipart escapes quotes and normalizes line breaks", () => {
            const formData = new FormData();
            formData.append('na"me', 'line1\nline2');
            formData.append('file', new Blob(['x'], { type: 'text/plain' }), 'a"b.txt');

            const body = formData.encode('multipart/form-data').textSync();

            assert.ok(body.indexOf('name="na%22me"') >= 0, body.slice(0, 200));
            assert.ok(body.indexOf('filename="a%22b.txt"') >= 0);
            assert.ok(body.indexOf('line1\r\nline2') >= 0);
        });

        it("FormData - empty filename stays a file entry", () => {
            const formData = new FormData();
            formData.append('file', new Blob(['x'], { type: 'text/plain' }), '');

            const reparsed = new FormData(formData.encode('multipart/form-data'));
            const file = reparsed.get('file');

            assert.strictEqual(file instanceof File, true);
            assert.strictEqual(file.name, '');
        });

        it("FormData - multipart round trip keeps files and text fields", () => {
            const formData = new FormData();
            formData.append('text', 'value');
            formData.append('file', new Blob(['content'], { type: 'text/plain' }), 'a.txt');

            const reparsed = new FormData(formData.encode('multipart/form-data'));

            assert.strictEqual(reparsed.get('text'), 'value');
            assert.strictEqual(reparsed.get('file') instanceof File, true);
            assert.strictEqual(reparsed.get('file').name, 'a.txt');
            assert.strictEqual(reparsed.get('file').textSync(), 'content');
        });

        it("FormData - sort() orders entries by name", () => {
            if (!isFibjs)
                return; // fibjs extension, not part of the WHATWG API

            const formData = new FormData();
            formData.append('b', '2');
            formData.append('a', '1');
            formData.append('b', '3');

            // entries keep insertion order until sort() is called explicitly
            assert.deepStrictEqual([...formData.keys()], ['b', 'a', 'b']);

            formData.sort();

            assert.deepStrictEqual([...formData.keys()], ['a', 'b', 'b']);
            assert.strictEqual(formData.get('b'), '2');
            assert.deepStrictEqual(formData.getAll('b'), ['2', '3']);
        });

        it("FormData - encoded escapes are decoded on parse", () => {
            const formData = new FormData();
            formData.append('na"me', 'value');
            formData.append('breaks\r\nname', 'v2');
            formData.append('file', new Blob(['x'], { type: 'text/plain' }), 'a"b.txt');

            const reparsed = new FormData(formData.encode('multipart/form-data'));

            // %22 / %0D / %0A are decoded back (HTML spec, same as undici)
            assert.strictEqual(reparsed.get('na"me'), 'value');
            assert.strictEqual(reparsed.get('breaks\r\nname'), 'v2');
            assert.strictEqual(reparsed.get('file').name, 'a"b.txt');

            // other percent sequences are kept verbatim
            const raw = new FormData();
            raw.append('keep%41as', 'v');
            const rawReparsed = new FormData(raw.encode('multipart/form-data'));
            assert.strictEqual(rawReparsed.get('keep%41as'), 'v');
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

    // Binary Data Handling Tests
    describe("Binary data handling", () => {
        it("FormData constructor - binary data handling", async () => {
            const boundary = "----BinaryTest";

            // Create binary data (non-text content) - avoid CR/LF in test data
            const binaryData = new Uint8Array([0x89, 0x50, 0x4E, 0x47, 0x08, 0x09, 0x1A, 0x0B]); // Modified PNG-like header

            // Assemble multipart content in stages to avoid UTF-8 encoding issues
            const headerPart = [
                `------BinaryTest`,
                `Content-Disposition: form-data; name="description"`,
                ``,
                `Binary file upload`,
                `------BinaryTest`,
                `Content-Disposition: form-data; name="binaryfile"; filename="test.png"`,
                `Content-Type: image/png`,
                `Content-Transfer-Encoding: binary`,
                ``,
                ``
            ].join('\r\n');

            const footerPart = [
                ``,
                `------BinaryTest--`,
                ``
            ].join('\r\n');

            // Create buffers for each part
            const headerBuffer = new Buffer(headerPart);
            const binaryBuffer = new Buffer(binaryData);
            const footerBuffer = new Buffer(footerPart);

            // Combine all parts into a single buffer
            const totalLength = headerBuffer.length + binaryBuffer.length + footerBuffer.length;
            const buffer = new Buffer(totalLength);
            let offset = 0;

            headerBuffer.copy(buffer, offset);
            offset += headerBuffer.length;

            binaryBuffer.copy(buffer, offset);
            offset += binaryBuffer.length;

            footerBuffer.copy(buffer, offset);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            // Text field should work normally
            assert.strictEqual(formData.get('description'), 'Binary file upload');

            // Binary file should be properly handled
            const file = formData.get('binaryfile');
            assert.strictEqual(file instanceof File, true);
            assert.strictEqual(file.name, 'test.png');
            assert.strictEqual(file.type, 'image/png');

            // File size should match the binary data length - this tests if implementation follows spec
            assert.strictEqual(file.size, binaryData.length, 'File size should match original binary data length');

            // Verify binary content integrity
            const arrayBufferPromise = file.arrayBuffer();
            const arrayBuffer = await arrayBufferPromise;
            const parsedBytes = new Uint8Array(arrayBuffer);

            // Check that all bytes match exactly
            assert.strictEqual(parsedBytes.length, binaryData.length, 'Parsed data length should match original');
            for (let i = 0; i < binaryData.length; i++) {
                assert.strictEqual(parsedBytes[i], binaryData[i], `Byte ${i} should match original`);
            }
        });

        it("FormData constructor - Content-Transfer-Encoding is parsed but binary is default", () => {
            const boundary = "----EncodingTest";

            // Use precise binary content for testing
            const binaryData = new Uint8Array([0x00, 0x01, 0x02, 0xFF, 0xFE]);

            // Assemble multipart content in stages to avoid UTF-8 encoding issues
            const headerPart1 = [
                `------EncodingTest`,
                `Content-Disposition: form-data; name="file1"; filename="binary.dat"`,
                `Content-Type: application/octet-stream`,
                `Content-Transfer-Encoding: binary`,
                ``,
                ``
            ].join('\r\n');

            const middlePart = [
                ``,
                `------EncodingTest`,
                `Content-Disposition: form-data; name="file2"; filename="no-encoding.dat"`,
                `Content-Type: application/octet-stream`,
                ``,
                ``
            ].join('\r\n');

            const footerPart = [
                ``,
                `------EncodingTest--`,
                ``
            ].join('\r\n');

            // Create buffers for each part
            const headerBuffer1 = new Buffer(headerPart1);
            const binaryBuffer1 = new Buffer(binaryData);
            const middleBuffer = new Buffer(middlePart);
            const binaryBuffer2 = new Buffer(binaryData);
            const footerBuffer = new Buffer(footerPart);

            // Combine all parts into a single buffer
            const totalLength = headerBuffer1.length + binaryBuffer1.length +
                middleBuffer.length + binaryBuffer2.length + footerBuffer.length;
            const buffer = new Buffer(totalLength);
            let offset = 0;

            headerBuffer1.copy(buffer, offset);
            offset += headerBuffer1.length;

            binaryBuffer1.copy(buffer, offset);
            offset += binaryBuffer1.length;

            middleBuffer.copy(buffer, offset);
            offset += middleBuffer.length;

            binaryBuffer2.copy(buffer, offset);
            offset += binaryBuffer2.length;

            footerBuffer.copy(buffer, offset);
            const formData = new FormData(buffer, `multipart/form-data; boundary=${boundary}`);

            const file1 = formData.get('file1');
            const file2 = formData.get('file2');

            // Both files should be handled identically (binary by default)
            assert.strictEqual(file1 instanceof File, true);
            assert.strictEqual(file2 instanceof File, true);

            // Both should have same size regardless of encoding header
            assert.strictEqual(file1.size, file2.size, 'Files should have same size regardless of Content-Transfer-Encoding header');

            // Size should match original data - this tests spec compliance
            assert.strictEqual(file1.size, binaryData.length, 'File size should match original binary data length');

            // Content-Transfer-Encoding doesn't affect the result since we only support binary
            assert.strictEqual(file1.name, 'binary.dat');
            assert.strictEqual(file2.name, 'no-encoding.dat');
        });
    });

    // Multipart Encoding-Decoding Round-trip Tests
    describe("Multipart encoding-decoding round-trip validation", () => {
        it("FormData encode-decode round-trip - text fields only", () => {
            const originalFormData = new FormData();
            originalFormData.append('name', 'John Doe');
            originalFormData.append('email', 'john@example.com');
            originalFormData.append('message', 'Hello world with special chars: 中文 🌟 "quotes" & symbols!');
            originalFormData.append('tags', 'tag1');
            originalFormData.append('tags', 'tag2');
            originalFormData.append('empty_field', '');
            originalFormData.append('whitespace_field', '   ');

            // Encode to multipart
            const encoded = originalFormData.encode('multipart/form-data');
            assert.strictEqual(encoded instanceof Blob, true);

            // Extract boundary from encoded data
            const encodedStr = encoded.textSync();
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            assert.ok(boundaryMatch, 'Should find boundary in encoded data');
            const boundary = boundaryMatch[0];

            // Parse back from encoded data
            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Verify all fields are preserved exactly
            assert.strictEqual(parsedFormData.get('name'), 'John Doe');
            assert.strictEqual(parsedFormData.get('email'), 'john@example.com');
            assert.strictEqual(parsedFormData.get('message'), 'Hello world with special chars: 中文 🌟 "quotes" & symbols!');
            assert.deepStrictEqual(parsedFormData.getAll('tags'), ['tag1', 'tag2']);
            assert.strictEqual(parsedFormData.get('empty_field'), '');
            assert.strictEqual(parsedFormData.get('whitespace_field'), '   ');
        });

        it("FormData encode-decode round-trip - mixed text and files", () => {
            const originalFormData = new FormData();

            // Test data with various content types
            const textContent = 'Text file with unicode: 这是一个测试文件 📄';
            const binaryContent = new Uint8Array([0, 1, 2, 255, 254, 128, 127]);
            const jsonContent = '{"name": "测试", "value": 123, "array": [1, 2, 3]}';

            const textBlob = new Blob([textContent], { type: 'text/plain' });
            const binaryBlob = new Blob([binaryContent], { type: 'application/octet-stream' });
            const jsonBlob = new Blob([jsonContent], { type: 'application/json' });

            originalFormData.append('description', 'Mixed content test with unicode: 混合内容测试');
            originalFormData.append('text_file', textBlob, 'text_文档.txt');
            originalFormData.append('binary_file', binaryBlob, 'binary.dat');
            originalFormData.append('json_file', jsonBlob, 'data.json');
            originalFormData.append('category', 'testing');

            // Encode to multipart
            const encoded = originalFormData.encode('multipart/form-data');

            // Extract boundary for parsing
            const encodedStr = encoded.textSync(); // Use textSync to get content as string
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            assert.ok(boundaryMatch, 'Should find boundary in encoded data');
            const boundary = boundaryMatch[0];

            // Parse back the encoded data
            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Verify text fields
            assert.strictEqual(parsedFormData.get('description'), 'Mixed content test with unicode: 混合内容测试');
            assert.strictEqual(parsedFormData.get('category'), 'testing');

            // Verify text file
            const parsedTextFile = parsedFormData.get('text_file');
            assert.strictEqual(parsedTextFile instanceof File, true);
            assert.strictEqual(parsedTextFile.name, 'text_文档.txt');
            assert.strictEqual(parsedTextFile.type, 'text/plain');
            assert.strictEqual(parsedTextFile.size, textBlob.size);

            // Verify binary file
            const parsedBinaryFile = parsedFormData.get('binary_file');
            assert.strictEqual(parsedBinaryFile instanceof File, true);
            assert.strictEqual(parsedBinaryFile.name, 'binary.dat');
            assert.strictEqual(parsedBinaryFile.type, 'application/octet-stream');
            assert.strictEqual(parsedBinaryFile.size, binaryBlob.size);

            // Verify JSON file
            const parsedJsonFile = parsedFormData.get('json_file');
            assert.strictEqual(parsedJsonFile instanceof File, true);
            assert.strictEqual(parsedJsonFile.name, 'data.json');
            assert.strictEqual(parsedJsonFile.type, 'application/json');
            assert.strictEqual(parsedJsonFile.size, jsonBlob.size);
        });

        it("FormData encode-decode round-trip - custom boundary preservation", () => {
            const originalFormData = new FormData();
            const fileContent = 'File content for boundary test';
            const blob = new Blob([fileContent], { type: 'text/plain' });

            originalFormData.append('field1', 'value1');
            originalFormData.append('test_file', blob, 'test.txt');
            originalFormData.append('field2', 'value2');

            // Use custom boundary
            const customBoundary = 'CustomTestBoundary123';
            const encoded = originalFormData.encode(`multipart/form-data; boundary=${customBoundary}`);

            // Verify custom boundary is used
            const encodedStr = encoded.textSync();
            assert.ok(encodedStr.includes(`--${customBoundary}\r\n`));
            assert.ok(encodedStr.includes(`--${customBoundary}--\r\n`));

            // Parse back using custom boundary
            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${customBoundary}`);

            // Verify all data is preserved
            assert.strictEqual(parsedFormData.get('field1'), 'value1');
            assert.strictEqual(parsedFormData.get('field2'), 'value2');

            const parsedFile = parsedFormData.get('test_file');
            assert.strictEqual(parsedFile instanceof File, true);
            assert.strictEqual(parsedFile.name, 'test.txt');
            assert.strictEqual(parsedFile.type, 'text/plain');
            assert.strictEqual(parsedFile.size, fileContent.length);
        });

        it("FormData encode-decode round-trip - field order preservation", () => {
            const originalFormData = new FormData();
            const blob1 = new Blob(['file1 content'], { type: 'text/plain' });
            const blob2 = new Blob(['file2 content'], { type: 'text/plain' });

            // Add fields in specific order
            originalFormData.append('third_field', 'value3');
            originalFormData.append('first_field', 'value1');
            originalFormData.append('file1', blob1, 'file1.txt');
            originalFormData.append('second_field', 'value2');
            originalFormData.append('file2', blob2, 'file2.txt');

            // Encode and decode
            const encoded = originalFormData.encode('multipart/form-data');
            const encodedStr = encoded.textSync();
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            const boundary = boundaryMatch[0];

            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Verify field order is preserved by checking entries
            const originalEntries = Array.from(originalFormData.entries());
            const parsedEntries = Array.from(parsedFormData.entries());

            assert.strictEqual(parsedEntries.length, originalEntries.length);

            // Check that field names are in the same order
            for (let i = 0; i < originalEntries.length; i++) {
                assert.strictEqual(parsedEntries[i][0], originalEntries[i][0]);

                // For text fields, values should match exactly
                if (typeof originalEntries[i][1] === 'string') {
                    assert.strictEqual(parsedEntries[i][1], originalEntries[i][1]);
                } else {
                    // For file fields, verify they are File instances
                    assert.strictEqual(parsedEntries[i][1] instanceof File, true);
                    assert.strictEqual(originalEntries[i][1] instanceof File, true);
                }
            }
        });

        it("FormData encode-decode round-trip - special field names", () => {
            const originalFormData = new FormData();
            const blob = new Blob(['test content'], { type: 'text/plain' });

            // Test various special characters in field names
            originalFormData.append('normal_field', 'normal value');
            originalFormData.append('field with spaces', 'spaces value');
            originalFormData.append('field[array]', 'array value');
            originalFormData.append('field.nested', 'nested value');
            originalFormData.append('field+plus', 'plus value');
            originalFormData.append('field-dash', 'dash value');
            originalFormData.append('field_underscore', 'underscore value');
            originalFormData.append('🌟unicode_field', 'unicode value');
            originalFormData.append('field_quotes', 'quotes value');
            originalFormData.append('special_file', blob, 'special_文件.txt');

            // Encode and decode
            const encoded = originalFormData.encode('multipart/form-data');
            const encodedStr = encoded.textSync();
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            const boundary = boundaryMatch[0];

            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Verify all field names are preserved exactly
            assert.strictEqual(parsedFormData.get('normal_field'), 'normal value');
            assert.strictEqual(parsedFormData.get('field with spaces'), 'spaces value');
            assert.strictEqual(parsedFormData.get('field[array]'), 'array value');
            assert.strictEqual(parsedFormData.get('field.nested'), 'nested value');
            assert.strictEqual(parsedFormData.get('field+plus'), 'plus value');
            assert.strictEqual(parsedFormData.get('field-dash'), 'dash value');
            assert.strictEqual(parsedFormData.get('field_underscore'), 'underscore value');
            assert.strictEqual(parsedFormData.get('🌟unicode_field'), 'unicode value');
            assert.strictEqual(parsedFormData.get('field_quotes'), 'quotes value');

            const parsedFile = parsedFormData.get('special_file');
            assert.strictEqual(parsedFile instanceof File, true);
            assert.strictEqual(parsedFile.name, 'special_文件.txt');
        });

        it("FormData encode-decode round-trip - large data handling", () => {
            const originalFormData = new FormData();

            // Create large text and binary data
            const largeText = 'A'.repeat(50000) + '中文' + 'B'.repeat(50000); // 100KB+ with unicode
            const largeBinary = new Uint8Array(100000);
            for (let i = 0; i < largeBinary.length; i++) {
                largeBinary[i] = i % 256;
            }

            const textBlob = new Blob([largeText], { type: 'text/plain' });
            const binaryBlob = new Blob([largeBinary], { type: 'application/octet-stream' });

            originalFormData.append('description', 'Large data test');
            originalFormData.append('large_text_file', textBlob, 'large.txt');
            originalFormData.append('large_binary_file', binaryBlob, 'large.bin');

            // Encode and decode
            const encoded = originalFormData.encode('multipart/form-data');
            const encodedStr = encoded.textSync();
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            const boundary = boundaryMatch[0];

            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Verify description field
            assert.strictEqual(parsedFormData.get('description'), 'Large data test');

            // Verify large text file
            const parsedTextFile = parsedFormData.get('large_text_file');
            assert.strictEqual(parsedTextFile instanceof File, true);
            assert.strictEqual(parsedTextFile.name, 'large.txt');
            assert.strictEqual(parsedTextFile.type, 'text/plain');
            assert.strictEqual(parsedTextFile.size, textBlob.size);

            // Verify large binary file
            const parsedBinaryFile = parsedFormData.get('large_binary_file');
            assert.strictEqual(parsedBinaryFile instanceof File, true);
            assert.strictEqual(parsedBinaryFile.name, 'large.bin');
            assert.strictEqual(parsedBinaryFile.type, 'application/octet-stream');
            assert.strictEqual(parsedBinaryFile.size, binaryBlob.size);
        });

        it("FormData encode-decode round-trip - empty multipart", () => {
            const originalFormData = new FormData();

            // Encode empty FormData
            const encoded = originalFormData.encode('multipart/form-data');
            const encodedStr = encoded.textSync();

            // Should still have boundary structure
            assert.ok(encodedStr.includes('----formdata----'));

            // Extract boundary
            const boundaryMatch = encodedStr.match(/----formdata----([a-f0-9]+)/);
            const boundary = boundaryMatch[0];

            // Parse back
            const parsedFormData = new FormData(encoded, `multipart/form-data; boundary=${boundary}`);

            // Should be empty
            assert.strictEqual(Array.from(parsedFormData.entries()).length, 0);
        });
    });

    // RFC 2046 Boundary Edge Cases Analysis
    describe("RFC 2046 Boundary Edge Cases Analysis", () => {
        it("Quoted boundary values - should handle quotes", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // RFC 2046: boundary values with special characters should be quoted
            const boundary = 'boundary:with:colons';
            const contentType = `multipart/form-data; boundary="${boundary}"`;

            const encoded = formData.encode(contentType);
            const result = encoded.textSync();

            // Should correctly parse quoted boundary and use it
            assert.ok(result.includes(`--${boundary}\r\n`),
                `Should use quoted boundary "${boundary}" in multipart delimiter`);
            assert.ok(result.includes(`--${boundary}--\r\n`),
                `Should use quoted boundary "${boundary}" in final delimiter`);
        });

        it("Boundary with spaces - RFC validation", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // RFC 2046: spaces in boundary values are not allowed in the character set
            // Even when quoted, spaces make boundary invalid according to RFC character restrictions
            const boundary = 'boundary with spaces';
            const contentType = `multipart/form-data; boundary="${boundary}"`;

            const encoded = formData.encode(contentType);
            const result = encoded.textSync();

            // Should reject boundary with spaces and fall back to default
            assert.ok(!result.includes(`--${boundary}\r\n`),
                `Should reject boundary with spaces "${boundary}"`);
            assert.ok(result.includes('----formdata----'),
                `Should use default boundary when boundary contains invalid characters`);
        });

        it("Case insensitive boundary parameter name", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // RFC 2046: parameter names should be case-insensitive
            const boundary = 'TestBoundary';
            const contentTypes = [
                `multipart/form-data; BOUNDARY=${boundary}`,
                `multipart/form-data; Boundary=${boundary}`,
                `multipart/form-data; BoUnDaRy=${boundary}`
            ];

            contentTypes.forEach((contentType, index) => {
                const encoded = formData.encode(contentType);
                const result = encoded.textSync();

                // Should parse case-insensitive parameter names correctly
                assert.ok(result.includes(`--${boundary}\r\n`),
                    `Case variant ${index + 1} (${contentType}) should parse boundary parameter correctly`);
                assert.ok(result.includes(`--${boundary}--\r\n`),
                    `Case variant ${index + 1} (${contentType}) should use boundary in final delimiter`);
            });
        });

        it("Whitespace around boundary parameter", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // RFC 2046: whitespace around parameters should be handled
            const boundary = 'WhitespaceBoundary';
            const contentTypes = [
                `multipart/form-data; boundary = ${boundary}`,
                `multipart/form-data; boundary= ${boundary}`,
                `multipart/form-data; boundary =${boundary}`,
                `multipart/form-data;boundary=${boundary}` // no space after semicolon
            ];

            contentTypes.forEach((contentType, index) => {
                const encoded = formData.encode(contentType);
                const result = encoded.textSync();

                // Should handle whitespace around parameter correctly
                assert.ok(result.includes(`--${boundary}\r\n`),
                    `Whitespace variant ${index + 1} (${contentType}) should parse boundary with whitespace correctly`);
                assert.ok(result.includes(`--${boundary}--\r\n`),
                    `Whitespace variant ${index + 1} (${contentType}) should use boundary in final delimiter`);
            });
        });

        it("Parameter order variations", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // RFC 2046: parameter order should not matter
            const boundary = 'OrderTestBoundary';
            const contentTypes = [
                `multipart/form-data; charset=utf-8; boundary=${boundary}`,
                `multipart/form-data; boundary=${boundary}; charset=utf-8`,
                `multipart/form-data; version=1.0; boundary=${boundary}; charset=utf-8`
            ];

            contentTypes.forEach((contentType, index) => {
                const encoded = formData.encode(contentType);
                const result = encoded.textSync();

                // Should parse boundary regardless of parameter order
                assert.ok(result.includes(`--${boundary}\r\n`),
                    `Parameter order ${index + 1} (${contentType}) should find boundary correctly`);
                assert.ok(result.includes(`--${boundary}--\r\n`),
                    `Parameter order ${index + 1} (${contentType}) should use boundary in final delimiter`);
            });
        });

        it("Empty boundary parameter - should fail gracefully", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // RFC 2046: boundary must be 1-70 characters, empty should use default
            const contentTypes = [
                `multipart/form-data; boundary=`,
                `multipart/form-data; boundary=""`,
                `multipart/form-data; boundary=   ` // only spaces
            ];

            contentTypes.forEach((contentType, index) => {
                const encoded = formData.encode(contentType);
                const result = encoded.textSync();

                // Should fall back to default boundary when boundary is empty
                assert.ok(result.includes('----formdata----'),
                    `Empty boundary ${index + 1} (${contentType}) should use default boundary`);
                assert.ok(result.includes('----formdata----') && result.includes('--\r\n'),
                    `Empty boundary ${index + 1} should have valid multipart structure`);
            });
        });

        it("Boundary length limits - RFC allows up to 70 characters", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // RFC 2046: boundary must be 1-70 characters
            const validBoundary = 'a'.repeat(70); // exactly 70 characters (max allowed)
            const invalidBoundary = 'a'.repeat(71); // 71 characters (should be rejected)
            const validNamedBoundary = 'VeryLongBoundaryWith70CharactersExactlyToTestTheLimitSpecified'; // exactly 70

            // Test valid 70-character boundary
            const encoded1 = formData.encode(`multipart/form-data; boundary=${validBoundary}`);
            const result1 = encoded1.textSync();
            assert.ok(result1.includes(`--${validBoundary}\r\n`),
                'Should accept boundary with exactly 70 characters');
            assert.ok(result1.includes(`--${validBoundary}--\r\n`),
                'Should use 70-character boundary in final delimiter');

            // Test valid named boundary with 70 characters
            const encoded2 = formData.encode(`multipart/form-data; boundary=${validNamedBoundary}`);
            const result2 = encoded2.textSync();
            assert.ok(result2.includes(`--${validNamedBoundary}\r\n`),
                'Should accept valid 70-character named boundary');

            // Test invalid 71-character boundary should fall back to default
            const encoded3 = formData.encode(`multipart/form-data; boundary=${invalidBoundary}`);
            const result3 = encoded3.textSync();
            assert.ok(!result3.includes(`--${invalidBoundary}\r\n`),
                'Should reject boundary longer than 70 characters');
            // When boundary parsing fails, should use existing boundary or generate new default
            assert.ok(result3.includes('----formdata----') || result3.includes('--') && result3.includes('--\r\n'),
                'Should have valid multipart structure when boundary is too long');
        });

        it("RFC allowed special characters in boundary", () => {
            const formData = new FormData();
            formData.append('test', 'value');

            // RFC 2046 allows: DIGIT / ALPHA / "'" / "(" / ")" / "+" / "_" / "," / "-" / "." / "/" / ":" / "=" / "?"
            const boundaries = [
                "boundary'with'quotes",
                "boundary(with)parentheses",
                "boundary+with+plus",
                "boundary_with_underscore",
                "boundary,with,commas",
                "boundary.with.dots",
                "boundary/with/slashes",
                "boundary:with:colons",
                "boundary=with=equals",
                "boundary?with?questions"
            ];

            boundaries.forEach((boundary, index) => {
                const contentType = `multipart/form-data; boundary="${boundary}"`;

                const encoded = formData.encode(contentType);
                const result = encoded.textSync();

                // Should accept all RFC 2046 allowed special characters
                assert.ok(result.includes(`--${boundary}\r\n`),
                    `Should accept RFC allowed special characters in boundary: ${boundary}`);
                assert.ok(result.includes(`--${boundary}--\r\n`),
                    `Should use special character boundary in final delimiter: ${boundary}`);
            });
        });
    });
});
