var test = require("test");
test.setup();

const { execFileSync } = require('child_process');

function runEnvOutside(addEnv, code) {
    return execFileSync(
        process.execPath,
        ['-e', `process.stdout.write(String(${code}));`],
        {
            env: {
                ...process.env,
                ...addEnv
            }
        }
    ).stdout;
}

var enero = runEnvOutside(
    { LANG: 'es' },
    'new Intl.DateTimeFormat(undefined, { month: "long" } ).format(new Date(9e8))'
);

const zones = [
    'America/New_York',
    'UTC',
    'Asia/Irkutsk',
    'Australia/North',
    'Antarctica/South_Pole',
];

const locales = [
    'en', 'zh', 'hi', 'es',
    'fr', 'ar', 'bn', 'ru',
    'pt', 'ur', 'id', 'de',
    'ja', 'pcm', 'mr', 'te',
];

describe('icu', () => {
    it('Date.toString', () => {
        assert.deepEqual(
            zones.map((TZ) => runEnvOutside({ TZ }, 'new Date(333333333333).toString()')),
            [
                "Thu Jul 24 1980 20:35:33 GMT-0400 (Eastern Daylight Time)",
                "Fri Jul 25 1980 00:35:33 GMT+0000 (Coordinated Universal Time)",
                "Fri Jul 25 1980 08:35:33 GMT+0800 (Irkutsk Standard Time)",
                "Fri Jul 25 1980 10:05:33 GMT+0930 (Australian Central Standard Time)",
                "Fri Jul 25 1980 12:35:33 GMT+1200 (New Zealand Standard Time)"
            ]
        );
    });

    it('Date.toLocaleString', () => {
        assert.deepEqual(
            zones.map((TZ) => runEnvOutside({ TZ }, 'new Date(333333333333).toLocaleString()')),
            [
                "7/24/1980, 8:35:33 PM",
                "7/25/1980, 12:35:33 AM",
                "7/25/1980, 8:35:33 AM",
                "7/25/1980, 10:05:33 AM",
                "7/25/1980, 12:35:33 PM"
            ]
        );
    });

    it('Date.toString with locale', () => {
        assert.deepEqual(
            locales.map((LANG) => runEnvOutside({ LANG, TZ: 'Europe/Zurich' }, 'new Date(333333333333).toString()')),
            [
                "Fri Jul 25 1980 01:35:33 GMT+0100 (Central European Standard Time)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (中欧标准时间)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (मध्य यूरोपीय मानक समय)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (hora estándar de Europa central)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (heure normale d’Europe centrale)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (توقيت وسط أوروبا الرسمي)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (GMT+01:00)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (Центральная Европа, стандартное время)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (Horário Padrão da Europa Central)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (GMT+01:00)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (Waktu Standar Eropa Tengah)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (Mitteleuropäische Normalzeit)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (中央ヨーロッパ標準時)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (GMT+01:00)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (GMT+01:00)",
                "Fri Jul 25 1980 01:35:33 GMT+0100 (GMT+01:00)"
            ]
        );
    });

    it('Date.toLocaleString with locale', () => {
        assert.deepEqual(
            locales.map((LANG) => runEnvOutside({ LANG, TZ: 'Europe/Zurich' }, 'new Date(333333333333).toLocaleString()')),
            [
                "7/25/1980, 1:35:33 AM",
                "1980/7/25 01:35:33",
                "25/7/1980, 1:35:33 am",
                "25/7/1980, 1:35:33",
                "25/07/1980 01:35:33",
                "25‏/7‏/1980 1:35:33 ص",
                "1980-7-25 1:35:33 AM",
                "25.07.1980, 01:35:33",
                "25/07/1980 01:35:33",
                "1980-7-25 1:35:33 AM",
                "25/7/1980 01.35.33",
                "25.7.1980, 01:35:33",
                "1980/7/25 1:35:33",
                "1980-7-25 1:35:33",
                "1980-7-25 1:35:33 AM",
                "1980-7-25 1:35:33 AM"
            ]
        );
    });

    it('Intl.DateTimeFormat', () => {
        assert.deepEqual(
            locales.map((LANG) => runEnvOutside({ LANG, TZ: 'Europe/Zurich' }, 'new Intl.DateTimeFormat().format(333333333333)')),
            [
                "7/25/1980", "1980/7/25",
                "25/7/1980", "25/7/1980",
                "25/07/1980", "25‏/7‏/1980",
                "1980-7-25", "25.07.1980",
                "25/07/1980", "1980-7-25",
                "25/7/1980", "25.7.1980",
                "1980/7/25", "1980-7-25",
                "1980-7-25", "1980-7-25"
            ]
        );
    });

    it('Intl.PluralRules', () => {
        assert.deepEqual(
            locales.map((LANG) => runEnvOutside({ LANG }, 'new Intl.PluralRules().select(0)')),
            [
                'other', 'other', 'one', 'other',
                'one', 'zero', 'one', 'many',
                'one', 'other', 'other', 'other',
                'other', 'one', 'other', 'other',
            ]
        );
    });
});

require.main === module && test.run(console.DEBUG);
