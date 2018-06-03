// / <reference path="fib-types/declare/index.d.ts" />
// / <reference path="fib-types/declare/_test_env.d.ts" />

import * as test from "test";
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
});

require.main === module && test.run(console.DEBUG);

