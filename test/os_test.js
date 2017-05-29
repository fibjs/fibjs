var test = require("test");
test.setup();

var test_util = require('./test_util');

var os = require('os');
var io = require('io');

describe('os', () => {
    it('stat', () => {
        console.dir({
            type: os.type,
            version: os.version,
            CPUs: os.CPUs(),
            uptime: os.uptime(),
            loadavg: os.loadavg(),
            totalmem: os.totalmem(),
            freemem: os.freemem()
        });
        console.dir(os.CPUInfo());
        console.dir(os.networkInterfaces());
    });

    it("nativeObjects", () => {
        GC();
        var no1 = test_util.countObject('Buffer');
        var b = new Buffer();
        assert.equal(no1 + 1, test_util.countObject('Buffer'));
        b.dispose();
        assert.equal(no1, test_util.countObject('Buffer'));
    });

    it('time', () => {
        var tms = ['98-4-14', '1998-4', '1998-4-14', '1998-4-14 12:00',
            '1998-4-14 1:00 pm', '1998-4-14 12:12:12.123', '4/14/1998',
            '4/14', '4/14/48', '4/14/49', '4/14/50', '4/14/51',
            '1998-04-14 1:12:12.123 pm', '1998/4/14 12:12:12',
            '98/4/14 12:12:12', '12/4/14 12:12:12',
            'Tue Apr 14 1998 09:46:05 GMT+0800 (CST)',
            'Tue 14 Apr 1998 09:46:05 GMT+0800',
            'Tue Apr 14 1998 09:46:05 GMT', 'Tue 14 Apr 1998 09:46:05 GMT',
            'Tue Apr 14 1998 09:46:05', 'Tue 14 Apr 1998 09:46:05',
            '1998-04-14T12:12:12.123Z'
        ];

        tms.forEach((s) => {
            assert.deepEqual(os.time(s), new Date(s));
        });

        var tmse = ['2000-1-32', '2001-2-29', '2000-13-1', '2000-12-32',
            '13/13/13'
        ];

        tmse.forEach((t) => {
            assert.ok(isNaN(os.time(t).getTime()), t);
        });

        var tmso = ['2000-1-31', '2001-2-28', '2000-12-1', '2000-12-31',
            '12/13/13'
        ];

        tmso.forEach((t) => {
            assert.ok(!isNaN(os.time(t).getTime()), t);
        });
    });

    it('dateAdd', () => {
        var das = [
            ["2000-1-1T10:10:10", 10, "second", "2000-1-1T10:10:20"],
            ["2000-1-1T10:10:10", 10, "minute", "2000-1-1T10:20:10"],
            ["2000-1-1T10:10:10", 10, "hour", "2000-1-1T20:10:10"],
            ["2000-1-1T10:10:10", 10, "day", "2000-1-11T10:10:10"],
            ["2000-1-1T10:10:10", 10, "month", "2000-11-1T10:10:10"],
            ["2000-1-1T10:10:10", 10, "year", "2010-1-1T10:10:10"],
            ["2000-5-1T10:10:10", 10, "month", "2001-3-1T10:10:10"],
            ["2000-1-1T10:10:10", 22, "month", "2001-11-1T10:10:10"],
            ["2000-1-31T10:10:10", 1, "month", "2000-2-29T10:10:10"],
            ["2000-2-29T10:10:10", 1, "month", "2000-3-31T10:10:10"]
        ];

        das.forEach((s) => {
            assert.deepEqual(os.dateAdd(os.time(s[0]), s[1], s[2]), os.time(s[3]));
        });
    });

    it('tmpdir', () => {
        process.env.TMPDIR = '/tmpdir';
        process.env.TMP = '/tmp';
        process.env.TEMP = '/temp';

        if (process.platform === 'win32') {
            process.env.SystemRoot = "\\SystemRoot";
            process.env.windir = "\\windir";

            assert.equal(os.tmpdir(), '\\temp');
            process.env.TEMP = '';
            assert.equal(os.tmpdir(), '\\tmp');
            delete process.env.TEMP;
            assert.equal(os.tmpdir(), '\\tmp');
            process.env.TMP = '';
            assert.equal(os.tmpdir(), "\\SystemRoot\\temp");
            delete process.env.TMP;
            assert.equal(os.tmpdir(), "\\SystemRoot\\temp");
            process.env.SystemRoot = '';
            assert.equal(os.tmpdir(), "\\windir\\temp");
            delete process.env.SystemRoot;
            assert.equal(os.tmpdir(), "\\windir\\temp");

            process.env.TEMP = '/temp/';
            assert.equal(os.tmpdir(), '\\temp');
            process.env.TEMP = '/temp/////';
            assert.equal(os.tmpdir(), '\\temp');
            process.env.TEMP = 'c:\\';
            assert.equal(os.tmpdir(), 'c:\\');
        } else {
            assert.equal(os.tmpdir(), '/tmpdir');
            process.env.TMPDIR = '';
            assert.equal(os.tmpdir(), '/tmp');
            delete process.env.TMPDIR;
            assert.equal(os.tmpdir(), '/tmp');
            process.env.TMP = '';
            assert.equal(os.tmpdir(), '/temp');
            delete process.env.TMP;
            assert.equal(os.tmpdir(), '/temp');
            process.env.TEMP = '';
            assert.equal(os.tmpdir(), '/tmp');
            delete process.env.TEMP;
            assert.equal(os.tmpdir(), '/tmp');

            process.env.TEMP = '/temp/';
            assert.equal(os.tmpdir(), '/temp');
            process.env.TEMP = '/temp/////';
            assert.equal(os.tmpdir(), '/temp');
        }
    });
});

// test.run(console.DEBUG);