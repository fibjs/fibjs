console.log('encoding testing....');

var assert = require('assert');
var encoding = require('encoding');

var quote = 'Man is distinguished, not only by his reason, but by this '
		+ 'singular passion from other animals, which is a lust '
		+ 'of the mind, that by a perseverance of delight in the continued '
		+ 'and indefatigable generation of knowledge, exceeds the short '
		+ 'vehemence of any carnal pleasure.';
var expected = 'TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24s'
		+ 'IGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltY'
		+ 'WxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZX'
		+ 'JzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmR'
		+ 'lZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo'
		+ 'ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=';

var b = encoding.base64Decode(expected);
assert.equal(b.toString(), quote);

var s = encoding.base64Encode(b);
assert.equal(s, expected);

// check that the base64 decoder ignores whitespace
var expectedWhite = expected.slice(0, 60) + ' \n' + expected.slice(60, 120)
		+ ' \n' + expected.slice(120, 180) + ' \n' + expected.slice(180, 240)
		+ ' \n' + expected.slice(240, 300) + '\n' + expected.slice(300, 360)
		+ '\n';
b = encoding.base64Decode(expectedWhite);
assert.equal(quote, b.toString());

// check that the base64 decoder ignores illegal chars
var expectedIllegal = expected.slice(0, 60) + ' \x80' + expected.slice(60, 120)
		+ ' \xff' + expected.slice(120, 180) + ' \x01' + // ' \x00'
		expected.slice(180, 240) + ' \x98' + expected.slice(240, 300) + '\x03'
		+ expected.slice(300, 360);
b = encoding.base64Decode(expectedIllegal);
assert.equal(quote, b.toString());

assert.equal(encoding.base64Decode('').toString(), '');
assert.equal(encoding.base64Decode('K').toString(), '');

// multiple-of-4 with padding
assert.equal(encoding.base64Decode('Kg==').toString(), '*');
assert.equal(encoding.base64Decode('Kio=').toString(), '**');
assert.equal(encoding.base64Decode('Kioq').toString(), '***');
assert.equal(encoding.base64Decode('KioqKg==').toString(), '****');
assert.equal(encoding.base64Decode('KioqKio=').toString(), '*****');
assert.equal(encoding.base64Decode('KioqKioq').toString(), '******');
assert.equal(encoding.base64Decode('KioqKioqKg==').toString(), '*******');
assert.equal(encoding.base64Decode('KioqKioqKio=').toString(), '********');
assert.equal(encoding.base64Decode('KioqKioqKioq').toString(), '*********');
assert
		.equal(encoding.base64Decode('KioqKioqKioqKg==').toString(),
				'**********');
assert.equal(encoding.base64Decode('KioqKioqKioqKio=').toString(),
		'***********');
assert.equal(encoding.base64Decode('KioqKioqKioqKioq').toString(),
		'************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKg==').toString(),
		'*************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKio=').toString(),
		'**************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioq').toString(),
		'***************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKg==').toString(),
		'****************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKio=').toString(),
		'*****************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKioq').toString(),
		'******************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKioqKg==').toString(),
		'*******************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKioqKio=').toString(),
		'********************');

// no padding, not a multiple of 4
assert.equal(encoding.base64Decode('Kg').toString(), '*');
assert.equal(encoding.base64Decode('Kio').toString(), '**');
assert.equal(encoding.base64Decode('KioqKg').toString(), '****');
assert.equal(encoding.base64Decode('KioqKio').toString(), '*****');
assert.equal(encoding.base64Decode('KioqKioqKg').toString(), '*******');
assert.equal(encoding.base64Decode('KioqKioqKio').toString(), '********');
assert.equal(encoding.base64Decode('KioqKioqKioqKg').toString(), '**********');
assert
		.equal(encoding.base64Decode('KioqKioqKioqKio').toString(),
				'***********');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKg').toString(),
		'*************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKio').toString(),
		'**************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKg').toString(),
		'****************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKio').toString(),
		'*****************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKioqKg').toString(),
		'*******************');
assert.equal(encoding.base64Decode('KioqKioqKioqKioqKioqKioqKio').toString(),
		'********************');

// handle padding graciously, multiple-of-4 or not
assert.equal(encoding
		.base64Decode('72INjkR5fchcxk9+VgdGPFJDxUBFR5/rMFsghgxADiw==').length,
		32);
assert.equal(encoding
		.base64Decode('72INjkR5fchcxk9+VgdGPFJDxUBFR5/rMFsghgxADiw=').length,
		32);
assert
		.equal(
				encoding
						.base64Decode('72INjkR5fchcxk9+VgdGPFJDxUBFR5/rMFsghgxADiw').length,
				32);
assert.equal(encoding
		.base64Decode('w69jACy6BgZmaFvv96HG6MYksWytuZu3T1FvGnulPg==').length,
		31);
assert
		.equal(
				encoding
						.base64Decode('w69jACy6BgZmaFvv96HG6MYksWytuZu3T1FvGnulPg=').length,
				31);
assert.equal(encoding
		.base64Decode('w69jACy6BgZmaFvv96HG6MYksWytuZu3T1FvGnulPg').length, 31);

var dot = encoding.base64Decode('//4uAA==');
assert.equal(dot[0], 0xff);
assert.equal(dot[1], 0xfe);
assert.equal(dot[2], 0x2e);
assert.equal(dot[3], 0x00);
assert.equal(encoding.base64Encode(dot), '//4uAA==');

var hexb = new Buffer(256);
for ( var i = 0; i < 256; i++) {
	hexb[i] = i;
}
var hexStr = encoding.hexEncode(hexb);
assert.equal(hexStr, '000102030405060708090a0b0c0d0e0f'
		+ '101112131415161718191a1b1c1d1e1f'
		+ '202122232425262728292a2b2c2d2e2f'
		+ '303132333435363738393a3b3c3d3e3f'
		+ '404142434445464748494a4b4c4d4e4f'
		+ '505152535455565758595a5b5c5d5e5f'
		+ '606162636465666768696a6b6c6d6e6f'
		+ '707172737475767778797a7b7c7d7e7f'
		+ '808182838485868788898a8b8c8d8e8f'
		+ '909192939495969798999a9b9c9d9e9f'
		+ 'a0a1a2a3a4a5a6a7a8a9aaabacadaeaf'
		+ 'b0b1b2b3b4b5b6b7b8b9babbbcbdbebf'
		+ 'c0c1c2c3c4c5c6c7c8c9cacbcccdcecf'
		+ 'd0d1d2d3d4d5d6d7d8d9dadbdcdddedf'
		+ 'e0e1e2e3e4e5e6e7e8e9eaebecedeeef'
		+ 'f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff');

var hexb2 = encoding.hexDecode(hexStr);
for ( var i = 0; i < 256; i++) {
	assert.equal(hexb2[i], hexb[i]);
}

var u = '中文测试';
var u1 = escape(u);
assert.equal(encoding.decodeURI(u1), u);

u1 = u1.replace(/%/g, '\\');
assert.equal(encoding.decodeURI(u1), u);

for ( var i = 32; i < 128; i++)
	u += String.fromCharCode(i);

assert.equal(encodeURI(u), encoding.encodeURI(u));
assert.equal(encodeURIComponent(u), encoding.encodeURIComponent(u));
assert.equal(encoding.decodeURI(encodeURI(u)), u);
assert.equal(encoding.decodeURI(encodeURIComponent(u)), u);

assert.equal(encoding.jsonEncode({
	a : 100,
	b : 200
}), '{"a":100,"b":200}');

console.dir(encoding.jsonDecode('{"a":100,"b":200}'));


