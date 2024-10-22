var test = require("test");
test.setup();

var gui = require("gui");
const url = require('url');
const path = require('path');
var coroutine = require("coroutine");

var win32 = process.platform === "win32";
var darwin64 = process.platform === "darwin";

describe("gui", () => {
    var wins = [];

    after(() => {
        wins.forEach(win => {
            try {
                win.close();
            } catch (e) { }
        });
    });

    describe("webview", () => {
        it("open and close", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

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
            wins.push(win);

            win.close();

            assert.throws(() => {
                win.close();
            });
        });

        describe("eval", () => {
            it("eval and result", () => {
                const o = {
                    a: 100,
                    b: "bbbb",
                    c: true,
                    d: [1, 2, 3],
                    e: { a: 1, b: 2 },
                    f: null
                }

                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                const o1 = win.eval(`(${JSON.stringify(o)})`);

                win.close();

                assert.deepEqual(o1, o);
            });

            it("fail eval", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert.throws(() => {
                    win.eval(`abc`);
                });

                assert.throws(() => {
                    win.eval(`"abc`);
                });

                win.close();
            });

            it("unknown type", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                const r1 = win.eval(`(new RegExp())`);

                win.close();

                assert.deepEqual(r1, {});
            });
        });

        it("close from inside", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

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
            wins.push(win);

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
            wins.push(win);

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
            const zpath1 = path.join(__dirname, "gui_files", "test.html.zip$/test.html");
            const url1 = url.pathToFileURL(path1).href.replace(/file:/, "fs:");
            const url2 = url.pathToFileURL(path2).href.replace(/file:/, "fs:");
            const zurl1 = url.pathToFileURL(zpath1).href.replace(/file:/, "fs:");

            function assert_url(win, url) {
                var last_received_message;
                var get_url;
                for (var i = 0; i < 1000; i++) {
                    coroutine.sleep(100);
                    var result = win.eval(`window.document.title + "|" +window.location.href`);
                    if (result && result != "|about:blank" && last_received_message == result) {
                        break;
                    }
                    last_received_message = result;
                }

                get_url = win.getUrl();

                win.close();

                assert.equal(result.toLowerCase(), "test|" + url.toLowerCase());
                assert.equal(get_url.toLowerCase(), url.toLowerCase());
            }

            it("open url", () => {
                const win = gui.open(url1, {
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert_url(win, url1);
            });

            it("open url in opt", () => {
                const win = gui.open({
                    url: url1,
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert_url(win, url1);
            });

            it("diffent url in arg and opt", () => {
                const win = gui.open(url1, {
                    url: url2,
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert_url(win, url1);
            });

            it("open file", () => {
                const win = gui.openFile(path1, {
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert_url(win, url1);
            });

            it("open file in opt", () => {
                const win = gui.open({
                    file: path1,
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert_url(win, url1);
            });

            it("diffent path in arg and opt", () => {
                const win = gui.openFile(path1, {
                    file: path2,
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert_url(win, url1);
            });

            it("load url and path in opt", () => {
                const win = gui.open(url1, {
                    file: path2,
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert_url(win, url1);
            });

            it("load path and url in opt", () => {
                const win = gui.openFile(path1, {
                    url: url2,
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert_url(win, url1);
            });

            it("load url in WebView", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                win.loadURL(url1);

                assert_url(win, url1);
            });

            it("load file in WebView", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                win.loadFile(path1);

                assert_url(win, url1);
            });

            it("load file in zip", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                win.loadFile(zpath1);

                assert_url(win, zurl1);
            });
        });

        describe("window style", () => {
            function fetch_size(opt) {
                const win = gui.open(opt);
                wins.push(win);

                var last_received_message;
                for (var i = 0; i < 1000; i++) {
                    coroutine.sleep(100);
                    var result = win.eval(`window.innerWidth + "|" + window.innerHeight`);
                    if (result && result != "0|0" && last_received_message == result) {
                        break;
                    }
                    last_received_message = result;
                }
                win.close();

                var size = result.split("|");
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

                assert.equal(size1.width, size2.width);
                assert.greaterThan(size1.height, size2.height);
            });
        });

        it("change title", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

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

        it("size", () => {
            const win = gui.open({
                width: 300,
                height: 200
            });
            wins.push(win);

            for (var i = 0; i < 100; i++) {
                if (win.getSize()[0] === 300)
                    break;
                coroutine.sleep(100);
            }
            assert.deepEqual(win.getSize(), [300, 200]);

            win.setSize(400, 300);

            for (var i = 0; i < 100; i++) {
                if (win.getSize()[0] === 400)
                    break;
                coroutine.sleep(100);
            }
            assert.deepEqual(win.getSize(), [400, 300]);

            win.close();

        });

        it("position", () => {
            const win = gui.open({
                left: 100,
                top: 100,
                width: 300,
                height: 200
            });
            wins.push(win);

            for (var i = 0; i < 100; i++) {
                if (win.getPosition()[0] === 100)
                    break;
                coroutine.sleep(100);
            }
            assert.deepEqual(win.getPosition(), [100, 100]);

            win.setPosition(200, 200);

            for (var i = 0; i < 100; i++) {
                if (win.getPosition()[0] === 200)
                    break;
                coroutine.sleep(100);
            }
            assert.deepEqual(win.getPosition(), [200, 200]);

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
            wins.push(win);

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

        describe("focus/blur event", () => {
            function test_focus(opts) {
                const win = gui.open({
                    ...opts,
                    left: 100,
                    top: 100,
                    width: 100,
                    height: 100
                });
                wins.push(win);

                var focus = 0;
                var blur = 0;
                var last_focus, last_blur;

                win.on("focus", () => {
                    focus++;
                });

                win.on("blur", () => {
                    blur++;
                });

                last_focus = focus;
                for (var i = 0; i < 500; i++) {
                    if (focus > last_focus)
                        break;
                    coroutine.sleep(100);
                }

                assert.equal(focus, last_focus + 1);

                last_blur = blur;
                const win1 = gui.open({
                    left: 200,
                    top: 200,
                    width: 100,
                    height: 100
                });
                wins.push(win1);

                for (var i = 0; i < 500; i++) {
                    if (blur > last_blur)
                        break;
                    coroutine.sleep(100);
                }

                assert.equal(blur, last_blur + 1);

                last_focus = focus;

                win1.close();
                for (var i = 0; i < 500; i++) {
                    if (focus > last_focus)
                        break;
                    coroutine.sleep(100);
                }

                assert.equal(focus, last_focus + 1);

                win.close();
            }

            it("normal window", () => {
                test_focus({});
            });

            it("no caption", () => {
                test_focus({
                    caption: false
                });
            });

            it("maximize", () => {
                test_focus({
                    maximize: true
                });
            });

            it("no frame", () => {
                test_focus({
                    frame: false
                });
            });

            it("fullscreen", () => {
                test_focus({
                    fullscreen: true
                });
            });
        });

        it("active", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            for (var i = 0; i < 1000; i++) {
                coroutine.sleep(100);
                if (win.isActived()) {
                    break;
                }
            }
            assert.equal(win.isActived(), true);

            const win1 = gui.open({
                left: 100,
                top: 100,
                width: 100,
                height: 100
            });
            wins.push(win1);

            for (var i = 0; i < 1000; i++) {
                coroutine.sleep(100);
                if (win1.isActived()) {
                    break;
                }
            }
            assert.equal(win1.isActived(), true);
            assert.equal(win.isActived(), false);

            win.active();
            for (var i = 0; i < 1000; i++) {
                coroutine.sleep(100);
                if (win.isActived()) {
                    break;
                }
            }
            assert.equal(win.isActived(), true);
            assert.equal(win1.isActived(), false);

            win1.close();
            win.close();
        });

        it("auto focus html", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            for (var i = 0; i < 1000; i++) {
                coroutine.sleep(100);
                var result = win.eval(`document.hasFocus()?"True":"False"`);
                if (result == "True") {
                    break;
                }
            }
            win.close();

            assert.equal(result, "True");
        });

        it("auto focus html after blur", () => {
            const win = gui.open({
                left: 100,
                top: 100,
                width: 100,
                height: 100
            });
            wins.push(win);

            win.setTitle("Test");

            const win1 = gui.open({
                left: 200,
                top: 200,
                width: 100,
                height: 100
            });
            wins.push(win1);
            win1.close();

            for (var i = 0; i < 1000; i++) {
                coroutine.sleep(100);
                var result = win.eval(`document.hasFocus()?"True":"False"`);
                if (result == "True") {
                    break;
                }
            }
            win.close();

            assert.equal(result, "True");
        });
    });

    describe("menu", () => {
        function get_menu(m) {
            const win = gui.open({
                menu: m
            });
            const m1 = win.getMenu();
            wins.push(win);
            win.close();

            return m1;
        }

        const menu_template = [
            {},
            {
                label: "item1"
            },
            {
                label: "item2",
                enabled: false
            },
            {
                label: "item3",
                checked: true
            },
            {
                label: "item4",
                submenu: [
                    {
                        label: "item5"
                    },
                    {
                        label: "item6",
                        enabled: false
                    },
                    {
                        label: "item7",
                        checked: true
                    }
                ]
            },
            {
                label: "item8",
                icon: path.join(__dirname, "gui_files", "toolbox.512.png")
            }
        ];

        const menu_json = [
            {
                "type": "separator"
            },
            {
                "type": "normal",
                "label": "item1"
            },
            {
                "type": "normal",
                "label": "item2",
                "enabled": false
            },
            {
                "type": "checkbox",
                "label": "item3",
                "checked": true
            },
            {
                "type": "submenu",
                "label": "item4",
                "submenu": [
                    {
                        "type": "normal",
                        "label": "item5"
                    },
                    {
                        "type": "normal",
                        "label": "item6",
                        "enabled": false
                    },
                    {
                        "type": "checkbox",
                        "label": "item7",
                        "checked": true
                    }
                ]
            },
            {
                "type": "normal",
                "icon": path.join(__dirname, "gui_files", "toolbox.512.png"),
                "label": "item8"
            }
        ];

        describe("type", () => {
            function test_type(opt, out) {
                const m1 = gui.createMenu([opt]);
                assert.deepEqual(m1[0].toJSON(), out);
            }

            it("normal", () => {
                test_type({
                    label: "item1"
                }, {
                    "type": "normal",
                    "label": "item1"
                });
            });

            it("checked", () => {
                test_type({
                    label: "item2",
                    checked: true
                }, {
                    "type": "checkbox",
                    "label": "item2",
                    "checked": true
                });

                test_type({
                    label: "item2",
                    checked: false
                }, {
                    "type": "checkbox",
                    "label": "item2",
                    "checked": true
                });
            });

            it("submenu", () => {
                test_type({
                    label: "item3",
                    submenu: []
                }, {
                    "type": "submenu",
                    "label": "item3",
                    "submenu": []
                });
            });

            it("separator", () => {
                test_type({}, {
                    "type": "separator"
                });
            });
        });

        describe("check separator", () => {
            it("not allow label", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "separator",
                        label: "separator"
                    }]);
                });
            });

            it("not allow submenu", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "separator",
                        submenu: []
                    }]);
                });
            });

            it("not allow checked", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "separator",
                        checked: true
                    }]);
                });
            });

            it("not allow icon", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "separator",
                        icon: "icon"
                    }]);
                });
            });

            it("not allow tooltip", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "separator",
                        tooltip: "tooltip"
                    }]);
                });
            });
        });

        describe("check normal", () => {
            it("must have label", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "normal"
                    }]);
                });
            });

            it("must allow tooltip", () => {
                const m1 = gui.createMenu([{
                    type: "normal",
                    label: "item",
                    tooltip: "tooltip"
                }]);
                assert.equal(m1[0].tooltip, "tooltip");
            });

            it("not allow submenu", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "normal",
                        label: "item",
                        submenu: []
                    }]);
                });
            });

            it("not allow checked", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "normal",
                        label: "item",
                        checked: true
                    }]);
                });
            });
        });

        describe("check checkbox", () => {
            it("must have label", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "checkbox"
                    }]);
                });
            });

            it("not allow submenu", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "checkbox",
                        label: "item",
                        submenu: []
                    }]);
                });
            });

            it("allow no checked", () => {
                const m1 = gui.createMenu([{
                    type: "checkbox",
                    label: "item1"
                }]);

                assert.equal(m1[0].checked, false);

                const m2 = gui.createMenu([{
                    type: "checkbox",
                    label: "item2",
                    checked: true
                }]);

                assert.equal(m2[0].checked, true);
            });
        });

        describe("check submenu", () => {
            it("must have label", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "submenu",
                        submenu: []
                    }]);
                });
            });

            it("must have submenu", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "submenu",
                        label: "item"
                    }]);
                });
            });

            it("not allow checked", () => {
                assert.throws(() => {
                    gui.createMenu([{
                        type: "submenu",
                        label: "item",
                        submenu: [],
                        checked: true
                    }]);
                });
            });
        });

        it("undefined property", () => {
            const m1 = gui.createMenu([{}]);
            assert.deepEqual(m1.toJSON(), [{
                type: "separator"
            }]);

            assert.isUndefined(m1[0].label);
            m1[0].label = "separator";
            assert.isUndefined(m1[0].label);

            assert.isUndefined(m1[0].checked);
            m1[0].checked = true;
            assert.isUndefined(m1[0].checked);

            assert.isUndefined(m1[0].icon);
            m1[0].icon = "icon";
            assert.isUndefined(m1[0].icon);

            assert.isUndefined(m1[0].tooltip);
            m1[0].tooltip = "tooltip";
            assert.isUndefined(m1[0].tooltip);

            assert.isNull(m1[0].submenu);
        });

        it("item should be a object", () => {
            assert.throws(() => {
                gui.createMenu([[1]]);
            });
        });

        it("menu template", () => {
            const m1 = gui.createMenu(menu_template);
            assert.deepEqual(m1.toJSON(), menu_json);
            assert.equal(m1.length, 6);
        });

        it("open with menu object", () => {
            const m1 = gui.createMenu(menu_template);
            const m2 = get_menu(m1);
            assert.equal(m1, m2);
        });

        it("open with menu templete", () => {
            const m1 = get_menu(menu_template);
            assert.deepEqual(m1.toJSON(), menu_json);
        });

        describe("Menu method", () => {
            let menu;

            beforeEach(() => {
                menu = gui.createMenu();
            });

            it("should append an item", () => {
                const item = { label: "Item 1" };
                menu.append(item);
                assert.equal(menu[0].label, "Item 1");
            });

            it("should insert an item at a specific position", () => {
                const item1 = { label: "Item 1" };
                const item2 = { label: "Item 2" };
                menu.append(item1);
                menu.insert(0, item2);
                assert.equal(menu[0].label, "Item 2");
                assert.equal(menu[1].label, "Item 1");
            });

            it("should remove an item from a specific position", () => {
                const item1 = { label: "Item 1" };
                const item2 = { label: "Item 2" };
                menu.append(item1);
                menu.append(item2);
                menu.remove(0);
                assert.equal(menu[0].label, "Item 2");
            });

            it("should return the correct length", () => {
                const item1 = { label: "Item 1" };
                const item2 = { label: "Item 2" };
                menu.append(item1);
                menu.append(item2);
                assert.equal(menu.length, 2);
            });

            it("should access items by index", () => {
                const item1 = { label: "Item 1" };
                const item2 = { label: "Item 2" };
                menu.append(item1);
                menu.append(item2);
                assert.equal(menu[0].label, "Item 1");
                assert.equal(menu[1].label, "Item 2");
            });

            it("getMenuItemById", () => {
                menu = gui.createMenu([
                    { label: "Item 1", id: "item1" },
                    { label: "Item 2", id: "item2" },
                    {
                        label: "submenu",
                        submenu: [
                            { label: "Item 3", id: "item3" }
                        ]
                    }

                ]);
                assert.equal(menu.getMenuItemById("item1").label, "Item 1");
                assert.equal(menu.getMenuItemById("item2").label, "Item 2");
                assert.equal(menu.getMenuItemById("item3").label, "Item 3");
            });
        });
    });
});

require.main === module && test.run(console.DEBUG);
