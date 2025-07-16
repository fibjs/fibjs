module.exports = {
    name: "web-app",
    start: function () {
        const uiLib = require('@myorg/ui-lib');
        return "Web app started with " + uiLib.getVersion();
    }
};
