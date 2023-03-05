const test = require("test");
test.setup();

var code = new Uint8Array([0, 97, 115, 109, 1, 0, 0, 0, 0, 12, 6, 100, 121, 108,
    105, 110, 107, 128, 128, 192, 2, 0, 1, 137, 128, 128, 128, 0, 2, 96, 1, 127,
    1, 127, 96, 0, 0, 2, 193, 128, 128, 128, 0, 4, 3, 101, 110, 118, 10, 109, 101,
    109, 111, 114, 121, 66, 97, 115, 101, 3, 127, 0, 3, 101, 110, 118, 6, 109,
    101, 109, 111, 114, 121, 2, 0, 128, 2, 3, 101, 110, 118, 5, 116, 97, 98,
    108, 101, 1, 112, 0, 0, 3, 101, 110, 118, 9, 116, 97, 98, 108, 101, 66, 97,
    115, 101, 3, 127, 0, 3, 132, 128, 128, 128, 0, 3, 0, 1, 1, 6, 139, 128, 128,
    128, 0, 2, 127, 1, 65, 0, 11, 127, 1, 65, 0, 11, 7, 174, 128, 128, 128, 0,
    3, 18, 95, 95, 112, 111, 115, 116, 95, 105, 110, 115, 116, 97, 110, 116,
    105, 97, 116, 101, 0, 2, 7, 95, 115, 113, 117, 97, 114, 101, 0, 0, 11, 114,
    117, 110, 80, 111, 115, 116, 83, 101, 116, 115, 0, 1, 9, 129, 128, 128, 128,
    0, 0, 10, 175, 128, 128, 128, 0, 3, 135, 128, 128, 128, 0, 0, 32, 0, 32, 0,
    108, 11, 131, 128, 128, 128, 0, 0, 1, 11, 149, 128, 128, 128, 0, 0, 2, 64, 35,
    0, 36, 2, 35, 2, 65, 128, 128, 192, 2, 106, 36, 3, 16, 1, 11, 11]);

function imports() {
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

    return imports;
}

describe("wasm", () => {
    it('load module', () => {
        var instance = new WebAssembly.Instance(new WebAssembly.Module(code), imports());
        var math = instance.exports;
        assert.equal(math._square(3), 9);
    });

    it('async load module', async () => {
        var instance = new WebAssembly.Instance(await WebAssembly.compile(code), imports());
        var math = instance.exports;
        assert.equal(math._square(3), 9);
    });
});

require.main === module && test.run(console.DEBUG);
