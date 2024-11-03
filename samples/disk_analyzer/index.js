const gui = require('gui');
const path = require('path');
const api = require('./api');

gui.open({
    file: path.join(__dirname, 'index.html'),
    width: 800,
    height: 600,
    minWidth: 700,
    devtools: true,
    app: api,
    onclose: () => process.exit()
});