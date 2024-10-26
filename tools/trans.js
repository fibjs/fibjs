var coroutine = require("coroutine");
var path = require("path");
var fs = require("fs");
var crypto = require("crypto");
var gui = require("gui");
var driver = require("fib-driver");

var langs = [
    "ca",
    "de",
    "en",
    "es",
    "fr",
    "gl",
    "it",
    "nl",
    "ru",
    "ja",
    "ko",
    "zh-tw",
];

var docs_path = path.join(__dirname, "../docs/web/dist");

const pages = [];

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

    var page = pages.pop() || gui.open({
        width: 1280,
        height: 1024,
        visible: false
    });

    page.loadFile(fname);
    coroutine.sleep(1000);

    for (var i = 0; i < 500 && !page.isReady(); i++)
        coroutine.sleep(100);

    if (!page.isReady()) {
        console.error("page not ready");
        pages.push(page);
        return false;
    }

    page.eval('document.body.style.transform = "scale(0.1)";');
    page.eval('document.body.style.transformOrigin = "0 0";');

    page.eval(`var e = document.querySelector("div.social-share");if(e)e.innerHTML=""`);
    page.eval(`var e = document.querySelector("div#toc");if(e)e.innerHTML=""`);

    page.eval(`var node = document.createElement('div');node.id='google_translate_element';document.body.appendChild(node);`);
    page.eval(`var node = document.createElement('script');node.id="google_translate_callback";node.text="function googleTranslateElementInit() {var tr = new google.translate.TranslateElement({ pageLanguage: 'zh-CN', autoDisplay: false }, 'google_translate_element');}";document.body.appendChild(node);`);
    page.eval(`var node = document.createElement('script');node.id="google_translate_script";node.src='https://translate.google.com/translate_a/element.js?cb=googleTranslateElementInit';document.body.appendChild(node);`);

    page.querySelector("select.goog-te-combo");
    coroutine.sleep(1000);

    page.eval(`var e = document.querySelector("select.goog-te-combo");var eo=e.options;for(i=0;i<eo.length;i++)if(eo[i].value="${lang}")e.selectedIndex=i;e.dispatchEvent(new Event('change'))`);

    for (var i = 0; i < 500; i++) {
        var footer = page.querySelector("div.footer").innerHTML;
        if (footer.indexOf("支持我们") == -1)
            break;
        else
            coroutine.sleep(100);

    }

    if (page.querySelector("div.footer").innerHTML.indexOf("支持我们") != -1) {
        console.error("translate failed.", lang, fname);
        pages.push(page);
        return false;
    }

    coroutine.sleep(1000);

    var html;
    html = page.getHtml();

    html = html.replace(/<html.*?>/g, "<html>");
    html = html.replace(/<link type.*?head>/g, "</head>");
    html = html.replace(/<body.+?>/g, "<body>");
    html = html.replace(/<body.*?svg>/g, "<body>");
    html = html.replace(/<div class=\"skiptranslate.*?\/div>/g, "");
    html = html.replace(/<div id=\"google.*iframe>/g, "");

    var res_dir = {
        imgs: 1,
        css: 1,
        js: 1,
        download: 1
    }
    html = html.replace(/(href|src)=\"([\.\.\/]*)([^\"]*?)(\/.*?)\"/g, (s, s1, s2, s3, s4) => {
        if (res_dir[s3] && s2 !== "./")
            s = `${s1}="${s2}../${s3}${s4}"`;
        return s;
    });

    html = html.replace(/url\(\'\.\/imgs/g, "url(\'..\/imgs");

    html = html.replace(/<div class=\"dropdown-menu\">.*?<\/div>/g, (s) => s.replace(/href=\"/g, s => s + "../"));

    console.notice("update", out_fname);
    fs.mkdir(path.dirname(out_fname), {
        recursive: true
    });
    fs.writeFile(out_fname, html);

    pages.push(page);

    return true;
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
    var md5 = crypto.createHash("md5").update(fs.readFile(path.join(docs_path, fname))).digest("hex");
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

pages.forEach(page => page.close());

// var item = trans_queue[0];
// translate(item[0], item[1], item[2]);
