// First module - imports shared with default import
console.log('[esm21_a] Loading...');

import TestClass from './esm21_shared.mjs';

console.log('[esm21_a] TestClass =', typeof TestClass);

export const aResult = TestClass ? new TestClass().value : 'FAILED';

console.log('[esm21_a] Done, aResult =', aResult);
