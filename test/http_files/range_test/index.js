const path = require('path')
const http = require('http')

const fh = http.fileHandler(path.resolve(__dirname, './'));

const server = new http.Server(9191, fh);

server.start();