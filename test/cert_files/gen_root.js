var crypto = require("crypto");
var fs = require("fs");

var cert = new crypto.X509Cert();
cert.loadFile('certdata.txt');
var a = cert.dump();
var a1 = [];

for (var i = 0; i < a.length; i++) {
	var txt = a[i];
	txt = txt.replace('-----BEGIN CERTIFICATE-----', '').replace('-----END CERTIFICATE-----', '');

	var buf = new Buffer(txt, 'base64');
	txt = buf.hex();
	var b = txt.match(/(.{1,2})/g);
	txt = '\\x' + b.join('\\x');
	var b = txt.match(/(.{1,80})/g);
	txt = '        "' + b.join('"\n        "') + '"';

	a1.push('    {');
	a1.push('        ' + buf.length + ',');
	a1.push(txt);
	a1.push('    },');
}

var txts = ['/***************************************************************************',
	' *                                                                         *',
	' *   This file was automatically generated using gen_root.js               *',
	' *   PLEASE DO NOT EDIT!!!!                                                *',
	' *                                                                         *',
	' ***************************************************************************/',
	'',
	'#include "X509Cert.h"',
	'',
	'namespace fibjs',
	'{',
	'',
	'X509Cert::_cert X509Cert::g_root_ca[] =',
	'{',
	''
];

fs.writeFile('../../fibjs/src/crypto/root_ca.cpp', txts.join('\n') + a1.join('\n') + '\n    {0, 0}\n};\n\n}\n');