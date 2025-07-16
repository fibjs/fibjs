module.exports = {
    getCurrentTime: function () {
        return new Date().toISOString();
    },
    format: function (str) {
        return "[UTILS] " + str;
    }
};
