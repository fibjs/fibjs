var test = require("test");
test.setup();

var test_util = require('./test_util');

var os = require("os");
var path = require("path");

var win = process.platform == 'win32';

var html =
    `<html>
<script>
    external.onmessage = function(m) {
        external.postMessage('send back: ' + m)
    };

    var first = true;
    external.onclose = function() {
        if(first)
        {
            first = false;
            external.postMessage('try close')
            return false;
        }
    }
</script>
</html>`;

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
                r.response.write(html);
            });
            svr.asyncRun();

            var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/");

            var cnt = 0;

            win.onmessage = (m) => {
                cnt++;

                if (m === 'try close') {
                    win.close();
                    win = undefined;
                } else {

                    win.close();
                }
            };

            win.onclosed = () => {
                closed = true;
            };

            win.onload = () => {
                win.postMessage('hello');
            };

            for (var i = 0; i < 1000 && !check; i++)
                coroutine.sleep(10);

            assert.ok(check);

            for (var i = 0; i < 1000 && test_util.countObject('WebView'); i++) {
                coroutine.sleep(100);
                GC();
            }

            GC();
            assert.equal(test_util.countObject('WebView'), 0);
            assert.equal(closed, true);
            assert.equal(cnt, 2);
        });

        it("log", () => {
            var p = process.open(process.execPath, [path.join(__dirname, 'gui_files', 'gui1.js')]);
            var r = p.readLines();
            assert.equal(r[0], 'this is.a log');
            assert.equal(r[1], '\u001b[0;33mthis is.a warn\u001b[0m');
            assert.equal(r[2].substr(0, 21), '\x1b[1;31mWebView Error:');
        });

        it("debug", () => {
            var p = process.open(process.execPath, [path.join(__dirname, 'gui_files', 'gui2.js')]);
            var r = p.readLines();
            assert.equal(r.length, 0);
        });
    });
}

repl && test.run(console.DEBUG);