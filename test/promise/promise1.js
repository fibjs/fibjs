Promise.resolve(123).then((value) => {
    throw new Error(1000);
});
