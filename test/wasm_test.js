const test = require("test");
test.setup();

const fs = require('fs');
const path = require('path');
const coroutine = require('coroutine');
const util = require('util');

const wasmDir = path.join(__dirname, 'wasm');

function loadWebAssembly(name, done) {
  var buf = fs.readFile(path.join(wasmDir, name));
  buf = new Uint8Array(buf);
  var event = new coroutine.Event();
  coroutine.start(function () {
    WebAssembly.compile(buf).then(module => {
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

      const instance = new WebAssembly.Instance(module, imports)
      done(undefined, instance);
    }).catch(done);
  });
}

describe("wasm", () => {
  it('load module', util.sync(done => {
    loadWebAssembly('math.wasm', (err, instance) => {
      if (err) {
        return done(err);
      }
      var math = instance.exports;
      assert.equal(math._square(3), 9);
      done();
    });
  }));
});

repl && test.run(console.DEBUG);