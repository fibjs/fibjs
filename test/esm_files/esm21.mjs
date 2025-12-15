// Entry point - sequential dynamic import of modules that share a static dependency
// Tests that cached modules correctly resolve default exports

const a = await import('./esm21_a.mjs');
const b = await import('./esm21_b.mjs');

export const a_result = a.aResult;
export const b_result = b.bResult;
