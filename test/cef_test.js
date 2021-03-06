var test = require("test");
test.setup();

var test_util = require("./test_util");

var path = require("path");
var os = require('os');
var child_process = require('child_process');
var gui = require("gui");
var util = require("util");

var cef_path = path.join(__dirname, "../temp/cef", os.type());
console.log(cef_path);
var cef_files_path = path.join(__dirname, "cef_files");

gui.config({
    "cache_path": `${os.homedir()}/.cache`,
    "cef_path": cef_path,
    "backend": {
        "cef://test/": cef_files_path
    }
});

describe("cef", () => {
    var svr;

    describe("basic", () => {
        it("basic", () => {
            var step = 0;
            var win = gui.open("cef://test/basic.html");

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
            var win = gui.open("cef://test/basic.html");
            win.close();
            win = undefined;

            for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
                test_util.gc();

            assert.equal(test_util.countObject("WebView"), 0);
        });
    });

    describe("window style", () => {
        function test_style(item, opt, check) {
            var opt1 = util.clone(opt);
            opt1.width = 120;
            opt1.height = 120;

            it(item, done => {
                var win = gui.open("cef://test/basic.html", opt1);

                win.on("load", () => {
                    var doc = win.dev.DOM.getDocument();
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

        test_style("headless", {
            headless: true
        }, (w, h) => {
            assert.equal(w, 800);
            assert.equal(h, 600);
        });
    });
});

require.main === module && test.run(console.DEBUG);