var test = require("test");
test.setup();

var test_util = require('./test_util');

var os = require('os');
var io = require('io');
var path = require('path');

var isWindows = process.platform === 'win32';

describe('os', () => {
    it('stat', () => {
        console.dir({
            type: os.type(),
            version: os.version,
            CPUs: os.cpuNumbers(),
            uptime: os.uptime(),
            loadavg: os.loadavg(),
            totalmem: os.totalmem(),
            freemem: os.freemem()
        });
        console.dir(os.cpus());
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

    it('endianness', () => {
        const endianness = os.endianness();
        assert.isString(endianness);
        assert(/[BL]E/.test(endianness));
    });

    it('release', () => {
        const release = os.release();
        assert.isString(release);
    });

    it('homedir', () => {
        const homedir = os.homedir();
        assert.isString(homedir);
        if (isWindows) {
            assert.equal(os.homedir(), process.env.USERPROFILE);
        } else {
            assert.equal(os.homedir(), process.env.HOME);
        }
    });

    it('userInfo', () => {
        const userInfo = os.userInfo();
        const userInfoBuffer = os.userInfo({
            encoding: 'buffer'
        });
        assert.isObject(userInfo);

        if (isWindows) {
            assert.strictEqual(userInfo.uid, -1);
            assert.strictEqual(userInfo.gid, -1);
            assert.strictEqual(userInfo.shell, null);
            assert.strictEqual(userInfoBuffer.uid, -1);
            assert.strictEqual(userInfoBuffer.gid, -1);
            assert.strictEqual(userInfoBuffer.shell, null);
        } else {
            assert.isNumber(userInfo.uid);
            assert.isNumber(userInfo.gid);
            assert.ok(userInfo.shell.includes(path.sep));
            assert.strictEqual(userInfo.uid, userInfoBuffer.uid);
            assert.strictEqual(userInfo.gid, userInfoBuffer.gid);
            assert.strictEqual(userInfo.shell, userInfoBuffer.shell.toString('utf8'));
        }

        assert.isString(userInfo.username);
        assert.ok(userInfo.homedir.includes(path.sep));
        assert.strictEqual(userInfo.username, userInfoBuffer.username.toString('utf8'));
        assert.strictEqual(userInfo.homedir, userInfoBuffer.homedir.toString('utf8'));
    });
});

repl && test.run(console.DEBUG);