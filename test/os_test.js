var test = require("test");
test.setup();

var test_util = require('./test_util');

var os = require('os');
var io = require('io');
var path = require('path');

var isWindows = process.platform === 'win32';

describe('os', () => {
    it('stat', () => {
        assert.isDefined(os.type());
        assert.isDefined(os.cpuNumbers());
        assert.isDefined(os.loadavg());
        assert.isDefined(os.totalmem());
        assert.isDefined(os.freemem());
    });

    it('cpus', () => {
        var r = os.cpus();
        assert.isArray(r);

        r.forEach(c => {
            assert.property(r[0], 'model');
            assert.property(r[0], 'speed');
            assert.property(r[0], 'times');
        });

        for (var i = 0; i < 1000; i++) {
            for (var j = 0; j < 1000; j++)
                Buffer.alloc(10000);

            var r1 = os.cpus();
            if (r[0].times.user != r1[0].times.user)
                break;
        }

        var r1 = os.cpus();
        assert.notEqual(r[0].times.user, r1[0].times.user);
    });

    it('networkInterfaces', () => {
        var r = os.networkInterfaces();
        assert.isObject(r);

        for (var n in r) {
            var a = r[n];
            assert.isArray(a);

            a.forEach(i => {
                assert.property(i, 'address');
                assert.property(i, 'netmask');
                assert.property(i, 'family');
                assert.property(i, 'mac');
                assert.property(i, 'internal');
            });
        }
    });

    it("nativeObjects", () => {
        test_util.gc();
        var no1 = test_util.countObject('Buffer');
        var b = Buffer.alloc(0);
        assert.equal(no1 + 1, test_util.countObject('Buffer'));
        b = undefined;
        test_util.gc();
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
            '1998-04-14T12:12:12.123Z', 'Sun, 17-Jan-2038 19:14:07 GMT'
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
        var tmpdir = process.env.TMPDIR;
        process.env.TMPDIR = '/tmpdir';
        var tmp = process.env.TMP;
        process.env.TMP = '/tmp';
        var temp = process.env.TEMP;
        process.env.TEMP = '/temp';

        process.env.TMPDIR = tmpdir;
        process.env.TMP = tmp;
        process.env.TEMP = temp;
    });

    it('endianness', () => {
        const endianness = os.endianness();
        assert.isString(endianness);
        assert(/[BL]E/.test(endianness));
    });

    it('release', () => {
        const release = os.release();
        assert.isString(release);
        assert.notEqual(release, "");
    });

    it('arch', () => {
        const arch = os.arch();
        assert.isString(arch);
        assert.notEqual(arch, "");
    });

    it('homedir', () => {
        const homedir = os.homedir();
        assert.isString(homedir);
        if (isWindows) {
            assert.equal(os.homedir(), process.env.USERPROFILE);
            delete process.env.USERPROFILE;
            assert.isString(os.homedir());
            assert.notEqual(os.homedir(), "");
            process.env.USERPROFILE = homedir;
        } else {
            assert.equal(os.homedir(), process.env.HOME);
            delete process.env.HOME;
            assert.isString(os.homedir());
            assert.notEqual(os.homedir(), "");
            process.env.HOME = homedir;
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

        // assert.equal(os.homedir(), userInfo.homedir);
        assert.isString(userInfo.username);
        assert.ok(userInfo.homedir.includes(path.sep));
        assert.strictEqual(userInfo.username, userInfoBuffer.username.toString('utf8'));
        assert.strictEqual(userInfo.homedir, userInfoBuffer.homedir.toString('utf8'));
    });
});

require.main === module && test.run(console.DEBUG);