var test = require("test");
test.setup();

var test_util = require("./test_util");

var path = require("path");
var os = require('os');
var fs = require('fs');
var gd = require('gd');
var child_process = require('child_process');
var gui = require("gui");
var util = require("util");
var coroutine = require("coroutine");
var encoding = require("encoding");

var cef_path = path.join(__dirname, "../temp/cef", os.type());
console.log(cef_path);
var cef_files_path = path.join(__dirname, "cef_files");

gui.config({
    "cache_path": `${os.homedir()}/.cache`,
    "cef_path": cef_path,
    "backend": {
        "cef://test/": cef_files_path,
        "http://cef_test/": {
            "/mock/:act": (r, a) => {
                if (a == 'header') {
                    r.response.write(r.firstHeader('test'));
                    r.response.addHeader("test", "world");
                } else if (a == 'post') {
                    r.response.write(`[${r.firstHeader("Content-type")}]`);
                    r.response.write(r.body.readAll());
                }
            },
            "/*": cef_files_path
        }
    }
});

var key_code = {
    "Backspace": 8,
    "Tab": 9,
    "Enter": 13,
    "ShiftLeft": 16,
    "ShiftRight": 16,
    "ControlLeft": 17,
    "ControlRight": 17,
    "AltLeft": 18,
    "AltRight": 18,
    "Pause": 19,
    "CapsLock": 20,
    "Escape": 27,
    "Theevent": 32,
    "PageUp": 33,
    "PageDown": 34,
    "End": 35,
    "Home": 36,
    "ArrowLeft": 37,
    "ArrowUp": 38,
    "ArrowRight": 39,
    "ArrowDown": 40,
    "PrintScreen": 44,
    "Insert": 45,
    "Delete": 46,
    "Digit0": 48,
    "Digit1": 49,
    "Digit2": 50,
    "Digit3": 51,
    "Digit4": 52,
    "Digit5": 53,
    "Digit6": 54,
    "Digit7": 55,
    "Digit8": 56,
    "Digit9": 57,
    "KeyA": 65,
    "KeyB": 66,
    "KeyC": 67,
    "KeyD": 68,
    "KeyE": 69,
    "KeyF": 70,
    "KeyG": 71,
    "KeyH": 72,
    "KeyI": 73,
    "KeyJ": 74,
    "KeyK": 75,
    "KeyL": 76,
    "KeyM": 77,
    "KeyN": 78,
    "KeyO": 79,
    "KeyP": 80,
    "KeyQ": 81,
    "KeyR": 82,
    "KeyS": 83,
    "KeyT": 84,
    "KeyU": 85,
    "KeyV": 86,
    "KeyW": 87,
    "KeyX": 88,
    "KeyY": 89,
    "KeyZ": 90,
    "MetaLeft": 91,
    "MetaRight": 92,
    "ContextMenu": 93,
    "Numpad0": 96,
    "Numpad1": 97,
    "Numpad2": 98,
    "Numpad3": 99,
    "Numpad4": 100,
    "Numpad5": 101,
    "Numpad6": 102,
    "Numpad7": 103,
    "Numpad8": 104,
    "Numpad9": 105,
    "NumpadMultiply": 106,
    "NumpadAdd": 107,
    "NumpadSubtract": 109,
    "NumpadDecimal": 110,
    "NumpadDivide": 111,
    "F1": 112,
    "F2": 113,
    "F3": 114,
    "F4": 115,
    "F5": 116,
    "F6": 117,
    "F7": 118,
    "F8": 119,
    "F9": 120,
    "F10": 121,
    "F11": 122,
    "F12": 123,
    "NumLock": 144,
    "ScrollLock": 145,
    "AudioVolumeMute": 173,
    "AudioVolumeDown": 174,
    "AudioVolumeUp": 175,
    "LaunchMediaPlayer": 181,
    "LaunchApplication1": 182,
    "LaunchApplication2": 183,
    "Semicolon": 186,
    "Equal": 187,
    "Comma": 188,
    "Minus": 189,
    "Period": 190,
    "Slash": 191,
    "Backquote": 192,
    "BracketLeft": 219,
    "Backslash": 220,
    "BracketRight": 221,
    "Quote": 222
};

describe("cef", () => {
    var svr;

    function getOuterHTML(win, selector) {
        var doc = win.dev.DOM.getDocument();
        var e = win.dev.DOM.querySelector({
            nodeId: doc.root.nodeId,
            selector: selector
        });

        var html = win.dev.DOM.getOuterHTML({
            nodeId: e.nodeId
        });

        return html.outerHTML;
    }

    function click(win, selector) {
        var doc = win.dev.DOM.getDocument();
        var e = win.dev.DOM.querySelector({
            nodeId: doc.root.nodeId,
            selector: selector
        });

        var box = win.dev.DOM.getBoxModel({
            nodeId: e.nodeId
        });

        var mi = {
            x: box.model.content[0] + 1,
            y: box.model.content[1] + 1,
            button: 'left',
            clickCount: 1
        }

        mi.type = "mousePressed";
        win.dev.Input.dispatchMouseEvent(mi);

        mi.type = "mouseReleased";
        win.dev.Input.dispatchMouseEvent(mi);
    }

    function type(win, text) {
        for (char of text) {
            win.dev.Input.dispatchKeyEvent({
                type: "char",
                text: char
            });
        }
    }

    function press(win, key) {
        var code = key_code[key];
        var ki = {
            key: key,
            code: key,
            windowsVirtualKeyCode: code,
            nativeVirtualKeyCode: code,
            macCharCode: code
        };

        ki.type = "keyDown";
        win.dev.Input.dispatchKeyEvent(ki);

        ki.type = "keyUp";
        win.dev.Input.dispatchKeyEvent(ki);
    }

    describe("basic", () => {
        it("basic", () => {
            var step = 0;
            var win = gui.open("cef://test/simple.html");

            win.on("open", () => {
                assert.equal(step, 0);
                step++;
            });

            win.on("load", () => {
                assert.equal(step, 1);
                step++;
                win.close();
            });

            win.on("closed", () => {
                assert.equal(step, 2);
                step++;
                win = undefined;
            });

            for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
                test_util.gc();

            assert.equal(step, 3);
            assert.equal(test_util.countObject("WebView"), 0);
        });

        it("close directly", () => {
            var win = gui.open("cef://test/simple.html");
            win.close();
            win = undefined;

            for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
                test_util.gc();

            assert.equal(test_util.countObject("WebView"), 0);
        });

        it("gc", () => {
            var check = false;
            var win = gui.open("cef://test/simple.html");

            win.on("load", function () {
                this.close();
            });

            win.on("closed", () => {
                check = true;
            });

            win = undefined;

            for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
                test_util.gc();

            assert.equal(test_util.countObject("WebView"), 0);
            assert.ok(check);
        });
    });

    describe("window style", () => {
        function test_style(item, opt, check) {
            var opt1 = util.clone(opt);
            opt1.width = 120;
            opt1.height = 120;

            it(item, done => {
                var win = gui.open("cef://test/simple.html", opt1);

                win.on("load", () => {
                    var doc = win.dev.DOM.getDocument();

                    coroutine.sleep(10);
                    var info = win.dev.DOM.getBoxModel({
                        nodeId: doc.root.nodeId
                    });

                    win.close();

                    try {
                        check(info.model.width, info.model.height);
                        done();
                    } catch (e) {
                        done(e);
                    }
                });
            });
        }

        test_style("normal", {},
            os.type() == "Linux" ? assert.equal : assert.greaterThan);

        test_style("no resize", {
                resizable: false
            },
            os.type() == "Linux" ? assert.equal : assert.greaterThan);

        test_style("no border", {
            border: false
        }, assert.equal);

        test_style("no caption", {
            caption: false
        }, assert.equal);

        test_style("maximize", {
            maximize: true
        }, (w, h) => {
            assert.greaterThan(w, 120);
            assert.greaterThan(h, 120);
        });

        test_style("fullscreen", {
            fullscreen: true,
            border: false
        }, (w, h) => {
            assert.greaterThan(w, 120);
            assert.greaterThan(h, 120);
        });

        test_style("headless", {
            headless: true
        }, (w, h) => {
            assert.equal(w, 120);
            assert.equal(h, 120);
        });
    });

    describe("custom backend", () => {
        var opt = {
            width: 100,
            height: 100
        };

        function test_div(item, txt) {
            it(item, done => {
                var win = gui.open(`http://cef_test/${item}.html`, opt);

                win.on("load", () => {
                    var html;
                    for (var i = 0; i < 10; i++) {
                        html = getOuterHTML(win, "div#test");
                        if (html != "<div id=\"test\"></div>")
                            break;
                        coroutine.sleep(10);
                    }

                    win.close();

                    try {
                        assert.equal(html, `<div id=\"test\">${txt}</div>`);
                        done();
                    } catch (e) {
                        done(e);
                    }
                });
            });
        }

        it("basic", done => {
            var win = gui.open("cef://test/basic.html", opt);

            win.on("load", () => {
                var doc = win.dev.DOM.getDocument();
                var e = win.dev.DOM.querySelector({
                    nodeId: doc.root.nodeId,
                    selector: "img"
                });

                var info = win.dev.DOM.getBoxModel({
                    nodeId: e.nodeId
                });

                win.close();

                try {
                    assert.equal(info.model.width, 400);
                    assert.equal(info.model.height, 140);
                    done();
                } catch (e) {
                    done(e);
                }
            });
        });

        test_div("fetch", "hello");

        test_div("xhr", "hello");

        test_div("header", "hello, world");

        test_div("post", "[application/x-www-form-urlencoded]foo=bar&amp;lorem=ipsum");
    });

    describe("devtools", () => {
        it("screenshot on headless", done => {
            var win = gui.open("cef://test/basic.html", {
                headless: true
            });

            win.on("load", () => {
                var ret = win.dev.Page.captureScreenshot({
                    clip: {
                        x: 0,
                        y: 0,
                        width: 100,
                        height: 100,
                        scale: 1
                    }
                });

                try {
                    var img = gd.load(encoding.base64.decode(ret.data));
                    assert.equal(img.width, 100);
                    assert.equal(img.height, 100);
                    assert.equal(img.getPixel(1, 1), gd.rgb(255, 255, 255));
                    done();
                } catch (e) {
                    done(e);
                }

                win.close();
            });
        });

        it("devtools event", done => {
            var win = gui.open("cef://test/basic.html", {
                headless: true
            });

            win.on("open", () => {
                win.dev.Page.enable();
                win.dev.Page.on("frameNavigated", ev => {
                    try {
                        assert.equal(ev.frame.url, "cef://test/basic.html");
                        done();
                    } catch (e) {
                        done(e);
                    }

                    win.close();
                });
            })
        });

        it("dispatch key/mouse event", done => {
            var win = gui.open("cef://test/form.html", {
                // headless: true
            });

            win.on("load", () => {
                click(win, "input#f1");
                type(win, "hello, 这是文本。");

                press(win, "ArrowLeft");
                press(win, "ArrowLeft");
                press(win, "ArrowLeft");
                type(win, "测试");

                click(win, "button#b1");

                var html;
                for (var i = 0; i < 10; i++) {
                    html = getOuterHTML(win, "div#test");
                    if (html != "<div id=\"test\"></div>")
                        break;
                    coroutine.sleep(10);
                }

                win.close();

                try {
                    assert.equal(html, `<div id=\"test\">hello, 这是测试文本。</div>`);
                    done();
                } catch (e) {
                    done(e);
                }
            });
        });

        it("FIX: screenshot with wrong clip", done => {
            var win = gui.open("cef://test/basic.html", {
                headless: true
            });

            win.on("load", () => {
                var ret = win.dev.Page.captureScreenshot({
                    clip: {}
                });

                try {
                    assert.equal(ret.code, -32602);
                    done();
                } catch (e) {
                    done(e);
                }

                win.close();
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);