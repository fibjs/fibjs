async function t() {
    throw new Error(100);
}

await t();
