var test = require("test");
test.setup();

var test_util = require('./test_util');

var coroutine = require("coroutine");
var http = require("http");

const arr = new Proxy(['one', 'two', 'three'], {
    get: function (target, name) {
        throw 'I am exception';
    }
});

const arr2 = new Proxy(['one', 'two', 'three'], {
    get: function (target, name) {
        if (name == "length") {
            return target[name];
        }
        throw 'I am exception';
    }
});

const obj = {
    get abc() {
        throw 'I am exception'
    }
};

const obj2 = new Proxy({}, {
    get: function (target, name) {
        throw 'I am exception';
    }
});

var base_port = coroutine.vmid * 10000;

describe('getter throw', () => {

    function checkthrow(cases) {
        cases.forEach((fn) => {
            assert.throws(function () {
                fn();
            });

            try {
                fn();
            } catch (e) {
                assert.equal(e, "I am exception");
            }
        })
    }

    assert.checkthrow = (fn) => {
        assert.throws(function () {
            fn();
        });

        try {
            fn();
        } catch (e) {
            assert.equal(e, "I am exception");
        }
    }

    before(() => {
        svr = new http.Server(9980 + base_port, (r) => { });
        svr.start();
        test_util.push(svr.socket);
    });

    after(test_util.cleanup);

    it('buffer', () => {
        checkthrow([
            () => {
                new Buffer(arr);
            },
            () => {
                new Buffer(arr2);
            },
            () => {
                new Buffer(obj2);
            },
        ]);

        var b = new Buffer(obj);
        assert.throws(() => {
            b.concat(arr);
        });
        assert.throws(() => {
            b.concat(arr2);
        });
        assert.throws(() => {
            b.concat(obj);
        });
        assert.throws(() => {
            b.concat(obj2);
        });
    });

    it('console', () => {
        checkthrow([
            () => {
                console.add(arr);
            },
            () => {
                console.add(arr2);
            },
            () => {
                console.add(obj2);
            },
        ]);
        assert.throws(() => {
            console.add(obj);
        })
    });


    xit('redis', () => { });

    it('utils.cpp CheckConfig', () => {
        assert.throws(() => {
            http.request('get', `http://127.0.0.1:${9980 + base_port}`, arr);
        })
        assert.throws(() => {
            http.request('get', `http://127.0.0.1:${9980 + base_port}`, arr2);
        })
        // assert.throws(() => {
        //     http.request('get', `http://127.0.0.1:${9980 + base_port}`, obj);
        // })
        assert.throws(() => {
            http.request('get', `http://127.0.0.1:${9980 + base_port}`, obj2);
        })
    });

    it('SimpleObject', () => {
        checkthrow([
            () => {
                http.get(`http://127.0.0.1:${9980 + base_port}`, {
                    headers: obj
                });
            },
        ]);

        http.get(`http://127.0.0.1:${9980 + base_port}`, {
            headers: arr
        });
        http.get(`http://127.0.0.1:${9980 + base_port}`, {
            headers: arr2
        });
        http.get(`http://127.0.0.1:${9980 + base_port}`, {
            headers: obj2
        });
    });

    it('HttpCollection', () => {
        checkthrow([
            () => {
                (new http.Request).headers.add(obj)
            },
            () => {
                (new http.Request).headers.set(obj)
            },
        ]);

        assert.throws(() => {
            (new http.Request).headers.add(arr)
        });
        assert.throws(() => {
            (new http.Request).headers.add(arr2)
        });
        assert.throws(() => {
            (new http.Request).headers.add(obj2)
        });

        assert.throws(() => {
            (new http.Request).headers.set(arr)
        });
        assert.throws(() => {
            (new http.Request).headers.set(arr2)
        });
        assert.throws(() => {
            (new http.Request).headers.set(obj2)
        });
    });


    it('querystring', () => {
        var querystring = require('querystring');
        assert.throws(() => {
            querystring.stringify(arr);
        });
        assert.throws(() => {
            querystring.stringify(arr2);
        });
        assert.throws(() => {
            querystring.stringify(obj);
        });
        assert.throws(() => {
            querystring.stringify(obj2);
        });

    });

    it('route', () => {
        var mq = require('mq');
        assert.throws(() => {
            new mq.Routing(arr);
        })
        assert.throws(() => {
            new mq.Routing(arr2);
        })
        assert.throws(() => {
            new mq.Routing(obj);
        })
        assert.throws(() => {
            new mq.Routing(objs);
        })
    });

    it('global', () => {

        assert.throws(() => {
            repl(arr);
        })
        assert.throws(() => {
            repl(arr2);
        })
        assert.throws(() => {
            repl(obj);
        })
        assert.throws(() => {
            repl(objs);
        })
    });

    it('sandbox', () => {
        var vm = require('vm');
        checkthrow([
            () => {
                var sbox = new vm.SandBox(obj);
                sbox.require('./path/to/mod');
            },
            () => {
                var sbox = new vm.SandBox({});
                sbox.add(obj);
            },
            () => {
                var sbox = new vm.SandBox({
                    a: obj2
                });
                sbox.modules.a.x;
            },
        ]);

        assert.throws(() => {
            var sbox = new vm.SandBox(obj2);
            sbox.require('./path/to/mod');
        });
        assert.throws(() => {
            var sbox = new vm.SandBox(arr);
            sbox.require('./path/to/mod');
        });
        assert.throws(() => {
            var sbox = new vm.SandBox(arr2);
            sbox.require('./path/to/mod');
        });

        assert.throws(() => {
            var sbox = new vm.SandBox({
                a: obj,
            });
            sbox.require('a');
        });
    });


    it('assert', () => {
        [
            () => {
                assert.deepEqual(arr, arr);
            },
            () => {
                assert.deepEqual(arr2, arr2);
            },
            () => {
                assert.deepEqual(obj, obj);
            },
            () => {
                assert.deepEqual([obj], [obj]);
            },
            () => {
                assert.deepEqual([obj.abc], [obj.abc]);
            },
            () => {
                assert.propertyVal(obj, "abc", 'ball');
            },
            () => {
                assert.propertyVal(obj2, "abc", 'ball');
            },

        ].forEach(fn => {
            assert.throws(() => {
                fn();
            });
        })

        assert.deepEqual(obj2, obj2);

        assert.throws(() => {
            assert.deepProperty({
                a: obj
            }, "a.abc.efg");
        });
        assert.throws(() => {
            assert.deepProperty(arr, arr);
        });
        assert.throws(() => {
            assert.deepProperty(obj2, obj2);
        });

        assert.throws(() => {
            assert.isNull(obj);
        });
    });

    it('stats', () => {
        var util = require("util");
        assert.throws(() => {
            var stats = new util.Stats(arr);
        });
        assert.throws(() => {
            var stats = new util.Stats(arr2);
        });
    })

    it('json format', () => {
        assert.property(obj, 'abc');
        assert.throws(() => {
            assert.propertyVal(arr, 'region', 'hangzhou');
        });
        assert.throws(() => {
            assert.propertyVal(obj, 'region', 'hangzhou');
        });
        assert.throws(() => {
            assert.propertyVal(obj2, 'region', 'hangzhou');
        });
    });


    it('util', () => {
        let util = require('util');
        //keys
        assert.deepEqual(util.keys(obj), ["abc"]);

        let cases = [
            () => {
                util.values(obj)
            },
            () => {
                util.deepFreeze(obj)
            },
            () => {
                util.extend(obj, obj)
            },
            () => {
                util.pick(obj, "abc")
            },
            () => {
                util.pick(obj, ["abc"])
            },
            () => {
                util.omit(obj, ["x"])
            },
            () => {
                util.omit(obj, arr)
            },
            () => {
                util.each(obj, (num, i) => { })
            },
            () => {
                util.each(arr, (num, i) => { })
            },
            () => {
                util.each(arr2, (num, i) => { })
            },
            () => {
                util.map(obj, () => { })
            },
            () => {
                util.map(arr, () => { })
            },
            () => {
                util.map(arr2, () => { })
            },
            () => {
                util.reduce(obj, () => { }, 0);
            },
            () => {
                util.reduce(arr, () => { }, 0);
            },
            () => {
                util.reduce(arr2, () => { }, 0);
            },
        ]
        checkthrow(cases);
    })
});

require.main === module && test.run(console.DEBUG);