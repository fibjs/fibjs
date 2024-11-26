const gui = require("gui");
const path = require("path");
const coroutine = require("coroutine");

const wins = {};

const tray = gui.createTray({
    icon: path.resolve(__dirname, "icon.png"),
    menu: [
        {
            label: "github",
            onclick: function () {
                if (wins.github)
                    wins.github.active();
                else {

                    wins.github = gui.open("https://github.com/fibjs", {
                        width: 500,
                        height: 400,
                        onclose: function () {
                            delete wins.github;
                        },
                        onmove: function (ev) {
                            console.log("move", ev.left, ev.top);
                        }
                    });
                }
            }
        },
        {
            label: "frame",
            onclick: function () {
                if (wins.fibjs) {
                    wins.fibjs.show();
                    wins.fibjs.active();
                }
                else {
                    wins.fibjs = gui.open({
                        file: path.join(__dirname, "frame.html"),
                        width: 500,
                        height: 400,
                        minWidth: 300,
                        minHeight: 200,
                        maxWidth: 800,
                        caption: false,
                        hideOnClose: true
                    });
                }
            }
        },
        {
            label: "message box",
            submenu: [
                {
                    label: "alert",
                    onclick: function () {
                        gui.alert("Hello World", "Hello World, this a message.");
                    }
                },
                {
                    label: "confirm",
                    onclick: function () {
                        console.log(gui.confirm("Confirm", "Do you want to exit?"));
                    }
                },
                {
                    label: "input",
                    onclick: function () {
                        console.log(gui.input("Input", "Enter your name:"));
                    }
                },
                {
                    label: "input password",
                    onclick: function () {
                        console.log(gui.input("Input Password", "Enter your password:", true));
                    }
                },
            ]
        },
        {
            label: "open dialog",
            submenu: [
                {
                    label: "open file",
                    onclick: function () {
                        const res = gui.chooseFile({
                            title: 'Choose a file',
                            filters: [
                                { name: 'Images', extensions: ['jpg', 'png', 'gif', 'jpeg'] },
                                { name: 'Movies', extensions: ['mkv', 'avi', 'mp4'] },
                                { name: 'Custom File Type', extensions: ['as'] },
                                { name: 'All Files', extensions: ['*'] }
                            ]
                        });

                        console.log(res);
                    }
                },
                {
                    label: "open file(multiSelections)",
                    onclick: function () {
                        const res = gui.chooseFile({
                            title: 'Choose a file',
                            multiSelections: true,
                            filters: [
                                { name: 'Images', extensions: ['jpg', 'png', 'gif', 'jpeg'] },
                                { name: 'Movies', extensions: ['mkv', 'avi', 'mp4'] },
                                { name: 'Custom File Type', extensions: ['as'] },
                                { name: 'All Files', extensions: ['*'] }
                            ]
                        });

                        console.log(res);
                    }
                },
                {
                    label: "save file",
                    onclick: function () {
                        const res = gui.chooseFile({
                            title: 'Choose a file',
                            type: 'saveFile',
                            filters: [
                                { name: 'Images', extensions: ['jpg', 'png', 'gif', 'jpeg'] },
                                { name: 'Movies', extensions: ['mkv', 'avi', 'mp4'] },
                                { name: 'Custom File Type', extensions: ['as'] },
                                { name: 'All Files', extensions: ['*'] }
                            ]
                        });

                        console.log(res);
                    }
                },
                {
                    label: "open folder",
                    onclick: function () {
                        const res = gui.chooseFile({
                            title: 'Choose a file',
                            type: 'openDirectory'
                        });

                        console.log(res);
                    }
                }
            ]
        },
        {
            label: "Exit",
            onclick: function () {
                tray.close();
                if (wins.fibjs)
                    wins.fibjs.close();
                if (wins.github)
                    wins.github.close();
            }
        }
    ]
});
