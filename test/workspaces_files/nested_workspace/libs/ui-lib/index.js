module.exports = {
    getVersion: function () {
        const utils = require('@myorg/utils');
        return "UI Lib v1.0.0 (" + utils.getCurrentTime() + ")";
    }
};
