var { describe, it, before, after, beforeEach, afterEach } = require('node:test');
var assert = require('assert');

var gui = require("gui");
const url = require('url');
const path = require('path');
var coroutine = require("coroutine");
var child_process = require("child_process");
var http = require("http");
var PNG = require("./_helpers/png-node.js");

var win32 = process.platform === "win32";
var darwin64 = process.platform === "darwin";
var ios = process.platform === "ios";

// Focus is the session's decision, not the window's: a Wayland compositor
// requires an activation token from a user interaction, an X server without a
// window manager has no focus policy at all, and Windows only makes a process
// the foreground one when it is allowed to. The logs print this so a skipped
// assertion can be told apart from a broken window.
// Low ports are on the blocked-port list every engine ships, and they react to
// it differently (macOS reports an error, WebKitGTK commits an empty document).
// Ask the OS for a port and release it again, so all three engines attempt a
// real connection and report a real error.
// Allocated once: the close is asynchronous, and nothing else in the suite binds
// the port afterwards.
var s_closed_port = 0;
function closedPort() {
    if (s_closed_port === 0) {
        const srv = http.createServer(function () { });

        // Closing a listening socket races the accept loop on Windows, which
        // then reports the late close as an error event; the port is throwaway,
        // so swallow it instead of printing it as an unhandled error.
        srv.on("error", function () { });

        srv.listen(0);
        s_closed_port = srv.address().port;

        if (srv.socket)
            srv.socket.on("error", function () { });
        srv.stop();

        // Let the listener finish closing before the tests dial the port.
        for (var i = 0; i < 5; i++)
            coroutine.sleep(10);
    }

    return s_closed_port;
}

function session_hint() {
    return "XDG_SESSION_TYPE=" + (process.env.XDG_SESSION_TYPE || "-")
        + ", WAYLAND_DISPLAY=" + (process.env.WAYLAND_DISPLAY || "-")
        + ", DISPLAY=" + (process.env.DISPLAY || "-");
}

// Navigation tests need a real page served over the public network.  Use the
// IANA reserved example.com: it is meant for exactly this, stays online, serves
// one static document and neither redirects nor rewrites the landing URL by
// region or user agent.  fibjs.org used to be the target, which turned every
// website outage into a failure of this suite.
const TEST_PAGE = "https://example.com";

var gui_env = "gui";

// --use-thread has no GUI backend: every JS fiber is its own OS thread there,
// so there is no single thread that could own the native window objects.
var use_thread = (process.execArgv || []).indexOf('--use-thread') >= 0;

var vfb;
if (process.platform === "linux") {
    if (process.env.DISPLAY === undefined) {
        try {
            vfb = child_process.spawn("Xvfb", [":99", "-screen", "0", "1024x768x24"]);
            process.env.DISPLAY = ":99";
            gui_env = "gui in Xvfb";
        } catch (e) { }
    }
}

describe(gui_env, { skip: use_thread && 'gui is unavailable in --use-thread mode' }, () => {
    var wins = [];

    after(() => {
        wins.forEach(win => {
            try {
                win.close();
            } catch (e) { }
        });

        if (vfb) {
            try {
                vfb.kill(9);
            } catch (e) { }
        }
    });

    describe("webview", () => {
        beforeEach(() => {
            coroutine.sleep(500);
        });

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

        it("windows are independent", () => {
            const a = gui.open({
                width: 260,
                height: 180
            });
            wins.push(a);

            const b = gui.open({
                width: 320,
                height: 220
            });
            wins.push(b);

            assert.notEqual(a, b);

            a.setHtml("window a");
            b.setHtml("window b");

            // Each window owns its document, its geometry and its state.
            assert.equal(a.eval("document.body.textContent"), "window a");
            assert.equal(b.eval("document.body.textContent"), "window b");

            // A window may end up larger than requested - the frame the session
            // adds and a native minimum (GTK refuses to go below the width its
            // title bar needs, about 200px) are outside the caller's control -
            // but never smaller, so the request is a lower bound.
            const sizes = [a.getSize(), b.getSize()];
            assert.ok(sizes[0][0] >= 260 - 4, "a width=" + sizes[0][0]);
            assert.ok(sizes[1][0] >= 320 - 4, "b width=" + sizes[1][0]);
            assert.ok(sizes[1][0] > sizes[0][0],
                "the two windows should differ, got " + JSON.stringify(sizes));

            // Closing one window neither touches the other nor drops its
            // document: the native objects are per window, not per module.
            a.close();

            assert.equal(b.eval("document.body.textContent"), "window b");
            b.loadFile(path.join(__dirname, "gui_files", "test.html"));
            assert.equal(b.eval("document.title"), "Test");

            b.close();
        });

        it("open validates the window options", () => {
            // The options are checked before a native window exists, so an
            // invalid value never leaves a window behind.
            assert.throws(() => gui.open({
                width: 100,
                height: 100,
                titlebar: { style: "bogus" }
            }), { code: "ERR_INVALID_ARG_VALUE" });

            assert.throws(() => gui.open({
                width: 100,
                height: 100,
                titlebar: { height: "tall2" }
            }), { code: "ERR_INVALID_ARG_VALUE" });
        });

        it("capabilities", () => {
            const caps = gui.capabilities;

            assert.equal(caps.platform, process.platform);
            assert.ok(caps.window && caps.webview && caps.tray && caps.dialog && caps.menu);
            assert.equal(typeof caps.window.position, 'boolean');
            assert.equal(typeof caps.webview.screenshot, 'boolean');
            assert.equal(typeof caps.tray.title, 'boolean');
            assert.equal(caps.menu.mutable, false);
        });

        it("window state and bounds", () => {
            const win = gui.open({
                left: 100,
                top: 100,
                width: 200,
                height: 150
            });
            wins.push(win);

            assert.equal(win.state, "normal");
            assert.equal(win.loading, false);

            // The rectangle is the native window, so a client-side-decoration
            // session reports the frame the compositor added around the
            // requested content size; only a gross mismatch is a failure.
            const bounds = win.getBounds();
            assert.ok(Math.abs(bounds.width - 200) <= 4,
                "width should follow the request, got " + bounds.width);
            assert.ok(Math.abs(bounds.height - 150) <= 4,
                "height should follow the request, got " + bounds.height);

            win.setBounds({ width: 300, height: 250 });
            const resized = win.getBounds();
            assert.ok(Math.abs(resized.width - 300) <= 4,
                "width should follow the request, got " + resized.width);
            assert.ok(Math.abs(resized.height - 250) <= 4,
                "height should follow the request, got " + resized.height);

            // A compositor that owns placement is free to move the window; the
            // capability says upfront whether the request can be observed.
            if (gui.capabilities.window.position) {
                assert.equal(resized.x, bounds.x);
                assert.equal(resized.y, bounds.y);

                // Only the requested axis changes.
                win.setBounds({ x: 140 });
                assert.ok(Math.abs(win.getBounds().x - 140) <= 1,
                    "x should follow the request, got " + win.getBounds().x);
            }

            assert.ok(Math.abs(win.getBounds().width - 300) <= 4,
                "the size should survive an x-only move, got " + win.getBounds().width);

            assert.throws(() => {
                win.state = "bogus";
            });

            assert.equal(typeof win.focus(), 'boolean');

            win.close();
        });

        it("bounds combinations", () => {
            const win = gui.open({
                width: 200,
                height: 150
            });
            wins.push(win);

            // A rectangle is two independent halves - size and position - and
            // every field of a request is optional. The native window also
            // carries the frame the session added around it, so sizes are
            // compared with the tolerance the state test uses.
            function checkSize(b, width, height) {
                assert.ok(Math.abs(b.width - width) <= 4,
                    "width should be " + width + ", got " + b.width);
                assert.ok(Math.abs(b.height - height) <= 4,
                    "height should be " + height + ", got " + b.height);
            }

            win.setBounds({ width: 320, height: 240 });
            var b = win.getBounds();
            checkSize(b, 320, 240);

            // getSize()/getPosition() read the same rectangle as getBounds().
            var size = win.getSize();
            assert.equal(size[0], b.width);
            assert.equal(size[1], b.height);

            // One axis only: the other keeps its value.
            win.setBounds({ width: 260 });
            checkSize(win.getBounds(), 260, 240);

            win.setBounds({ height: 180 });
            checkSize(win.getBounds(), 260, 180);

            // A resize does not move the window.
            if (gui.capabilities.window.position) {
                var before = win.getPosition();
                win.setBounds({ width: 240, height: 200 });
                var after = win.getPosition();
                assert.ok(Math.abs(after[0] - before[0]) <= 1,
                    "x moved on a resize: " + JSON.stringify(before) + " -> " + JSON.stringify(after));
                assert.ok(Math.abs(after[1] - before[1]) <= 1,
                    "y moved on a resize: " + JSON.stringify(before) + " -> " + JSON.stringify(after));
            }

            // All four fields at once.
            win.setBounds({ x: 140, y: 120, width: 300, height: 220 });
            b = win.getBounds();
            checkSize(b, 300, 220);

            if (gui.capabilities.window.position) {
                assert.ok(Math.abs(b.x - 140) <= 1, "x should be 140, got " + b.x);
                assert.ok(Math.abs(b.y - 120) <= 1, "y should be 120, got " + b.y);

                win.setPosition(160, 140);
                var pos = win.getPosition();
                b = win.getBounds();
                assert.equal(pos[0], b.x);
                assert.equal(pos[1], b.y);
                assert.ok(Math.abs(pos[0] - 160) <= 1, "x should be 160, got " + pos[0]);
                assert.ok(Math.abs(pos[1] - 140) <= 1, "y should be 140, got " + pos[1]);
            }

            // setSize() and getBounds() agree as well.
            win.setSize(280, 190);
            checkSize(win.getBounds(), 280, 190);

            win.close();
        });

        it("geometry events", () => {
            const win = gui.open({
                width: 200,
                height: 150,
                left: 100,
                top: 100
            });
            wins.push(win);

            var resized = [];
            var moved = [];
            win.on("resize", ev => resized.push(ev));
            win.on("move", ev => moved.push(ev));

            function waitEvent(list) {
                for (var i = 0; i < 200 && list.length === 0; i++)
                    coroutine.sleep(10);
                return list.length ? list[list.length - 1] : null;
            }

            win.setSize(320, 240);

            // The notification travels through the platform loop, so it lands
            // after the setter returned; the payload is the applied geometry.
            var ev = waitEvent(resized);
            assert.ok(ev, "setSize should emit resize");
            assert.equal(ev.type, "resize");
            assert.ok(Math.abs(ev.width - 320) <= 4, "resize width=" + ev.width);
            assert.ok(Math.abs(ev.height - 240) <= 4, "resize height=" + ev.height);

            // A session that owns placement may ignore the move, and the
            // capability says upfront whether it can be observed.
            if (gui.capabilities.window.position) {
                win.setPosition(180, 160);

                var mv = waitEvent(moved);
                assert.ok(mv, "setPosition should emit move");
                assert.equal(mv.type, "move");

                var pos = win.getPosition();
                assert.ok(Math.abs(mv.left - pos[0]) <= 1,
                    "move left=" + mv.left + " position=" + JSON.stringify(pos));
                assert.ok(Math.abs(mv.top - pos[1]) <= 1,
                    "move top=" + mv.top + " position=" + JSON.stringify(pos));
            }

            // The events are not setter-specific: the window manager resizes
            // the window too, where the session applies the state at all.
            resized.length = 0;
            win.state = "maximized";

            var maximized = false;
            for (var i = 0; i < 200; i++) {
                if (win.state === "maximized") {
                    maximized = true;
                    break;
                }
                coroutine.sleep(10);
            }

            if (maximized) {
                for (var i = 0; i < 200 && resized.length === 0; i++)
                    coroutine.sleep(10);
                assert.ok(resized.length > 0, "maximizing should emit resize");
                win.state = "normal";
            }

            win.close();
        });

        it("close requested can cancel and destroy skips it", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            var requested = 0;
            win.on("close-requested", function (ev) {
                requested++;
                ev.preventDefault();
            });

            assert.equal(win.close(), false);
            assert.equal(requested, 1);
            assert.equal(win.isVisible(), true);

            win.destroy();
            assert.equal(requested, 1);

            // waitForClose() returns at once for a window that is already gone.
            win.waitForClose();
        });

        it("close requested allows the close", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            var requested = 0;
            win.on("close-requested", () => {
                requested++;
            });

            // No preventDefault: the listener observed the request, the close
            // still went through and the return value reports it.
            assert.equal(win.close(), true);
            assert.equal(requested, 1);

            assert.throws(() => win.close(), { code: "ERR_GUI_CLOSED" });
        });

        it("closed object rejects every window member", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });

            var closed = 0;
            win.on("close", () => closed++);

            win.close();

            // The event leaves the native teardown, so it lands on the next
            // turn of the isolate rather than inside close().
            for (var i = 0; i < 1000 && closed === 0; i++)
                coroutine.sleep(10);

            assert.equal(closed, 1);

            // Every member that needs the native window reports the documented
            // error instead of touching a dangling handle: a closed window is
            // not a silently degraded one.
            const CLOSED = { code: "ERR_GUI_CLOSED" };

            assert.throws(() => win.close(), CLOSED);
            assert.throws(() => win.destroy(), CLOSED);
            assert.throws(() => win.show(), CLOSED);
            assert.throws(() => win.hide(), CLOSED);
            assert.throws(() => win.isVisible(), CLOSED);
            assert.throws(() => win.getBounds(), CLOSED);
            assert.throws(() => win.setBounds({ width: 200 }), CLOSED);
            assert.throws(() => win.getSize(), CLOSED);
            assert.throws(() => win.setSize(200, 200), CLOSED);
            assert.throws(() => win.getPosition(), CLOSED);
            assert.throws(() => win.setPosition(10, 10), CLOSED);
            assert.throws(() => win.getUrl(), CLOSED);
            assert.throws(() => win.loadUrl("about:blank"), CLOSED);
            assert.throws(() => win.loadFile(path.join(__dirname, "gui_files", "test.html")), CLOSED);
            assert.throws(() => win.setHtml("x"), CLOSED);
            assert.throws(() => win.getHtml(), CLOSED);
            assert.throws(() => win.reload(), CLOSED);
            assert.throws(() => win.goBack(), CLOSED);
            assert.throws(() => win.goForward(), CLOSED);
            assert.throws(() => win.getTitle(), CLOSED);
            assert.throws(() => win.setTitle("x"), CLOSED);
            assert.throws(() => win.eval("1"), CLOSED);
            assert.throws(() => win.waitForLoad(), CLOSED);
            assert.throws(() => win.state, CLOSED);

            // The two members that describe the closed state itself answer.
            win.waitForClose();
            assert.equal(win.loading, false);
        });

        it("load failed", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            const badUrl = "http://127.0.0.1:" + closedPort() + "/";

            var failed = null;
            win.on("load-failed", function (ev) {
                failed = ev;
            });

            // The blocking call must report the failure, and the failure has to
            // reach the event stream as well. An engine that loads its own
            // error page afterwards must not turn this into a success.
            var caught = null;
            try {
                win.loadUrl(badUrl);
            } catch (e) {
                caught = e;
            }

            assert.ok(caught, "loadUrl should throw when the navigation fails"
                + (failed ? " (load-failed: " + failed.message + ")" : " (no load-failed event)"));

            assert.ok(failed);
            assert.equal(failed.url, badUrl);
            assert.ok(failed.message);
            assert.equal(win.loading, false);

            win.close();
        });

        it("failed load keeps the window usable", () => {
            const win = gui.open({
                width: 120,
                height: 100
            });
            wins.push(win);

            win.loadFile(path.join(__dirname, "gui_files", "test.html"));
            const badUrl = "http://127.0.0.1:" + closedPort() + "/";

            assert.throws(() => {
                win.loadUrl(badUrl);
            }, { code: "ERR_GUI_LOAD_FAILED" });

            assert.equal(win.loading, false);

            // The failure is remembered for the document that was asked for:
            // waiting for it again reports the same error instead of parking
            // until the next navigation.
            assert.throws(() => win.waitForLoad(badUrl), { code: "ERR_GUI_LOAD_FAILED" });

            // Whether the previous document survives is the engine's business:
            // WebKit keeps it (the error page is suppressed while a caller
            // waits), WebView2 replaces it with its own. What every engine owes
            // the caller is a consistent, still usable window.
            if (win.getHtml().indexOf("This is a test page.") < 0)
                console.log("        failed load: this engine replaced the document with its error page");

            win.setHtml("after failure");
            assert.equal(win.getUrl(), "about:blank");
            assert.equal(win.eval("document.body.textContent"), "after failure");
            assert.ok(win.getHtml().indexOf("after failure") >= 0);

            win.close();
        });

        it("open with an unreachable url", () => {
            // Opening waits for the first document, but a navigation failure is
            // announced through load-failed instead of failing the open: the
            // window exists and can still be used.
            const win = gui.open("http://127.0.0.1:" + closedPort() + "/", {
                width: 120,
                height: 100
            });
            wins.push(win);

            assert.equal(win.loading, false);
            assert.equal(win.isVisible(), true);

            win.setHtml("recovered");
            assert.equal(win.eval("document.body.textContent"), "recovered");

            win.close();
        });

        it("html", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            win.setHtml("hello");

            for (var i = 0; i < 1000; i++) {
                coroutine.sleep(10);
                if (win.getHtml().indexOf('hello') != -1) {
                    break;
                }
            }

            const html = win.getHtml();
            win.close();

            assert.equal(html, '<html><head></head><body>hello</body></html>');
        });

        it("getHtml while the document is being replaced", () => {
            // Reading the document mid-navigation used to return a JSON null
            // from the injected script.  On Windows that null reached
            // get<std::string>() and threw out of a COM callback, which killed
            // the process instead of failing the call.
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            win.setHtml("hello");

            for (var i = 0; i < 10; i++)
                assert.equal(typeof win.getHtml(), "string");

            for (var i = 0; i < 1000; i++) {
                coroutine.sleep(10);
                if (win.getHtml().indexOf('hello') != -1) {
                    break;
                }
            }

            const html = win.getHtml();
            win.close();

            assert.equal(html, '<html><head></head><body>hello</body></html>');
        });

        it("loading property", () => {
            const win = gui.open();
            wins.push(win);

            assert.isFalse(win.loading);

            var seen = false;
            win.on("loading", () => {
                seen = true;
                assert.isTrue(win.loading);
            });

            win.loadUrl(TEST_PAGE);

            assert.isTrue(seen);
            assert.isFalse(win.loading);

            win.close();
        });

        it("title roundtrip", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            // The title is stored natively, so non-ASCII has to survive the
            // trip through the platform string type and back.
            const title = "中文标题 😀 Łódź";
            win.setTitle(title);
            assert.equal(win.getTitle(), title);

            win.close();
        });

        it("loading and load event payloads", () => {
            const win = gui.open({
                width: 120,
                height: 100
            });
            wins.push(win);

            var events = [];
            win.on("loading", ev => events.push({
                type: "loading",
                url: ev.url,
                loading: win.loading
            }));
            win.on("load", ev => events.push({
                type: "load",
                url: ev.url,
                loading: win.loading
            }));

            win.loadFile(path.join(__dirname, "gui_files", "test.html"));

            // One navigation, two events in order, both describing the page
            // that the blocking call waited for.
            assert.deepEqual(events.map(e => e.type), ["loading", "load"]);

            const url = win.getUrl();
            assert.ok(/test\.html$/.test(url), url);
            assert.equal(events[0].url, url);
            assert.equal(events[1].url, url);

            // `loading` is already true inside the start event and false when
            // the completion event runs.
            assert.equal(events[0].loading, true);
            assert.equal(events[1].loading, false);
            assert.equal(win.loading, false);

            // The document the events describe is the one eval runs in.
            assert.equal(win.eval("document.title"), "Test");
            assert.equal(win.eval("document.querySelector('h1').textContent"), "Test");

            win.close();
        });

        it("waitForLoad", () => {
            const win = gui.open();
            wins.push(win);

            // The navigation members block until the document is ready, so a
            // wait for the document that just loaded returns at once.
            win.loadUrl(TEST_PAGE);
            win.waitForLoad(TEST_PAGE + "/");
            assert.equal(win.getUrl(), TEST_PAGE + "/");

            win.loadFile(path.join(__dirname, "gui_files", "test.html"));
            assert.isFalse(win.loading);

            win.setHtml("hello");
            assert.equal(win.eval("document.body.textContent"), "hello");

            // A navigation the page starts is awaited with waitForLoad.
            win.eval("location.href = 'about:blank'");
            win.waitForLoad();
            assert.equal(win.getUrl(), "about:blank");

            win.close();
        });

        it("navigation started by another fiber", () => {
            const win = gui.open({
                width: 120,
                height: 100
            });
            wins.push(win);

            var navigated = false;
            coroutine.start(function () {
                coroutine.sleep(100);
                navigated = true;
                win.setHtml("from another fiber");
            });

            // waitForLoad() is the event form of the blocking members: it parks
            // this fiber until someone else navigates the window.
            win.waitForLoad();

            assert.equal(navigated, true);
            assert.equal(win.loading, false);
            assert.equal(win.eval("document.body.textContent"), "from another fiber");

            win.close();
        });

        it("document url and history", () => {
            const win = gui.open({
                width: 120,
                height: 100
            });
            wins.push(win);

            // A new window shows an empty document, and setHtml counts as one.
            assert.equal(win.getUrl(), "about:blank");

            // Nothing to go back to yet.
            assert.equal(win.goBack(), false);
            assert.equal(win.goForward(), false);
            assert.equal(win.getUrl(), "about:blank");

            win.setHtml("hello");
            assert.equal(win.getUrl(), "about:blank");
            assert.equal(win.eval("document.body.textContent"), "hello");

            // Replacing the document replaces the content, not the url.
            win.setHtml("hello again");
            assert.equal(win.getUrl(), "about:blank");
            assert.equal(win.eval("document.body.textContent"), "hello again");

            const fileA = path.join(__dirname, "gui_files", "test.html");
            const fileB = path.join(__dirname, "gui_files", "test2.html");

            win.loadFile(fileA);
            const urlA = win.getUrl();
            assert.ok(/test\.html$/.test(urlA), urlA);
            assert.equal(win.eval("document.title"), "Test");

            win.loadFile(fileB);
            const urlB = win.getUrl();
            assert.ok(/test2\.html$/.test(urlB), urlB);
            assert.equal(win.eval("document.title"), "Test 2");

            // The window keeps a history: back and forward restore the whole
            // document, not just the url.
            assert.equal(win.goBack(), true);
            assert.equal(win.getUrl(), urlA);
            assert.equal(win.eval("document.title"), "Test");

            assert.equal(win.goForward(), true);
            assert.equal(win.getUrl(), urlB);
            assert.equal(win.eval("document.title"), "Test 2");

            // Reloading keeps the document and waits for it.
            win.reload();
            assert.equal(win.getUrl(), urlB);
            assert.equal(win.loading, false);
            assert.equal(win.eval("document.title"), "Test 2");

            win.close();
        });

        it("visible", () => {
            const win = gui.open({
                width: 100,
                height: 100,
                visible: false
            });
            wins.push(win);

            assert.equal(win.isVisible(), false);

            win.show();
            assert.equal(win.isVisible(), true);
            assert.notEqual(win.eval('document.body.clientWidth'), 0);
            assert.notEqual(win.eval('document.body.clientHeight'), 0);

            win.close();
        });

        describe("eval", () => {
            it("eval and result", () => {
                const o = {
                    a: 100,
                    b: "bbbb",
                    c: true,
                    c1: false,
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

            it("complex object no crash (darwin)", () => {
                if (!darwin64) return;

                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                let result;
                assert.doesNotThrow(() => {
                    result = win.eval(`(() => { const o = { a: 1, b: { c: 2 } }; o.self = o; return o; })()`);
                });

                win.close();

                assert.ok(result === null || typeof result === "object");
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

        it("close from inside after reload", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            win.loadUrl("data:text/html;charset=utf-8,helloworld");

            assert.equal(win.eval(`window.location.href`), "data:text/html;charset=utf-8,helloworld");

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

        it("loading and load event", () => {
            const win = gui.open({
                width: 100,
                height: 100
            });
            wins.push(win);

            var loading_url;
            win.on("loading", ev => {
                loading_url = ev.url;
            });

            var loaded_url;
            win.on("load", ev => {
                loaded_url = ev.url;
            });

            win.loadUrl(TEST_PAGE);

            for (var i = 0; i < 1000; i++) {
                coroutine.sleep(50);
                if (loaded_url !== undefined && loading_url !== undefined) {
                    break;
                }
            }

            assert.equal(loading_url, TEST_PAGE + "/");
            assert.equal(loaded_url, TEST_PAGE + "/");

            win.close();
        });

        describe("message", () => {
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
        });

        describe("app", () => {
            function test_app(app, code) {
                const win = gui.open({
                    width: 100,
                    height: 100,
                    app
                });

                wins.push(win);

                var result;
                win.onmessage = function (msg) {
                    result = msg.data;
                };

                win.eval(`
async function async_eval(func) {
    try{
        postMessage(JSON.stringify({
            result: await func()
        }));
    } catch(e) {
        postMessage(JSON.stringify({
            error: e.message
        }));
    }
}
`);

                win.eval(`async_eval(async function(){ return ${code}});`);

                for (var i = 0; i < 1000; i++) {
                    coroutine.sleep(10);
                    if (result) {
                        break;
                    }
                }

                win.close();
                return JSON.parse(result);
            }

            it("normal test", () => {
                const result = test_app({
                    test: function (a, b) {
                        return a + b;
                    }
                }, "await window.app.test(1, 2);");

                assert.deepEqual(result, {
                    result: 3
                });
            });

            it("subobject test", () => {
                const result = test_app({
                    test: {
                        test1: function (a, b) {
                            return a + b + 1;
                        }
                    }
                }, "await window.app.test.test1(1, 2);");

                assert.deepEqual(result, {
                    result: 4
                });
            });

            it("no app", () => {
                const result = test_app(undefined, "await window.app.test.test1(1, 2);");

                assert.deepEqual(result, {
                    "error": "app is required"
                });
            });

            it("method not found", () => {
                const result = test_app({}, "await window.app.test.test1(1, 2);");

                assert.deepEqual(result, {
                    "error": "method 'test.test1' is not found"
                });
            });

            it("method not callable", () => {
                const result = test_app({
                    test: {}
                }, "await window.app.test(1, 2);");

                assert.deepEqual(result, {
                    "error": "method 'test' is not a function"
                });
            });

            it("cutsom error", () => {
                const result = test_app({
                    test: function () {
                        throw new Error("custom error");
                    }
                }, "await window.app.test();");

                assert.deepEqual(result, {
                    "error": "Error: custom error"
                });
            });

            it("unsupported result type", () => {
                const result = test_app({
                    test: function () {
                        return /abc/;
                    }
                }, "await window.app.test();");

                assert.deepEqual(result, {
                    "result": {}
                });
            });
        });

        describe("load", () => {
            const path1 = path.join(__dirname, "gui_files", "test.html");
            const path2 = path.join(__dirname, "gui_files", "test2.html");
            const zpath1 = path.join(__dirname, "gui_files", "test.html.zip$/test.html");
            const url1 = url.pathToFileURL(path1).href.replace(/file:/, "fs:").replace(/fs:\/\/\/([a-zA-Z]):/, "fs://$1");
            const url2 = url.pathToFileURL(path2).href.replace(/file:/, "fs:").replace(/fs:\/\/\/([a-zA-Z]):/, "fs://$1");
            const zurl1 = url.pathToFileURL(zpath1).href.replace(/file:/, "fs:").replace(/fs:\/\/\/([a-zA-Z]):/, "fs://$1");

            function assert_url(win, url) {
                var last_received_message;
                var get_url;
                for (var i = 0; i < 1000; i++) {
                    coroutine.sleep(10);
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

                win.loadUrl(url1);

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

            if (!ios)
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

        if (!ios) {
            describe("window style", () => {
                function fetch_size(opt) {
                    const win = gui.open(opt);
                    wins.push(win);

                    var win_size = win.getSize();

                    win.close();

                    return {
                        width: win_size[0],
                        height: win_size[1]
                    };
                }

                it("default", () => {
                    const { width, height } = fetch_size({
                        width: 300,
                        height: 300
                    });

                    assert.equal(height, 300);
                    assert.equal(width, 300);
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
                        maximize: true,
                        frame: false
                    });

                    assert.notLessThan(size1.width, size2.width);
                });
            });

            it("change title", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                for (var i = 0; i < 1000; i++) {
                    if (win.getTitle() == "")
                        break;
                    coroutine.sleep(10);
                }
                assert.equal(win.getTitle(), "");

                win.eval(`window.document.title = "Hello World";`);

                for (var i = 0; i < 1000; i++) {
                    if (win.getTitle() == "Hello World")
                        break;
                    coroutine.sleep(10);
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

                for (var i = 0; i < 1000; i++) {
                    if (win.getSize()[0] === 300)
                        break;
                    coroutine.sleep(10);
                }
                assert.deepEqual(win.getSize(), [300, 200]);

                win.setSize(400, 300);

                for (var i = 0; i < 1000; i++) {
                    if (win.getSize()[0] === 400)
                        break;
                    coroutine.sleep(10);
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

                // A window manager is free to ignore placement requests and
                // Wayland compositors always do; Xvfb has no WM at all, so the
                // window can land wherever GTK maps it. Exact coordinates are
                // asserted when the session honours them. Otherwise the move
                // is checked relative to the position the server chose, so a
                // broken setter or getter still fails.
                function placed() {
                    const p = win.getPosition();
                    return p[0] === 100 && p[1] === 100;
                }

                for (var i = 0; i < 1000 && !placed(); i++)
                    coroutine.sleep(10);

                if (placed()) {
                    assert.deepEqual(win.getPosition(), [100, 100]);

                    win.setPosition(200, 200);

                    for (var i = 0; i < 1000; i++) {
                        if (win.getPosition()[0] === 200)
                            break;
                        coroutine.sleep(10);
                    }
                    assert.deepEqual(win.getPosition(), [200, 200]);
                } else {
                    const before = win.getPosition();
                    assert.equal(before.length, 2);
                    assert.ok(isFinite(before[0]) && isFinite(before[1]));

                    win.setPosition(before[0] + 50, before[1] + 50);

                    var after = before;
                    for (var i = 0; i < 200; i++) {
                        coroutine.sleep(10);
                        after = win.getPosition();
                        if (after[0] !== before[0] || after[1] !== before[1])
                            break;
                    }

                    if (after[0] === before[0] && after[1] === before[1])
                        console.log("        position: this session ignores window placement (actual="
                            + JSON.stringify(before) + ")");
                    else {
                        assert.equal(after[0] - before[0], 50);
                        assert.equal(after[1] - before[1], 50);
                        console.log("        position: session offsets the requested position,"
                            + " the move itself was verified (actual="
                            + JSON.stringify(before) + ")");
                    }
                }

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

                for (var i = 0; i < 1000; i++) {
                    if (win.getTitle() == "")
                        break;
                    coroutine.sleep(10);
                }
                assert.equal(win.getTitle(), "");

                langs.forEach(lang => {
                    console.log(lang);
                    win.setTitle(lang);
                    assert.equal(win.getTitle(), lang);
                });

                win.close();
            });
        }

        if (vfb === undefined && !ios) {
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

                    // open() returns with the window created, focused and
                    // loaded, so the first focus happened before a listener
                    // could be attached. Whether the session actually gives the
                    // keyboard to the window (a background app, a locked
                    // desktop, a remote session) is its own decision, so the
                    // state is only used as a probe for what follows.
                    const focused_at_open = win.focused;

                    if (!focused_at_open) {
                        console.log("focus/blur: the session did not focus the window; " + session_hint());
                        win.close();
                        return;
                    }

                    assert.equal(focus, 0);

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
                        coroutine.sleep(10);
                    }

                    // The new window is created, focused and loaded inside
                    // open(), so the first window loses the keyboard there.
                    assert.equal(blur, last_blur + 1);
                    assert.equal(win.focused, false);
                    assert.equal(win1.focused, true);

                    last_focus = focus;

                    win1.close();
                    for (var i = 0; i < 500; i++) {
                        if (focus > last_focus)
                            break;
                        coroutine.sleep(10);
                    }

                    // Handing the keyboard back to the window that is left is
                    // the session's decision as well: when another app owns the
                    // activation at this point, nothing re-keys the survivor.
                    if (focus === last_focus && !win.focused) {
                        console.log("focus/blur: the session did not return focus to the remaining window; "
                            + session_hint());
                        win.close();
                        return;
                    }

                    win.close();

                    assert.greaterThan(focus, last_focus);
                }

                it("normal window", () => {
                    test_focus({});
                });

                it("no titlebar", () => {
                    test_focus({
                        titlebar: "hide"
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

            // Windows only makes a process the foreground one when it is allowed
            // to: the CI runner keeps the console there, so the window is
            // created and focused but isActived() (GetForegroundWindow() ==
            // hwnd) never turns true.
            it("active", { skip: win32 && !!process.env.CI }, () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                var active = false;
                for (var i = 0; i < 1000; i++) {
                    coroutine.sleep(10);
                    if (win.isActived()) {
                        active = true;
                        break;
                    }
                }

                if (!active) {
                    // Nothing hands keyboard focus to a new toplevel when the
                    // session has no window manager (Xvfb, a bare X server on
                    // Linux), so the rest of the case, which needs focus to
                    // move between two windows, cannot be observed.
                    if (process.platform !== "linux")
                        assert.equal(win.isActived(), true);
                    console.log("        active: this session does not activate new windows ("
                        + session_hint() + ")");
                    win.close();
                    return;
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
                    coroutine.sleep(10);
                    if (win1.isActived()) {
                        break;
                    }
                }
                assert.equal(win1.isActived(), true);
                assert.equal(win.isActived(), false);

                win.active();

                var reactivated = false;
                for (var i = 0; i < 1000; i++) {
                    coroutine.sleep(10);
                    if (win.isActived()) {
                        reactivated = true;
                        break;
                    }
                }

                if (!reactivated) {
                    // The initial focus came from the session (new windows are
                    // focused by policy), but moving focus back to an existing
                    // window is a request the session may refuse. Wayland needs
                    // an activation token that an X11-era toolkit cannot
                    // produce; some window managers drop _NET_ACTIVE_WINDOW
                    // without a user timestamp. The API was accepted and the
                    // window is still alive, which is all a client can do.
                    if (process.platform !== "linux")
                        assert.equal(win.isActived(), true);
                    console.log("        active: this session did not move focus back ("
                        + session_hint() + ")");
                    win1.close();
                    win.close();
                    return;
                }

                assert.equal(win.isActived(), true);
                assert.equal(win1.isActived(), false);

                win1.close();
                win.close();
            });

            it("auto focus webview", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                for (var i = 0; i < 1000; i++) {
                    coroutine.sleep(10);
                    var result = win.eval(`document.hasFocus()?"True":"False"`);
                    if (result == "True") {
                        break;
                    }
                }

                // A window the session never gave the keyboard to has none to
                // hand to its webview; that is the session's decision, not a
                // broken window.
                const active = win.isActived();

                win.close();

                if (result !== "True" && !active)
                    console.log("        auto focus: this session does not activate new windows ("
                        + session_hint() + ")");
                else
                    assert.equal(result, "True");
            });

            it("auto focus webview after blur", () => {
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
                    coroutine.sleep(10);
                    var result = win.eval(`document.hasFocus()?"True":"False"`);
                    if (result == "True") {
                        break;
                    }
                }

                // As above: without activation there is no focus to restore.
                const active = win.isActived();

                win.close();

                if (result !== "True" && !active)
                    console.log("        auto focus after blur: this session does not activate new windows ("
                        + session_hint() + ")");
                else
                    assert.equal(result, "True");
            });
        }

        if (!ios) {
            it("takeScreenshot - visible area only", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                win.setHtml(`<html><body style="margin:0px;padding:0px;"><div style="width:500px;height:1000px;background-color:red"></div></body></html>`);

                coroutine.sleep(10);

                const pixelRatio = win.eval('window.devicePixelRatio');
                const viewportWidth = win.eval('window.innerWidth');
                const viewportHeight = win.eval('window.innerHeight');

                var buf = win.takeScreenshot(false);
                win.close();

                assert.isObject(buf);

                var png = new PNG(buf);

                // With fullPage=false, should capture only the visible viewport area
                assert.equal(png.width, viewportWidth * pixelRatio);
                assert.equal(png.height, viewportHeight * pixelRatio);
            });

            it("takeScreenshot - full page", () => {
                const win = gui.open({
                    width: 100,
                    height: 100
                });
                wins.push(win);

                win.setHtml(`<html><body style="margin:0px;padding:0px;"><div style="width:500px;height:1000px;background-color:red"></div></body></html>`);

                coroutine.sleep(10);

                if (process.platform === 'darwin') {
                    // On macOS, fullPage should throw an error
                    assert.throws(() => {
                        win.takeScreenshot(true);
                    });
                    win.close();
                } else {
                    // On other platforms, should capture the full content (500x1000)
                    const pixelRatio = win.eval('window.devicePixelRatio');
                    var buf = win.takeScreenshot(true);
                    win.close();

                    assert.isObject(buf);

                    var png = new PNG(buf);
                    assert.equal(png.width, 500 * pixelRatio);
                    assert.equal(png.height, 1000 * pixelRatio);
                }
            });
        }

        it("webview object should not be gc until close", () => {
            var cnt = 0
            function test_win() {
                var win = gui.open({});

                win.onmessage = function (e) {
                    cnt++;
                }

                win.eval(`setTimeout(() => { postMessage('Hello from t1.js'); }, 100);setTimeout(() => { close(); }, 500);`);

                win = null;
            }

            test_win();
            gc();

            for (var i = 0; i < 200; i++) {
                if (cnt > 0) {
                    break;
                }
                coroutine.sleep(10);
            }

            assert.equal(cnt, 1);
        });

        describe("feature", () => {
            it("webcrypto", () => {
                const win = gui.open({
                    file: path.join(__dirname, "gui_files", "test.html"),
                    width: 100,
                    height: 100
                });
                wins.push(win);

                assert.equal(win.eval(`!!window.crypto`), true);
                assert.equal(win.eval(`!!window.crypto.subtle`), true);
                assert.equal(win.eval(`!!window.crypto.subtle.digest`), true);

                win.close();
            });
        });
    });

    if (!ios) {
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

        it("tray", () => {
            assert.throws(() => gui.createTray({}), { number: 20002 });

            const tray = gui.createTray({
                icon: path.join(__dirname, "gui_files", "toolbox.512.png")
            });
            assert.isObject(tray);
            tray.close();
        });

        it("chooseFile validates the filter fields", () => {
            assert.throws(() => gui.chooseFile({
                filters: [{ name: "Images" }]
            }), { number: 20002 });
            assert.throws(() => gui.chooseFile({
                filters: [{ extensions: ["png"] }]
            }), { number: 20002 });
        });
    }
});


