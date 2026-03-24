exports.DEFAULT_GIT_ORIGIN = 'https://github.com'
exports.CPU_NUM = require('os').cpuNumbers();
exports.DEFAULT_FIBERS = Math.min(exports.CPU_NUM * 4, 16);