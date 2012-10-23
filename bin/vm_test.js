console.log('vm testing....');

var assert = require('assert');
var vm = require('vm');

var sbox = vm.create({
	a : 100,
	assert : assert
});

sbox.require('./vm_test/t1').fun();
