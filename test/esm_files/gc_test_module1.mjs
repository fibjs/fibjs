export async function doImport() {
    const m2 = await import('./gc_test_module2.mjs');
    return m2;
}
