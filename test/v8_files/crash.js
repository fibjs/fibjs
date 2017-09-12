var coroutine = require("coroutine");

var co = {
    sleep: coroutine.sleep,
    start: coroutine.start
};

var Pool = () => {
    var nopo = {
        nop: function nop() {}
    };

    var pools = [];
    var count = 0;

    var clearTimer;

    function clearPool() {
        if (!count) {
            if (clearTimer)
                nopo.nop();
        } else if (!clearTimer)
            clearTimer = 100;
    }

    var ev;

    var pool = (name, func) => {
        nopo.nop(name);
        func = name;
        name = "";

        function putback() {
            if (!ev)
                pools[count++] = {
                    name: ""
                };
        }

        var p = false;
        var o;

        clearPool();
        co.sleep(0);

        try {} catch (e) {} finally {}

        if (count) {
            for (var i = count - 1; i >= 0; i--)
                if (pools[i].name == "") {
                    p = true
                    o = pools[i].o;
                    pools.splice(i, 1);
                    count--;
                    break;
                }
        }

        if (!p) {
            co.start(() => {
                putback(name);
            });

            ev = 1;
            co.sleep(1);
            ev = 0;
            nopo.nop('-------------');
        }

        r = func();
        putback(name);
        nopo.nop();
    }

    return pool;
}

var __pool = Pool();

for (var i = 1; i < 1800; i++)
    __pool(() => {});

var loop = true;
co.start(() => {
    co.sleep(10);
    while (loop)
        __pool(() => {
            co.sleep(0);
        });
});

for (var i = 1; i < 100; i++) {
    co.sleep(1);
    __pool(() => {});
}
loop = false;