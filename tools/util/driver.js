var os = require("os");
var ws = require("ws");
var http = require("http");
var url = require("url");
var coroutine = require("coroutine");
var child_process = require("child_process");

const chromeExecutablePath = {
    win32: `${process.env['PROGRAMFILES']}\\Google\\Chrome\\Application\\chrome.exe`,
    darwin: '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome',
    linux: '/opt/google/chrome/chrome'
};

var m_id = 1;
function gen_api(conn, protocol) {
    var contexts = {};

    conn.on("message", data => {
        data = data.json();

        if (data.method) {
            // console.log(conn.type, data);
            conn.emit(data.method, data.params);
        } else {
            var res = contexts[data.id];
            delete contexts[data.id];

            clearTimeout(res.timer);

            if (data.error)
                res.error = data.error;
            else
                res.result = data.result;

            res.ev.set();
        }
    });

    protocol.domains.forEach(domain => {
        var dom = conn[domain.domain] = {};
        domain.commands.forEach(command => {
            if (!command.deprecated) {
                var func = dom[command.name] = function () {
                    var id = m_id++;
                    var res = contexts[id] = {
                        ev: new coroutine.Event(),
                        result: null,
                        error: null,
                        timer: setTimeout(() => {
                            delete contexts[id];
                            res.error = {
                                code: -32000,
                                message: `'${domain.domain}.${command.name}' timeout`
                            };
                            res.ev.set();
                        }, 30000)
                    };

                    var params = {};
                    if (command.parameters) {
                        for (var i = 0; i < arguments.length && i < command.parameters.length; i++) {
                            var p = command.parameters[i];
                            params[p.name] = arguments[i];
                        }
                    }

                    conn.send(JSON.stringify({
                        id: id,
                        method: `${domain.domain}.${command.name}`,
                        params: params
                    }));
                    res.ev.wait();
                    if (res.error)
                        throw new Error(res.error.message);
                    return res.result;
                };

                func.name = command.name;
                if (command.description)
                    func.description = command.description;
                if (command.parameters)
                    func.parameters = command.parameters;
                if (command.returns)
                    func.returns = command.returns;
            }
        });
    });
}

exports.launch = function (opt) {
    const chromeArguments = [
        '--allow-pre-commit-input',
        '--disable-background-networking',
        '--disable-background-timer-throttling',
        '--disable-backgrounding-occluded-windows',
        '--disable-breakpad',
        '--disable-client-side-phishing-detection',
        '--disable-component-extensions-with-background-pages',
        '--disable-component-update',
        '--disable-default-apps',
        '--disable-dev-shm-usage',
        '--disable-extensions',
        '--disable-features=Translate,BackForwardCache,AcceptCHFrame,MediaRouter,OptimizationHints',
        '--disable-hang-monitor',
        '--disable-ipc-flooding-protection',
        '--disable-popup-blocking',
        '--disable-prompt-on-repost',
        '--disable-renderer-backgrounding',
        '--disable-sync',
        '--enable-automation',
        '--enable-blink-features=IdleDetection',
        '--enable-features=NetworkServiceInProcess2',
        '--export-tagged-pdf',
        '--force-color-profile=srgb',
        '--metrics-recording-only',
        '--no-first-run',
        '--password-store=basic',
        '--use-mock-keychain',
        '--headless',
        '--remote-debugging-port=0',
        ...opt,
        'about:blank'
    ];

    var chrome = child_process.spawn(chromeExecutablePath[os.platform()], chromeArguments);

    var msg = chrome.stderr.read();
    if (msg === null)
        throw new Error("Failed to launch chrome");
    msg = msg.toString();
    var match = msg.match(/DevTools listening on (ws:\/\/.*)/);
    if (!match)
        throw new Error("Failed to launch chrome");
    var devtoolsUrl = match[1];
    var u = new url.URL(devtoolsUrl);

    var protocol = http.get(`http://${u.host}/json/protocol`).json();

    var browser = new ws.Socket(devtoolsUrl);
    browser.type = "browser";

    browser.open = function (url = "") {
        var t = browser.Target.createTarget(url);

        var page = new ws.Socket(`${u.protocol}//${u.host}/devtools/page/${t.targetId}`);
        page.type = "page";
        page.targetId = t.targetId;

        page.wait = function (selector) {
            do {
                var doc = page.DOM.getDocument();
                try {
                    var e = page.DOM.querySelector(doc.root.nodeId, selector);
                } catch (e) { };
                coroutine.sleep(100);
            } while (e && e.nodeId == 0);

            return e;
        };

        page.getOuterHTML = function (selector) {
            var doc = page.DOM.getDocument();
            var e = doc.root;
            if (selector)
                e = page.DOM.querySelector(e.nodeId, selector);
            return page.DOM.getOuterHTML(e.nodeId).outerHTML;
        };

        page.close = () => browser.Target.closeTarget(t.targetId);
        gen_api(page, protocol);

        var ev = new coroutine.Event();
        page.on("open", () => ev.set());
        ev.wait();

        return page;
    };

    browser.close = () => browser.Browser.close();

    gen_api(browser, protocol);

    var ev = new coroutine.Event();
    browser.on("open", () => ev.set());
    ev.wait();

    return browser;
};
