var test = require("test");
test.setup();

var qs = require('querystring');

describe('querystring', () => {
    var qsTestCases = [
        ['foo=918854443121279438895193',
            'foo=918854443121279438895193',
            { 'foo': '918854443121279438895193' }
        ],
        ['foo=bar', 'foo=bar', { 'foo': 'bar' }],
        ['foo=bar&foo=quux', 'foo=bar&foo=quux', { 'foo': ['bar', 'quux'] }],
        ['foo=1&bar=2', 'foo=1&bar=2', { 'foo': '1', 'bar': '2' }],
        ['my+weird+field=q1%212%22%27w%245%267%2Fz8%29%3F',
            'my%20weird%20field=q1!2%22\'w%245%267%2Fz8)%3F',
            { 'my weird field': 'q1!2"\'w$5&7/z8)?' }
        ],
        ['foo%3Dbaz=bar', 'foo%3Dbaz=bar', { 'foo=baz': 'bar' }],
        ['foo=baz=bar', 'foo=baz%3Dbar', { 'foo': 'baz=bar' }],
        ['str=foo&arr=1&arr=2&arr=3&somenull=&undef=',
            'str=foo&arr=1&arr=2&arr=3&somenull=&undef=', {
                'str': 'foo',
                'arr': ['1', '2', '3'],
                'somenull': '',
                'undef': ''
            }
        ],
        [' foo = bar ', '%20foo%20=%20bar%20', { ' foo ': ' bar ' }],
        ['foo=%zx', 'foo=%25zx', { 'foo': '%zx' }],
        ['foo=%EF%BF%BD', 'foo=%EF%BF%BD', { 'foo': '\ufffd' }],
        ['foo&bar=baz', 'foo=&bar=baz', { foo: '', bar: 'baz' }]
    ];

    var qsColonTestCases = [
        ['foo:bar', 'foo:bar', { 'foo': 'bar' }],
        ['foo:bar;foo:quux', 'foo:bar;foo:quux', { 'foo': ['bar', 'quux'] }],
        ['foo:1&bar:2;baz:quux',
            'foo:1%26bar%3A2;baz:quux',
            { 'foo': '1&bar:2', 'baz': 'quux' }
        ],
        ['foo%3Abaz:bar', 'foo%3Abaz:bar', { 'foo:baz': 'bar' }],
        ['foo:baz:bar', 'foo:baz%3Abar', { 'foo': 'baz:bar' }]
    ];

    var qsNoMungeTestCases = [
        ['', {}],
        ['foo=bar&foo=baz', { 'foo': ['bar', 'baz'] }],
        ['blah=burp', { 'blah': 'burp' }],
        ['gragh=1&gragh=3&goo=2', { 'gragh': ['1', '3'], 'goo': '2' }],
        ['frappucino=muffin&goat%5B%5D=scone&pond=moose',
            { 'frappucino': 'muffin', 'goat[]': 'scone', 'pond': 'moose' }
        ],
        ['trololol=yes&lololo=no', { 'trololol': 'yes', 'lololo': 'no' }]
    ];

    describe('parse', () => {
        it('simple parse', () => {
            assert.strictEqual('918854443121279438895193',
                qs.parse('id=918854443121279438895193').id);
        });

        it('simple cases', () => {
            // test that the canonical qs is parsed properly.
            qsTestCases.forEach((testCase) => {
                assert.deepEqual(testCase[2], qs.parse(testCase[0]).toJSON());
            });
        });

        it('colon test cases', () => {
            qsColonTestCases.forEach((testCase) => {
                assert.deepEqual(testCase[2], qs.parse(testCase[0], ';', ':').toJSON());
            });
        });

        it('test the nested qs-in-qs case', () => {
            var f = qs.parse('a=b&q=x%3Dy%26y%3Dz').toJSON();
            f.q = qs.parse(f.q).toJSON();
            assert.deepEqual(f, { a: 'b', q: { x: 'y', y: 'z' } });
        });

        it('nested in colon', () => {
            var f = qs.parse('a:b;q:x%3Ay%3By%3Az', ';', ':').toJSON();
            f.q = qs.parse(f.q, ';', ':').toJSON();
            assert.deepEqual(f, { a: 'b', q: { x: 'y', y: 'z' } });
        });

        it('multi char', () => {
            assert.deepEqual(
                qs.parse('foo=>bar&&bar=>baz', '&&', '=>').toJSON(), { foo: 'bar', bar: 'baz' }
            );

            assert.deepEqual(
                qs.parse('foo==>bar, bar==>baz', ', ', '==>').toJSON(), { foo: 'bar', bar: 'baz' }
            );
        });
    });

    describe('stringify', () => {
        it('simple cases', () => {
            qsTestCases.forEach((testCase) => {
                assert.equal(testCase[1], qs.stringify(testCase[2]));
            });
        });

        it('colon test cases', () => {
            qsColonTestCases.forEach(function(testCase) {
                assert.equal(testCase[1], qs.stringify(testCase[2], ';', ':'));
            });
        });

        it('coerce numbers to string', () => {
            assert.strictEqual('foo=0', qs.stringify({ foo: 0 }));
            assert.strictEqual('foo=0', qs.stringify({ foo: -0 }));
            assert.strictEqual('foo=3', qs.stringify({ foo: 3 }));
            assert.strictEqual('foo=-72.42', qs.stringify({ foo: -72.42 }));
        });

        it('multi char', () => {
            assert.strictEqual(
                qs.stringify({ foo: 'bar', bar: 'baz' }, '&&', '=>'),
                'foo=>bar&&bar=>baz'
            );

            assert.strictEqual(
                qs.stringify({ foo: 'bar', bar: 'baz' }, ', ', '==>'),
                'foo==>bar, bar==>baz'
            );
        });
    });
});

argv.length && test.run(console.DEBUG);
