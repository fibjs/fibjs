const assert = require('assert')
const dns = require('dns')

dns.resolve('fibjs.org');
console.log('resolve domain success!');

dns.lookup('fibjs.org');
console.log('lookup domain success!');
