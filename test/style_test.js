const gui = require("gui");
const path = require("path");


const menu_template = [
    {
        label: "file",
        submenu: [
            {
                label: "item1"
            },
            {
                label: "icon",
                icon: path.join(__dirname, "gui_files", "icon512.png"),
            }
        ]
    }
]

const chared_menu = gui.createMenu([
    {
        label: "file",
        submenu: [
            {
                label: "item1"
            },
            {
                label: "check test",
                checked: false
            }
        ]
    }
]);

gui.open({
    icon: path.join(__dirname, "gui_files", "toolbox.512.png"),
    left: 100,
    top: 100,
    width: 300,
    height: 200,
    menu: menu_template
}).setHtml(`<H1>title, resize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 450,
    top: 100,
    width: 300,
    height: 200,
    titlebar: "hide",
    menu: menu_template
}).setHtml(`<H1>no title, resize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 800,
    top: 100,
    width: 300,
    height: 200,
    menu: chared_menu
}).setHtml(`<H1>shared  menu</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    icon: path.join(__dirname, "gui_files", "icon512.png"),
    left: 100,
    top: 350,
    width: 300,
    height: 200,
    resizable: false,
    menu: menu_template
}).setHtml(`<H1>title, no resize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 450,
    top: 350,
    width: 300,
    height: 200,
    titlebar: "hide",
    resizable: false,
    menu: menu_template
}).setHtml(`<H1>no title, no resize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 800,
    top: 350,
    width: 300,
    height: 200,
    menu: chared_menu
}).setHtml(`<H1>shared  menu</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);


gui.open({
    left: 100,
    top: 600,
    width: 300,
    height: 200,
    frame: false,
    menu: menu_template
}).setHtml(`<H1>no frame</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 450,
    top: 600,
    width: 300,
    height: 200,
    titlebar: "transparent",
    menu: menu_template
}).setHtml(`<H1>title, transparent</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

gui.open({
    left: 800,
    top: 600,
    width: 300,
    height: 200,
    titlebar: {
        "style": "transparent",
        "height": "tall"
    },
    menu: menu_template
}).setHtml(`<H1>title, transparent</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

// gui.open({
//     maximize: true
// }).setHtml(`<H1>Maximize</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);

// gui.open({
//     fullscreen: true
// }).setHtml(`<H1>Fullscreen</H1><button onclick="window.close()" style="font-size: 50px;">Close</button>`);
