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

function runEnvInside(addEnv, func, ...args) {
    Object.assign(process.env, addEnv); // side effects!
    return func(...args);
}

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
    'ja', 'pcm', 'mr',
];

describe('icu', () => {
    var tz, lang;

    before(() => {
        tz = process.env.TZ;
        lang = process.env.LANG;
        process.env.LANG = "en_US.UTF-8";
    });

    after(() => {
        process.env.TZ = tz;
        process.env.LANG = lang;
    });

    describe('env for child process', () => {
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
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (মধ্য ইউরোপীয় মানক সময়)",
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (Центральная Европа, стандартное время)",
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (Horário Padrão da Europa Central)",
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (وسطی یورپ کا معیاری وقت)",
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (Waktu Standar Eropa Tengah)",
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (Mitteleuropäische Normalzeit)",
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (中央ヨーロッパ標準時)",
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (GMT+01:00)",
                    "Fri Jul 25 1980 01:35:33 GMT+0100 (मध्‍य युरोपियन प्रमाण वेळ)"
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
                    "২৫/৭/১৯৮০, ১:৩৫:৩৩ AM",
                    "25.07.1980, 01:35:33",
                    "25/07/1980, 01:35:33",
                    "25/7/1980، 1:35:33 AM",
                    "25/7/1980, 01.35.33",
                    "25.7.1980, 01:35:33",
                    "1980/7/25 1:35:33",
                    "1980-7-25 1:35:33",
                    "२५/७/१९८०, १:३५:३३ AM"
                ]
            );
        });

        it('Intl.DateTimeFormat', () => {
            assert.deepEqual(
                locales.map((LANG) => runEnvOutside({ LANG, TZ: 'Europe/Zurich' }, 'new Intl.DateTimeFormat().format(333333333333)')),
                [
                    "7/25/1980",
                    "1980/7/25",
                    "25/7/1980",
                    "25/7/1980",
                    "25/07/1980",
                    "25‏/7‏/1980",
                    "২৫/৭/১৯৮০",
                    "25.07.1980",
                    "25/07/1980",
                    "25/7/1980",
                    "25/7/1980",
                    "25.7.1980",
                    "1980/7/25",
                    "1980-7-25",
                    "२५/७/१९८०"
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
                    'other', 'one', 'other'
                ]
            );
        });
    });

    if (process.platform !== 'win32') {
        describe('process.env.TZ', () => {
            it('toString', () => {
                assert.deepEqual(
                    zones.map((TZ) => runEnvInside({ TZ }, () => new Date(333333333333).toString())),
                    [
                        "Thu Jul 24 1980 20:35:33 GMT-0400 (Eastern Daylight Time)",
                        "Fri Jul 25 1980 00:35:33 GMT+0000 (Coordinated Universal Time)",
                        "Fri Jul 25 1980 08:35:33 GMT+0800 (Irkutsk Standard Time)",
                        "Fri Jul 25 1980 10:05:33 GMT+0930 (Australian Central Standard Time)",
                        "Fri Jul 25 1980 12:35:33 GMT+1200 (New Zealand Standard Time)"
                    ]
                );
            });

            it('toLocaleString', () => {
                assert.deepEqual(
                    zones.map((TZ) => runEnvInside({ TZ }, () => new Date(333333333333).toLocaleString())),
                    [
                        "7/24/1980, 8:35:33 PM",
                        "7/25/1980, 12:35:33 AM",
                        "7/25/1980, 8:35:33 AM",
                        "7/25/1980, 10:05:33 AM",
                        "7/25/1980, 12:35:33 PM"
                    ]
                );
            });
        });

        describe('process.env.LANG', () => {
            it('toString', () => {
                assert.deepEqual(
                    locales.map((LANG) => runEnvInside({ LANG, TZ: 'Europe/Zurich' }, () => new Date(333333333333).toString())),
                    [
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (Central European Standard Time)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (中欧标准时间)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (मध्य यूरोपीय मानक समय)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (hora estándar de Europa central)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (heure normale d’Europe centrale)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (توقيت وسط أوروبا الرسمي)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (মধ্য ইউরোপীয় মানক সময়)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (Центральная Европа, стандартное время)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (Horário Padrão da Europa Central)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (وسطی یورپ کا معیاری وقت)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (Waktu Standar Eropa Tengah)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (Mitteleuropäische Normalzeit)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (中央ヨーロッパ標準時)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (GMT+01:00)",
                        "Fri Jul 25 1980 01:35:33 GMT+0100 (मध्‍य युरोपियन प्रमाण वेळ)"
                    ]
                );
            });

            it('toLocaleString', () => {
                assert.deepEqual(
                    locales.map((LANG) => runEnvInside({ LANG, TZ: 'Europe/Zurich' }, () => new Date(333333333333).toLocaleString())),
                    [
                        "7/25/1980, 1:35:33 AM",
                        "1980/7/25 01:35:33",
                        "25/7/1980, 1:35:33 am",
                        "25/7/1980, 1:35:33",
                        "25/07/1980 01:35:33",
                        "25‏/7‏/1980 1:35:33 ص",
                        "২৫/৭/১৯৮০, ১:৩৫:৩৩ AM",
                        "25.07.1980, 01:35:33",
                        "25/07/1980, 01:35:33",
                        "25/7/1980، 1:35:33 AM",
                        "25/7/1980, 01.35.33",
                        "25.7.1980, 01:35:33",
                        "1980/7/25 1:35:33",
                        "1980-7-25 1:35:33",
                        "२५/७/१९८०, १:३५:३३ AM"
                    ]
                );
            });

            it('DateTimeFormat().format', () => {
                assert.deepEqual(
                    locales.map((LANG) => runEnvInside({ LANG, TZ: 'Europe/Zurich' }, () => new Intl.DateTimeFormat().format(333333333333))),
                    [
                        "7/25/1980",
                        "1980/7/25",
                        "25/7/1980",
                        "25/7/1980",
                        "25/07/1980",
                        "25‏/7‏/1980",
                        "২৫/৭/১৯৮০",
                        "25.07.1980",
                        "25/07/1980",
                        "25/7/1980",
                        "25/7/1980",
                        "25.7.1980",
                        "1980/7/25",
                        "1980-7-25",
                        "२५/७/१९८०"
                    ]
                );
            });
        });
    }

    it('Intl.Segmenter', () => {

        const cases = [
            {
                "text": "Hello world!",
                "words": [
                    "Hello",
                    "world"
                ]
            },
            {
                "text": " Hello world! ",
                "words": [
                    "Hello",
                    "world"
                ]
            },
            {
                "text": " Hello world? Foo bar!",
                "words": [
                    "Hello",
                    "world",
                    "Foo",
                    "bar"
                ]
            },
            {
                "text": "Jedovatou mambu objevila žena v zahrádkářské kolonii.",
                "words": [
                    "Jedovatou",
                    "mambu",
                    "objevila",
                    "žena",
                    "v",
                    "zahrádkářské",
                    "kolonii"
                ]
            },
            {
                "text": "Việt Nam: Nhất thể hóa sẽ khác Trung Quốc?",
                "words": [
                    "Việt",
                    "Nam",
                    "Nhất",
                    "thể",
                    "hóa",
                    "sẽ",
                    "khác",
                    "Trung",
                    "Quốc"
                ]
            },
            {
                "text": "Σοβαρές ενστάσεις Κομισιόν για τον προϋπολογισμό της Ιταλίας",
                "words": [
                    "Σοβαρές",
                    "ενστάσεις",
                    "Κομισιόν",
                    "για",
                    "τον",
                    "προϋπολογισμό",
                    "της",
                    "Ιταλίας"
                ]
            },
            {
                "text": "Решение Индии о покупке российских С-400 расценили как вызов США",
                "words": [
                    "Решение",
                    "Индии",
                    "о",
                    "покупке",
                    "российских",
                    "С",
                    "400",
                    "расценили",
                    "как",
                    "вызов",
                    "США"
                ]
            },
            {
                "text": "הרופא שהציל נשים והנערה ששועבדה ע",
                "words": [
                    "הרופא",
                    "שהציל",
                    "נשים",
                    "והנערה",
                    "ששועבדה",
                    "ע"
                ]
            },
            {
                "text": "ترامب للملك سلمان: أنا جاد للغاية.. عليك دفع المزيد",
                "words": [
                    "ترامب",
                    "للملك",
                    "سلمان",
                    "أنا",
                    "جاد",
                    "للغاية",
                    "عليك",
                    "دفع",
                    "المزيد"
                ]
            },
            {
                "text": "भारत की एस 400 मिसाइल के मुकाबले पाक की थाड, जानें कौन कितना ताकतवर",
                "words": [
                    "भारत",
                    "की",
                    "एस",
                    "400",
                    "मिसाइल",
                    "के",
                    "मुकाबले",
                    "पाक",
                    "की",
                    "थाड",
                    "जानें",
                    "कौन",
                    "कितना",
                    "ताकतवर"
                ]
            },
            {
                "text": "ரெட் அலர்ட் எச்சரிக்கை; புதுச்சேரியில் நாளை அரசு விடுமுறை!",
                "words": [
                    "ரெட்",
                    "அலர்ட்",
                    "எச்சரிக்கை",
                    "புதுச்சேரியில்",
                    "நாளை",
                    "அரசு",
                    "விடுமுறை"
                ]
            },
            {
                "text": "'ఉత్తర్వులు అందే వరకు ఓటర్ల తుది జాబితాను వెబ్‌సైట్లో పెట్టవద్దు'",
                "words": [
                    "ఉత్తర్వులు",
                    "అందే",
                    "వరకు",
                    "ఓటర్ల",
                    "తుది",
                    "జాబితాను",
                    "వెబ్‌సైట్లో",
                    "పెట్టవద్దు"
                ]
            },
            {
                "text": "台北》抹黑柯P失敗？朱學恒酸：姚文智氣pupu嗆大老闆",
                "words": [
                    "台北",
                    "抹黑",
                    "柯",
                    "P",
                    "失敗",
                    "朱學",
                    "恒",
                    "酸",
                    "姚",
                    "文",
                    "智",
                    "氣",
                    "pupu",
                    "嗆",
                    "大",
                    "老闆"
                ]
            },
            {
                "text": "วัดไทรตีระฆังเบาลงช่วงเข้าพรรษา เจ้าอาวาสเผยคนร้องเรียนรับผลกรรมแล้ว",
                "words": [
                    "วัด",
                    "ไทร",
                    "ตี",
                    "ระฆัง",
                    "เบา",
                    "ลง",
                    "ช่วง",
                    "เข้า",
                    "พรรษา",
                    "เจ้า",
                    "อาวาส",
                    "เผย",
                    "คน",
                    "ร้อง",
                    "เรียน",
                    "รับ",
                    "ผล",
                    "กรรม",
                    "แล้ว"
                ]
            },
            {
                "text": "九州北部の一部が暴風域に入りました(日直予報士 2018年10月06日) - 日本気象協会 tenki.jp",
                "words": [
                    "九州",
                    "北部",
                    "の",
                    "一部",
                    "が",
                    "暴風",
                    "域",
                    "に",
                    "入り",
                    "ま",
                    "した",
                    "日",
                    "直",
                    "予報",
                    "士",
                    "2018",
                    "年",
                    "10",
                    "月",
                    "06",
                    "日",
                    "日本",
                    "気象",
                    "協会",
                    "tenki",
                    "jp"
                ]
            },
            {
                "text": "법원 “다스 지분 처분권·수익권 모두 MB가 보유”",
                "words": [
                    "법원",
                    "다스",
                    "지분",
                    "처분권",
                    "수익권",
                    "모두",
                    "MB",
                    "가",
                    "보유"
                ]
            }
        ];

        const seg = new Intl.Segmenter([], { granularity: "word" })

        for (const c of cases) {
            const iter = seg.segment(c.text);
            const words = [];
            for (const { segment, isWordLike } of iter) {
                if (isWordLike)
                    words.push(segment);
            }

            assert.deepEqual(words, c.words);
        }
    });
});

require.main === module && test.run(console.DEBUG);
