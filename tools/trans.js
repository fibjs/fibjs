var path = require("path");
var os = require("os");
var coroutine = require("coroutine");
var fs = require("fs");
var hash = require("hash");

var driver = require("./util/driver");

var langs = [
    "ca",
    "de",
    "en",
    "es",
    "fr",
    "gl",
    "it",
    "ru",
    "ja",
    "ko",
    "zh-tw",
];

var docs_path = path.join(__dirname, "../docs/web/dist");

function translate(fname, lang) {
    var out_fname = path.join(docs_path, lang, fname);
    fname = path.join(docs_path, fname);
    if (fs.exists(out_fname))
        return true;

    console.log("translate", lang, fname);

    if (path.extname(fname) != ".html") {
        fs.mkdir(path.dirname(out_fname), {
            recursive: true
        });
        fs.copyFile(fname, out_fname);
        return true;
    }

    var done = false;

    function trans_page() {
        function trans() {
            page.Runtime.evaluate(`var e = document.querySelector("div.social-share");if(e)e.innerHTML=""`);
            page.Runtime.evaluate(`var e = document.querySelector("div#toc");if(e)e.innerHTML=""`);

            page.Runtime.evaluate(`var node = document.createElement('div');node.id='google_translate_element';document.body.appendChild(node);`);
            page.Runtime.evaluate(`var node = document.createElement('script');node.id="google_translate_callback";node.text="function googleTranslateElementInit() {var tr = new google.translate.TranslateElement({ pageLanguage: 'zh-CN', autoDisplay: false }, 'google_translate_element');}";document.body.appendChild(node);`);
            page.Runtime.evaluate(`var node = document.createElement('script');node.id="google_translate_script";node.src='https://translate.google.com/translate_a/element.js?cb=googleTranslateElementInit';document.body.appendChild(node);`);

            var e = page.wait("select.goog-te-combo");
            coroutine.sleep(1000);

            page.Runtime.evaluate(`var e = document.querySelector("select.goog-te-combo");var eo=e.options;for(i=0;i<eo.length;i++)if(eo[i].value="${lang}")e.selectedIndex=i;e.dispatchEvent(new Event('change'))`);

            var html;
            for (var i = 0; i < 100; i++) {
                html = page.getOuterHTML("div.footer");
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
            html = page.getOuterHTML();

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

        var win = page.Browser.getWindowForTarget(page.targetId);
        var doc = page.DOM.getDocument();
        var e = page.DOM.querySelector(doc.root.nodeId, "body");
        var box = page.DOM.getBoxModel(e.nodeId);

        page.Browser.setWindowBounds(win.windowId, {
            width: box.model.width,
            height: box.model.height
        });

        if (trans()) {
            fix_html();
            fix_res();
            fix_lang();

            console.notice("update", out_fname);
            fs.mkdir(path.dirname(out_fname), {
                recursive: true
            });
            fs.writeFile(out_fname, html);
            done = true;
        } else
            console.error("timeout", out_fname);

        page.close();
        ev.set();
    }

    var browser = driver.launch([
        '--window-size=2000,100'
    ]);
    var page = browser.open();
    page.Page.enable();
    page.on("Page.loadEventFired", trans_page);
    page.Page.navigate(`file://${fname}`);

    var ev = new coroutine.Event();
    ev.wait();
    browser.close();

    return done;
}

function check_docs() {
    function check_folder(lang, p) {
        if (!fs.exists(path.join(docs_path, lang, p)))
            return;

        var dir = fs.readdir(path.join(docs_path, lang, p));
        dir.forEach(function (name) {
            var fname = path.join(p, name);
            var f = fs.stat(path.join(docs_path, lang, fname));
            if (f.isDirectory()) {
                check_folder(lang, fname);
            } else {
                if (!fs.exists(path.join(docs_path, fname))) {
                    console.log("delete", path.join(docs_path, lang, fname));
                    fs.unlink(path.join(docs_path, lang, fname));
                }
            }
        });
    }

    langs.forEach(lang => check_folder(lang, ""));
}

check_docs();

var trans_queue = [];
var docs_md5;
var new_docs_md5 = {};

function queue_one(fname) {
    var md5 = hash.md5(fs.readFile(path.join(docs_path, fname))).digest().hex();
    new_docs_md5[md5] = true;

    langs.forEach(lang => {
        trans_queue.push([
            fname,
            lang
        ])
    });

    if (!docs_md5[md5]) {
        langs.forEach(lang => {
            try {
                console.log("delete", path.join(docs_path, lang, fname));
                fs.unlink(path.join(docs_path, lang, fname));
            } catch (e) { }
        });
    }
}

function queue_docs(p) {
    var dir = fs.readdir(path.join(docs_path, p));
    dir.forEach(function (name) {
        var fname = path.join(p, name);
        var f = fs.stat(path.join(docs_path, fname));
        if (f.isDirectory()) {
            queue_docs(fname);
        } else
            queue_one(fname);
    });
}

try {
    docs_md5 = JSON.parse(fs.readTextFile(path.join(docs_path, "docs_md5.json")));
} catch (e) {
    docs_md5 = {};
}

queue_one("index.html");
queue_one("support.html");

queue_docs("docs");

if (JSON.stringify(new_docs_md5, 2) != JSON.stringify(docs_md5, 2)) {
    console.notice("update", path.join(docs_path, "docs_md5.json"));
    fs.writeTextFile(path.join(docs_path, "docs_md5.json"), JSON.stringify(new_docs_md5, 2));
}

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
