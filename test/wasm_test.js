const test = require("test");
test.setup();

const fs = require('fs');
const path = require('path');
const coroutine = require('coroutine');
const util = require('util');

const wasmDir = path.join(__dirname, 'wasm');

function loadWebAssembly(name) {
  var buf = fs.readFile(path.join(wasmDir, name)).hex(' ');
  buf = new Uint8Array(buf.trim().split(/[\s\r\n]+/g).map(str => parseInt(str, 16)));
  var event = new coroutine.Event();
  var res, err;
  coroutine.start(function() {
    WebAssembly.compile(buf).then(module => {
      var imports = {};
      imports.env = imports.env || {}

      imports.env.memoryBase = imports.env.memoryBase || 0
      if (!imports.env.memory) {
        imports.env.memory = new WebAssembly.Memory({ initial: 256 })
      }

      imports.env.tableBase = imports.env.tableBase || 0
      if (!imports.env.table) {
        imports.env.table = new WebAssembly.Table({ initial: 0, element: 'anyfunc' })
      }

      const instance = new WebAssembly.Instance(module, imports)
      res = instance;
    }).catch(e => {
      err = e;
    });
  });
  event.wait();
  if (err) {
    throw err;
  }
  return res;
}

describe("wasm", () => {
  it('test1', () => {
    var buf = fs.readFile(path.join(wasmDir, 'math.wasm')).hex(' ');
    buf = new Uint8Array(buf.trim().split(/[\s\r\n]+/g).map(str => parseInt(str, 16)));
    var event = new coroutine.Event();
    coroutine.start(function() {
      WebAssembly.compile(buf).then(module => {
        var imports = {};
        imports.env = imports.env || {}

        imports.env.memoryBase = imports.env.memoryBase || 0
        if (!imports.env.memory) {
          imports.env.memory = new WebAssembly.Memory({ initial: 256 })
        }

        imports.env.tableBase = imports.env.tableBase || 0
        if (!imports.env.table) {
          imports.env.table = new WebAssembly.Table({ initial: 0, element: 'anyfunc' })
        }

        const instance = new WebAssembly.Instance(module, imports)
        var math = instance.exports;
        assert.equal(math._square(3), 9);
      }).catch(e => console.log(e.stack));
    });
  });

  it('test2', () => {
    var instance = loadWebAssembly('math.wasm');
    var math = instance.exports;
    assert.equal(math._square(3), 10);
  })
});

test.run(console.DEBUG);