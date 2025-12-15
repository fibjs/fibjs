// Second module - also imports shared with default import
console.log('[esm21_b] Loading...');

import TestClass from './esm21_shared.mjs';

console.log('[esm21_b] TestClass =', typeof TestClass);

export const bResult = TestClass ? new TestClass().value : 'FAILED';

console.log('[esm21_b] Done, bResult =', bResult);
