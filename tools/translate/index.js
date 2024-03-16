// @ts-nocheck
Object.defineProperty(exports, "__esModule", { value: true });
exports.translate = exports.langs = void 0;
const http = require("http");
const url = require("url");
const ssl = require("ssl");

const { encodeTextToTk } = require('./uglified');
function getToken(text, opts) {
    var tk = encodeTextToTk(text, opts.tld);
    return {
        name: 'tk',
        value: tk
    };
}
const langs = {
    'auto': 'Automatic',
    'af': 'Afrikaans',
    'sq': 'Albanian',
    'am': 'Amharic',
    'ar': 'Arabic',
    'hy': 'Armenian',
    'az': 'Azerbaijani',
    'eu': 'Basque',
    'be': 'Belarusian',
    'bn': 'Bengali',
    'bs': 'Bosnian',
    'bg': 'Bulgarian',
    'ca': 'Catalan',
    'ceb': 'Cebuano',
    'ny': 'Chichewa',
    'zh-CN': 'Chinese (Simplified)',
    'zh-TW': 'Chinese (Traditional)',
    'co': 'Corsican',
    'hr': 'Croatian',
    'cs': 'Czech',
    'da': 'Danish',
    'nl': 'Dutch',
    'en': 'English',
    'eo': 'Esperanto',
    'et': 'Estonian',
    'tl': 'Filipino',
    'fi': 'Finnish',
    'fr': 'French',
    'fy': 'Frisian',
    'gl': 'Galician',
    'ka': 'Georgian',
    'de': 'German',
    'el': 'Greek',
    'gu': 'Gujarati',
    'ht': 'Haitian Creole',
    'ha': 'Hausa',
    'haw': 'Hawaiian',
    'he': 'Hebrew',
    'iw': 'Hebrew',
    'hi': 'Hindi',
    'hmn': 'Hmong',
    'hu': 'Hungarian',
    'is': 'Icelandic',
    'ig': 'Igbo',
    'id': 'Indonesian',
    'ga': 'Irish',
    'it': 'Italian',
    'ja': 'Japanese',
    'jw': 'Javanese',
    'kn': 'Kannada',
    'kk': 'Kazakh',
    'km': 'Khmer',
    'ko': 'Korean',
    'ku': 'Kurdish (Kurmanji)',
    'ky': 'Kyrgyz',
    'lo': 'Lao',
    'la': 'Latin',
    'lv': 'Latvian',
    'lt': 'Lithuanian',
    'lb': 'Luxembourgish',
    'mk': 'Macedonian',
    'mg': 'Malagasy',
    'ms': 'Malay',
    'ml': 'Malayalam',
    'mt': 'Maltese',
    'mi': 'Maori',
    'mr': 'Marathi',
    'mn': 'Mongolian',
    'my': 'Myanmar (Burmese)',
    'ne': 'Nepali',
    'no': 'Norwegian',
    'ps': 'Pashto',
    'fa': 'Persian',
    'pl': 'Polish',
    'pt': 'Portuguese',
    'pa': 'Punjabi',
    'ro': 'Romanian',
    'ru': 'Russian',
    'sm': 'Samoan',
    'gd': 'Scots Gaelic',
    'sr': 'Serbian',
    'st': 'Sesotho',
    'sn': 'Shona',
    'sd': 'Sindhi',
    'si': 'Sinhala',
    'sk': 'Slovak',
    'sl': 'Slovenian',
    'so': 'Somali',
    'es': 'Spanish',
    'su': 'Sundanese',
    'sw': 'Swahili',
    'sv': 'Swedish',
    'tg': 'Tajik',
    'ta': 'Tamil',
    'te': 'Telugu',
    'th': 'Thai',
    'tr': 'Turkish',
    'uk': 'Ukrainian',
    'ur': 'Urdu',
    'uz': 'Uzbek',
    'vi': 'Vietnamese',
    'cy': 'Welsh',
    'xh': 'Xhosa',
    'yi': 'Yiddish',
    'yo': 'Yoruba',
    'zu': 'Zulu'
};
exports.langs = langs;
/**
 * Returns the ISO 639-1 code of the desiredLang – if it is supported by Google Translate
 * @param {string} desiredLang – the name or the code(case sensitive) of the desired language
 * @returns {string|boolean} The ISO 639-1 code of the language or false if the language is not supported
 */
function getCode(desiredLang) {
    if (!desiredLang) {
        return false;
    }
    if (langs[desiredLang]) {
        return desiredLang;
    }
    const keys = Object.keys(langs).filter(function (key) {
        if (typeof langs[key] !== 'string') {
            return false;
        }
        return langs[key].toLowerCase() === desiredLang.toLowerCase();
    });
    return keys[0] || false;
}
function isSupported(desiredLang) {
    return Boolean(getCode(desiredLang));
}
function translate(text, opts) {
    opts = opts || {};
    [opts.from, opts.to].forEach(function (lang) {
        if (lang && !isSupported(lang)) {
            var e = new Error();
            e.code = 400;
            e.message = 'The language \'' + lang + '\' is not supported';
            throw e;
        }
    });
    opts.format = opts.format || 'auto';
    opts.to = opts.to || 'en';
    opts.tld = opts.tld || 'cn';
    opts.client = opts.client || 'webapp';
    opts.from = getCode(opts.from) || '';
    opts.to = getCode(opts.to) || '';
    var token = getToken(text, {
        tld: opts.tld
    });
    var res = http.post(url.format({
        protocol: 'https:',
        hostname: `translate.google.${opts.tld}`,
        pathname: '/translate_a/t',
        query: {
            client: opts.client,
            format: opts.format,
            sl: opts.from,
            tl: opts.to,
            hl: opts.from,
            tk: token.value
        }
    }), {
        headers: {
            referer: `https://translate.google.${opts.tld}`
        },
        body: {
            q: text
        }
    });
    var res = res.data.toString();
    try {
        return JSON.parse(res);
    }
    catch (e) {
        console.error(`error occured when parsing result!`);
        console.error(`res is: `, res);
        throw new Error(e);
    }
}
exports.translate = translate;
