const util = require('util');

Master.onmessage = (evt) => {
    Master.postMessage(util.buildInfo());
};