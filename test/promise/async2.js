var util = require('util');

async function t() {
    throw new Error(100);
}

util.sync(t)();
