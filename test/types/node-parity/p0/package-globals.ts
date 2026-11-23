// P0: the package entry must expose the CommonJS globals as *globals*.
// `npm/types/index.d.ts` used to be an external module, so every declaration
// below was scoped to that module and invisible here.
const fs = require('fs');
const b: Buffer = Buffer.from('x');
const u: URL = new URL('http://x');
const td: TextDecoder = new TextDecoder();
const t = setTimeout(() => {}, 1);
const immediate = setImmediate(() => {});
const fetched: Promise<any> = fetch('http://x');

module.exports = {
    fs,
    b,
    u,
    td,
    t,
    immediate,
    fetched,
    __filename,
    __dirname,
    gc,
};
