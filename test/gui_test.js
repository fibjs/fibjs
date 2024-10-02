var test = require("test");
test.setup();

var gui = require("gui");
const url = require('url');
const path = require('path');
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

    it("post non-string message", () => {
        const win = gui.open({
            width: 100,
            height: 100
        });

        var received_message;
        win.on("message", (msg) => {
            received_message = msg.data;
        });

        win.eval(`window.postMessage({num:1});`);
        win.eval(`window.postMessage("Hello World");`);

        for (var i = 0; i < 1000; i++) {
            coroutine.sleep(10);
            if (received_message) {
                break;
            }
        }
        win.close();

        assert.equal(received_message, "Hello World");
    });

    describe("load", () => {
        const path1 = path.join(__dirname, "gui_files", "test.html");
        const path2 = path.join(__dirname, "gui_files", "test2.html");
        const url1 = url.pathToFileURL(path1).href;
        const url2 = url.pathToFileURL(path2).href;

        function assert_url(win, url) {
            var received_message;
            win.on("message", (msg) => {
                received_message = msg.data;
            });

            var last_received_message;
            var get_url;
            for (var i = 0; i < 1000; i++) {
                win.eval(`window.postMessage(window.location.href);`);
                coroutine.sleep(100);
                if (received_message && received_message != "about:blank" && last_received_message == received_message) {
                    break;
                }
                last_received_message = received_message;
            }

            get_url = win.getUrl();

            win.close();

            assert.equal(received_message.toLowerCase(), url.toLowerCase());
            assert.equal(get_url.toLowerCase(), url.toLowerCase());
        }

        it("open url", () => {
            const win = gui.open(url1, {
                width: 100,
                height: 100
            });

            assert_url(win, url1);
        });

        it("open url in opt", () => {
            const win = gui.open({
                url: url1,
                width: 100,
                height: 100
            });

            assert_url(win, url1);
        });

        it("diffent url in arg and opt", () => {
            const win = gui.open(url1, {
                url: url2,
                width: 100,
                height: 100
            });

            assert_url(win, url1);
        });

        it("open file", () => {
            const win = gui.openFile(path1, {
                width: 100,
                height: 100
            });

            assert_url(win, url1);
        });

        it("open file in opt", () => {
            const win = gui.open({
                file: path1,
                width: 100,
                height: 100
            });

            assert_url(win, url1);
        });

        it("diffent path in arg and opt", () => {
            const win = gui.openFile(path1, {
                file: path2,
                width: 100,
                height: 100
            });

            assert_url(win, url1);
        });

        it("load url and path in opt", () => {
            const win = gui.open(url1, {
                file: path2,
                width: 100,
                height: 100
            });

            assert_url(win, url1);
        });

        it("load path and url in opt", () => {
            const win = gui.openFile(path1, {
                url: url2,
                width: 100,
                height: 100
            });

            assert_url(win, url1);
        });

        it("load url in WebView", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });

            win.loadURL(url1);

            assert_url(win, url1);
        });

        it("load file in WebView", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });

            win.loadFile(path1);

            assert_url(win, url1);
        });
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

    it("change title", () => {
        const win = gui.open({
            width: 100,
            height: 100
        });

        for (var i = 0; i < 100; i++) {
            if (win.getTitle() == "")
                break;
            coroutine.sleep(100);
        }
        assert.equal(win.getTitle(), "");

        win.eval(`window.document.title = "Hello World";`);

        for (var i = 0; i < 100; i++) {
            if (win.getTitle() == "Hello World")
                break;
            coroutine.sleep(100);
        }
        assert.equal(win.getTitle(), "Hello World");

        win.setTitle("Hello World 2");
        assert.equal(win.getTitle(), "Hello World 2");

        win.close();
    });

    it("multilanguage", () => {
        var langs = [
            "Hello World",
            "你好，世界",
            "こんにちは世界",
            "안녕하세요 세계",
            "Привет, мир",
            "Hola Mundo",
            "Bonjour le monde",
            "Hallo Welt",
            "Ciao mondo",
            "Olá Mundo",
            "Hej världen",
            "Hei maailma",
            "Merhaba Dünya",
            "Hallå världen"
        ];

        const win = gui.open({
            width: 100,
            height: 100
        });

        for (var i = 0; i < 100; i++) {
            if (win.getTitle() == "")
                break;
            coroutine.sleep(100);
        }
        assert.equal(win.getTitle(), "");

        langs.forEach(lang => {
            console.log(lang);
            win.setTitle(lang);
            assert.equal(win.getTitle(), lang);
        });

        win.close();
    });
});

require.main === module && test.run(console.DEBUG);
