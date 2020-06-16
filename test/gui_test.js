var test = require("test");
test.setup();

var test_util = require("./test_util");

var fs = require("fs");
var path = require("path");

var win32 = process.platform === "win32";
var darwin = process.platform === "darwin";

var htmlDir = path.resolve(__dirname, './gui_files/html');
var html = fs.readTextFile(path.resolve(htmlDir, './basic-loop.html'));

if (win32 || darwin) {
  var http = require("http");
  var gui = require("gui");
  var coroutine = require("coroutine");

  var htmlHandler = new http.fileHandler(htmlDir);
  var base_port = coroutine.vmid * 10000;

  describe("gui", () => {
    after(test_util.cleanup);

    var check = false;
    var svr;

    before(() => {
      svr = new http.Server(8999 + base_port, {
        '/(.+\.html$)': r => {
          htmlHandler.invoke(r)
        },
        '/': r => {
          check = true;
          r.response.write(html);
        },
      });
      svr.start();
      test_util.push(svr.socket);
    });

    describe("webview", () => {
      it("basic", () => {
        var closed = false;
        var events = {};
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/");

        var cnt = 0;

        win.onmessage = m => {
          cnt++;

          if (m === "try close") {
            win.close();
          } else {
            win.close();
          }
        };

        win.onclosed = () => {
          closed = true;
          win = undefined;
        };

        win.onload = () => {
          win.postMessage("hello");
        };

        // In Dardinw, WebView would auto-centralize its window by default, it trigger once emit("move")
        win.onmove = () => {
          if (darwin) events.onmove = true;
        }

        for (var i = 0; i < 1000 && !check; i++) coroutine.sleep(10);

        assert.ok(check);

        for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
          test_util.gc();

        assert.equal(test_util.countObject("WebView"), 0);
        assert.equal(closed, true);
        assert.equal(cnt, 2);

        assert.isTrue(events.onmove)
      });
    });

    describe("close", () => {
      it("close directly by default", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/close-directly.html");

        win.close();
      });
    });

    darwin && describe("move", () => {
      var events_resize = {};

      it("onmove by default on darwin", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/onmove.html", {
          title: "onmove"
        });

        win.onmove = (evt) => {
          events_resize.onmove = true;
          console.log('window onmove, origin: {x: %s, y: %s}', evt.x, evt.y);

          events_resize.onmove_x_ok = evt.hasOwnProperty('x');
          events_resize.onmove_y_ok = evt.hasOwnProperty('y');

          win.close();
          win = undefined;
        }

        for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
          test_util.gc();

        assert.isTrue(events_resize.onmove)
        assert.isTrue(events_resize.onmove_x_ok)
        assert.isTrue(events_resize.onmove_y_ok)
      });
    });

    darwin && describe("resize", () => {
      var events_resize = {};

      it("resiable (default)", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/resizable.html", {
          title: "Resizable"
        });

        var MAX_RESIZE_CNT = 5;
        var resize_cnt = 0;

        win.onload = () => {
          console.log('window onload');
        }

        win.on('resizestart', () => {
          console.log('window onresizestart');
        })

        win.on('resizeend', () => {
          console.log('window onresizeend');
          win.close()
          win = undefined
        });

        win.onresize = (evt) => {
          events_resize.onresize = true;

          events_resize.onresize_width_ok = evt.hasOwnProperty('width');
          events_resize.onresize_height_ok = evt.hasOwnProperty('height');
        }

        var timeouted = false
        setTimeout(() => {
          timeouted = true
          win.close()
          win = undefined
        }, 500);

        for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
          test_util.gc();

        if (!timeouted) {
          assert.isTrue(events_resize.onresize)
          assert.isTrue(events_resize.onresize_width_ok)
          assert.isTrue(events_resize.onresize_height_ok)
        }
      });
    });

    darwin && describe("options", () => {
      it("visible", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/normal.html", {
          title: "Normal - visible",
          visible: true,
        });

        assert.isTrue(win.visible);
        win.close();
      });

      it("invisible", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/normal.html", {
          title: "Normal - invisible",
          visible: false,
        });

        assert.isFalse(win.visible);

        win.visible = true;
        coroutine.sleep(500);
        assert.isTrue(win.visible);
        win.close();
      });

      it("maxmize", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/normal.html", {
          title: "Maxmize",
          maximize: true,
        });

        assert.isTrue(win.visible);
        coroutine.sleep(500);
        win.close();
      });
    });

    process.env.MANUAL && describe.only("manual", () => {
      var prevWin
      var closePreWin = () => {
        if (prevWin) prevWin.close()
        prevWin = undefined
      }
      it("not resizable", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/normal.html", {
          title: "Manual Test - not resizable",
          resizable: false
        });

        coroutine.sleep(500);
        win.close();
      });

      it("resizable", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/normal.html", {
          title: "Manual Test - resizable",
        });

        win.onload = () => {
          console.log('[resizable] win.onload');
          closePreWin()
        }

        coroutine.sleep(500);
        // win.close();
      });
    });

    it.skip("log", () => {
      var p = process.open(process.execPath, [
        path.join(__dirname, "gui_files", "gui1.js")
      ]);
      var r = p.stdout.readLines();
      assert.equal(r[0], "this is.a log");
      assert.equal(r[1], "this is.a warn");
      assert.ok(r[2].startsWith("WebView Error:"));
    });

    it.skip("debug", () => {
      var p = process.open(process.execPath, [
        path.join(__dirname, "gui_files", "gui2.js")
      ]);
      var r = p.stdout.readLines();
      assert.equal(r.length, 0);
    });
  });
}

require.main === module && test.run(console.DEBUG);
