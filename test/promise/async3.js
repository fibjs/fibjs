function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function a_test() {
    await sleep(100);
    throw new Error(100);
    console.log('log');
}

a_test();
