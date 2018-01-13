'use strict';

const test = require("test");
const path = require('path');
const fs = require('fs');

test.setup();

describe('out', () => {
  it('out tests', () => {
    const messageDir = path.join(__dirname, 'message');
    let messages = fs.readdir(messageDir);
    messages = messages.filter(m => m.endsWith('.js'));

    const execPath = process.execPath;

    messages.forEach(m => {
      const js = path.join(messageDir, m);
      const expected = fs.readFile(path.join(js.slice(0, -2) + 'out'), 'utf-8');
      const p = process.open(execPath, [js]);
      let tmp;
      let actual = '';
      while (tmp = p.readLine()) {
        actual += tmp + '\n';
      }

      const err = `
  message(${m}) output test failed! at ${js}

  actual: 
${actual}
  expected: 
${expected}
`;

      assert.ok(failed(expected, actual), err);
    });

    function failed(expected, actual) {
      expected = expected.split('\n');
      actual = actual.split('\n');

      if (expected.length !== actual.length) {
        return false;
      }

      for (var i = 0; i < expected.length; i++) {
        if (expected[i] && actual[i]) {
          if (expected[i].indexOf('*') > -1) {
            let pattern = expected[i].replace(/\*/g, '.*');
            pattern = new RegExp(pattern);
            if (!pattern.test(actual[i])) {
              console.log('actual:', actual[i]);
              console.log('expected:', pattern);
              return false;
            }
          } else if (expected[i] !== actual[i]) {
            console.log('actual:', actual[i]);
            console.log('expected:', expected[i]);
            return false;
          }
        }
      }
      return true;
    }
  });
});

require.main === module && test.run(console.DEBUG);
