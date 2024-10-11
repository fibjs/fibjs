const gui = require("gui");
const path = require("path");

gui.open({
    icon: path.join(__dirname, "gui_files", "toolbox.512.png"),
    left: 100,
    top: 100,
    width: 400,
    height: 200,
}).setHtml(`<H1>title, resize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 550,
    top: 100,
    width: 400,
    height: 200,
    caption: false
}).setHtml(`<H1>no title, resize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    icon: path.join(__dirname, "gui_files", "icon512.png"),
    left: 100,
    top: 350,
    width: 400,
    height: 200,
    resizable: false
}).setHtml(`<H1>title, no resize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 550,
    top: 350,
    width: 400,
    height: 200,
    caption: false,
    resizable: false
}).setHtml(`<H1>no title, no resize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 100,
    top: 600,
    width: 400,
    height: 200,
    frame: false
}).setHtml(`<H1>no frame</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    maximize: true
}).setHtml(`<H1>Maximize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    fullscreen: true
}).setHtml(`<H1>Fullscreen</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);
