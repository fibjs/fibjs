new Promise(function (resolve) {
    throw new Error(1000);
}).catch(e => {
    console.log("catch promise error.");
});