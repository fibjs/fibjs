var test = require("test");
test.setup();

var encoding = require("encoding");
var typescript = require("internal/typescript");
var util = require("util");
var vm = require("vm");

const compilerOptions = {
    module: typescript.ModuleKind.CommonJS
}

function _getOptions(options, locals) {
    return util.extend({}, compilerOptions, options, locals)
}

describe('typescript', () => {
    function assertBasicModule(rawModule) {
        assert.isObject(rawModule)
        assert.isFunction(rawModule.add)
        assert.isFunction(rawModule.http)
        assert.equal(rawModule.http(), 'http')
        assert.isFunction(rawModule.add)
        assert.equal(rawModule.add(1, 2), 3)

        assert.isFunction(rawModule.hello)
        assert.isFunction(rawModule.hello)
        assert.equal(rawModule.hello(), 'hello, world')
    }

    it('basic check', () => {
        assert.exist(typescript.ModuleKind)
    });

    it('transpile raw', () => {
        const compiledJScript = typescript.transpile(`
        export function add (a: number, b: number) {
            return a + b
        }
        
        export function http () {
            return 'http'
        }
        
        export function hello () {
            return 'hello, world'
        }
        `, _getOptions())
        assert.isString(compiledJScript)

        const sbox = new vm.SandBox({})
        sbox.addScript('basic', new Buffer(compiledJScript))
        const rawModule = sbox.require('basic', __dirname)

        assertBasicModule(rawModule)
    })

    it('require *.ts file', () => {
        const basic = require('./ts_files/basic')
        const interface = require('./ts_files/interface')
        assertBasicModule(basic)
    })
});

require.main === module && test.run(console.DEBUG);

