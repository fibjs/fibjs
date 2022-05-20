Promise.resolve(123).then((value) => {
    throw new Error(1000);
}).catch(e => {
    console.log("catch promise error.");
});
