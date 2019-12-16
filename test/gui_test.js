var test = require("test");
test.setup();

var test_util = require("./test_util");

var path = require("path");

var win = process.platform === "win32";
var darwin = process.platform === "darwin";

var html = `<html>
<script>
    external.onmessage = function(m) {
        external.postMessage('send back: ' + m)
    };

    var first = true;
    external.onclose = function() {
        if(first)
        {
            first = false;
            external.postMessage('try close');
            return false;
        }
    }
</script>
<body>
    <div id="app">click button to load...</div>
    <button id="btn" onclick="updateAppContent()">load</button>
    <script type="text/javascript">
      function updateAppContent(n) {
        document.getElementById('app').innerText = 'test in test/gui_test.js';
        document.getElementById('btn').remove();
      }
    </script>
</body>
</html>`;

if (win || darwin) {
  var http = require("http");
  var gui = require("gui");
  var coroutine = require("coroutine");

  var base_port = coroutine.vmid * 10000;

  describe("gui", () => {
    after(test_util.cleanup);

    oit("webview", () => {
      var check = false;
      var closed = false;
      var svr = new http.Server(8999 + base_port, r => {
        check = true;
        r.response.write(html);
      });
      svr.start();
      test_util.push(svr.socket);

      // console.log("js side, would gui.open");

      var win = gui.open("http://127.0.0.1:" + (8999 + base_port) + "/");

      // console.log("[JSSide::after gui.open]");

      var cnt = 0;

      win.onmessage = m => {
        cnt++;

        console.log("[JSSide::win::onmessage]", m);

        if (m === "try close") {
          win.close();
        } else {
          win.close();
        }
      };

      win.onclosed = () => {
        console.log("[JSSide::win::onclosed]");

        closed = true;
        win = undefined;
      };

      win.onload = () => {
        console.log("[JSSide::win::onload]");

        win.postMessage("hello");
      };

      for (var i = 0; i < 1000 && !check; i++) coroutine.sleep(10);

      assert.ok(check);

      for (var i = 0; i < 1000 && test_util.countObject("WebView"); i++)
        test_util.gc();

      assert.equal(test_util.countObject("WebView"), 0);
      assert.equal(closed, true);
      assert.equal(cnt, 2);
    });

    it("log", () => {
      var p = process.open(process.execPath, [
        path.join(__dirname, "gui_files", "gui1.js")
      ]);
      var r = p.stdout.readLines();
      assert.equal(r[0], "this is.a log");
      assert.equal(r[1], "this is.a warn");
      assert.ok(r[2].startsWith("WebView Error:"));
    });

    it("debug", () => {
      var p = process.open(process.execPath, [
        path.join(__dirname, "gui_files", "gui2.js")
      ]);
      var r = p.stdout.readLines();
      assert.equal(r.length, 0);
    });
  });
}

require.main === module && test.run(console.DEBUG);
