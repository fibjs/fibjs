const packageA = require('package-a');

module.exports = {
    name: "package-b",
    usePackageA: function () {
        return packageA.greet() + " used by package B";
    }
};
