var test = require("test");
test.setup();

describe('expect', () => {
    it('assertion', () => {
        //    expect('test').to.be.a('string');
        //    expect('foo').to.equal('foo');
    });

    it('true', () => {
        expect(true).to.be.true;
        expect(false).to.not.be.true;
        expect(1).to.not.be.true;

        assert.throws(() => {
            expect('test').to.be.true;
        }, "expected 'test' to be true")
    });

    it('ok', () => {
        expect(true).to.be.ok;
        expect(false).to.not.be.ok;
        expect(1).to.be.ok;
        expect(0).to.not.be.ok;

        assert.throws(() => {
            expect('').to.be.ok;
        }, "expected '' to be truthy");

        assert.throws(() => {
            expect('test').to.not.be.ok;
        }, "expected 'test' to be falsy");
    });

    it('false', () => {
        expect(false).to.be.false;
        expect(true).to.not.be.false;
        expect(0).to.not.be.false;

        assert.throws(() => {
            expect('').to.be.false;
        }, "expected '' to be false")
    });

    it('null', () => {
        expect(null).to.be.null;
        expect(false).to.not.be.null;

        assert.throws(() => {
            expect('').to.be.null;
        }, "expected '' to be null")
    });

    it('undefined', () => {
        expect(undefined).to.be.undefined;
        expect(null).to.not.be.undefined;

        assert.throws(() => {
            expect('').to.be.undefined;
        }, "expected '' to be undefined")
    });

    it('exist', () => {
        var foo = 'bar',
            bar;
        expect(foo).to.exist;
        expect(bar).to.not.exist;
    });

    it('.equal()', () => {
        var foo;
        expect(undefined).to.equal(foo);
    });

    it('typeof', () => {
        expect('test').to.be.a('string');

        assert.throws(() => {
            expect('test').to.not.be.a('string');
        }, "expected 'test' not to be a string");

        expect(5).to.be.a('number');
        expect(5).to.be.a.number;
        expect(new Number(1)).to.be.a('number');
        expect(Number(1)).to.be.a('number');
        expect(true).to.be.a('boolean');
        expect(new Array()).to.be.a('array');
        expect(new Array()).to.be.a.array;
        expect(new Object()).to.be.a('object');
        expect({}).to.be.a('object');
        expect([]).to.be.a('array');
        expect(() => {}).to.be.a('function');
        expect(null).to.be.a('null');

        assert.throws(() => {
            expect(5).to.not.be.a('number', 'blah');
        }, "blah: expected 5 not to be a number");
    });

    it('above(n)', () => {
        expect(5).to.be.above(2);
        expect(5).to.be.greaterThan(2);
        expect(5).to.not.be.above(5);
        expect(5).to.not.be.above(6);

        assert.throws(() => {
            expect(5).to.be.above(6, 'blah');
        }, "blah: expected 5 to be above 6");

        assert.throws(() => {
            expect(10).to.not.be.above(6, 'blah');
        }, "blah: expected 10 to be at most 6");

    });

    it('least(n)', () => {
        expect(5).to.be.at.least(2);
        expect(5).to.be.at.least(5);
        expect(5).to.not.be.at.least(6);

        assert.throws(() => {
            expect(5).to.be.at.least(6, 'blah');
        }, "blah: expected 5 to be at least 6");

        assert.throws(() => {
            expect(10).to.not.be.at.least(6, 'blah');
        }, "blah: expected 10 to be below 6");
    });

    it('below(n)', () => {
        expect(2).to.be.below(5);
        expect(2).to.be.lessThan(5);
        expect(2).to.not.be.below(2);
        expect(2).to.not.be.below(1);

        assert.throws(() => {
            expect(6).to.be.below(5, 'blah');
        }, "blah: expected 6 to be below 5");

        assert.throws(() => {
            expect(6).to.not.be.below(10, 'blah');
        }, "blah: expected 6 to be at least 10");

    });

    it('most(n)', () => {
        expect(2).to.be.at.most(5);
        expect(2).to.be.at.most(2);
        expect(2).to.not.be.at.most(1);
        expect(2).to.not.be.at.most(1);

        assert.throws(() => {
            expect(6).to.be.at.most(5, 'blah');
        }, "blah: expected 6 to be at most 5");

        assert.throws(() => {
            expect(6).to.not.be.at.most(10, 'blah');
        }, "blah: expected 6 to be above 10");

    });

    it('eql(val)', () => {
        expect('test').to.eql('test');
        expect({
            foo: 'bar'
        }).to.eql({
            foo: 'bar'
        });
        expect(1).to.eql(1);
        expect('4').to.not.eql(4);

        assert.throws(() => {
            expect(4).to.eql(3, 'blah');
        }, 'blah: expected 4 to deeply equal 3');
    });

    it('equal(val)', () => {
        expect('test').to.equal('test');
        expect(1).to.equal(1);

        assert.throws(() => {
            expect(4).to.equal(3, 'blah');
        }, 'blah: expected 4 to equal 3');

        assert.throws(() => {
            expect('4').to.equal(4, 'blah');
        }, "blah: expected '4' to equal 4");
    });

    it('deep.equal(val)', () => {
        expect({
            foo: 'bar'
        }).to.deep.equal({
            foo: 'bar'
        });
        expect({
            foo: 'bar'
        }).not.to.deep.equal({
            foo: 'baz'
        });
    });

    it('deep.equal(/regexp/)', () => {
        expect(/a/).to.deep.equal(/a/);
        expect(/a/).not.to.deep.equal(/b/);
        expect(/a/).not.to.deep.equal({});
        expect(/a/g).to.deep.equal(/a/g);
        expect(/a/g).not.to.deep.equal(/b/g);
        expect(/a/i).to.deep.equal(/a/i);
        expect(/a/i).not.to.deep.equal(/b/i);
        expect(/a/m).to.deep.equal(/a/m);
        expect(/a/m).not.to.deep.equal(/b/m);
    });

    it('deep.equal(Date)', () => {
        var a = new Date(1, 2, 3),
            b = new Date(4, 5, 6);
        expect(a).to.deep.equal(a);
        expect(a).not.to.deep.equal(b);
        expect(a).not.to.deep.equal({});
    });


    it('deep.property(name)', () => {
        expect({
                'foo.bar': 'baz'
            })
            .to.not.have.deep.property('foo.bar');
        expect({
                foo: {
                    bar: 'baz'
                }
            })
            .to.have.deep.property('foo.bar');

        assert.throws(() => {
            expect({
                    'foo.bar': 'baz'
                })
                .to.have.deep.property('foo.bar');
        }, "expected { 'foo.bar': 'baz' } to have a deep property 'foo.bar'");
    });

    it('property(name, val)', () => {
        expect('test').to.have.property('length', 4);

        assert.throws(() => {
            expect('asd').to.have.property('length', 4, 'blah');
        }, "blah: expected 'asd' to have a property 'length' of 4, but got 3");

        assert.throws(() => {
            expect('asd').to.not.have.property('length', 3, 'blah');
        }, "blah: expected 'asd' to not have a property 'length' of 3");

        assert.throws(() => {
            expect('asd').to.not.have.property('foo', 3, 'blah');
        }, "blah: 'asd' has no property 'foo'");
    });

    it('deep.property(name, val)', () => {
        expect({
            foo: {
                bar: 'baz'
            }
        }).to.have.deep.property('foo.bar', 'baz');

        assert.throws(() => {
            expect({
                foo: {
                    bar: 'baz'
                }
            }).to.have.deep.property('foo.bar', 'quux', 'blah');
        }, "blah: expected { foo: { bar: 'baz' } } to have a deep property 'foo.bar' of 'quux', but got 'baz'");
        assert.throws(() => {
            expect({
                foo: {
                    bar: 'baz'
                }
            }).to.not.have.deep.property('foo.bar', 'baz', 'blah');
        }, "blah: expected { foo: { bar: 'baz' } } to not have a deep property 'foo.bar' of 'baz'");
        assert.throws(() => {
            expect({
                foo: 5
            }).to.not.have.deep.property('foo.bar', 'baz', 'blah');
        }, "blah: { foo: 5 } has no deep property 'foo.bar'");
    });

    it('closeTo', () => {
        expect(1.5).to.be.closeTo(1.0, 0.5);
        expect(10).to.be.closeTo(20, 20);
        expect(-10).to.be.closeTo(20, 30);

        assert.throws(() => {
            expect(2).to.be.closeTo(1.0, 0.5, 'blah');
        }, "blah: expected 2 to be close to 1 +/- 0.5");

        assert.throws(() => {
            expect(-10).to.be.closeTo(20, 29, 'blah');
        }, "blah: expected -10 to be close to 20 +/- 29");
    });
});

//test.run();
