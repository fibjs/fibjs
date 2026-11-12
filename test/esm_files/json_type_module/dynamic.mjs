export async function load() {
    const m = await import('./data.json', { with: { type: 'json' } });
    return m.default.answer;
}
