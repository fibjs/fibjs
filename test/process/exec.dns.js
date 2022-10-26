const assert = require('assert')
const dns = require('dns')

dns.resolve('www.icann.org');
console.log('resolve domain success!');

dns.lookup('www.icann.org');
console.log('lookup domain success!');
