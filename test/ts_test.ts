var test = require("test");
test.setup();

var encoding = require("encoding");
var util = require("util");
var vm = require("vm");

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

    it('require *.ts file', () => {
        const basic = require('./ts_files/basic')
        assertBasicModule(basic)
    })
});

require.main === module && test.run(console.DEBUG);

