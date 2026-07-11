// ESM file - uses import/export syntax
// This file is in a directory WITHOUT "type": "module" in package.json,
// so it's treated as CJS by default, but contains ESM syntax.
// Node.js v22.12+ supports require(esm) - require() can load this file.
import { prefix } from './shared.js';
export default function generate(color) {
    return prefix + color;
}
