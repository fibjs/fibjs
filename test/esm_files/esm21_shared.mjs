// Shared module with default export
// Used to test that cached modules correctly export default

console.log('[esm21_shared] Loading...');

export class TestClass {
    constructor() {
        this.value = 42;
    }
}

export default TestClass;

console.log('[esm21_shared] Done');
