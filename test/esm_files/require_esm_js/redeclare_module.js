// Secondary ESM pattern: identifier redeclaration
// `module` is a parameter in CJS wrapper, so this is a compile error in CJS.
// But in ESM, `module` is not predefined, so this is valid.
const module = { id: 'esm' };

export const moduleId = module.id;
