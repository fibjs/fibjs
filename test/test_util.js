var os = require('os');
var fs = require('fs');
var path = require('path');
var coroutine = require('coroutine');

process.on('SIGINT', () => {
    coroutine.fibers.forEach(f => console.error("\nFiber %d:\n%s", f.id, f.stack));
    process.exit();
});

var ss = [];

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
    for (var i = 0; i < 20; i++) {
        coroutine.sleep(1);
        GC();
    }
}

exports.push = s => ss.push(s);

exports.cleanup = () => {
    ss.forEach(s => {
        s.close();
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