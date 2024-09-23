var test = require("test");
test.setup();

var gui = require("gui");
var coroutine = require("coroutine");

var win32 = process.platform === "win32";
var darwin64 = process.platform === "darwin";

describe("gui", () => {
    it("open and close", () => {
        const win = gui.open({
            width: 100,
            height: 100
        });

        var closed = false;
        win.on("close", () => {
            closed = true;
        });

        win.close();

        for (var i = 0; i < 1000; i++) {
            coroutine.sleep(10);
            if (closed) {
                break;
            }
        }

        assert.equal(closed, true);
    });

    it("close after closed", () => {
        const win = gui.open({
            width: 100,
            height: 100
        });

        win.close();

        assert.throws(() => {
            win.close();
        });
    });

    it("close from inside", () => {
        const win = gui.open({
            width: 100,
            height: 100
        });

        var closed = false;
        win.on("close", () => {
            closed = true;
        });

        win.eval(`window.close();`);

        for (var i = 0; i < 1000; i++) {
            coroutine.sleep(10);
            if (closed) {
                break;
            }
        }

        assert.equal(closed, true);
    });

    it("post message", () => {
        const win = gui.open({
            width: 100,
            height: 100
        });

        win.eval(`window.addEventListener("message", function (msg) { window.postMessage("send from browser: " + msg.data); });`);

        var received_message;
        win.on("message", (msg) => {
            received_message = msg.data;
        });

        win.postMessage("Hello World");

        for (var i = 0; i < 1000; i++) {
            coroutine.sleep(10);
            if (received_message) {
                break;
            }
        }
        win.close();

        assert.equal(received_message, "send from browser: Hello World");
    });

    describe("window style", () => {
        function fetch_size(opt) {
            const win = gui.open(opt);

            var received_message;
            win.on("message", (msg) => {
                received_message = msg.data;
            });

            win.eval(`window.postMessage(window.innerWidth + "|" + window.innerHeight);`);

            var last_received_message;
            for (var i = 0; i < 1000; i++) {
                win.eval(`window.postMessage(window.innerWidth + "|" + window.innerHeight);`);
                coroutine.sleep(100);
                if (received_message && received_message != "0|0" && last_received_message == received_message) {
                    break;
                }
                last_received_message = received_message;
            }
            win.close();

            var size = received_message.split("|");
            var width = Number(size[0]);
            var height = Number(size[1]);

            return { width, height };
        }

        it("default", () => {
            const { width, height } = fetch_size({
                width: 300,
                height: 300
            });

            if (process.platform == "linux")
                assert.equal(height, width);
            else
                assert.lessThan(height, width);
        });

        it("no frame", () => {
            const { width, height } = fetch_size({
                width: 300,
                height: 300,
                frame: false
            });

            assert.equal(width, 300);
            assert.equal(height, 300);
        });

        it("fullscreen and maximize", () => {
            const size1 = fetch_size({
                fullscreen: true
            });

            const size2 = fetch_size({
                maximize: true
            });

            if (process.platform == "win32")
                assert.greaterThan(size1.width, size2.width);
            else
                assert.equal(size1.width, size2.width);

            assert.greaterThan(size1.height, size2.height);
        });
    });
});

require.main === module && test.run(console.DEBUG);
