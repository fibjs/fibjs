const test = require("test");
test.setup();

const tty = require('tty');

describe('tty', () => {
  it("isatty", () => {
    assert.isTrue(tty.isatty(0));
    assert.isTrue(tty.isatty(1));
    assert.isTrue(tty.isatty(2));
    assert.isFalse(tty.isatty(3));
    assert.throws(() => tty.isatty(-1));
    assert.throws(() => tty.isatty(9999999999999999));
  });
});

repl && test.run(console.DEBUG);
