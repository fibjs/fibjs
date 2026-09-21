var os = require('os');
var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');

process.on('SIGINT', () => {
    coroutine.fibers.forEach(f => console.error("\nFiber %d:\n%s", f.id, f.stack));
    process.exit(1);
});

var ss = [];

// Environment keys that a filtered env has to keep for the child process to
// start at all under emulation: the qemu-user behind the cross-arch CI builds
// finds the guest loader through QEMU_LD_PREFIX (the build images keep the
// loader outside /lib64, e.g. /usr/mips64el-linux-gnuabi64/lib64/ld.so.1), so a
// child spawned without it dies with
// "/lib64/ld.so.1: No such file or directory" and exit code 255.
exports.emulationEnvKeys = [
    'QEMU_LD_PREFIX'
];

// Environment keys that must survive a filtered env when a child process is
// spawned on an iOS simulator.  The simulator loads the binary through the
// platform loader (DYLD_ROOT_PATH + SIMULATOR_ROOT) and attaches the process to
// the device data container through SIMULATOR_SHARED_RESOURCES_DIRECTORY; a
// child started without them silently fails to run (empty stdout, exit 0).
exports.simulatorEnvKeys = [
    'DYLD_ROOT_PATH',
    'SIMULATOR_ROOT',
    'SIMULATOR_SHARED_RESOURCES_DIRECTORY'
];

// Everything a spawned child always needs to keep
exports.childEnvKeys = exports.emulationEnvKeys.concat(exports.simulatorEnvKeys);

exports.pickEnv = (base, keys) => {
    var out = {};

    keys.concat(exports.childEnvKeys).forEach(k => {
        if (base[k] !== undefined)
            out[k] = base[k];
    });

    return out;
};

exports.countObject = nm => {
    var cnt = 0;

    function count(l) {
        if (l['class'] == nm) {
            cnt += l.objects;
            return;
        }

        if (l.inherits)
            l.inherits.forEach((v) => {
                count(v);
            });
    }

    count(process.memoryUsage().nativeObjects);
    return cnt;
}

exports.makeid = length => {
    var result = '';
    var characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    var charactersLength = characters.length;
    for (var i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

exports.gc = () => {
    var t1 = new Date();
    while (new Date() - t1 < 1000) {
        // Allocate large memory to force GC
        for (var i = 0; i < 10; i++)
            new Array(1024 * 1024);
        coroutine.sleep(1);
        gc();
    }
}

// Event delivery is asynchronous: an emitted event is posted to the isolate's
// job queue, and a single `coroutine.sleep(0)` (a 0ms timer) does not guarantee
// that the job has run by the time the next statement executes -- with a busy
// job queue the delivery can take milliseconds.  Wait for the flag instead of
// assuming a fixed number of yields.
exports.waitUntil = (fn, timeout = 1000) => {
    var t1 = Date.now();

    while (!fn() && Date.now() - t1 < timeout)
        coroutine.sleep(1);

    return fn();
}

exports.push = s => ss.push(s);

exports.cleanup = () => {
    ss.forEach(s => {
        try {
            s.close();
        } catch (e) {}
    });
}

exports.cleanup_folder = function cleanup_folder(p) {
    var dir = fs.readdir(p);
    console.log("clean", p);
    dir.forEach(function (name) {
        var fname = path.join(p, name);
        var f = fs.stat(fname);
        if (f.isDirectory()) {
            cleanup_folder(fname);
            fs.rmdir(fname);
        } else
            fs.unlink(fname);
    });
}