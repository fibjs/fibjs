var test = require("test");
test.setup();

var os = require('os');

var win = os.type == "Windows";

function has_class(o, cls) {
    if (o.class == cls)
        return true;

    var inherits = o.inherits;

    if (inherits)
        for (var i = 0; i < inherits.length; i++)
            if (has_class(inherits[i], cls))
                return true;

    return false;
}

if (win) {
    var http = require("http");
    var gui = require("gui");
    var coroutine = require("coroutine");

    var base_port = coroutine.vmid * 10000;

    describe("gui", () => {
        it("webview", () => {
            var check = false;
            var closed = false;
            var svr = new http.Server(8899 + base_port, (r) => {
                check = true;
            });
            svr.asyncRun();

            var win = gui.open("http://127.0.0.1:" + (8899 + base_port) + "/");

            for (var i = 0; i < 1000 && !check; i++)
                coroutine.sleep(10);

            assert.ok(check);

            win.onclose = () => {
                closed = true;
            };

            win.close();
            win = undefined;

            for (var i = 0; i < 1000 && !closed; i++)
                coroutine.sleep(10);

            coroutine.sleep(100);

            GC();
            assert.notOk(has_class(os.memoryUsage().nativeObjects, 'WebView'));
        });
    });
}

// test.run(console.DEBUG);
