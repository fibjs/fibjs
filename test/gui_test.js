var test = require("test");
test.setup();

var test_util = require('./test_util');

var os = require("os");

var win = process.platform == 'win32';

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
            var svr = new http.Server(8999 + base_port, (r) => {
                check = true;
            });
            svr.asyncRun();

            var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/");

            for (var i = 0; i < 1000 && !check; i++)
                coroutine.sleep(10);

            assert.ok(check);

            win.onclose = () => {
                closed = true;
            };

            win.close();
            win = undefined;

            for (var i = 0; i < 1000 && test_util.countObject('WebView'); i++) {
                coroutine.sleep(100);
                GC();
            }

            GC();
            assert.equal(test_util.countObject('WebView'), 0);
        });
    });
}

argv.length && test.run(console.DEBUG);