// Secondary ESM pattern: identifier redeclaration
// `require` is a parameter in CJS wrapper, so this is a compile error in CJS.
// But in ESM, `require` is not predefined, so this is valid.
// Node.js v22.12+ detects this and loads the file as ESM.
const require = 100;

export const value = 42;
