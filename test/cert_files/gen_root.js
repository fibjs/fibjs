const http = require('http');
const crypto = require('crypto');
const fs = require('fs');
const path = require('path');

const LINE_WIDTH = 76;
const MOZILLA_CERT_URL = "http://hg.mozilla.org/releases/mozilla-release/raw-file/default/security/nss/lib/ckfw/builtins/certdata.txt";

const TRUSTED_PURPOSES = [
    "DIGITAL_SIGNATURE", "NON_REPUDIATION", "KEY_ENCIPHERMENT",
    "DATA_ENCIPHERMENT", "KEY_AGREEMENT", "KEY_CERT_SIGN", "CRL_SIGN",
    "SERVER_AUTH", "CLIENT_AUTH", "CODE_SIGNING", "EMAIL_PROTECTION",
    "IPSEC_END_SYSTEM", "IPSEC_TUNNEL", "IPSEC_USER", "TIME_STAMPING",
    "STEP_UP_APPROVED"
];

const TRUST_LEVELS = ["TRUSTED_DELEGATOR", "NOT_TRUSTED", "MUST_VERIFY_TRUST", "TRUSTED"];
const REQUIRED_TRUST_PURPOSES = ["SERVER_AUTH"];
const REQUIRED_TRUST_LEVELS = ["TRUSTED_DELEGATOR"];

function isValidTrustAttribute(attribute, validList) {
    return validList.includes(attribute);
}

function isCertificateTrusted(trustAttributes) {
    return REQUIRED_TRUST_LEVELS.some(level => {
        const purposes = trustAttributes[level] || [];
        return purposes.some(purpose =>
            REQUIRED_TRUST_PURPOSES.includes(purpose));
    });
}

function fetchMozillaCertData() {
    console.log('Downloading certificates');
    const secureUrl = MOZILLA_CERT_URL.replace(/^http:/, 'https:');
    try {
        const response = http.get(secureUrl);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        console.log('Download complete');
        return response.data.toString();
    } catch (err) {
        console.error('Download failed');
        throw err;
    }
}

function extractCertificates(rawData) {
    let certCount = 0;
    let skipCount = 0;
    let startOfCert = false;
    let caName = '';
    const output = [];

    output.push("##\n## Bundle of CA Root Certificates\n##\n\n");

    const lines = rawData.split('\n');
    let i = 0;

    while (i < lines.length) {
        let line = lines[i].trim();

        if (line.match(/\*\*\*\*\* BEGIN LICENSE BLOCK \*\*\*\*\*/)) {
            while (i < lines.length && !lines[i].match(/\*\*\*\*\* END LICENSE BLOCK \*\*\*\*\*/)) {
                output.push(lines[i] + '\n');
                i++;
            }
            output.push(lines[i] + '\n');
            i++;
            continue;
        }

        if (line.match(/^#/) || line === '') {
            i++;
            continue;
        }

        if (line.match(/^CKA_CLASS CK_OBJECT_CLASS CKO_CERTIFICATE/)) {
            startOfCert = true;
        }

        if (startOfCert && line.match(/^CKA_LABEL UTF8 "(.*?)"/)) {
            caName = RegExp.$1;
        }

        if (startOfCert && line.match(/^CKA_VALUE MULTILINE_OCTAL/)) {
            let certData = '';
            i++;
            while (i < lines.length && !lines[i].match(/^END/)) {
                const octets = lines[i].trim().split('\\').slice(1);
                certData += octets.map(oct => String.fromCharCode(parseInt(oct, 8))).join('');
                i++;
            }

            while (i < lines.length && !lines[i].match(/^CKA_CLASS CK_OBJECT_CLASS CKO_NSS_TRUST/)) {
                i++;
            }

            const trustAttributes = {};
            while (i < lines.length && !lines[i].match(/^#/)) {
                const trustMatch = lines[i].match(/^CKA_TRUST_([A-Z_]+)\s+CK_TRUST\s+CKT_NSS_([A-Z_]+)/);
                if (trustMatch) {
                    const [, purpose, level] = trustMatch;
                    if (isValidTrustAttribute(purpose, TRUSTED_PURPOSES) &&
                        isValidTrustAttribute(level, TRUST_LEVELS)) {
                        if (!trustAttributes[level]) {
                            trustAttributes[level] = [];
                        }
                        trustAttributes[level].push(purpose);
                    }
                }
                i++;
            }

            if (isCertificateTrusted(trustAttributes)) {
                const encoded = Buffer.from(certData, 'binary').toString('base64');
                const wrappedCert = encoded.match(new RegExp(`.{1,${LINE_WIDTH}}`, 'g')).join('\n');
                output.push(`\n${caName}\n${'='.repeat(caName.length)}\n`);
                output.push('-----BEGIN CERTIFICATE-----\n');
                output.push(wrappedCert + '\n');
                output.push('-----END CERTIFICATE-----\n');
                certCount++;
            } else {
                skipCount++;
            }
            startOfCert = false;
        }
        i++;
    }

    return { certCount, skipCount, bundle: output.join('') };
}

function generateCppSourceCode(certBundle, outputPath) {
    const certificate = new crypto.X509Certificate(certBundle);
    const certList = certificate.pem.split('-----END CERTIFICATE-----');
    const processedCerts = [];

    certList.forEach((certText) => {
        certText = certText.replace('-----BEGIN CERTIFICATE-----', '').replace('-----END CERTIFICATE-----', '');
        const certBuffer = new Buffer(certText, 'base64');
        if (certBuffer.length) {
            const hexString = convertToHexFormat(certBuffer);
            processedCerts.push(formatCertStruct(certBuffer.length, hexString));
        }
    });

    const sourceCode = [
        generateFileHeader(),
        processedCerts.join('\n'),
        generateFileFooter()
    ].join('');

    fs.writeTextFile(outputPath, sourceCode);
    console.log('Generate C++ source complete');
}

function convertToHexFormat(buffer) {
    const hexData = buffer.hex();
    const hexPairs = hexData.match(/(.{1,2})/g);
    const hexString = '\\x' + hexPairs.join('\\x');
    const lines = hexString.match(/(.{1,80})/g);
    return '        "' + lines.join('"\n        "') + '"';
}

function formatCertStruct(length, hexData) {
    return [
        '    {',
        `        ${length},`,
        hexData,
        '    },'
    ].join('\n');
}

function generateFileHeader() {
    return [
        '/***************************************************************************',
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
    ].join('\n');
}

function generateFileFooter() {
    return '\n    {0, 0}\n};\n\n}\n';
}

function main() {
    const bundleFile = path.join(__dirname, 'ca-bundle.crt');
    const cppOutputPath = path.join(__dirname, '../../fibjs/src/crypto/root_ca.cpp');

    try {
        const certData = fetchMozillaCertData();
        console.log('Processing certificates');

        const { certCount, skipCount, bundle } = extractCertificates(certData);
        console.log(`Process complete: ${certCount} processed, ${skipCount} skipped`);

        if (fs.existsSync(bundleFile)) {
            fs.unlinkSync(bundleFile);
            console.log('Remove existing bundle');
        }

        fs.writeFileSync(bundleFile, bundle);
        console.log('Generate bundle complete');

        generateCppSourceCode(fs.readFile(bundleFile), cppOutputPath);
        console.log('All complete');
    } catch (error) {
        console.error('Error:', error.message);
        process.exit(1);
    }
}

main();
