var os = require('os');
var coroutine = require('coroutine');

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

    count(os.memoryUsage().nativeObjects);
    return cnt;
}

exports.gc = () => {
    for (var i = 0; i < 20; i++) {
        coroutine.sleep(1);
        GC();
    }
}