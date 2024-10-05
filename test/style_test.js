const gui = require("gui");

gui.open({
    left: 100,
    top: 100,
    width: 400,
    height: 200,
}).setHtml("<H1>Normal</H1>");

gui.open({
    left: 600,
    top: 100,
    width: 400,
    height: 200,
    caption: false
}).setHtml("<H1>without caption</H1>");

gui.open({
    left: 100,
    top: 400,
    width: 400,
    height: 200,
    resizable: false
}).setHtml("<H1>without resize</H1>");

gui.open({
    left: 600,
    top: 400,
    width: 400,
    height: 200,
    frame: false
}).setHtml("<H1>without frame</H1>");


gui.open({
    maximize: true
}).setHtml("<H1>Maximize</H1>");

gui.open({
    fullscreen: true
}).setHtml(`<H1>Fullscreen</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);
