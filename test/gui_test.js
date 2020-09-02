var test = require("test");
test.setup();

var test_util = require("./test_util");

var fs = require("fs");
var path = require("path");
var io = require('io');
var child_process = require('child_process');

var win32 = process.platform === "win32";
var darwin64 = process.platform === "darwin" && process.arch === 'x64';

var htmlDir = path.resolve(__dirname, './gui_files/html');
var html = fs.readTextFile(path.resolve(htmlDir, './basic-loop.html'));

const isCI = !!process.env.CI;

if (win32 || darwin64) {
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
          if (darwin64) events.onmove = true;
        }

        for (var i = 0; i < 1000 && !check; i++) coroutine.sleep(10);

        assert.ok(check);

        for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
          test_util.gc();

        assert.equal(test_util.countObject("WebView"), 0);
        assert.equal(closed, true);
        assert.equal(cnt, 2);

        if (darwin64)
          assert.isTrue(events.onmove)
      });
    });

    describe("close", () => {
      it("close directly by default", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/close-directly.html");

        win.close();
      });
    });

    darwin64 && describe("move", () => {
      var events_resize = {};

      /**
       * NOTICE: emit for `move` trigged by NSWindow's centered method could delay, improve that.
       * 
       */
      it("onmove by default on darwin", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/onmove.html", {
          title: "onmove"
        });

        win.onmove = (evt) => {
          events_resize.onmove = true;
          console.log('window onmove, origin: {left: %s, top: %s}', evt.left, evt.top);

          events_resize.onmove_left_ok = evt.hasOwnProperty('left');
          events_resize.onmove_top_ok = evt.hasOwnProperty('top');

          win.close();
          win = undefined
        }

        coroutine.sleep(2000);

        assert.isTrue(events_resize.onmove)
        assert.isTrue(events_resize.onmove_left_ok)
        assert.isTrue(events_resize.onmove_top_ok)
      });
    });

    darwin64 && false && describe("resize", () => {
      var events_resize = {};

      // TODO: trigger it by msg posted to NSWindow, tell it use internal darwin API
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

        coroutine.sleep(500)

        for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
          test_util.gc();

        if (!timeouted) {
          assert.isTrue(events_resize.onresize)
          assert.isTrue(events_resize.onresize_width_ok)
          assert.isTrue(events_resize.onresize_height_ok)
        }
      });
    });

    darwin64 && describe("options", () => {
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

        /**
         * in C++ layer of previous version fibjs(<=0.30.x), on Windows platform,
         * some internal variables of WebView would be re-initialized when WebView::open(),
         * such as `m_visible`, which determined by the value of `webview.visible`.
         * 
         * we could't create one `WebView` object by calling `new WebView()`, but
         * by calling `gui.open(...)` instead. On ther other hand, `gui.open()` is asynchoronous in C++ layer,
         * that is, `webview.visible` is always true WHEN `WebView` created in `gui.open`, but we cannot expect its value
         * after that, we couldn't take a snapshot for the moment `WebView` are created!
         * The value of `webview.visible` is indeterminated to developer just after `gui.open()` executed!
         * 
         * When I implemented webview on Darwin, I take all initialization for internal variables of
         * `WebView` on its constructor(`WebView::WebView()`), to ensure the value of `win.visible` is expectable!
         */
        important_test: {
          assert.isTrue(win.visible);
          coroutine.sleep(500);
          assert.isTrue(win.visible);
        }

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

    darwin64 && describe("html5", () => {
      it("input.value", () => {
        var postedMsg
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/normal-html5.html", {
          title: "Manual Test - html5",
        });

        win.onmessage = (msg) => {
          postedMsg = msg
        }

        coroutine.sleep(500);

        assert.equal(postedMsg, 'input.value: initText')

        win.close();
      })
    });

    process.env.MANUAL && describe("manual", () => {
      var prevWin
      var closePreWin = () => {
        if (prevWin) prevWin.close()
        prevWin = undefined
      }
      it("not resizable", () => {
        var win = prevWin = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/normal.html", {
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
          closePreWin()
          assert.isTrue(win.visible)
        }

        coroutine.sleep(500);
        win.close();
        win = undefined;
      });

      it("log", () => {
        var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/normal-log.html", {
          title: "Manual Test - log",
        });

        coroutine.sleep(500 /*  * 1e4 */ );
        win.close();
        win = undefined;
      });
    });

    describe("fs://", () => {
      it("respond 404 info when trying to open one invalid html file", () => {
        var win = gui.open("fs://" + __dirname + "/gui_files/non-existed.html", {
          debug: false
        });

        setTimeout(() => {
          win.close()
        }, 500);
      });

      it("open one automatic-close html file", () => {
        gui.open("fs://" + __dirname + "/gui_files/t1000.html", {
          debug: false
        });
      });

      !isCI && it("open one automatic-close html file in zip file", () => {
        gui.open("fs://" + __dirname + "/gui_files/t1000.zip$/t1000.html", {
          debug: false
        });
      });
    });

    it("log", () => {
      var bs = child_process.spawn(process.execPath, [
        path.join(__dirname, "gui_files", "gui1.js")
      ]);
      var p = new io.BufferedStream(bs.stdout);
      var r = p.readLines();
      assert.equal(r[0], "this is.a log");
      assert.equal(r[1], "this is.a warn");

      assert.ok(r[2].startsWith("WebView Error:"));
    });

    it("debug", () => {
      var bs = child_process.spawn(process.execPath, [
        path.join(__dirname, "gui_files", "gui2.js")
      ]);
      var p = new io.BufferedStream(bs.stdout);
      var r = p.readLines();
      assert.equal(r.length, 0);
    });
  });
}

require.main === module && test.run(console.DEBUG);