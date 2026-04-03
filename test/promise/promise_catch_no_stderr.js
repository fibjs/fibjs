// Promise rejection caught by .catch() should not produce stderr noise
var fs = require('fs');

fs.promises.stat('/nonexistent_path_for_test').catch(e => {
    console.log("caught");
});
