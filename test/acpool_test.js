var { describe, it } = require('node:test');
var assert = require('assert');
var child_process = require('child_process');
var fs = require('fs');
var os = require('os');
var path = require('path');

const isFibjs = typeof process !== 'undefined' && process.versions && process.versions.fibjs;

// Android's bionic userland does not provide the POSIX identity switch this
// test relies on, and the other platforms have no RLIMIT_NPROC to set.
const canRun = isFibjs
    && process.platform === 'linux'
    && fs.exists('/bin/bash')
    && fs.exists('/proc/self/status')
    && typeof process.getuid === 'function';

// The blocking pool (result_t::longsync) parks a floor of OS threads and can
// grow on demand.  A worker can only retire above the floor, and "above the
// floor" must be measured on the workers that actually started: a refused
// pthread_create cannot be reported back (exlib::OSThread::start() is void),
// so a request whose thread never started leaves a worker credit behind.  If
// retirement counted those credits, every real worker could retire after a
// failed creation and a queued longsync call would wait forever.
//
// The trigger is an OS-level thread budget, which a single process cannot set
// for itself from JS.  The test therefore runs the scenario in a child under a
// small RLIMIT_NPROC (`ulimit -u`):
//
//   * as root (the CI containers) it switches the child to an otherwise unused
//     uid, so the budget is the child's alone even though RLIMIT_NPROC is
//     accounted per uid on the whole host;
//   * as an ordinary user it budgets the current uid's existing threads plus a
//     fixed headroom (accurate only when /proc is not namespaced - the probe
//     below skips the environments where that does not hold).
//
// The headroom has to stay well below the burst size: with too much room the
// pool keeps up and almost no creation is refused, which is exactly the case
// the retirement bug needs.  64 leaves ~45 threads above startup on the tested
// targets and reliably refuses plenty of the 256 batch.
//
// A burst of blocking opens is queued while the budget is exhausted, then the
// retired pool must still serve one more blocking call.  The child reports the
// thread count it reached; only a run in which the budget actually refused
// creations proves anything, so a run that managed all of them is skipped
// rather than passed.
//
// RLIMIT_AS is not usable here: V8 reserves tens of GB of address space during
// the burst and the first allocation failure aborts the process (SIGSEGV or
// std::bad_alloc) long before pthread_create is the only thing failing.
var CHILD = `
const fs = require('fs');
const db = require('db');
const coroutine = require('coroutine');

const N = 256;

function snapshot() {
    try {
        const fd = fs.open('/proc/self/status', 'r');
        try {
            const buf = Buffer.alloc(8192);
            const n = fs.read(fd, buf, 0, 8192, 0);
            const txt = buf.slice(0, n).toString();
            const v = /VmSize:\\s+(\\d+) kB/.exec(txt);
            const t = /Threads:\\s+(\\d+)/.exec(txt);
            return (v ? parseInt(v[1], 10) : -1) + ':' + (t ? parseInt(t[1], 10) : -1);
        } finally {
            fs.close(fd);
        }
    } catch (e) {
        return '-1:-1';
    }
}

let pending = N;

function burstDone() {
    if (--pending !== 0)
        return;

    console.log('BURST-DONE ' + snapshot());

    // Idle past POOL_IDLE_MS so workers above the floor retire.
    coroutine.sleep(2500);
    console.log('RETIRED');

    const timer = setTimeout(function () {
        console.log('STUCK');
        process.exit(2);
    }, 10000);

    let ok = false;
    try {
        const conn = db.open('sqlite::memory:');
        conn.close();
        ok = true;
    } catch (e) {
        console.log('ERROR ' + e.message);
    }

    clearTimeout(timer);
    console.log('SINGLE ' + ok);
    process.exit(ok ? 0 : 3);
}

setTimeout(function () {
    console.log('BURST-STUCK pending=' + pending);
    process.exit(4);
}, 30000);

for (let i = 0; i < N; i++) {
    try {
        db.open('sqlite::memory:', function (err, conn) {
            try {
                if (conn)
                    conn.close();
            } catch (e) { }
            burstDone();
        });
    } catch (e) {
        console.log('OPEN-ERROR ' + e.message);
        burstDone();
    }
}
`;

function readStatus(pid) {
    var fd = fs.open('/proc/' + pid + '/status', 'r');
    try {
        var buf = Buffer.alloc(8192);
        var n = fs.read(fd, buf, 0, 8192, 0);
        return buf.slice(0, n).toString();
    } finally {
        fs.close(fd);
    }
}

function countUserThreads(uid) {
    var total = 0;

    fs.readdir('/proc').forEach(function (name) {
        if (!/^\d+$/.test(name))
            return;

        try {
            var txt = readStatus(name);
            var u = /^Uid:\s+(\d+)/m.exec(txt);
            var g = /^Tgid:\s+(\d+)/m.exec(txt);
            var t = /^Threads:\s+(\d+)/m.exec(txt);

            // Only thread-group leaders: their Threads field already counts
            // every thread of the group.
            if (u && g && parseInt(u[1], 10) === uid && parseInt(g[1], 10) === parseInt(name, 10))
                total += t ? parseInt(t[1], 10) : 0;
        } catch (e) {
        }
    });

    return total;
}

function runChild(execPath, limit, uid, code, timeout) {
    var opts = {
        cwd: os.tmpdir(),
        env: Object.assign({}, process.env, {
            HOME: os.tmpdir(),
            PATH: '/usr/bin:/bin:/usr/sbin:/sbin'
        }),
        encoding: 'utf8',
        timeout: timeout
    };

    if (uid !== null) {
        opts.uid = uid;
        opts.gid = uid;
    }

    return child_process.spawnSync('/bin/bash',
        ['-c', 'ulimit -u ' + limit + '; exec "$0" -e "$1"', execPath, code], opts);
}

describe('the blocking thread pool', { skip: !canRun }, () => {
    it('keeps a worker when thread creation is refused', (ctx) => {
        var uid = process.getuid();
        var altUid = uid === 0 ? 60000 + (process.pid % 5000) : null;
        var limit;

        if (altUid === null) {
            try {
                limit = countUserThreads(uid) + 64;
            } catch (e) {
                return ctx.skip();
            }
        } else
            limit = 64;

        var execPath = process.execPath;
        var copy = null;

        try {
            var probe = runChild(execPath, limit, altUid, 'console.log("PROBE")', 30000);
            if (!probe || probe.status !== 0 || (probe.stdout || '').indexOf('PROBE') < 0) {
                // Switching the uid can fail when the binary is not reachable
                // through every parent directory.  A copy in the temp dir is
                // owned by root and world-readable, so the child can run it.
                if (altUid === null)
                    return ctx.skip();

                copy = path.join(os.tmpdir(), 'fibjs-acpool-child-' + process.pid);
                fs.copyFileSync(process.execPath, copy);
                fs.chmodSync(copy, 493); // 0755
                execPath = copy;

                probe = runChild(execPath, limit, altUid, 'console.log("PROBE")', 30000);
                if (!probe || probe.status !== 0 || (probe.stdout || '').indexOf('PROBE') < 0) {
                    console.log('[acpool] the thread budget cannot be applied here: ' +
                        (probe ? probe.stderr || probe.stdout : 'spawn failed'));
                    return ctx.skip();
                }
            }

            var r = runChild(execPath, limit, altUid, CHILD, 120000);

            assert.equal(r.status, 0,
                'the pooled call did not complete (status ' + r.status +
                ', signal ' + r.signal + ', error ' + (r.error ? r.error.message : 'none') +
                ')\nstdout:\n' + (r.stdout || '') + '\nstderr:\n' + (r.stderr || ''));
            assert.ok((r.stdout || '').indexOf('BURST-DONE') >= 0,
                'the burst did not finish\nstdout:\n' + (r.stdout || ''));
            assert.ok((r.stdout || '').indexOf('SINGLE true') >= 0,
                'the single call did not succeed\nstdout:\n' + (r.stdout || ''));

            // The unlimited pool reaches one worker per queued task (256 plus
            // the startup threads).  A count below half means the budget
            // refused plenty of creations, including more than the floor, so
            // the run really exercised the retirement path.
            var m = /BURST-DONE\s+\d+:(\d+)/.exec(r.stdout || '');
            assert.ok(m, 'the burst did not report its thread count');

            var threads = parseInt(m[1], 10);
            if (threads <= 0 || threads >= 128)
                return ctx.skip();
        } finally {
            if (copy) {
                try {
                    fs.unlinkSync(copy);
                } catch (e) {
                }
            }
        }
    });
});
