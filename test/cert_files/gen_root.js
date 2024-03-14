var crypto = require("crypto");
var fs = require("fs");
var child_process = require("child_process");

// child_process.run('perl', ['mk-ca-bundle.pl']);

var cert = new crypto.X509Certificate(fs.readFile('ca-bundle.crt'));
var a = cert.pem.split('-----END CERTIFICATE-----');
var a1 = [];

a.forEach(txt => {
    txt = txt.replace('-----BEGIN CERTIFICATE-----', '').replace('-----END CERTIFICATE-----', '');

    var buf = new Buffer(txt, 'base64');
    if (buf.length) {
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
});

var txts = ['/***************************************************************************',
    ' *                                                                         *',
    ' *   This file was automatically generated using gen_root.js               *',
    ' *   PLEASE DO NOT EDIT!!!!                                                *',
    ' *                                                                         *',
    ' ***************************************************************************/',
    '',
    '#include "crypto_util.h"',
    '',
    'namespace fibjs',
    '{',
    '',
    'EmbedCert g_root_ca[] =',
    '{',
    ''
];

fs.writeTextFile('../../fibjs/src/crypto/root_ca.cpp', txts.join('\n') + a1.join('\n') + '\n    {0, 0}\n};\n\n}\n');