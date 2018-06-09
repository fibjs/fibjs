// / <reference path="fib-types/declare/index.d.ts" />
// / <reference path="fib-types/declare/_test_env.d.ts" />

import * as test from "test";
import _class from './ts_files/class';
test.setup();

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

    it('class', () => {
        const basic = require('./ts_files/basic')
        assert.throws(() => {
            // not exist
            const _classModule = require('./ts_files/_class')
        })
        const _classModule = require('./ts_files/class')
        assert.exist(_class, _classModule.default)

        assert.exist(_class.bar);
        assert.exist(_class.bar2);

        const ins = new _class()
        assert.exist(ins);

        assert.equal(ins.foo1(1, 2), basic.add(1, 2))
        assert.equal(ins.foo2(), basic.hello())
    })
});

require.main === module && test.run(console.DEBUG);

