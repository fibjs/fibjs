/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/EventEmitter.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/// <reference path="../interface/Buffer.d.ts" />
/**
 * @description WebView object, an embedded browser view owned by a desktop application window
 *
 *  A WebView is a native window whose content is rendered by a platform browser
 *  engine (WebKitGTK on Linux, WKWebView on macOS, WebView2 on Windows). It is the
 *  object returned by `gui.open` and `gui.openFile` and the only way to show web
 *  content in fibjs: the gui module creates windows, menus and trays, while the
 *  WebView object drives one window - navigation, page execution, native window
 *  state, and the JavaScript bridge between the page and fibjs.
 *
 *  The page runs in the browser engine, not in the fibjs isolate, so the two sides
 *  share no objects: they exchange strings through messages and calls through the
 *  `app` bridge. The page keeps its own DOM, timers and origin model, and fibjs
 *  keeps its fibers and native handles.
 *
 *  Obtained from:
 *  - `gui.open(url[, options])` — open a window and navigate to `url`;
 *  - `gui.open(options)` — open a window; the `url` or `file` property selects the
 *    initial content (`about:blank` when neither is set);
 *  - `gui.openFile(file[, options])` — open a window and load a local file or a
 *    path inside a zip archive.
 *
 *  `new WebView()` is not supported ("not a constructor"): the constructor belongs
 *  to the gui module. `gui.open` returns a window that already exists - the native
 *  window is created before the call returns - so every member can use it at once
 *  and no member has a "window not created yet" state.
 *
 *  Concepts:
 *
 *  - **Document model**: the page is an ordinary web document in its own engine.
 *    DOM APIs, CSS, fetch/XHR, WebSocket, IndexedDB and Web Crypto behave as in a
 *    standalone browser (each platform engine has its own version and feature
 *    set), and fibjs objects are not visible from the page.
 *  - **Navigation lifecycle**: the engine loads asynchronously, the API does not.
 *    `loadUrl`, `loadFile`, `setHtml`, `reload`, `goBack` and `goForward` block the
 *    calling fiber until the document finished loading and throw when it failed.
 *    The `loading` event marks the start, `load` the end and `load-failed` a
 *    failure, for code that prefers events; `waitForLoad` waits for the next load
 *    and is how a page-initiated navigation (a link, a redirect) is awaited, while
 *    the `loading` property reports whether a navigation is in progress. Each
 *    window keeps a browsing history, so back/forward and `reload` behave like a
 *    browser.
 *  - **JavaScript bridge**: `postMessage` sends a string into the page, where it
 *    arrives as a DOM `message` event; the page's own `window.postMessage` is
 *    rewired by fibjs and sends a string back, which arrives as the `message`
 *    event on the fibjs side. Only strings cross the bridge, so encode structured
 *    data with JSON.stringify/JSON.parse. The `app` option of gui.open installs
 *    `window.app` in the page: `window.app.name(...)` performs an RPC into fibjs
 *    and returns a Promise. Host methods run synchronously and must return a
 *    JSON-serializable value - a Promise returned by an async host function does
 *    not resolve through the bridge (it serializes as an empty object) - and a
 *    thrown exception rejects the page-side Promise with the error message.
 *  - **Resource loading**: the engine loads `http:`/`https:` URLs over the
 *    network and `data:`/`about:blank` inline; local content goes through the
 *    internal `fs:` scheme registered by the gui module, which serves plain files
 *    and paths inside zip archives (`app.zip$/index.html`). `loadFile` converts a
 *    path to such an `fs:` URL. Cookies and site storage belong to the browser
 *    engine; the `devtools` option of gui.open enables the engine developer tools.
 *  - **Window and process lifetime**: the WebView also owns the native window -
 *    title, size, position, visibility, activation, menu and screenshots. An open
 *    window references the isolate, so the fibjs process stays alive until the
 *    window is closed; `ref`/`unref` adjust that reference. The `close` event
 *    fires once the native window is gone, after which every member that needs the
 *    window throws.
 *
 * Example 1 — open a window and exchange a message with the page:
 * ```JavaScript
 * // requires: long-running
 * const gui = require('gui');
 *
 * const win = gui.open({ width: 480, height: 320 });
 *
 * win.setHtml(`<html><body><script>
 * window.addEventListener('message', function (ev) {
 *     window.postMessage('pong: ' + ev.data);
 * });
 * </script></body></html>`);
 *
 * win.on('message', function (ev) {
 *     console.log(ev.data); // pong: ping
 *     win.close();
 * });
 *
 * win.postMessage('ping');
 * ```
 *
 * Example 2 — expose host functions to the page through the app option:
 * ```JavaScript
 * // requires: long-running
 * const gui = require('gui');
 *
 * const win = gui.open({
 *     width: 480,
 *     height: 320,
 *     app: {
 *         math: {
 *             add: function (a, b) {
 *                 return a + b; // synchronous method, JSON-serializable result
 *             }
 *         }
 *     }
 * });
 *
 * win.setHtml(`<html><body><script>
 * window.app.math.add(1, 2).then(function (sum) {
 *     window.postMessage('sum=' + sum);
 * });
 * </script></body></html>`);
 *
 * win.on('message', function (ev) {
 *     console.log(ev.data); // sum=3
 *     win.close();
 * });
 * ```
 *
 * Example 3 — follow the navigation lifecycle and change the window state:
 * ```JavaScript
 * // requires: long-running
 * const gui = require('gui');
 *
 * const win = gui.open({ width: 320, height: 200 });
 *
 * win.on('loading', function (ev) {
 *     console.log('loading ' + ev.url);
 * });
 *
 * win.on('load', function (ev) {
 *     console.log('loaded ' + ev.url);
 * });
 *
 * win.loadUrl('data:text/html;charset=utf-8,<title>Done</title><p>ok</p>');
 *
 * console.log(win.eval('document.querySelector("p").textContent')); // ok
 *
 * win.setTitle('fibjs');
 * win.setSize(640, 400);
 * win.setPosition(120, 80);
 * console.log(JSON.stringify(win.getSize()));
 * console.log(JSON.stringify(win.getPosition()));
 *
 * win.close();
 * ```
 *
 * Notes:
 *
 *  - Desktop builds only: the Linux (GTK/WebKitGTK), macOS and Windows builds
 *    provide WebView; the iOS/embedded stub answers every gui call with
 *    "Webview not supported in this platform".
 *  - A display server is required on Linux (X11 or Wayland). Without one the GUI
 *    cannot initialize: window creation fails with "Unable to init server: Could
 *    not connect: Connection refused" and calls that wait for the window block.
 *  - The gtk4 implementation does not support the `icon`, `left` and `top`
 *    options of gui.open; on macOS a full-page screenshot is not supported and a
 *    fullscreen window cannot be closed by the page.
 *  - The browser engine is provided by the host, so page behavior (codecs, fonts,
 *    user agent, available Web APIs) follows that engine rather than fibjs.
 *
 */
declare class Class_WebView extends Class_EventEmitter {
    /**
     * @description Loads the page at the specified url
     *
     *      Starts a navigation and blocks the calling fiber until the document it
     *      loaded finished loading; a navigation that fails throws. Any scheme
     *      understood by the platform engine works (`http:`, `https:`, `data:`,
     *      `about:blank`), and the internal `fs:` scheme opens local files and paths
     *      inside zip archives. The navigation replaces the current document and adds
     *      a history entry, so the previous page stays reachable with `goBack`. When
     *      the page finished, `getUrl`/`eval`/`getHtml` are safe to call; the `load`
     *      event fires as well, for code that prefers event handlers.
     *
     *      Example — load a page and read a value from it:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *      win.loadUrl('data:text/html;charset=utf-8,<p id="p">done</p>');
     *
     *      console.log(win.eval('document.getElementById("p").textContent')); // done
     *      win.close();
     *      ```
     *
     *      @param url the url to load
     *
     */
    loadUrl(url: string): void;

    /**
     * @description Loads the page of the specified file
     *
     *      Converts the path to an `fs:` URL with url.pathToFileURL and navigates to
     *      it, so a plain file or a path inside a zip archive (`app.zip$/page.html`)
     *      can be loaded without building a URL by hand. Relative resources of the
     *      page resolve against the file. Like `loadUrl` the call blocks until the
     *      document finished loading and throws when it failed.
     *
     *      Example — load a local file and read a value from it:
     *      ```JavaScript
     *      // requires: long-running
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const gui = require('gui');
     *
     *      const file = path.join(fs.mkdtempSync(path.join(os.tmpdir(), 'webview-')), 'page.html');
     *      fs.writeFileSync(file, '<html><body><h1>local</h1></body></html>');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *      win.loadFile(file);
     *
     *      console.log(win.eval('document.querySelector("h1").textContent')); // local
     *      win.close();
     *      ```
     *
     *      @param file the file to load
     *
     */
    loadFile(file: string): void;

    /**
     * @description Queries the url of the current page
     *
     *      Returns the URL of the document loaded in the window, as reported by the
     *      browser engine; before the first navigation it is `about:blank`. The value
     *      is the engine-normalized form and can differ from the input: a host root
     *      gains a trailing slash and `loadFile` produces an `fs:` URL. Read it after
     *      the `load` event to identify the page that actually loaded.
     *
     *      @return returns the url of the current page
     *
     */
    getUrl(): string;

    /**
     * @description Sets the page html of the webview
     *
     *      Replaces the current document with the given HTML text. The engine parses
     *      the string as a document and wraps plain text, so `setHtml("hello")`
     *      produces `<html><head></head><body>hello</body></html>`. The base URL is
     *      empty, so relative URLs do not resolve to a useful location; include a
     *      `<base>` element or absolute URLs when the page needs subresources. The
     *      call blocks until the submitted document finished loading, exactly like
     *      `loadUrl`.
     *
     *      @param html the html to set
     *
     */
    setHtml(html: string): void;

    /**
     * @description Gets the page html of the webview
     *
     *      Runs `document.documentElement.outerHTML.toString()` in the page and
     *      returns the serialized live DOM, including the mutations made by page
     *      scripts. It is the counterpart of `setHtml` and blocks the calling fiber
     *      until the engine answered, so the value always belongs to the document
     *      loaded now; while a navigation is in progress it can still return the
     *      previous document, so check the `loading` property first when that
     *      matters.
     *
     *      Example — read the DOM of the current document:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *      win.setHtml('<html><body><h1>hello</h1></body></html>');
     *
     *      console.log(win.getHtml()); // <html><head></head><body><h1>hello</h1></body></html>
     *      win.close();
     *      ```
     *
     *      @return returns the page html of the webview
     *
     */
    getHtml(): string;

    /**
     * @description Whether a navigation is currently in progress
     *
     *      True from the moment a navigation is requested (or the page starts one)
     *      until the engine finished or failed it; the `loading` event marks the
     *      start and `load`/`load-failed` the end. The flag exists for code that
     *      decides whether to navigate again or whether a query would see the
     *      previous document; the blocking members do not need it.
     *
     */
    readonly loading: boolean;

    /**
     * @description Waits for the next page to finish loading
     *
     *      Blocks the calling fiber until a document finished loading. An empty url
     *      waits for the next load of any document; a url waits for that URL in its
     *      engine-normalized form (a host root gains a trailing slash), skipping the
     *      loads of other documents. The call is how a page-initiated navigation - a
     *      link click, a redirect, a script that sets location - is awaited; the
     *      navigation members already wait themselves. A load that fails throws, and
     *      there is no timeout: a page that never finishes waits forever, so wrap the
     *      call in a coroutine timeout when the target can hang.
     *
     *      Example — wait for a navigation started by the page:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *      win.setHtml('<html><body><a id="a" href="data:text/html,<p>next</p>">next</a>' +
     *          '<script>document.getElementById("a").click();</script></body></html>');
     *
     *      win.waitForLoad(); // the click above already started the second load
     *      console.log(win.eval('document.body.textContent.trim()')); // next
     *      win.close();
     *      ```
     *
     *      @param url the url to wait for; empty means the next load of any document
     *
     */
    waitForLoad(url?: string): void;

    /**
     * @description Refreshes the current page
     *
     *      Reloads the current URL from its source and keeps its history entry, as
     *      the browser reload button does. The call blocks until the reloaded
     *      document finished loading, like `loadUrl`; the `loading` and `load` events
     *      fire again for it.
     *
     */
    reload(): void;

    /**
     * @description Goes back to the previous page
     *
     *      Moves to the previous entry of this window's browsing history and blocks
     *      until the restored document finished loading. Returns false without
     *      navigating when there is no previous entry, so the call is safe at the
     *      start of a history; the `loading`/`load` events fire for the restored
     *      document and `getUrl` reports its URL.
     *
     *      @return true when the navigation happened, false when there is no previous entry
     *
     */
    goBack(): boolean;

    /**
     * @description Goes forward to the next page
     *
     *      Moves to the next entry of this window's browsing history and blocks until
     *      the restored document finished loading. Returns false without navigating
     *      when the window is already at the latest entry. Like `goBack`, the restored
     *      document emits the navigation events and `getUrl` reports its URL.
     *
     *      @return true when the navigation happened, false when there is no next entry
     *
     */
    goForward(): boolean;

    /**
     * @description Runs a piece of JavaScript code in the current window
     *
     *      Executes code in the page's own JavaScript engine and returns the result
     *      as a fibjs value. Booleans, numbers, strings, null, arrays and plain
     *      objects survive the round trip; values that cannot be serialized (a DOM
     *      node, function, RegExp, Promise, ...) become an empty object or undefined
     *      depending on the engine instead of raising, so return plain data. A
     *      syntax error or an exception in the code is thrown to the caller. The call
     *      blocks the calling fiber until the engine answered and returns the value
     *      directly. The code shares no objects with fibjs: exchange data through
     *      `postMessage` or
     *      `window.app`, and await Promises there rather than inside `eval`.
     *
     *      Example — evaluate expressions and read a DOM value:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.setHtml('<html><body><p id="p">42</p></body></html>');
     *
     *      console.log(win.eval('1 + 1')); // 2
     *      console.log(win.eval('document.getElementById("p").textContent')); // 42
     *      console.log(JSON.stringify(win.eval('[1, 2, 3]'))); // [1,2,3]
     *
     *      win.close();
     *      ```
     *
     *      @param code the JavaScript code to execute
     *      @return returns the execution result
     *
     */
    eval(code: string): any;

    /**
     * @description Sets the title of the window
     *
     *      Sets the native window title directly. The window title is normally owned
     *      by the page: the engine mirrors `document.title` into the window title, so
     *      a later page-side change overwrites this value, and the page can in turn
     *      be overwritten by a new `setTitle` call. `getTitle` reads the value back.
     *
     *      @param title the title of the window
     *
     */
    setTitle(title: string): void;

    /**
     * @description Queries the title of the window
     *
     *      Returns the native window title, which the engine keeps in sync with the
     *      page's `document.title`; it is an empty string before the first document
     *      sets one. The value is the exact Unicode string, so non-ASCII titles round
     *      trip unchanged. See `setTitle` for the ownership of the value.
     *
     *      @return returns the title of the window
     *
     */
    getTitle(): string;

    /**
     * @description Queries whether the window is visible
     *
     *      Returns the native window visibility, not the Page Visibility API of the
     *      document. A window created with `visible: false` reports false until
     *      `show` is called, and `hide` makes it false again; page scripts keep
     *      running and content keeps loading while the window is hidden.
     *
     *      @return returns whether the window is visible
     *
     */
    isVisible(): boolean;

    /**
     * @description Shows the window
     *
     *      Makes the window visible and brings it to the front; it is a no-op when
     *      the window is already visible. A window created with `visible: false` can
     *      be shown later, and its page has been loading in the background all along.
     *
     */
    show(): void;

    /**
     * @description Hides the window
     *
     *      Hides the native window without closing it: the page keeps running and its
     *      events keep firing, and `show` makes the window visible again. Hiding is
     *      not a close, so the `close` event does not fire and the object stays
     *      usable.
     *
     */
    hide(): void;

    /**
     * @description Sets the size of the window
     *
     *      Resizes the native window to the given width and height in window pixels.
     *      The requested size is clamped by the `minWidth`/`minHeight` and
     *      `maxWidth`/`maxHeight` options given to gui.open. The web content is
     *      resized with the window (`window.innerWidth`/`innerHeight` change), the
     *      page reflows, and a `resize` event is emitted.
     *
     *      @param width the width of the window
     *      @param height the height of the window
     *
     */
    setSize(width: number, height: number): void;

    /**
     * @description Queries the size of the window
     *
     *      Returns the current window size as [width, height] in logical window
     *      pixels. The page uses CSS pixels scaled by `window.devicePixelRatio`, so a
     *      screenshot of the visible area measures this size multiplied by that
     *      ratio. The window manager can adjust the value shortly after `setSize`
     *      while it applies its constraints.
     *
     *      @return returns the size of the window as an array whose first element is the width
     *       and second element is the height
     *
     */
    getSize(): number[];

    /**
     * @description Sets the position of the window
     *
     *      Moves the window so that its top-left corner is at (left, top) in screen
     *      coordinates; where the native API uses a different origin the platform
     *      layer translates the coordinates. The window manager can adjust the
     *      position, and a `move` event is emitted for the resulting placement.
     *
     *      @param left the x coordinate of the top-left corner of the window
     *      @param top the y coordinate of the top-left corner of the window
     *
     */
    setPosition(left: number, top: number): void;

    /**
     * @description Queries the position of the window
     *
     *      Returns [left, top] in screen coordinates as currently reported by the
     *      platform. The value can differ from the last `setPosition` request when
     *      the window manager repositions, snaps or decorates the window.
     *
     *      @return returns the position of the window as an array whose first element is the x
     *       coordinate and second element is the y coordinate
     *
     */
    getPosition(): number[];

    /**
     * @description Queries whether the window is the active window
     *
     *      Returns whether this window currently has the desktop focus; the name
     *      keeps the runtime's historic spelling of "activated". The focus events are
     *      the reliable way to track focus: on Windows the foreground rules can keep
     *      a focused window false for a while, and virtual desktops or CI sessions
     *      can suppress activation entirely.
     *
     *      @return returns whether the window is the active window
     *
     */
    isActived(): boolean;

    /**
     * @description Activates the window
     *
     *      Asks the window manager to focus and raise the window, the programmatic
     *      equivalent of clicking it: this window receives `focus` and the previously
     *      active window receives `blur`. A window manager can refuse the request,
     *      for example when the application is not allowed to steal focus.
     *
     */
    active(): void;

    /**
     * @description Whether the window currently has keyboard focus
     *
     *      The readable counterpart of the `focus`/`blur` events. On Windows the
     *      foreground rules can keep a focused window false for a while, and virtual
     *      desktops or CI sessions can suppress activation entirely; use the events
     *      when the exact transition matters.
     *
     */
    readonly focused: boolean;

    /**
     * @description Reads or changes the native window state
     *
     *      Accepted values are `normal`, `minimized`, `maximized` and `fullscreen`.
     *      Reading returns what the platform reports; writing maps the value to the
     *      native operation (restore, iconify/miniaturize, maximize, fullscreen).
     *      A platform or session that cannot expose one of the states reports
     *      `normal`; see `gui.capabilities.window.state`.
     *
     *      The state change is asynchronous on some window managers: the following
     *      `minimize` / `maximize` / `restore` / `enter-fullscreen` /
     *      `leave-fullscreen` event tells when the platform applied it.
     *
     */
    state: string;

    /**
     * @description Returns the window position and size
     *
     *      The record carries the same values as getPosition() and getSize() in one
     *      call. Coordinates and sizes are logical pixels as reported by the
     *      platform; a scaled display can return fractional values.
     *
     *      @return returns { x, y, width, height }
     *
     */
    getBounds(): {
        x: number;
        y: number;
        width: number;
        height: number;
    };

    /**
     * @description Moves and resizes the window in one call
     *
     *      The atomic form of setPosition() plus setSize(): platforms that support it
     *      avoid the intermediate resize, and a window manager that repositions new
     *      windows receives both values together.
     *
     *      @param bounds the requested position and size
     *
     */
    setBounds(bounds: {
        x?: number;
        y?: number;
        width?: number;
        height?: number;
    }): void;

    /**
     * @description Asks the desktop session to focus and raise the window
     *
     *      The programmatic counterpart of clicking the window, with the same
     *      semantics as active(). Unlike active() it reports whether the window
     *      claims keyboard focus right after the request: a session can deny it
     *      (Wayland compositors, foreground restrictions, CI desktops) and the call
     *      then returns false. `gui.capabilities.window.focusRequest` tells upfront
     *      whether the session is able to honour the request at all.
     *
     *      @return true when the window reports keyboard focus after the request
     *
     */
    focus(): boolean;

    /**
     * @description Waits until the window is closed
     *
     *      Blocks the calling fiber until the native window is gone and the `close`
     *      event fired. Calling it on a window that is already closed returns at
     *      once. It is the counterpart of a user-initiated close (the window manager
     *      button, Alt+F4, `window.close` from the page), which the caller cannot
     *      observe otherwise; `close()` itself already waits for the window to be
     *      gone before it returns.
     *
     */
    waitForClose(): void;

    /**
     * @description Queries the menu of the window
     *
     *      Returns the Menu object passed with the `menu` option of gui.open, or null
     *      when the window was created without one. The accessor is synchronous and
     *      there is no setter: the menu of a window is fixed at creation time. See
     *      the gui module's createMenu for the menu item format.
     *
     *      @return returns the menu of the window
     *
     */
    getMenu(): Class_Menu;

    /**
     * @description Captures an image of the current window
     *
     *      Captures the rendered window as PNG bytes. With fullPage = false (the
     *      default) only the visible viewport is captured, at window size multiplied
     *      by `window.devicePixelRatio`; with fullPage = true the whole document is
     *      captured, which is not supported on macOS (the call throws there). Capture
     *      works for most pages, but lazily loaded content can be missing from a
     *      full-page shot: test on the target page and resize or scroll the window to
     *      trigger the loading before capturing.
     *
     *      Example — capture the window into a PNG file:
     *      ```JavaScript
     *      // requires: long-running
     *      const fs = require('fs');
     *      const os = require('os');
     *      const path = require('path');
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *      win.setHtml('<html><body style="margin:0"><h1>shot</h1></body></html>');
     *
     *      const png = win.takeScreenshot();
     *      fs.writeFileSync(path.join(os.tmpdir(), 'webview-shot.png'), png);
     *      console.log(png.read(0, 8).toString('hex')); // 89504e470d0a1a0a
     *
     *      win.close();
     *      ```
     *
     *      @param fullPage true captures the whole document, false (default) captures the visible area
     *      @return returns the captured image
     *
     */
    takeScreenshot(fullPage?: boolean): Class_Buffer;

    /**
     * @description Closes the current window
     *
     *      Destroys the native window, releases its page and blocks the calling fiber
     *      until the window is gone and the `close` event fired. A window opened with
     *      `hideOnClose: true` only hides when the user closes it with the window
     *      manager, while this call overrides that and destroys the window. A
     *      `close-requested` listener can cancel the close by calling
     *      `ev.preventDefault()`; the call then returns false and the window stays
     *      open. After the window is gone every member that needs it throws
     *      "WebView: webview is closed", including a second `close`; the page can
     *      close its own window with `window.close`, which follows the same path and
     *      emits the same event. Use `destroy` to skip the listeners.
     *
     *      Example — observe the close event of a window closed from fibjs:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('close', function () {
     *          console.log('window closed');
     *      });
     *
     *      console.log(win.close()); // true
     *      ```
     *
     *      @return true when the window was destroyed, false when a close-requested listener cancelled it
     *
     */
    close(): boolean;

    /**
     * @description Destroys the current window, ignoring the close listeners
     *
     *      The unconditional form of `close`: the native window is destroyed and the
     *      call blocks until it is gone, whether or not a `close-requested` listener
     *      would have cancelled a normal close. It is what an application shutdown or
     *      a "quit now" menu item uses, where a page with unsaved state must not be
     *      able to block the exit.
     *
     *      Example — tear the window down from a shutdown path:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *      win.on('close-requested', function (ev) { ev.preventDefault(); });
     *
     *      console.log(win.close()); // false: the listener refused
     *      win.destroy();
     *      console.log(win.isVisible()); // throws: the window is gone
     *      ```
     *
     */
    destroy(): void;

    /**
     * @description Sends a message into the webview
     *
     *      Sends a string into the page, where it is delivered as a DOM `message`
     *      event whose `data` property is the message; the page listens with
     *      `window.addEventListener('message', ...)`. The document must be loaded
     *      first - the delivery is a script call against the current page, so
     *      messages sent before `load` are lost. Only strings cross the bridge:
     *      encode structured data with JSON.stringify in fibjs and parse it in the
     *      page, or use the `app` bridge for calls. The reverse direction is the
     *      `message` event.
     *
     *      Example — send a message and receive the page's reply:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.eval(`
     *          window.addEventListener('message', function (ev) {
     *              window.postMessage('echo: ' + ev.data);
     *          });
     *      `);
     *
     *      win.on('message', function (ev) {
     *          console.log(ev.data); // echo: hello
     *          win.close();
     *      });
     *
     *      win.postMessage('hello');
     *      ```
     *
     *      @param msg the message to send
     *
     */
    postMessage(msg: string): void;

    /**
     * @description Queries and binds the window load start event, equivalent to on("loading", func);
     *
     *      Fired when a navigation starts, before the new document replaces the
     *      current one. `ev.url` carries the target URL and `ev.type`/`ev.target`
     *      identify the event and the WebView. Every navigation emits it - the first
     *      load, `reload`, history moves and page-initiated navigation - so it can be
     *      paired with `load` to bracket a navigation for progress UI.
     *
     *      Example — follow a navigation from start to finish:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('loading', function (ev) {
     *          console.log('loading: ' + ev.url);
     *      });
     *
     *      win.on('load', function (ev) {
     *          console.log('loaded: ' + ev.url);
     *          win.close();
     *      });
     *
     *      win.loadUrl('data:text/html;charset=utf-8,<p>hi</p>');
     *      ```
     *
     *      @param ev the event object, carrying the loading url in its url property
     *
     */
    on(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "loading", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the window load start event, equivalent to on("loading", func);
     *
     *      Fired when a navigation starts, before the new document replaces the
     *      current one. `ev.url` carries the target URL and `ev.type`/`ev.target`
     *      identify the event and the WebView. Every navigation emits it - the first
     *      load, `reload`, history moves and page-initiated navigation - so it can be
     *      paired with `load` to bracket a navigation for progress UI.
     *
     *      Example — follow a navigation from start to finish:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('loading', function (ev) {
     *          console.log('loading: ' + ev.url);
     *      });
     *
     *      win.on('load', function (ev) {
     *          console.log('loaded: ' + ev.url);
     *          win.close();
     *      });
     *
     *      win.loadUrl('data:text/html;charset=utf-8,<p>hi</p>');
     *      ```
     *
     *      @param ev the event object, carrying the loading url in its url property
     *
     */
    onloading: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the window load completed event, equivalent to on("load", func);
     *
     *      Fired when the main document finished loading. `ev.url` carries the loaded
     *      URL and `ev.type`/`ev.target` identify the event and the WebView. The
     *      event releases a `waitForLoad` waiting for that URL, so it marks the point
     *      where the DOM can be read with `eval` or `getHtml`; the navigation members
     *      block until it fired and do not need a listener.
     *
     *      @param ev the event object, carrying the loaded url in its url property
     *
     */
    on(event: "load", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "load", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "load", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "load", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "load", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "load", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "load", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "load", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "load", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the window load completed event, equivalent to on("load", func);
     *
     *      Fired when the main document finished loading. `ev.url` carries the loaded
     *      URL and `ev.type`/`ev.target` identify the event and the WebView. The
     *      event releases a `waitForLoad` waiting for that URL, so it marks the point
     *      where the DOM can be read with `eval` or `getHtml`; the navigation members
     *      block until it fired and do not need a listener.
     *
     *      @param ev the event object, carrying the loaded url in its url property
     *
     */
    onload: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the navigation failed event, equivalent to on("load-failed", func);
     *
     *      Fired when a navigation ended without loading its document: an
     *      unreachable host, a refused connection, a certificate error or a URL the
     *      engine cannot handle. `ev.url` carries the URL that was requested and
     *      `ev.message` describes the failure as reported by the engine; the
     *      navigation members throw the same failure instead of returning, so a
     *      listener is only needed when the navigation was started by the page.
     *
     *      Example — report a broken navigation started by the page:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('load-failed', function (ev) {
     *          console.log(ev.url + ': ' + ev.message);
     *          win.close();
     *      });
     *
     *      win.setHtml('<html><body><script>location.href = "http://127.0.0.1:1/";</script></body></html>');
     *      win.waitForLoad();
     *      ```
     *
     *      @param ev the event object, carrying the failed url and the engine message
     *
     */
    on(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "load-failed", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the navigation failed event, equivalent to on("load-failed", func);
     *
     *      Fired when a navigation ended without loading its document: an
     *      unreachable host, a refused connection, a certificate error or a URL the
     *      engine cannot handle. `ev.url` carries the URL that was requested and
     *      `ev.message` describes the failure as reported by the engine; the
     *      navigation members throw the same failure instead of returning, so a
     *      listener is only needed when the navigation was started by the page.
     *
     *      Example — report a broken navigation started by the page:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('load-failed', function (ev) {
     *          console.log(ev.url + ': ' + ev.message);
     *          win.close();
     *      });
     *
     *      win.setHtml('<html><body><script>location.href = "http://127.0.0.1:1/";</script></body></html>');
     *      win.waitForLoad();
     *      ```
     *
     *      @param ev the event object, carrying the failed url and the engine message
     *
     */
    "onload-failed": ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the window move event, equivalent to on("move", func);
     *
     *      Fired when the window moves, both for a user drag and for `setPosition`.
     *      `ev.left` and `ev.top` carry the new top-left corner in screen
     *      coordinates, and `ev.type`/`ev.target` identify the event and the WebView.
     *
     *      Example — follow the window position:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('move', function (ev) {
     *          console.log(ev.left, ev.top);
     *      });
     *
     *      win.setPosition(120, 80);
     *      win.close();
     *      ```
     *
     *      @param ev the event object, carrying the position of the window
     *
     */
    on(event: "move", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "move", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "move", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "move", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "move", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "move", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "move", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "move", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "move", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the window move event, equivalent to on("move", func);
     *
     *      Fired when the window moves, both for a user drag and for `setPosition`.
     *      `ev.left` and `ev.top` carry the new top-left corner in screen
     *      coordinates, and `ev.type`/`ev.target` identify the event and the WebView.
     *
     *      Example — follow the window position:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('move', function (ev) {
     *          console.log(ev.left, ev.top);
     *      });
     *
     *      win.setPosition(120, 80);
     *      win.close();
     *      ```
     *
     *      @param ev the event object, carrying the position of the window
     *
     */
    onmove: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the window size change event, equivalent to on("resize", func);
     *
     *      Fired when the window is resized, whether by the user, by the window
     *      manager or by `setSize`. `ev.width` and `ev.height` carry the new size in
     *      window pixels, and `ev.type`/`ev.target` identify the event and the
     *      WebView; the page is resized together with the window.
     *
     *      Example — follow the window size:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('resize', function (ev) {
     *          console.log(ev.width, ev.height);
     *      });
     *
     *      win.setSize(640, 400);
     *      win.close();
     *      ```
     *
     *      @param ev the event object, carrying the size of the window
     *
     */
    on(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "resize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the window size change event, equivalent to on("resize", func);
     *
     *      Fired when the window is resized, whether by the user, by the window
     *      manager or by `setSize`. `ev.width` and `ev.height` carry the new size in
     *      window pixels, and `ev.type`/`ev.target` identify the event and the
     *      WebView; the page is resized together with the window.
     *
     *      Example — follow the window size:
     *      ```JavaScript
     *      // requires: long-running
     *      const gui = require('gui');
     *
     *      const win = gui.open({ width: 320, height: 200 });
     *
     *      win.on('resize', function (ev) {
     *          console.log(ev.width, ev.height);
     *      });
     *
     *      win.setSize(640, 400);
     *      win.close();
     *      ```
     *
     *      @param ev the event object, carrying the size of the window
     *
     */
    onresize: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the window focus event, equivalent to on("focus", func);
     *
     *      Fired when this window becomes the active window, either because the user
     *      activated it or because `active` was called. The event object carries the
     *      usual `type` and `target` fields; no extra data is attached. Pair it with
     *      `blur` to track whether the window is in the foreground.
     *
     *      @param ev the event object
     *
     */
    on(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "focus", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the window focus event, equivalent to on("focus", func);
     *
     *      Fired when this window becomes the active window, either because the user
     *      activated it or because `active` was called. The event object carries the
     *      usual `type` and `target` fields; no extra data is attached. Pair it with
     *      `blur` to track whether the window is in the foreground.
     *
     *      @param ev the event object
     *
     */
    onfocus: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the window blur event, equivalent to on("blur", func);
     *
     *      Fired when this window loses the desktop focus to another window. The
     *      event object carries the usual `type` and `target` fields; no extra data is
     *      attached. It is the counterpart of the `focus` event.
     *
     *      @param ev the event object
     *
     */
    on(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "blur", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the window blur event, equivalent to on("blur", func);
     *
     *      Fired when this window loses the desktop focus to another window. The
     *      event object carries the usual `type` and `target` fields; no extra data is
     *      attached. It is the counterpart of the `focus` event.
     *
     *      @param ev the event object
     *
     */
    onblur: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the window close event, equivalent to on("close", func);
     *
     *      Fired once after the native window has been destroyed, whatever closed it:
     *      `close`, the user's window-manager button or the page's `window.close`.
     *      The event object is otherwise empty apart from `type` and `target`. After
     *      it fires the WebView object is unusable - its members throw - so release
     *      any state kept for the window here.
     *
     */
    on(event: "close", listener: ()=>void): this;

    once(event: "close", listener: ()=>void): this;

    off(event: "close", listener: ()=>void): this;

    addListener(event: "close", listener: ()=>void): this;

    removeListener(event: "close", listener: ()=>void): this;

    addEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close", listener: ()=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close", listener: ()=>void): this;

    prependOnceListener(event: "close", listener: ()=>void): this;

    /**
     * @description Queries and binds the window close event, equivalent to on("close", func);
     *
     *      Fired once after the native window has been destroyed, whatever closed it:
     *      `close`, the user's window-manager button or the page's `window.close`.
     *      The event object is otherwise empty apart from `type` and `target`. After
     *      it fires the WebView object is unusable - its members throw - so release
     *      any state kept for the window here.
     *
     */
    onclose: (()=>void) | null;

    /**
     * @description Emitted before the window is closed, allowing the application to cancel
     *
     *      Fired when the window is about to be destroyed: the window-manager close
     *      button, the page's `window.close` and the `close()` member all consult it
     *      first. Call `ev.preventDefault()` in a listener to keep the window open;
     *      the platform then treats the request as handled and nothing is destroyed,
     *      and `close()` returns false. `destroy()` skips this event and closes
     *      unconditionally. The event object is a DOMEvent: it carries `type`,
     *      `cancelable`, `defaultPrevented` and the `preventDefault()` and
     *      `stopPropagation()` methods.
     *
     *      @param ev the cancelable DOMEvent whose preventDefault() cancels the close
     *
     */
    on(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "close-requested", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted before the window is closed, allowing the application to cancel
     *
     *      Fired when the window is about to be destroyed: the window-manager close
     *      button, the page's `window.close` and the `close()` member all consult it
     *      first. Call `ev.preventDefault()` in a listener to keep the window open;
     *      the platform then treats the request as handled and nothing is destroyed,
     *      and `close()` returns false. `destroy()` skips this event and closes
     *      unconditionally. The event object is a DOMEvent: it carries `type`,
     *      `cancelable`, `defaultPrevented` and the `preventDefault()` and
     *      `stopPropagation()` methods.
     *
     *      @param ev the cancelable DOMEvent whose preventDefault() cancels the close
     *
     */
    "onclose-requested": ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Emitted when the window becomes visible
     *
     *      Fired when the native window is mapped and starts to show, whether the
     *      change came from the show API, from restoring a minimized window or from
     *      the platform revealing it again. The event object carries only `type` and
     *      `target`; there is no payload.
     *
     *      @param ev the event object
     *
     */
    on(event: "show", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "show", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "show", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "show", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "show", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "show", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "show", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "show", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "show", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted when the window becomes visible
     *
     *      Fired when the native window is mapped and starts to show, whether the
     *      change came from the show API, from restoring a minimized window or from
     *      the platform revealing it again. The event object carries only `type` and
     *      `target`; there is no payload.
     *
     *      @param ev the event object
     *
     */
    onshow: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Emitted when the window is hidden
     *
     *      Fired when the native window stops being mapped: the hide API, minimizing
     *      on platforms that unmap the window, or the window manager taking it off
     *      screen. The event object carries only `type` and `target`.
     *
     *      @param ev the event object
     *
     */
    on(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "hide", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted when the window is hidden
     *
     *      Fired when the native window stops being mapped: the hide API, minimizing
     *      on platforms that unmap the window, or the window manager taking it off
     *      screen. The event object carries only `type` and `target`.
     *
     *      @param ev the event object
     *
     */
    onhide: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Emitted when the window is minimized
     *
     *      Fired after the platform applied the minimized state, which may be a
     *      moment after the `state` property was written. The window keeps rendering
     *      its last frame; resources are not released.
     *
     *      @param ev the event object
     *
     */
    on(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "minimize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted when the window is minimized
     *
     *      Fired after the platform applied the minimized state, which may be a
     *      moment after the `state` property was written. The window keeps rendering
     *      its last frame; resources are not released.
     *
     *      @param ev the event object
     *
     */
    onminimize: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Emitted when the window is maximized
     *
     *      Fired after the platform applied the maximized state. Maximizing is a
     *      window-manager operation, so the resulting size can differ from the screen
     *      size on platforms that reserve space for panels or docks.
     *
     *      @param ev the event object
     *
     */
    on(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "maximize", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted when the window is maximized
     *
     *      Fired after the platform applied the maximized state. Maximizing is a
     *      window-manager operation, so the resulting size can differ from the screen
     *      size on platforms that reserve space for panels or docks.
     *
     *      @param ev the event object
     *
     */
    onmaximize: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Emitted when the window leaves the minimized or maximized state
     *
     *      Fired when the platform puts the window back into the normal state, from
     *      minimizing, maximizing or fullscreen depending on the platform. The event
     *      object carries only `type` and `target`.
     *
     *      @param ev the event object
     *
     */
    on(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "restore", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted when the window leaves the minimized or maximized state
     *
     *      Fired when the platform puts the window back into the normal state, from
     *      minimizing, maximizing or fullscreen depending on the platform. The event
     *      object carries only `type` and `target`.
     *
     *      @param ev the event object
     *
     */
    onrestore: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Emitted when the window enters fullscreen
     *
     *      Fired after the platform entered fullscreen, which includes the animation
     *      time on macOS where the transition is asynchronous. `state` reads
     *      `fullscreen` once the event fired.
     *
     *      @param ev the event object
     *
     */
    on(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "enter-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted when the window enters fullscreen
     *
     *      Fired after the platform entered fullscreen, which includes the animation
     *      time on macOS where the transition is asynchronous. `state` reads
     *      `fullscreen` once the event fired.
     *
     *      @param ev the event object
     *
     */
    "onenter-fullscreen": ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Emitted when the window leaves fullscreen
     *
     *      Fired after the platform left fullscreen and the previous window state was
     *      restored. `state` reads `normal`, `minimized` or `maximized` once the
     *      event fired, depending on what the session restored.
     *
     *      @param ev the event object
     *
     */
    on(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "leave-fullscreen", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Emitted when the window leaves fullscreen
     *
     *      Fired after the platform left fullscreen and the previous window state was
     *      restored. `state` reads `normal`, `minimized` or `maximized` once the
     *      event fired, depending on what the session restored.
     *
     *      @param ev the event object
     *
     */
    "onleave-fullscreen": ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Queries and binds the webview message event, equivalent to on("message", func);
     *
     *      Fired when the page sends a message to fibjs through its
     *      `window.postMessage` (rewired by the injected bridge). `ev.data` carries
     *      the received string, so JSON-encode structured data in the page; the
     *      event has no other payload. The host-to-page direction is `postMessage`.
     *
     *      @param ev the event object, carrying the received message in its data property
     *
     */
    on(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    once(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    off(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    removeListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    addEventListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    removeEventListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void, options?: FIBJS.GeneralObject): this;

    prependListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    prependOnceListener(event: "message", listener: (ev: FIBJS.GeneralObject)=>void): this;

    /**
     * @description Queries and binds the webview message event, equivalent to on("message", func);
     *
     *      Fired when the page sends a message to fibjs through its
     *      `window.postMessage` (rewired by the injected bridge). `ev.data` carries
     *      the received string, so JSON-encode structured data in the page; the
     *      event has no other payload. The host-to-page direction is `postMessage`.
     *
     *      @param ev the event object, carrying the received message in its data property
     *
     */
    onmessage: ((ev: FIBJS.GeneralObject)=>void) | null;

    /**
     * @description Keeps the fibjs process alive; prevents exit while the object is bound
     *
     *      Increases the keep-alive reference of the isolate: while the reference is
     *      held, a finished script does not let the process exit. An open WebView
     *      already takes such a reference when it is created and releases it when the
     *      window closes, so `ref` is mainly used to re-arm the process after an
     *      explicit `unref`. Returns the object itself, so calls can be chained.
     *
     *      @return returns the current object
     *
     */
    ref(): Class_WebView;

    /**
     * @description Allows the fibjs process to exit while the object is bound
     *
     *      Releases the keep-alive reference taken when the window was created or by
     *      a previous `ref`: the process can then exit even while the window is open,
     *      and pending events or window output may be cut short. Returns the object
     *      itself, so calls can be chained.
     *
     *      @return returns the current object
     *
     */
    unref(): Class_WebView;

    on(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    on(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    once(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    once(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    off(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    off(ev: any): FIBJS.GeneralObject;

    off(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    addListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    removeListener(ev: any): FIBJS.GeneralObject;

    removeListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    addEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    removeEventListener(ev: any, func: (...args: any[])=>void, options?: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

    prependOnceListener(ev: any, func: (...args: any[])=>void): FIBJS.GeneralObject;

    prependOnceListener(map: FIBJS.GeneralObject): FIBJS.GeneralObject;

}

