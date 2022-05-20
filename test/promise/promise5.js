new Promise(function (resolve) {
    throw new Error(1000);
}).then(r => { }, e => {
    console.log("catch promise error.");
});