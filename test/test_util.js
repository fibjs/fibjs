var os = require('os');

exports.countObject = function(nm) {
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
