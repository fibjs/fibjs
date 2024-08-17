
var resolvers = [];
var state = false;

module.exports = {
    wait: function () {
        var promise = new Promise((resolve, reject) => {
            if (state)
                resolve();
            else
                resolvers.push(resolve);
        });

        return promise;
    },
    set: function () {
        state = true;
        resolvers.forEach(resolve => resolve());
    },
    clear: function () {
        state = false;
    }
}
