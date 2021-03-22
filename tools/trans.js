var gui = require("gui");
var path = require("path");
var os = require("os");
var coroutine = require("coroutine");
var fs = require("fs");
var mkdirp = require('@fibjs/mkdirp');

var cef_path = path.join(__dirname, "../temp/cef", os.type());

gui.config({
    // "cache_path": `${os.homedir()}/.cache`,
    "cef_path": cef_path,
    // "proxy": {
    //     mode: "fixed_servers",
    //     server: "192.168.65.5:1087"
    // }
});

function wait(win, selector) {
    do {
        var doc = win.dev.DOM.getDocument();
        try {
            var e = win.dev.DOM.querySelector({
                nodeId: doc.root.nodeId,
                selector: "select.goog-te-combo"
            });
        } catch (e) {};
        coroutine.sleep(100);
    } while (e && e.nodeId == 0);

    return e;
}

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

function translate(fname, lang, overwrite) {
    console.notice(lang, fname);

    var out_fname = path.join(__dirname, "../docs/web/dist", lang, fname);
    fname = path.join(__dirname, "../docs/web/dist", fname);
    if (!overwrite && fs.exists(out_fname))
        return true;

    if (path.extname(fname) != ".html") {
        mkdirp(path.dirname(out_fname));
        fs.copyFile(fname, out_fname);
        return true;
    }

    var done = false;
    var ev = new coroutine.Event();
    const height = 100;

    var win = gui.open(`file://${fname}`, {
        width: 800,
        height: height,
        headless: true
    });

    function trans_win() {
        function trans() {
            win.executeJavaScript(`var e = document.querySelector("div.social-share");if(e)e.innerHTML=""`);
            win.executeJavaScript(`var e = document.querySelector("div#toc");if(e)e.innerHTML=""`);

            win.executeJavaScript(`var node = document.createElement('div');node.id='google_translate_element';document.body.appendChild(node);`);
            win.executeJavaScript(`var node = document.createElement('script');node.id="google_translate_callback";node.text="function googleTranslateElementInit() {var tr = new google.translate.TranslateElement({ pageLanguage: 'zh-CN', autoDisplay: false }, 'google_translate_element');}";document.body.appendChild(node);`);
            win.executeJavaScript(`var node = document.createElement('script');node.id="google_translate_script";node.src='https://translate.google.com/translate_a/element.js?cb=googleTranslateElementInit';document.body.appendChild(node);`);

            var e = wait(win, "select.goog-te-combo");
            coroutine.sleep(1000);

            win.executeJavaScript(`var e = document.querySelector("select.goog-te-combo");var eo=e.options;for(i=0;i<eo.length;i++)if(eo[i].value="${lang}")e.selectedIndex=i;e.dispatchEvent(new Event('change'))`);

            var html;
            for (var i = 0; i < 100; i++) {
                html = getOuterHTML(win, "div.footer");
                if (html.indexOf("支持我们") == -1)
                    break;
                coroutine.sleep(100);
            }
            if (html.indexOf("支持我们") != -1)
                return false;

            coroutine.sleep(1000);

            return true;
        }

        var html;

        function fix_html() {
            var doc = win.dev.DOM.getDocument();
            html = win.dev.DOM.getOuterHTML({
                nodeId: doc.root.nodeId
            }).outerHTML;

            html = html.replace(/<html.*?>/g, "<html>");
            html = html.replace(/<link type.*?head>/g, "</head>");
            html = html.replace(/<body.+?>/g, "<body>");
            html = html.replace(/<body.*?svg>/g, "<body>");
            html = html.replace(/<div class=\"skiptranslate.*?\/div>/g, "");
            html = html.replace(/<div id=\"google.*iframe>/g, "");
        }

        var res_dir = {
            imgs: 1,
            css: 1,
            js: 1,
            download: 1
        }

        function fix_res() {
            html = html.replace(/(href|src)=\"([\.\.\/]*)([^\"]*?)(\/.*?)\"/g, (s, s1, s2, s3, s4) => {
                if (res_dir[s3] && s2 !== "./")
                    s = `${s1}="${s2}../${s3}${s4}"`;
                return s;
            });

            html = html.replace(/url\(\'\.\/imgs/g, "url(\'..\/imgs");
        }

        function fix_lang() {
            html = html.replace(/<div class=\"dropdown-menu\">.*?<\/div>/g, (s) => s.replace(/href=\"/g, s => s + "../"));
        }

        if (trans()) {
            fix_html();
            fix_res();
            fix_lang();

            console.log("write", out_fname);
            mkdirp(path.dirname(out_fname));
            fs.writeFile(out_fname, html);
            done = true;
        } else
            console.error("Timeeout");

        win.close();
        ev.set();
    }

    win.on("load", () => {
        var doc = win.dev.DOM.getDocument();
        var e = win.dev.DOM.querySelector({
            nodeId: doc.root.nodeId,
            selector: "body"
        });

        var box = win.dev.DOM.getBoxModel({
            nodeId: e.nodeId
        });

        win.close();
        win = gui.open(`file://${fname}`, {
            width: 800,
            height: box.model.content[5] + 200,
            headless: true
        });

        win.on("load", trans_win);
    });

    ev.wait();

    return done;
}

function clean_folder(p) {
    var dir = fs.readdir(p);
    console.log("clean", p);
    dir.forEach(function (name) {
        var fname = path.join(p, name);
        var f = fs.stat(fname);
        if (f.isDirectory()) {
            clean_folder(fname);
            fs.rmdir(fname);
        } else
            fs.unlink(fname);
    });
}

function queue_docs(p, overwrite) {
    var dir = fs.readdir(path.join(__dirname, "../docs/web/dist", p));
    dir.forEach(function (name) {
        var fname = path.join(p, name);
        var f = fs.stat(path.join(__dirname, "../docs/web/dist", fname));
        if (f.isDirectory()) {
            queue_docs(fname, overwrite);
        } else
            queue_one(fname, overwrite);
    });
}

var langs = [
    "en",
    "ja",
    "de",
    "es",
    "fr",
    "ko",
    "it",
    "ru",
];

// langs.forEach(lang => clean_folder(path.join(__dirname, "../docs/web/dist", lang)));

var trans_queue = [];

function queue_one(fname, overwrite) {
    langs.forEach(lang => trans_queue.push([
        fname,
        lang,
        overwrite
    ]));
}

queue_one("index.html");
queue_one("support.html");

queue_docs("docs");

// queue_one("docs/manual/module/ifs/child_process.md.html", true);

trans_queue = trans_queue.sort((n1, n2) => {
    if (n1[0] < n2[0])
        return -1;

    if (n1[0] > n2[0])
        return 1;

    return 0;
});

coroutine.parallel(trans_queue, item => {
    translate(item[0], item[1], item[2]) || translate(item[0], item[1], item[2]);
}, 20);