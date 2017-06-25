const test = require("test");
test.setup();

const fs = require('fs');
const path = require('path');
const util = require('util');

const wasmDir = path.join(__dirname, 'wasm');

WebAssembly.compileSync = util.sync(WebAssembly.compile, true);

function loadWebAssembly(name) {
    var imports = {};
    imports.env = imports.env || {}

    imports.env.memoryBase = imports.env.memoryBase || 0
    if (!imports.env.memory) {
        imports.env.memory = new WebAssembly.Memory({
            initial: 256
        })
    }

    imports.env.tableBase = imports.env.tableBase || 0
    if (!imports.env.table) {
        imports.env.table = new WebAssembly.Table({
            initial: 0,
            element: 'anyfunc'
        })
    }

    return new WebAssembly.Instance(WebAssembly.compileSync(
        new Uint8Array(fs.readFile(path.join(wasmDir, name)))), imports)
}

describe("wasm", () => {
    it('load module', () => {
        var instance = loadWebAssembly('math.wasm');
        var math = instance.exports;
        assert.equal(math._square(3), 9);
    });
});

repl && test.run(console.DEBUG);