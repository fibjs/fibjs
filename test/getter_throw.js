var test = require("test");
test.setup();

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


    xit('redis', () => {});

    it('utils.cpp CheckConfig', () => {
        let http = require('http');
        assert.throws(() => {
            http.request('get', 'http://www.fibjs.org', arr);
        })
        assert.throws(() => {
            http.request('get', 'http://www.fibjs.org', arr2);
        })
        assert.throws(() => {
            http.request('get', 'http://www.fibjs.org', obj);
        })
        assert.throws(() => {
            http.request('get', 'http://www.fibjs.org', obj2);
        })
    });

    it('SimpleObject', () => {
        let http = require('http');
        checkthrow([
            () => {
                http.get('http://www.fibjs.org', {
                    headers: obj
                });
            },
        ]);

        http.get('http://www.fibjs.org', {
            headers: arr
        });
        http.get('http://www.fibjs.org', {
            headers: arr2
        });
        http.get('http://www.fibjs.org', {
            headers: obj2
        });
    });

    it('console_osx_keyCheckKeyFlags ', () => {
        checkthrow([
            () => {
                console.keyDown('up', arr);
            },
            () => {
                console.keyDown('up', arr2);
            },
            () => {
                console.keyDown('up', obj2);
            },
        ]);
        assert.throws(() => {
            console.keyDown('up', obj);
        });
    });

    xit('db format', () => {
        var db = require('db');

        db.format('?, ?', arr);
        db.format('?, ?', arr2);
        db.format('?, ?', obj);
        db.format('?, ?', obj2);
    });

    xit('level db', () => {});

    xit('redis pub sub', () => {});
    xit('redis sortedSet', () => {});

    it('base64vlq', () => {
        var base64vlq = require('base64vlq');
        checkthrow([
            () => {
                base64vlq.encode(arr);
            },
            () => {
                base64vlq.encode(arr2);
            },
            () => {
                base64vlq.encode(obj2);
            },
        ]);
    });

    it('HttpCollection', () => {
        let http = require('http');
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
                util.each(obj, (num, i) => {})
            },
            () => {
                util.each(arr, (num, i) => {})
            },
            () => {
                util.each(arr2, (num, i) => {})
            },
            () => {
                util.map(obj, () => {})
            },
            () => {
                util.map(arr, () => {})
            },
            () => {
                util.map(arr2, () => {})
            },
            () => {
                util.reduce(obj, () => {}, 0);
            },
            () => {
                util.reduce(arr, () => {}, 0);
            },
            () => {
                util.reduce(arr2, () => {}, 0);
            },
        ]
        checkthrow(cases);
    })
});

require.main === module && test.run(console.DEBUG);